//
// Created by nourisaj on 5/24/19.
//

#ifndef ABM_MODEL_H
#define ABM_MODEL_H



#include "patch.h"
#include "agent.h"
#include <chrono>
#include <thread>
#include "settings.h"
#include <pybind11/stl.h>
namespace py = pybind11;
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
    Model(py::dict agent_modelObjs);
    ~Model(){};
    bool setup();        
    py::dict run();
    void reset();
    void  visualize() const; //!< To log requrested outputs for visualization
    static void initialize_patchmodel(py::object &patch_model){
        Patch<dim>::patch_model() = patch_model;
    };
    static std::map<std::string,std::vector<unsigned>>& agent_counts(){
        static std::map<std::string,std::vector<unsigned>> var{}; return var;
    };
    static domain_data_t& data(){
        static domain_data_t var{}; return var;
    }; // trajectory of domain results taged based on patch, self, etc.
    static settings_t & settings(){static settings_t var{}; return var;};
    
    std::map<string,py::object>  agent_models;
protected:
    bool append_flag=false;
    void _update();
    void _passaging(); 
    unsigned iCount;            //!< Iteration index of the simulation incremented in #run.
    unsigned end_passaging_n;   //!< Count of passaging occured in the model
    unsigned start_passaging_n;
    unsigned passaging_count;
    unsigned duration_ticks;    //!< experiment duration
    
    domain_measurements_scheme_t measurements_scheme; //! the scheme for obtaining #data

};
template<unsigned dim>
void Model<dim>::reset(){
    agent_counts().clear();
    data().clear();
    Patch<dim>::container().clear();
    Patch<dim>::configs().clear();
    Cell<dim>::container().clear();
    
    
}
template<unsigned dim>
inline Model<dim>::Model(py::dict agent_modelObjs):
        iCount(0)
{     
    srand(time(0));
   auto keys = agent_modelObjs.attr("keys")();
   for (auto &key:keys) {
       py::object agent_model = agent_modelObjs[key];
       std::string agent_type = key.cast<string>();
       agent_models.insert(std::pair<std::string,py::object>(agent_type,agent_model));
   }
}

template<unsigned dim>
inline bool Model<dim>::setup(){
    tools::create_directory(main_output_folder);
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
        this->duration_ticks = py::cast<unsigned>(settings()["configs"]["exp_duration"]);

        auto PROCESS_DOMAIN_MEASUREMENTS_SCHEME = [&](){
            auto measurements = settings()["configs"]["domain"]["measurements"];
            map<string,variant<string,vector<string>>> m_item;
            for (auto &m_item_:measurements){
                m_item.clear();
                for (auto &tag_:m_item_.attr("keys")()){
                    string tag = py::cast<string>(tag_);
                    if (tag == "on" or tag == "mode") m_item[tag] = py::cast<string>(m_item_[tag_]);
                    else if (tag == "tags") m_item[tag] = py::cast<vector<string>>(m_item_[tag_]);
                    else {
                        std::cerr<<RED<<"Error: "<<RESET<<" input name '"<<RED<<tag<<RESET<<"' is not acceptable for domain measurements inputs."<<endl;
                        std::terminate();
                    }
                }
                this->measurements_scheme.push_back(m_item);
            }
        };
        PROCESS_DOMAIN_MEASUREMENTS_SCHEME();   
    };
    INITIALIZE_VARIABLES();
    Patch<dim>::setup_patches(this->shared_from_this(), settings()["configs"]);
    Cell<dim>::setup_cells(this->shared_from_this(), settings()["configs"]);
    this->append_flag = true;
    this->_update();
    return true;
}






