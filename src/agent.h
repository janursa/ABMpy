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
    static shared_ptr<Model<dim>> getModelPointer();    //!< Returns _mPtr                                                          //!< creates a cell based on the given type and locate its randomly in the domain

    static shared_ptr<Cell<dim>> hatch_a_cell(shared_ptr<Patch<dim>>& host_patch, std::shared_ptr<Cell<dim>> & ref_cell);
    static shared_ptr<Cell<dim>> hatch_a_cell(shared_ptr<Patch<dim>>& host_patch, string cell_type);

    static void initialize_attributes(shared_ptr<Cell<dim>> &agent);
    static std::map<string,py::object> & agent_models() { static std::map<string,py::object> var{}; return var;};

    bool has_patch = false;

    void setPatch(shared_ptr<Patch<dim>> pPtr);         //!< Sets _patchPtr
    shared_ptr<Patch<dim>> getPatch();                  //!< Returns _patchPtr
    std::shared_ptr<Cell<dim>> getPtr();                //!< Returns a shared pointer of the current cell (this).
    void function();                        //!< Cell function. This is the main function that calls all other cell activity functions such as #_mitosisFunc and #_diffFunc.

    string c_type;                                      //!< Cell's type based on #cell_t
    map<string,float> data;
    std::pair <bool,std::string> switch_info = std::make_pair(false,"");
    bool hatch_flag = false;
    bool walk_flag = false;
    bool disappear_flag = false;

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
    return cPtr;
}
template<unsigned dim>
inline shared_ptr<Cell<dim>> Cell<dim>::hatch_a_cell(shared_ptr<Patch<dim>>& host_patch, string cell_type ){
    shared_ptr<Cell<dim>> cPtr(new Cell<dim>(cell_type));
    host_patch->setCell(cPtr);   // cross-association
    cPtr->setPatch(host_patch);  // cross-association
    initialize_attributes(cPtr);
    return cPtr;
}


