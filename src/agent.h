//
// Created by nourisaj on 5/23/19.
//

#ifndef DIFFUSION_CELL_H
#define DIFFUSION_CELL_H
#include <iostream>
#include "patch.h"
#include "model.h"
#include "toolbox.h"
#include "settings.h"
template <unsigned dim>
class Cell;
template <unsigned dim>
class Patch;
template <unsigned dim>
class Model;
#ifdef PYTHON
struct interface{
 static output_t const forward(string &agent_type, input_t&inputs){
        py::dict inputs_dict = py::cast(inputs);
        if (agent_models().size() == 0){
            std::cerr<<"Error: no agent model is defined"<<std::endl; std::terminate();
        }
        py::object agent_model;
        try{agent_model = agent_models().at(agent_type);}
        catch(out_of_range &er) {
            std::cerr<<"Error: no agent model is defined for agent type '"<<agent_type<<"'."<<std::endl; std::terminate();
        }

        auto PROCESS_OUTPUTS = [&](py::dict &outputs_dict){
            output_t outputs;
            using sub_output_t = std::map<string,float>;
            for (auto &key1:outputs_dict.attr("keys")()){ // iterate over each main key in the results
                sub_output_t sub_outputs; //!< outputs for each of keys self, patch
                for (auto &key2:outputs_dict[key1].attr("keys")()){ // iterate over each sub key in the results
                    auto sub_output = std::pair<string,float>(key2.cast<string>(),outputs_dict[key1][key2].cast<float>());
                    sub_outputs.insert(sub_output);
                }
                outputs.insert(std::pair<string,sub_output_t>(key1.cast<string>(),sub_outputs));
            }
            return outputs;
        };
        py::dict outputs_dict = agent_model.attr("forward")(inputs_dict);
        auto outputs = PROCESS_OUTPUTS(outputs_dict);
        return outputs;
    }
    static std::map<string,py::object> & agent_models() {
        static std::map<string,py::object> var{};
        return var;
    };
};
#endif //PYTHON

//!  Main class for defining cell as an agent.
/*!
*/
template<unsigned dim>
class Cell:public enable_shared_from_this<Cell<dim>>{
public:
    Cell(string);                                       //!< Cell constructor
    explicit Cell(std::shared_ptr<Cell<dim>> &cellPtr); //!< Cell constructor based on a given cell. This is used in proliferation. #_attr is transferred to the child cell.
    virtual ~Cell();
    static void setup_cells(weak_ptr<Model<dim>> mPtr,json configs_,float downscale_factor);
    static void run(const unsigned & iCount){
            for (int ii=0; ii< Cell<dim>::container().size(); ii++) {
                Cell<dim>::container()[ii]->function();
            }
        }
    static void update();                              //!< Updates class variables
    static const void visualize(const unsigned &iCount,const json &specs);
    static shared_ptr<Model<dim>> getModelPointer();    //!< Returns _mPtr
    static shared_ptr<Cell<dim>> create_and_locate_a_cell(shared_ptr<Cell<dim>> ref_cell); 
    static bool create_and_locate_a_cell(const string cell_type);                                                           //!< creates a cell based on the given type and locate its randomly in the domain

    static shared_ptr<Cell<dim>> hatch_a_cell(shared_ptr<Patch<dim>>& host_patch, std::shared_ptr<Cell<dim>> & ref_cell);
    static bool hatch_a_cell(shared_ptr<Patch<dim>>& host_patch, string cell_type);

    void setPatch(shared_ptr<Patch<dim>> pPtr);         //!< Sets _patchPtr
    shared_ptr<Patch<dim>> getPatch();                  //!< Returns _patchPtr
    std::shared_ptr<Cell<dim>> getPtr();                //!< Returns a shared pointer of the current cell (this).
    void function();                        //!< Cell function. This is the main function that calls all other cell activity functions such as #_mitosisFunc and #_diffFunc.

    string c_type;                                      //!< Cell's type based on #cell_t
    bool alive;                                         //!< A flag of cell's aliveness
    map<string,float> data;
    bool hatch_flag = false;
    bool walk_flag = false;
    bool die_flag = false;

