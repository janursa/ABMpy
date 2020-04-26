//
// Created by nourisaj on 5/24/19.
//

#ifndef ABM_MODEL_H
#define ABM_MODEL_H



#include "patch.h"
#include "agent.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <thread>
#include "settings.h"
#include <pybind11/stl.h>
namespace py = pybind11;
using json = nlohmann::json;
template <unsigned dim>
class Cell;
template <unsigned dim>
class Patch;
using namespace std;
//!  Model class
/*!
  Takes care of generating mesh, creating patch, creating cells, etc. as well as executing them on an iterative manner.
*/
template<unsigned dim>
class Model:public enable_shared_from_this<Model<dim>>{
    friend class Cell<dim>;
    friend class Patch<dim>;
public:
    Model(settings_t settings);             
    ~Model(){};
    bool setup();        
    void run();    
    void  visualize() const; //!< To log requrested outputs for visualization
protected:

    void _update();
    void _passaging(); 
    unsigned iCount;            //!< Iteration index of the simulation incremented in #run.
    unsigned end_passaging_n;   //!< Count of passaging occured in the model
    unsigned start_passaging_n;
    unsigned passaging_count;
    unsigned duration_ticks;    //!< experiment duration
    settings_t settings;
};
template<unsigned dim>
inline Model<dim>::Model(settings_t settings_):
        iCount(0)
{     
    srand(time(0));
    settings = settings_;
   
}

template<unsigned dim>
inline bool Model<dim>::setup(){
     auto CHECK_PARAMS = [&](){
        // auto pIter = inputs::params()["passaging_n"];
        // if (pIter > inputs::params()["end_passaging_n"]){
        //     cerr<<"end_passaging_n cannot be lower than 1"<<endl;
        //     std::terminate();
        // }
    };
    CHECK_PARAMS();
    // auto PRE_CALCULATIONS = [&](){ // calculates ABM parameters bsed on the given inputs
    //     float patch_size = configs["patch"]["patch_size"];
    //     float confluence = configs["agent"]["initial_confluence"];
    //     vector<int> cell_count_list = configs["agent"]["agents_initial_n"];
    //     int cell_count = cell_count_list[0] ; // TODO:: sum up the list

    //     float scale_factor = inputs::params()["scale_factor"];

    //     auto down_scaled_cell_count = cell_count * scale_factor;
    //     auto A = down_scaled_cell_count * patch_size * patch_size / confluence;
    //     auto domain_x_l = pow(A,0.5);
    //     // cout<<" down_scaled_cell_count: "<<down_scaled_cell_count<<" A:"<<A<<" domain_x_l:"<<domain_x_l<<endl;
    //     inputs::params().insert(pair<string,float>("domain_x_l",domain_x_l));
    //     // cout<<inputs::params()["initial_cellCount"]<<" "<<inputs::params()["domain_x_l"]<<endl;
    //     // exit(2);
    // };
    // PRE_CALCULATIONS();
    auto INITIALIZE_VARIABLES = [&](){
        // this->start_passaging_n = 1;
        // this->passaging_count = 1;
        // if (settings["configs"]["end_passaging_n"] != nullptr) this->end_passaging_n = settings["configs"]["end_passaging_n"];
        // else this->end_passaging_n = this->start_passaging_n;
        this->duration_ticks = py::cast<unsigned>(settings["configs"]["exp_duration"]);
    };
    INITIALIZE_VARIABLES();
    Patch<dim>::setup_patches(this->shared_from_this(), settings["configs"]);
    Cell<dim>::setup_cells(this->shared_from_this(), settings["configs"]);
    _update();
    return true;
}






