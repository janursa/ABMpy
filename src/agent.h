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

//!  Main class for defining cell as an agent.
/*!
*/
template<unsigned dim>
class Cell:public enable_shared_from_this<Cell<dim>>{
public:
    Cell(string);                                       //!< Cell constructor
    explicit Cell(std::shared_ptr<Cell<dim>> &cellPtr); //!< Cell constructor based on a given cell. This is used in proliferation. #_attr is transferred to the child cell.
    virtual ~Cell();
    static void setup_cells(weak_ptr<Model<dim>> mPtr,settings_t configs_);
    static void run(const unsigned & iCount){
            for (int ii=0; ii< Cell<dim>::container().size(); ii++) {
                // cout<<"cell number "<<ii<<" out of "<<Cell<dim>::container().size()<<" dead "<<Cell<dim>::container()[ii]->die_flag<<endl;
                Cell<dim>::container()[ii]->function();
            }
        }
    static void update();                              //!< Updates class variables
    static const void visualize(const unsigned &iCount,const settings_t &specs);
    static shared_ptr<Model<dim>> getModelPointer();    //!< Returns _mPtr
    static shared_ptr<Cell<dim>> create_and_locate_a_cell(shared_ptr<Cell<dim>> ref_cell); 
    static shared_ptr<Cell<dim>> create_and_locate_a_cell(const string cell_type);                                                           //!< creates a cell based on the given type and locate its randomly in the domain

    static shared_ptr<Cell<dim>> hatch_a_cell(shared_ptr<Patch<dim>>& host_patch, std::shared_ptr<Cell<dim>> & ref_cell);
    static shared_ptr<Cell<dim>> hatch_a_cell(shared_ptr<Patch<dim>>& host_patch, string cell_type);

    static std::map<string,py::object> & agent_models() { static std::map<string,py::object> var{}; return var;};


    void setPatch(shared_ptr<Patch<dim>> pPtr);         //!< Sets _patchPtr
    shared_ptr<Patch<dim>> getPatch();                  //!< Returns _patchPtr
    std::shared_ptr<Cell<dim>> getPtr();                //!< Returns a shared pointer of the current cell (this).
    void function();                        //!< Cell function. This is the main function that calls all other cell activity functions such as #_mitosisFunc and #_diffFunc.

    string c_type;                                      //!< Cell's type based on #cell_t
    map<string,float> data;
    bool die_flag = false;                                         //!< A flag of cell's aliveness
    bool hatch_flag = false;
    bool walk_flag = false;

    static vector<shared_ptr<Cell<dim>>>& container(){
        static vector<shared_ptr<Cell<dim>>> var{};
        return var;
    };   //!< Bank of Cell.
    static settings_t& configs(){ 
        static settings_t var{};
        return var;
    };
    static weak_ptr<Model<dim>>& _mPtr(){     //!< Weak pointer to the model
        static weak_ptr<Model<dim>> var{};
        return var;
    };  
protected:
    weak_ptr<Patch<dim>> _patchPtr;                    //!< weak pointer to the patch the cell resides in
                                                