template<unsigned dim>
inline py::dict Model<dim>::run(){
    auto EXECUTE = [&](){
        this->append_flag = true;
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
        _update();
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
        while (iCount < duration_ticks) {
            int barWidth = 70;
            std::cout << "[";
            int pos = barWidth * (float)iCount/duration_ticks;
            for (int i = 0; i < barWidth; ++i) {
                if (i < pos) std::cout << "=";
                else if (i == pos) std::cout << ">";
                else std::cout << " ";
            }
            std::cout << "] " << iCount << " Iterations\r";
            std::cout.flush();

            EXECUTE();
            iCount++;
        }
        std::cout<<endl;

    };
    _clock::start();
    TIME_CONTROL();
    
    // returns
    py::dict returns;
    for (auto &key_:settings()["returns"].attr("keys")()){
        string key = py::cast<string>(key_);
        if (key == "agents_count"){
            vector<unsigned> time_points = py::cast<vector<unsigned>>(settings()["returns"][key_]);
            auto sliced_data = tools::slice_map(agent_counts(),time_points);
            py::dict returns1 = py::cast(sliced_data);
            returns[key_] = returns1;
        }
//        cout<<key<<endl;
    };
    
    _clock::end();
    cout<<"Run completed"<<endl;
    return returns;
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
    auto UPDATE_AGENT_COUNT = [&](){
        map<string,unsigned> agent_counts_single;
        for (unsigned i = 0; i < Cell<dim>::container().size(); i++) {
            auto cell =  Cell<dim>::container()[i];
            agent_counts_single[cell->c_type]++;
        }
        for (auto &agent_model:agent_models){
            auto agent_tag = agent_model.first;
            if (agent_counts_single.find(agent_tag) == agent_counts_single.end()){ // this agent doesnt have any record in container
                agent_counts_single[agent_tag] = 0;
            }
        }
        for (auto &item:agent_counts_single){
            auto tag = item.first;
            auto value = item.second;
            if (this->agent_counts().find(tag) == this->agent_counts().end()){
                this->agent_counts()[tag] = vector<unsigned>();
            }
            if (this->append_flag == true) this->agent_counts()[tag].push_back(value);
            else this->agent_counts()[tag].back() = value;
        }
    };
    UPDATE_AGENT_COUNT();

    auto UPDATE_DOMAIN_DATA = [&](){
        auto UPDATE_DOMAIN_PATCH_DATA = [&](auto &m_item){
            auto tags = std::get<vector<string>>(m_item["tags"]);
            auto mode = std::get<string>(m_item["mode"]);
            for (const auto &tag:tags){
                float sum = 0;
                for (auto &patch:Patch<dim>::container()){
                    sum+=patch->data[tag];
                }
                float result = 0;
                if (mode == "sum") result = sum;
                else if (mode == "mean") result = sum/Patch<dim>::container().size();
                else {
                    std::cerr<<RED<<"Error: "<<RESET<<" mode entered as '"<<RED<<mode<<RESET<<"' is not acceptable."<<endl;
                    std::terminate();
                }
                if (this->append_flag == true) this->data()["patch"][tag].push_back(result);
                else this->data()["patch"][tag].back() = result;
            }      
        };
        auto UPDATE_DOMAIN_AGENT_DATA = [&](auto &m_item){
            auto tags = std::get<vector<string>>(m_item["tags"]);
            auto mode = std::get<string>(m_item["mode"]);
            for (const auto &tag:tags){
                float sum = 0;
                for (auto &agent:Cell<dim>::container()){
                    sum+=agent->data[tag];
                }
                float result = 0;
                if (mode == "sum") result = sum;
                else if (mode == "mean") result = sum/Cell<dim>::container().size();
                else {
                    std::cerr<<RED<<"Error: "<<RESET<<" mode entered as '"<<RED<<mode<<RESET<<"' is not acceptable."<<endl;
                    std::terminate();
                }
                if (this->append_flag == true) this->data()["agent"][tag].push_back(result);
                else this->data()["agent"][tag].back() = result;
            }      
        };
        for ( auto& m_item: this->measurements_scheme){
            auto on = std::get<string>(m_item["on"]);
            if (on == "patch") UPDATE_DOMAIN_PATCH_DATA(m_item);
            else if (on == "agent") UPDATE_DOMAIN_AGENT_DATA(m_item);
            else {
                std::cerr<<RED<<"Error: "<<RESET<<" the entered '"<<RED<<on<<RESET<<"' as domain measurements on is neither patch or agent."<<endl;
                std::terminate();
            }
        }        
    };
    UPDATE_DOMAIN_DATA();
    this->append_flag = false;

}



template<unsigned dim>
inline void Model<dim>::visualize() const {
    bool logs_flag = py::cast<bool>(settings()["logs"]["flag"]);
    if (!logs_flag) return; // no visualization
    // /***  cell distriburion visualization ***/
    // std::chrono::seconds dura( 2);
    // std::this_thread::sleep_for( dura );
    Cell<dim>::visualize(iCount,settings()["logs"]);
    Patch<dim>::visualize(iCount,settings()["logs"]);
}


#endif //ABM_MODEL_H