template<unsigned dim>
inline void Model<dim>::run(){
    auto EXECUTE = [&](){
        visualize();
        // cout<<"visualized"<<endl;
        Patch<dim>::run(iCount);
        // cout<<"patch runned"<<endl;
        Cell<dim>::run(iCount);
        // cout<<"cell runned"<<endl;
        Cell<dim>::update();
        // cout<<"cell updated"<<endl;
        Patch<dim>::update();
        // cout<<"patch updated"<<endl;
    };
    auto TIME_CONTROL = [&](){
        
        // first passage the model before starting the actual run
        // while (pIter < inputs::params()["end_passaging_n"]){
        //     iCount = 1;
        //     // while (results_repos.back()._quantities_float[] < inputs::params()["confluence_t"]){
        //     //     try{
        //     //        EXECUTE(); 
        //     //    }catch(...){
        //     //         cerr<<"got a problem in execute"<<endl;
        //     //         exit(2);
        //     //    }
                
        //     //     iCount++;
        //     //     if (iCount >=tools::day_to_tick_convertor(inputs::params()["convergence_max_days"])) throw convergence_error("ABM cannot converge");
        //     // }
        //     _passaging();
        //     pIter++;
        // }
        // this is the actual run
        iCount = 1;
        while (iCount <= duration_ticks){
            cout<<"Iteration "<<std::to_string(iCount)<<endl;
            EXECUTE();
            iCount++;
        }

    };
    TIME_CONTROL();
    // return results_repos;

}


template <unsigned dim>
inline void Model<dim>::_passaging() {
    // auto REDISTRIBUTE_LIVE_CELLS = [&](){
    //     /** delete patches' cell occupancy **/
    //     for (auto &patch:Patch<dim>::container()) patch->removeCell();
    //     /** pick MSC_initial number among the entire cells **/
    //     vector<shared_ptr<Cell<dim>>> cell_tem_container;
    //     for (int cIter=0; cIter < inputs::params()["initial_cellCount"]; cIter++){
    //         auto total_cell_number = Cell<dim>::container().size();
    //         auto lucky_cell_index = tools::create_random_value(0,total_cell_number);
    //         auto lucky_cell =Cell<dim>::container()[lucky_cell_index];
    //         cell_tem_container.push_back(lucky_cell);
    //         Cell<dim>::container().erase(Cell<dim>::container().begin() + lucky_cell_index); //!< Delete the chosen cell to not choose it again
    //     }
    //     Cell<dim>::container() = cell_tem_container;
    //     /** distribute cells on the patches **/
    //     for (auto &cell:Cell<dim>::container()){
    //         shared_ptr<Patch<dim>> host_patch;
    //         try{
    //             host_patch = _find_an_empty_patch();
    //         }catch(no_available_patch& nap){
    //             throw nap;
    //         }
    //         host_patch->setCell(cell);   // cross-association
    //         cell->setPatch(host_patch);
    //     }
    //     /** renew all cell activities  **/
    //     for (auto &cell:Cell<dim>::container()) {
    //         cell->cell_activities->reset_all();
    //     }

    // };
    // REDISTRIBUTE_LIVE_CELLS();
    // auto REFRESH_MODEL = [&](){
    //     /** refresh the medium and results **/
    //     // _results->output_results(_eCount);
    //     // _results = make_shared<Result>();
    //     // _results->initialize();
    //     // append_flag = false;
    //     // iCount = 0; //start from the begining
    //     // _results->reset();
    //     // append_flag = true;
    //     // this->_medium_change();
    //     // this->_update();
    // };
    // REFRESH_MODEL();
    // auto message = "Passaging # "+ to_string(passaging_count)+ " finished";
    // LOG(message);
    // passaging_count++;

}


