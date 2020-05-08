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
    // static void setup_cells(weak_ptr<Model<dim>> mPtr,settings_t configs_);
    
    static shared_ptr<Model<dim>> getModelPointer();    //!< Returns _mPtr                                                          //!< creates a cell based on the given type and locate its randomly in the domain

    static shared_ptr<Cell<dim>> hatch_a_cell(shared_ptr<Patch<dim>>& host_patch, std::shared_ptr<Cell<dim>> & ref_cell);
    static shared_ptr<Cell<dim>> hatch_a_cell(shared_ptr<Patch<dim>>& host_patch, string cell_type);

    void initialize_attributes();
    

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

    
    settings_t configs(){
        return getModelPointer()->settings["configs"];
        
    };
    static weak_ptr<Model<dim>>& _mPtr(){     //!< Weak pointer to the model
        static weak_ptr<Model<dim>> var{};
        return var;
    };
    
    py::object get_agent_model(string &type){
        return getModelPointer()->agent_models[type];
    }
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
    cPtr->initialize_attributes();
    return cPtr;
}


template<unsigned dim>
inline void Cell<dim>::initialize_attributes(){
    py::str tag = py::cast(c_type);
    // cout<<"start"<<endl;
    for (auto attr_key:configs()["agents"][tag]["attrs"].attr("keys")()){
        string attr = py::cast<string>(attr_key);
        float value = py::cast<float>(configs()["agents"][tag]["attrs"][attr_key]);
        this->data.insert(std::pair<string,float>(attr,value));
    }
    // cout<<"end"<<endl;
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
    auto main_tags = get_agent_model(this->c_type).attr("inputs").attr("keys")();
    // for self inputs
    auto EXTRACT_SELF_INPUTS = [&](){
        for (auto tag:get_agent_model(this->c_type).attr("inputs")["self"].attr("keys")()){ // collecting inputs from self
            // py::print(tag);
            float value;
//            cout<<"update_agent_inputs "<<tag<<" "<<this->data.at(py::cast<string>(tag))<<endl;
            try {value = this->data.at(py::cast<string>(tag));}
            catch (out_of_range & er) {
                cerr<<"The input '"<<tag<<"' for agent [self] is not defined in '"<<this->c_type<<"' attributes but requested as input"<<endl;
                std::terminate();
            }
            get_agent_model(this->c_type).attr("inputs")["self"][tag] = value;
        }
    };
    // for patch inputs
    auto EXTRACT_PATCH_INPUTS = [&](){
        for (auto &tag:get_agent_model(this->c_type).attr("inputs")["patch"].attr("keys")()){ // collecting inputs from patch
            float value;
            try {value = this->getPatch()->data.at(py::cast<string>(tag));}
            catch (out_of_range & er) {
                cerr<<"The input key '"<<tag<<"' for agent [patch] is not defined in patch attributes "<<endl;
                std::terminate();
            }
            vector<float> values {value};
            get_agent_model(this->c_type).attr("inputs")["patch"][tag] = value ;
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
    get_agent_model(c_type).attr("forward")();  // updates the outputs in python file
    auto main_tags = get_agent_model(c_type).attr("outputs").attr("keys")();
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
//                    cout<<"receive outputs "<<event_key<<this->data.at(event_key)<<endl;
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
        auto sub_output = get_agent_model(c_type).attr("outputs")[tag_];
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