    static vector<shared_ptr<Cell<dim>>>& container(){
        static vector<shared_ptr<Cell<dim>>> var{};
        return var;
    };   //!< Bank of Cell.
    static json& configs(){ 
        static json var{};
        return var;
    };
    static weak_ptr<Model<dim>>& _mPtr(){     //!< Weak pointer to the model
        static weak_ptr<Model<dim>> var{};
        return var;
    };  
protected:
    weak_ptr<Patch<dim>> _patchPtr;                    //!< weak pointer to the patch the cell resides in
                                                
    /*** common methods ***/
    void _adaptation();                                //!< Calculates the effect of pH sudden change on cell
    void _calculate_class_variables();                 //!< Calculates internal variables by calling functions #_calculate_bonded_mg, #_calculate_MI, #_calculate_alkaline_effect.
    void _calculate_bonded_mg();                       //!< Calculates bonded Mg based on binding and unbinding rates.
    input_t _extract_inputs();
    void _calculate_MI();

};
template<unsigned dim>
inline shared_ptr<Cell<dim>> Cell<dim>::hatch_a_cell(shared_ptr<Patch<dim>>& host_patch, std::shared_ptr<Cell<dim>> & ref_cell){
    shared_ptr<Cell<dim>> cPtr(new Cell<dim>(ref_cell));
    host_patch->setCell(cPtr);   // cross-association
    cPtr->setPatch(host_patch);  // cross-association
    Cell<dim>::container().push_back(cPtr);


    return cPtr;
}
template<unsigned dim>
inline bool Cell<dim>::hatch_a_cell(shared_ptr<Patch<dim>>& host_patch, string cell_type ){
    shared_ptr<Cell<dim>> cPtr(new Cell<dim>(cell_type));
    host_patch->setCell(cPtr);   // cross-association
    cPtr->setPatch(host_patch);  // cross-association
    Cell<dim>::container().push_back(cPtr);
    return true;
}
template<unsigned dim>
inline bool Cell<dim>::create_and_locate_a_cell(const std::string cell_type) {
    shared_ptr<Patch<dim>> host_patch;
    try{
        host_patch = Patch<dim>::find_an_empty_patch();
    }catch(no_available_patch& nap){
        throw nap;
    }

    hatch_a_cell(host_patch,cell_type);
    return true;
}

template<unsigned dim>
inline shared_ptr<Cell<dim>> Cell<dim>::create_and_locate_a_cell(shared_ptr<Cell<dim>> ref_cell){
    auto ref_patch = ref_cell->getPatch();
    shared_ptr<Patch<dim>> host_patch;
    try{
        host_patch = Patch<dim>::find_an_empty_patch(ref_patch);
    }catch(no_available_patch& nap){
        throw nap;
    }
    if (host_patch == nullptr) cerr<<"nll pointer is returned"<<endl;
    try{
        return hatch_a_cell(host_patch,ref_cell);
    }catch(...){
        __throw_bad_alloc();
    }

}

template<unsigned dim>
inline const void Cell<dim>::visualize(const unsigned &iCount,const json &specs){
    if (specs["flag"] == false) return;
    unsigned interval = specs["interval"];
      // cout<<"cell visualzie 1"<<endl;
      if (iCount != 0) {
        if ( iCount % interval != 0) return;  // interval is not met
      }
      string dir = specs["dir"];
      ofstream O(dir);
      // header
      vector<string> tags = specs["tags"];
      O<<"x,y,type";
      for (auto &tag:tags){
        O<<","<<tag;
      }
      O<<"\n";        
      for (auto &agent:container()){ 
        float x = agent->getPatch()->xyzcoords[0];
        float y = agent->getPatch()->xyzcoords[1];
        string type = agent->c_type;
        O<<x<<","<<y<<","<<type;
        for (auto &tag:tags){
            float value;
            try {value = agent->data.at(tag);}
            catch(out_of_range &er){
                cerr<<"Error: '"<<tag<<"' requested for visualization is not defined in agent attributes"<<endl;
                std::terminate();
            }
            O<<","<<value;
          }
        O<<"\n";
      }
      O.close();

}
template<unsigned dim>
inline void Cell<dim>::setup_cells(weak_ptr<Model<dim>> mPtr,json configs_,float downscale_factor){
    _mPtr() = mPtr;
    configs() = configs_;
    // create the agents
    vector<int> agents_initial_n = configs()["initial_n"];
    vector<std::string> cell_types = configs()["names"];
    for (unsigned i = 0; i < cell_types.size(); i ++){
        string cell_type = cell_types[i];
        int cell_count = agents_initial_n[i] / downscale_factor;
        for(int iter=0; iter<cell_count; iter++) {
            try{
                create_and_locate_a_cell(cell_type);
            }catch(no_available_patch&nap){
                cerr<<"Error:: no available patch for agents during setup"<<endl;
                std::terminate();
            }
        }
    }
    // assign initial attributes to agents
    for (auto & cell:container()){
        string name = cell->c_type;
        auto attrs = configs()[name]["attrs"];
        if (attrs == nullptr) {
            // cerr<<"Warning:: Attrs is not entered for agent type '"<<name<<"'"<<endl;
            // std::terminate();
        }
        for (auto &item:attrs){
            for (auto& el : item.items()) {
              string attr = el.key();
              float value = el.value();
              cell->data.insert(std::pair<string,float>(attr,value));
              // attr_tags().push_back(name);
            }
        }
    }
    // /// adjust the mitosis record for missing experiments
    // if (inputs::params()["passaging_n"] != 1) {
    //     for (auto &cell: _cellContainer) {
    //         cell->_attr_int[enums::mitosis_cycle_c_a] = inputs::params()["MSC_mitosis_age"];
    //     }

    string message = std::to_string(container().size()) + " cells successfully created";
    LOG(message);


}