    /*** common methods ***/
    void update_agent_inputs();
    void receive_agent_outputs();


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
inline shared_ptr<Cell<dim>> Cell<dim>::hatch_a_cell(shared_ptr<Patch<dim>>& host_patch, string cell_type ){
    shared_ptr<Cell<dim>> cPtr(new Cell<dim>(cell_type));
    host_patch->setCell(cPtr);   // cross-association
    cPtr->setPatch(host_patch);  // cross-association
    Cell<dim>::container().push_back(cPtr);
    return cPtr;
}
template<unsigned dim>
inline shared_ptr<Cell<dim>> Cell<dim>::create_and_locate_a_cell(const std::string cell_type) {
    shared_ptr<Patch<dim>> host_patch;
    try{
        host_patch = Patch<dim>::find_an_empty_patch();
    }catch(no_available_patch& nap){
        throw nap;
    }

    auto agent = hatch_a_cell(host_patch,cell_type);
    return agent;
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
inline const void Cell<dim>::visualize(const unsigned &iCount,const settings_t &specs){
    bool agent_logs_flag = py::cast<bool>(specs["flag"]);
    if (agent_logs_flag == false) return;
    unsigned interval = py::cast<unsigned>(specs["interval"]);
      if (iCount != 0) {
        if ( iCount % interval != 0) return;  // interval is not met
      }
      // cout<<"cell visualzie "<<endl;

      string dir =py::cast<string>(specs["dir"]);
      ofstream O(dir);
      // header
      vector<string> tags = specs["tags"].cast<vector<string>>();
      O<<"x,y,type";
      for (auto &tag:tags){
        O<<","<<tag;
      }
      O<<"\n";        
      unsigned ii =0; 
      for (auto &agent:container()){ 
        float x = agent->getPatch()->xyzcoords[0];
        float y = agent->getPatch()->xyzcoords[1];
        // if (ii == 0) cout<<agent->getPatch()->patchIndex<<endl;

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
        ii++;
      }
      O.close();

}
template<unsigned dim>
inline void Cell<dim>::setup_cells(weak_ptr<Model<dim>> mPtr,settings_t configs_){
    _mPtr() = mPtr;
    configs() = configs_;
    // create the agents
    for (auto key:configs()["agents"].attr("keys")()){
        unsigned agent_count = configs()["agents"][key]["initial_n"].cast<unsigned>();

        string agent_name = key.cast<string>();
        // create initial agents and assign attributes
        for(int iter=0; iter<agent_count; iter++) {
            try{
                auto agent = create_and_locate_a_cell(agent_name);
                for (auto attr_key:configs()["agents"][key]["attrs"].attr("keys")()){
                    string attr = attr_key.cast<string>();
                    float value = configs()["agents"][key]["attrs"][attr_key].cast<float>();
                    agent->data.insert(std::pair<string,float>(attr,value));
                }
                
            }catch(no_available_patch&nap){
                cerr<<"Error:: no available patch for agents during setup"<<endl;
                std::terminate();
            }
        }

    }
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
                if (Cell<dim>::container()[ii]->die_flag == true) {
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
        if (cell->hatch_flag == true and cell->die_flag == false) {
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
        // cout<<iter<<" out of "<<Cell<dim>::container().size()<<endl;
        auto cell =  Cell<dim>::container()[iter];
        if (cell->walk_flag == false or cell->die_flag == true) continue;
        // cout<<"M 1"<<endl;
        shared_ptr<Patch<dim>> destination;
        try {destination = CHOOSE_A_PATCH_TO_MOVE(cell);}
        catch (no_available_patch & err) {
            continue;
        } 
        auto current_patch = cell->getPatch();
        current_patch->removeCell();
        destination->setCell(cell);
        cell->setPatch(destination);
    }

#endif //MIGRATiON

}




template<unsigned dim>
inline Cell<dim>::Cell(string cell_type):
        c_type(cell_type)
{

}
template<unsigned dim>
inline Cell<dim>::Cell(shared_ptr<Cell<dim>> &cellPtr):Cell(cellPtr->c_type){
   this->c_type = cellPtr->c_type;
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
inline void Cell<dim>::update_agent_inputs(){
    // for self inputs
    auto EXTRACT_SELF_INPUTS = [&](){
        for (auto tag:agent_models()[this->c_type].attr("inputs")["self"].attr("keys")()){ // collecting inputs from self
            // py::print(tag);
            float value;
            try {value = this->data.at(py::cast<string>(tag));}
            catch (out_of_range & er) {
                cerr<<"The input '"<<tag<<"' for agent [self] is not defined in '"<<this->c_type<<"' attributes but requested as input"<<endl;
                std::terminate();
            } 
            agent_models()[this->c_type].attr("inputs")["self"][tag] = value;
        }
    };
    EXTRACT_SELF_INPUTS();
    

    // for patch inputs
    auto EXTRACT_PATCH_INPUTS = [&](){
        for (auto &tag:agent_models()[this->c_type].attr("inputs")["patch"].attr("keys")()){ // collecting inputs from patch
            float value;
            try {value = this->getPatch()->data.at(py::cast<string>(tag));}
            catch (out_of_range & er) {
                cerr<<"The input key '"<<tag<<"' for agent [patch] is not defined in patch attributes "<<endl;
                std::terminate();
            } 
            vector<float> values {value};
            agent_models()[this->c_type].attr("inputs")["patch"][tag] = value ;
        }
    };
    EXTRACT_PATCH_INPUTS();
    
    // // for neighbors  inputs
    // auto EXTRACT_NEIGHBORS_INPUTS = [&](){
    //     vector<string> from_neighbors_inputs = configs()[this->c_type]["inputs"]["neighbors"];
    //     for (auto &tag:from_neighbors_inputs){ // collecting inputs from patch
    //         vector<float> values; 
    //         for (auto &patch:this->getPatch()->neighborPatches){
    //             float value;
    //             try {value = patch->data.at(tag);}
    //             catch (out_of_range & er) {
    //                 cerr<<"The input key '"<<tag<<"' for agent [neighbors] is not defined in patch attributes "<<endl;
    //                 std::terminate();
    //             } 
    //             values.push_back(value);
    //         }
    //         inputs["neighbors"].insert(std::pair<string,vector<float>>(tag,values));
    //     }
    // };
    // if (configs()[this->c_type]["inputs"]["neighbors"] != nullptr) EXTRACT_NEIGHBORS_INPUTS();
    // return inputs;
}
template <unsigned dim>
inline void Cell<dim>::receive_agent_outputs(){
    agent_models()[c_type].attr("forward")();  // updates the outputs in python file
    // output_t outputs; 
    for (auto key:agent_models()[c_type].attr("outputs").attr("keys")()){
        auto sub_output = agent_models()[c_type].attr("outputs")[key];
        if (py::cast<string>(key) == "self"){
            for (const auto event_key_:sub_output.attr("keys")()){
                auto event_key = py::cast<string>(event_key_);
                // auto it = this->data.find(item.first);
                if (event_key == "hatch"){
                    this->hatch_flag = py::cast<bool>(sub_output[event_key_]);
                }
                else if (event_key == "walk"){
                    this->walk_flag = py::cast<bool>(sub_output[event_key_]);
                }
                else if (event_key == "die"){
                    this->die_flag = py::cast<bool>(sub_output[event_key_]);
                }
                else{
                    this->data[event_key] = py::cast<float>(sub_output[event_key_]);
                }
            }
        } 
        else if (py::cast<string>(key) == "patch"){
            for (const auto event_key_:sub_output.attr("keys")()){
                auto event_key = py::cast<string>(event_key_);
                this->getPatch()->data[event_key] = py::cast<float>(sub_output[event_key_]);
            }
        }
        else {
            cerr<<"Error: output key '"<<py::cast<string>(key)<<"' is neither self or patch. Correct the output formating. "<<endl;
            std::terminate();
        }

    }
}
template <unsigned dim>
inline void Cell<dim>::function(){
    // cout<<"inside function"<<endl;
    if (this->die_flag == true) return;
    this->update_agent_inputs();
    // cout<<"agnet input updated"<<endl;
    this->receive_agent_outputs();
    // cout<<"agnet outputs received"<<endl;
    
}
    

#endif //DIFFUSION_CELL_H