template<unsigned dim>
inline const void Cell<dim>::visualize(const unsigned &iCount,const settings_t &specs){

    auto SCATTER_LOG = [&](auto &item){
        vector<string> tags = py::cast<vector<string>>(item["tags"]);
        string dir =py::cast<string>(item["dir"]);
        ofstream O(dir);
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
    };
    auto AGENT_COUNT_LOG = [&](auto &item){
        string dir =py::cast<string>(item["dir"]);
        ofstream O(dir);
        unsigned ii =0;
        for (auto &agent_count_item: Model<dim>::agent_counts()){
            auto agent_tag = agent_count_item.first;
            if (ii == 0) {
                O << agent_tag;
            }
            else O<<","<<agent_tag;
            ii++;
        }
        O<<"\n";       
        for (unsigned iter =0; iter != iCount; iter++){
            unsigned ii =0;
            for (auto &agent_count_item: Model<dim>::agent_counts()){
                auto value = agent_count_item.second[iter];
                if (ii == 0) O << value;
                else O<<","<<value;
                ii++;
            }
            O<<"\n"; 
        }
        
        O.close();
    };
    auto TRAJ_LOG = [&](auto &item){
        string dir = py::cast<string>(item["dir"]);
        ofstream O(dir);
        vector<string> tags = py::cast<vector<string>>(item["tags"]);
        //header
        unsigned ii = 0;
        for (auto &tag:tags){
            if (ii == 0) O<<tag;
            else O<<","<<tag;
            ii ++;
        }
        O<<"\n";

        for (unsigned iter =0; iter != iCount; iter++){
            unsigned ii =0;
            for (auto &tag: tags){
                if (Model<dim>::data()["agent"].find(tag) == Model<dim>::data()["agent"].end()){
                    std::cerr<<RED<<"Error: "<<RESET<<" the tag '"<<RED<<tag<<RESET<<"' requested for agent log but missing in the domain variables."<<endl;
                    std::terminate();
                }
                else{
                    auto value = Model<dim>::data()["agent"][tag][iter];
                    if (ii == 0) O << value;
                    else O<<","<<value;
                    ii++;
                }
                
            }
            O<<"\n"; 
        }
        O.close();
    };
    for (auto &item:specs["agent"]){
        bool item_logs_flag = py::cast<bool>(item["flag"]);
        if (item_logs_flag == false) continue;
        unsigned interval = py::cast<unsigned>(item["interval"]);
        if (iCount != 0) {
            if ( iCount % interval != 0) continue;  // interval is not met
        }
        string type = py::cast<string>(item["type"]);
        if (type == "scatter") SCATTER_LOG(item);
        else if (type == "agents_count_traj") AGENT_COUNT_LOG(item);
        else if (type == "traj") TRAJ_LOG(item);
        else {
            std::cerr<<RED<<"Error: "<<RESET<<" type of log entered as '"<<RED<<type<<RESET<<"' is not acceptable."<<endl;
            std::terminate();
        }
        
    }


}
template<unsigned dim>
inline void Cell<dim>::initialize_attributes(shared_ptr<Cell<dim>> &agent){
    py::str tag = py::cast(agent->c_type);
    // cout<<"start"<<endl;
    for (auto attr_key:configs()["agents"][tag]["attrs"].attr("keys")()){
        string attr = attr_key.cast<string>();
        float value = configs()["agents"][tag]["attrs"][attr_key].cast<float>();
        agent->data.insert(std::pair<string,float>(attr,value));
    }
    // cout<<"end"<<endl;
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
                auto host_patch = Patch<dim>::find_an_empty_patch();
                auto agent = hatch_a_cell(host_patch,agent_name);
                container().push_back(agent);
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
        if (cell->switch_info.first == true){ // switch
            // cout<<"switching"<<endl;
            shared_ptr<Patch<dim>> host_patch = cell->getPatch();
            string new_type = cell->switch_info.second;
            host_patch->removeCell();
            auto new_cell = Cell<dim>::hatch_a_cell(host_patch,new_type);
            auto pos = distance(container().begin(), find(container().begin(), container().end(), cell));
            container()[pos] = new_cell;
        }
    }

    /* in case of disappear, Cell<dim>::container() needs to be updated */

    int jj = 0;
    while (true) {
        if (jj >= Cell<dim>::container().size()) break;
        for (int ii = jj; ii < Cell<dim>::container().size(); ii++) {
            if (Cell<dim>::container()[ii]->disappear_flag == true) {
                Cell<dim>::container().erase(Cell<dim>::container().begin() + ii);
                break;
            }
            jj++;
        };
    }

    /*** Check for mitosis ***/
    for (unsigned iter = 0 ; iter < Cell<dim>::container().size(); iter++){
        auto cell = Cell<dim>::container()[iter];
        if (cell->hatch_flag == true) {
            // cout<<"hatching"<<endl;
            shared_ptr<Cell<dim>> new_agent;
            try {
                shared_ptr<Patch<dim>> ref_patch = cell->getPatch();
                shared_ptr<Patch<dim>> host_patch = Patch<dim>::find_an_empty_patch(ref_patch);
                shared_ptr<Cell<dim>> ref_cell = cell->getPtr();
                new_agent = hatch_a_cell(host_patch,ref_cell);
                container().push_back(new_agent);
            } catch (no_available_patch & er) {
                // cerr<<"Warning: hatch cannot occur due to high agent density"<<endl;
                continue;
            }
            cell->hatch_flag = false;
            new_agent->hatch_flag = false;
            cell->data["hatch_cycle_c"]++;
            new_agent->data["hatch_cycle_c"]++;
        }
    }
    
    
    
    for (unsigned iter = 0; iter < Cell<dim>::container().size(); iter++){
        // cout<<iter<<" out of "<<Cell<dim>::container().size()<<endl;
        auto cell =  Cell<dim>::container()[iter];
        if (cell->walk_flag == false) continue;
        // cout<<"M 1"<<endl;
        shared_ptr<Patch<dim>> destination;
        shared_ptr<Patch<dim>> ref_patch = cell->getPatch();
        try {destination = Patch<dim>::find_an_empty_patch(ref_patch);}
        catch (no_available_patch & err) {
            continue;
        } 
        auto current_patch = cell->getPatch();
        current_patch->removeCell();
        destination->setCell(cell);
        cell->setPatch(destination);
    }


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
        this->has_patch = true;
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
    auto main_tags = agent_models()[this->c_type].attr("inputs").attr("keys")();
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
    for (auto &tag_:main_tags){
        string tag = py::cast<string>(tag_);
        if (tag == "self") EXTRACT_SELF_INPUTS();
        else if (tag == "patch") EXTRACT_PATCH_INPUTS();
        else {
            cerr<<"Error: output key '"<<tag<<"' is neither self or patch. Correct the input formating. "<<endl;
            std::terminate();
        }
    }
    
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
    auto main_tags = agent_models()[c_type].attr("outputs").attr("keys")();
    auto RECEIVE_SELF_OUTPUTS = [&](py::dict sub_output){
        for (const auto event_key_:sub_output.attr("keys")()){
                auto event_key = py::cast<string>(event_key_);
                // auto it = this->data.find(item.first);
                if (event_key == "hatch"){
                    this->hatch_flag = py::cast<bool>(sub_output[event_key_]);
                }
                else if (event_key == "walk"){
                    this->walk_flag = py::cast<bool>(sub_output[event_key_]);
                }
                else if (event_key == "switch"){
                    auto flag = py::str(sub_output[event_key_]).is(py::str(Py_False));
                    if (!flag) {
                        this->switch_info.first = true;
                        this->switch_info.second = py::cast<string>(sub_output[event_key_]);
                    }
                }
                else if (event_key == "disappear"){
                    this->disappear_flag = py::cast<bool>(sub_output[event_key_]);
                }
                else{
                    this->data[event_key] = py::cast<float>(sub_output[event_key_]);
                }
            }
    };
    auto RECEIVE_PATCH_OUTPUTS = [&](py::dict sub_output){
        for (const auto event_key_:sub_output.attr("keys")()){
                auto event_key = py::cast<string>(event_key_);
                this->getPatch()->data[event_key] = py::cast<float>(sub_output[event_key_]);
            }
        };
    for (auto &tag_:main_tags){
        string tag = py::cast<string>(tag_);
        auto sub_output = agent_models()[c_type].attr("outputs")[tag_];
        if (tag == "self") RECEIVE_SELF_OUTPUTS(sub_output);
        else if (tag == "patch") RECEIVE_PATCH_OUTPUTS(sub_output);
        else {
            cerr<<"Error: output key '"<<tag<<"' is neither self or patch. Correct the output formating. "<<endl;
            std::terminate();
        }
    }
    
}
template <unsigned dim>
inline void Cell<dim>::function(){
    this->update_agent_inputs();
    this->receive_agent_outputs();
}
    

#endif //DIFFUSION_CELL_H