template<unsigned dim>
inline void Cell<dim>::update() {
    /*** Update age-related members ***/

    for (auto &cell:Cell<dim>::container()){
        if (cell->die_flag == true){//check if it's dying
#ifdef AUTO_REMOVAL_DEADCELLS
            auto pPtr = cell->getPatch();
            pPtr->removeCell();

#endif //AUTO_REMOVAL_DEADCELLS
            cell->alive = false;
            cell->c_type = "Dead";
        }
    }

    /* in case of apoptosis, Cell<dim>::container() needs to be updated */
#ifdef AUTO_REMOVAL_DEADCELLS
    {
        int jj = 0;
        while (true) {
            if (jj >= Cell<dim>::container().size()) break;
            for (int ii = jj; ii < Cell<dim>::container().size(); ii++) {
                if (Cell<dim>::container()[ii]->alive == false) {
                    Cell<dim>::container().erase(Cell<dim>::container().begin() + ii);
                    break;
                }
                jj++;
            };
        }
    }
#endif //AUTO_REMOVAL_DEADCELLS
    /*** Check for mitosis ***/
    for (unsigned iter = 0 ; iter < Cell<dim>::container().size(); iter++){
        auto cell = Cell<dim>::container()[iter];
        if (cell->hatch_flag == true) {
            shared_ptr<Cell<dim>> new_cell;
            try {
                new_cell = create_and_locate_a_cell(cell->getPtr());
            } catch (no_available_patch & er) {
                // cerr<<"Warning: hatch cannot occur due to high agent density"<<endl;
                continue;
            }
            cell->hatch_flag = false;
            new_cell->hatch_flag = false;
            cell->data["hatch_cycle_c"]++;
            new_cell->data["hatch_cycle_c"]++;
        }
    }
    
#ifdef MIGRATION
    auto CHOOSE_A_PATCH_TO_MOVE = [&](shared_ptr<Cell<dim>> & cell){
        auto patch = cell->getPatch();
        auto neighborPatches = patch->neighborPatches;
        vector<int> available_patches_indices;
        for (unsigned i=0; i < neighborPatches.size(); i++){
            if (neighborPatches[i]->hasCell()) continue;
            available_patches_indices.push_back(i);
        }
        if (available_patches_indices.size() == 0) throw no_available_patch();
        else {
            unsigned chosen_index = tools::create_random_value(0,available_patches_indices.size());
            return neighborPatches[chosen_index];
        }
    };
    
    for (unsigned iter = 0; iter < Cell<dim>::container().size(); iter++){
        auto cell =  Cell<dim>::container()[iter];
        if (cell->walk_flag == false) continue;
        shared_ptr<Patch<dim>> destination;
        try {destination = CHOOSE_A_PATCH_TO_MOVE(cell);}
        catch (no_available_patch & err) {
            continue;
        } 
        auto current_patch = cell->getPatch();
//            cout<<"got the patch"<<endl;
        current_patch->removeCell();
//            cout<<"removed the cell"<<endl;
        destination->setCell(cell);
        Cell<dim>::container().push_back(cell);
    }

#endif //MIGRATiON

}