template<unsigned dim>
inline void Model<dim>::_update(){
    Cell<dim>::update();

    // create a result object for this episode
    // auto results = make_shared<Result>();
    // for (auto &cell:Cell<dim>::container()){
    //     results->agents_count[cell->c_type] ++; // if the tag is there, it increaments it. if not, it creates it and increaments it
    // }

    // // extract the accumulated values of the factors entered as patch attributes
    // for (auto &key:Patch<dim>::attr_tags()){
    //     float sum = 0;
    //     for (auto &patch:Patch<dim>::container()){
    //         float value = patch->attributes[key];
    //         sum += value;
    //     }
    //     results->data[key] = sum;
    // }
    // _cellTotalCount = Cell<dim>::container().size();

    // results_repos[iCount] = results;

    // json jj(results_repos);
    // cout<<jj<<endl;


    // if (iCount == results_repos.size()-1 

    // auto results = make_shared<Result>();

//     auto UPDATE = [&](){
//         LOG("---updating---");
        
//         /// update cell count for each cell type
//         for (auto cell:Cell<dim>::container()){
//             _results->cell_count[cell->c_type][iCount]++;
//         }

//         /// update over cell count
//         int sum = 0;
//         for (int celltype_i=0; celltype_i < enums::CELL_TYPE_COUNT; celltype_i++){
//             vector<int> value_list;
//             try {
//                 value_list = _results->cell_count[celltype_i];
//             }catch(...){
//                 cerr<<"Error: cell type "<<celltype_i<<" causes error in update"<<endl;
//                 exit(2);
//             }
//             int value;
//             try {
//                 value = value_list[iCount];
//             }catch(...){
//                 cerr<<"Error: iCount "<<iCount<<" cases error in update"<<endl;
//                 exit(2);
//             }
//             sum += value;
//         }
//         _cellTotalCount = sum;
//         // cout<<" update sum "<<sum<<endl;

//         auto _deadCellTotalCount = _results->cell_count[enums::deadcell_t][iCount];
//         auto _aliveCellCount = _cellTotalCount - _deadCellTotalCount;
//         _results->_quantities_int[enums::liveCellCount_q][iCount] = _aliveCellCount;
//         // cout<<"update: "<<"total "<<_cellTotalCount<<" alive "<<_aliveCellCount<<" dead "<<_deadCellTotalCount<<endl;
//         _results->_quantities_float[enums::viability_q][iCount] = (float)_aliveCellCount/_cellTotalCount;

//     };
//     auto APPEND = [&](){
// //        cout<<"---appending---"<<endl;
//         vector<int> counts(enums::CELL_TYPE_COUNT);
//         for (auto cell:Cell<dim>::container()){
//             counts[cell->c_type]++;
//         }
//         for (unsigned celltype_i=0; celltype_i<enums::CELL_TYPE_COUNT; celltype_i++){
//             _results->cell_count[celltype_i].push_back(counts[celltype_i]);
//         }
//         /// update over cell count
//         int sum = 0;
//         for (int celltype=0; celltype < enums::CELL_TYPE_COUNT; celltype++){
//             // if (celltype == enums::deadcell_t) continue;
//             sum += _results->cell_count[celltype][iCount];
//         }
//         // cout<<"append sum "<<sum<<endl;
//         _cellTotalCount = sum;
//         _results->_quantities_int[enums::liveCellCount_q].push_back(sum);
//         auto _deadCellTotalCount = _results->cell_count[enums::deadcell_t][iCount];
//         auto _aliveCellCount = _cellTotalCount - _deadCellTotalCount;
//         // cout<<"append: "<<"total "<<_cellTotalCount<<" alive "<<_aliveCellCount<<" dead "<<_deadCellTotalCount<<endl;
//         _results->_quantities_float[enums::viability_q].push_back((float)_aliveCellCount/_cellTotalCount);
//         append_flag = false;

//     };
//     try{

//         if (append_flag) APPEND();
//         else UPDATE();
//     }catch(...){
//         cerr<<"_update_cellCount is the shitty problem"<<endl;
//         exit(2);
//     }



 // float sum_ph = 0;
 //    float sum_ECM = 0;
 //    float sum_mineral = 0;

 //    for (auto patch:Patch<dim>::container()) {
 //        sum_ph+=patch->get_ph();
 //        sum_ECM+=patch->get_ECM();
 //        sum_mineral+=patch->get_mineral();
 //    }
 //    float mean_ph = sum_ph / _patchCount;
 //    float mean_ECM = sum_ECM / _patchCount; //!< mean ECM on patch
 //    float mean_mineral = sum_mineral / _patchCount; //!< mean mineral on patch
 //    _results->_quantities_float[enums::mean_ph_q].push_back(mean_ph);

// _confluence = 20; 
    // _confluence = (float)(_cellTotalCount)/_patchCount;
//    cout<<"*********** confluence "<<_confluence<<endl;

}



template<unsigned dim>
inline void Model<dim>::visualize() const {
    bool logs_flag = py::cast<bool>(settings["logs"]["flag"]);
    if (!logs_flag) return; // no visualization
    // /***  cell distriburion visualization ***/
    std::chrono::seconds dura( 2);
    std::this_thread::sleep_for( dura );
    Cell<dim>::visualize(iCount,settings["logs"]["agent"]);
    Patch<dim>::visualize(iCount,settings["logs"]["patch"]);
}


#endif //ABM_MODEL_H