template<unsigned dim>
inline Cell<dim>::Cell(string cell_type):
        alive(true),
        c_type(cell_type)
{

}
template<unsigned dim>
inline Cell<dim>::Cell(shared_ptr<Cell<dim>> &cellPtr):Cell(cellPtr->c_type){
   this->c_type = cellPtr->c_type;
   this->alive = true;
   this->data = cellPtr->data;
}

template<unsigned dim>
inline Cell<dim>::~Cell(){

}

template<unsigned dim>
inline void Cell<dim>::setPatch(shared_ptr<Patch<dim>> pPtr){
    if (!pPtr) {
        std:cerr<<"null ptr from inside cell"<<endl;
        std::terminate();
    }
    try{
        this->_patchPtr  = weak_ptr<Patch<dim>>(pPtr);
    } catch(...){
        std::cerr<<"couldn't convert to shared ptr in set patch inside cell class"<<endl;

    }
}


template<unsigned dim>
inline shared_ptr<Patch<dim>> Cell<dim>::getPatch(){
    if(this->_patchPtr.expired()){
        cout<<"Weak_ptr to patch is exipred"<<endl;
        std::terminate();
    }
    auto s_patchPtr = shared_ptr<Patch<dim>>(this->_patchPtr);
    return s_patchPtr;
}



template <unsigned dim>
inline shared_ptr<Model<dim>> Cell<dim>::getModelPointer(){
    return shared_ptr<Model<dim>>(Cell<dim>::_mPtr());
}

template<unsigned dim>
inline std::shared_ptr<Cell<dim>> Cell<dim>::getPtr(){
    return this->shared_from_this();
}



template <unsigned dim>
inline void Cell<dim>::_adaptation(){

    // auto pH = this->getPatch()->get_ph();
    // float adapt_rate;
    // if (this->c_type== "MSC") {
    //     adapt_rate = inputs::params()["B_AErec_MSC"];
    // } else{
    //     cerr<<"cell type "<< this->c_type<<" is not expected"<<endl;
    //     std::terminate();
    // }
    // if (pH > _attr_float[enums::adapted_pH_a]){
    //     _attr_float[enums::adapted_pH_a] += adapt_rate;
    // }else{
    //     _attr_float[enums::adapted_pH_a] -= adapt_rate;
    // }

}
template <unsigned dim>
inline void Cell<dim>::_calculate_class_variables() {
//    cout<<" class variable calculations"<<endl;
//    this->_calculate_bonded_mg();
//    this->_calculate_alkaline_effect();
//    this->_calculate_MI();


}
template<unsigned dim>
inline void Cell<dim>::_calculate_bonded_mg(){
//     auto mg = this->getPatch()->get_mg();
// //    _attr_float[enums::bonded_mg_a] += inputs::params().at("B_ab")*mg - inputs::params().at("B_ex")*_attr_float[enums::bonded_mg_a] ;
//     _attr_float[enums::bonded_mg_a] += inputs::params()["B_ab"]*mg  ;
//     (_attr_float[enums::bonded_mg_a]>inputs::params()["S_mg_ab"])?(_attr_float[enums::bonded_mg_a] = inputs::params()["S_mg_ab"]):(true); // bonded_mg cannot exceed mg saturation limit
// //    assert(("The value of bonded mg cannot be negative",_attr_float[bonded_mg_a]>=0));
// //    cout<<"mg: "<<mg<<" bonded mg:"<<_attr_float[bonded_mg_a]<<endl;
}
template<unsigned dim>
inline void Cell<dim>::_calculate_MI(){
//     auto tag = "metabolism";
//     try{
// //        auto Me = plugs::fuzzy.controllers[c_type]->getOutput(tag);
//     float Me = 1;
//         _attr_float[enums::MI_a] = Me;
//     }catch(...){
//         cerr<<"error in getting metabolism from fuzzy controller"<<endl;
//         std::terminate();
//     }



}

template <unsigned dim>
inline input_t Cell<dim>::_extract_inputs(){

    /** determine inputs **/
    input_t inputs;  // containing both self and patch inputs taged for each seperately
    // for self inputs
    auto EXTRACT_SELF_INPUTS = [&](){
        vector<string> from_self_inputs = configs()[this->c_type]["inputs"]["self"];  // inputs of agents from its attributes

        for (auto &tag:from_self_inputs){ // collecting inputs from self
            float value;
            try {value = this->data.at(tag);}
            catch (out_of_range & er) {
                cerr<<"The input '"<<tag<<"' for agent [self] is not defined in '"<<this->c_type<<"' attributes but requested as input"<<endl;
                std::terminate();
            } 
            vector<float> values {value};
            inputs["self"].insert(std::pair<string,vector<float>>(tag,values));
        }
    };
    if (configs()[this->c_type]["inputs"]["self"] != nullptr) EXTRACT_SELF_INPUTS();
    
    

    // for patch inputs
    auto EXTRACT_PATCH_INPUTS = [&](){
        vector<string> from_patch_inputs = configs()[this->c_type]["inputs"]["patch"]; // inputs of agents from its patch
        for (auto &tag:from_patch_inputs){ // collecting inputs from patch
            float value;
            try {value = this->getPatch()->data.at(tag);}
            catch (out_of_range & er) {
                cerr<<"The input key '"<<tag<<"' for agent [patch] is not defined in patch attributes "<<endl;
                std::terminate();
            } 
            vector<float> values {value};
            inputs["patch"].insert(std::pair<string,vector<float>>(tag,values));
        }
    };
    if (configs()[this->c_type]["inputs"]["patch"] != nullptr) EXTRACT_PATCH_INPUTS();
    
    // for neighbors  inputs
    auto EXTRACT_NEIGHBORS_INPUTS = [&](){
        vector<string> from_neighbors_inputs = configs()[this->c_type]["inputs"]["neighbors"];
        for (auto &tag:from_neighbors_inputs){ // collecting inputs from patch
            vector<float> values; 
            for (auto &patch:this->getPatch()->neighborPatches){
                float value;
                try {value = patch->data.at(tag);}
                catch (out_of_range & er) {
                    cerr<<"The input key '"<<tag<<"' for agent [neighbors] is not defined in patch attributes "<<endl;
                    std::terminate();
                } 
                values.push_back(value);
            }
            inputs["neighbors"].insert(std::pair<string,vector<float>>(tag,values));
        }
    };
    if (configs()[this->c_type]["inputs"]["neighbors"] != nullptr) EXTRACT_NEIGHBORS_INPUTS();
    return inputs;
}
template <unsigned dim>
inline void Cell<dim>::function(){
    if (this->alive == false) return;
//    this->_calculate_class_variables();

    auto inputs = this->_extract_inputs();
    auto outputs = interface::forward(c_type,inputs);
    // output_t outputs; 
    for (const auto &output : outputs){
        if (output.first == "self"){
            auto self_output = output.second;
            for (const auto &item:self_output){
                auto it = this->data.find(item.first);
                if (item.first == "hatch"){
                    this->hatch_flag = true;
                }
                else if (item.first == "walk"){
                    this->walk_flag = true;
                }
                else if (it == this->data.end()){
                    cerr<<"Error: the output keyword '"<<item.first<<"' is outputed but not defined as agent attributes."<<endl;
                    std::terminate();
                }else{
                    it->second = item.second;
                }
            }
        } 
        else if (output.first == "patch"){
            auto patch_output = output.second;
            for (const auto &item:patch_output){
                auto it = this->getPatch()->data.find(item.first);
                if (it == this->getPatch()->data.end()){
                    cerr<<"Error: the output keyword '"<<item.first<<"' is outputed but not defined as patch attributes."<<endl;
                    std::terminate();
                }else{
                    it->second = item.second;
                }
            }
        }
        else {
            cerr<<"Error: output key '"<<output.first<<"' is neither self or patch. Correct the output formating. "<<endl;
            std::terminate();
        }

    }


//     this->_calculate_MI();
//     this->_adaptation();
// //    cout<<"start"<<endl;

// //    cout<<"calculate_class"<<endl;
//     this->_mortalityFunc();
// ////    cout<<"_necrosisFunc"<<endl;
// #ifdef MIGRATION
//     this->_migrationFunc();
// #endif //migration
// ////    cout<<"_migrationFunc"<<endl;
//     this->_mitosisFunc();
////    cout<<"_mitosisFunc"<<endl;
//
//    this->_diffFunc();
////    cout<<"_diffFunc"<<endl;
//    this->_cellProductions();
//    cout<<"_cellProductions"<<endl;

#ifdef FUZZY
//    plugs::fuzzy.controllers[enums::MSC_t]->restart();
#endif //FUZZY
}
    

#endif //DIFFUSION_CELL_H
