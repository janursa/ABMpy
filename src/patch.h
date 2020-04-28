
#pragma once
#include <map>
#include <string>
#include "toolbox.h"
#include "model.h"

#include "mesh.h"
using namespace std;
template <unsigned dim>
class Model;
template <unsigned dim>
class Cell;
//!  Patch class
/*!
  A more elaborate class description.
*/
template<unsigned dim>
class Patch {
public:


    Patch();                  //!< Patch constructor
    ~Patch(){};

    static void setup_patches(std::shared_ptr<Model<dim>> modelPtr, settings_t  configs);
    static void run(unsigned const &iCount); 
    static void visualize(const unsigned &, const settings_t &);
    static void update();

    static vector<shared_ptr<Patch<dim>>>& container(){static vector<shared_ptr<Patch<dim>>> var{}; return var;}

    static shared_ptr<Patch<dim>>& find_an_empty_patch();
    static shared_ptr<Patch<dim>> find_an_empty_patch(shared_ptr<Patch<dim>>& refPatch);
    static std::shared_ptr<Model<dim>> get_modelPtr();              //!< Returns #_modelPtr
    static settings_t& configs(){ static settings_t var{}; return var;};


    void removeCell();        //!< Removes cell from patch
    void initialize();             //!< initialize patch variables
    bool hasCell() const;
    void setCell(shared_ptr<Cell<dim>> cellPtr);
    shared_ptr<Cell<dim>> getCell();
    vector<std::shared_ptr<Patch<dim>>> neighborPatches;
    std::map<std::string,float> data; 
    vector<float> xyzcoords; 
    unsigned patchIndex;

private:
    static weak_ptr<Model<dim>>& _modelPtr(){static  weak_ptr<Model<dim>> var{}; return var;};
    weak_ptr<Cell<dim>> _cellPtr;
    bool _hasCell;

};
template<unsigned dim>
inline shared_ptr<Patch<dim>> Patch<dim>::find_an_empty_patch(shared_ptr<Patch<dim>>& refPatch){
    auto neighborPatches = refPatch->neighborPatches;
    refPatch.reset(); //pointer to refPatch is no longer needed
    vector<shared_ptr<Patch<dim>>> free_patch_container; //available neighbor patches will be stored here
    for (auto neighborPatch:neighborPatches){
        if (!neighborPatch->hasCell()) free_patch_container.push_back(neighborPatch);
    }

    int freeNeighborsCount = free_patch_container.size();
    for (int iter = 0; iter<freeNeighborsCount; iter++){
        int randn = rand()%freeNeighborsCount;
        auto freeAvailablePatch = free_patch_container[randn];
        return freeAvailablePatch;
    }
    throw no_available_patch();
}

template<unsigned dim>
inline void Patch<dim>::run(unsigned const & iCount){
    // for (auto &patch:Patch<dim>::container()) patch->function();

    // medium change
    auto MEDIUM_CHANGE = [&]() {
        LOG("changing medium");
        for (auto &patch : Patch<dim>::container()) {
            patch->initialize();
        }
    };
    // if ((iCount%(int)inputs::params()["medium_change_interval"])!=0) return;
    // else MEDIUM_CHANGE();

}

template<unsigned dim>
inline void Patch<dim>::visualize(const unsigned &iCount,const settings_t &specs){
    auto DENSITYMAP_LOG = [&](auto &item){
        string dir = py::cast<string>(item["dir"]);
        ofstream O(dir);
        // header
        vector<string> tags = py::cast<vector<string>>(item["tags"]);
        O<<"x,y";
        for (auto &tag:tags){
            O<<","<<tag;
        }
        O<<"\n";        
        for (auto &patch:container()){ 
            float x = patch->xyzcoords[0];
            float y = patch->xyzcoords[1];
            O<<x<<","<<y;
            for (auto &tag:tags){
                float value;
                try {value = patch->data.at(tag);}
                catch(out_of_range &er){
                    cerr<<"Error: '"<<tag<<"' requested for visualization is not defined in patch attributes"<<endl;
                    std::terminate();
                }
                O<<","<<value;
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
                if (Model<dim>::data()["patch"].find(tag) == Model<dim>::data()["patch"].end()){
                    std::cerr<<RED<<"Error: "<<RESET<<" requested tag '"<<RED<<tag<<RESET<<"' for patch log is not defined in the domain variables."<<endl;
                    std::terminate();
                }
                else{
                    auto value = Model<dim>::data()["patch"][tag][iter];
                    if (ii == 0) O << value;
                    else O<<","<<value;
                    ii++;
                }
                
            }
            O<<"\n"; 
        }
        

        O.close();
    };

    for (auto &item:specs["patch"]){
        bool item_logs_flag = py::cast<bool>(item["flag"]);
        if (item_logs_flag == false) continue;
        unsigned interval = py::cast<unsigned>(item["interval"]);
        if (iCount != 0) {
            if ( iCount % interval != 0) continue;  // interval is not met
        }
        string type = py::cast<string>(item["type"]);
        if (type == "densitymap"){
            DENSITYMAP_LOG(item);
        }
        else if (type == "traj"){
            TRAJ_LOG(item);
        }
        else {
            std::cerr<<RED<<"Error: "<<RESET<<" type of log entered as '"<<RED<<type<<RESET<<"' is not acceptable."<<endl;
            std::terminate();
        }
    }
      

}
template<unsigned dim>
inline shared_ptr<Patch<dim>>& Patch<dim>::find_an_empty_patch(){
    int randIndex;
    for(int iter=0; iter<container().size(); iter++){
        randIndex =rand() %container().size();
        if (container()[randIndex]->hasCell()) {
            continue;
        }
        else return Patch<dim>::container()[randIndex];

    }
    throw no_available_patch(); //in case there is no empty patch available
}
template<unsigned dim>
inline Patch<dim>::Patch(){
    _hasCell = false;
}
template <unsigned dim>
inline void Patch<dim>::initialize(){
    for (auto key:configs()["patch"]["attrs"].attr("keys")()) {
          string name =key.cast<string>();
          float value =configs()["patch"]["attrs"][key].cast<float>();
          this->data.insert(std::pair<string,float>(name,value));
    }
}
template<unsigned dim>
inline void Patch<dim>::setCell(shared_ptr<Cell<dim>> cellPtr){
    _cellPtr=weak_ptr<Cell<dim>>(cellPtr);
    _hasCell = true;
}
template <unsigned dim>
void Patch<dim>::removeCell(){
    _hasCell = false;
}
template<unsigned dim>
inline void Patch<dim>::update(){
    auto CALCULATE_AGENTDENSITY = [&](){
        for (auto &patch:container()){
            int cellCount=0;
            for (auto neighborPatch:patch->neighborPatches){
                if (neighborPatch->_hasCell) {
                    cellCount ++;
                }
            }
            if (patch->_hasCell) {
                cellCount ++;
            }
            patch->data["agent_density"] = float(cellCount)/(patch->neighborPatches.size()+1);
        }
    };
    CALCULATE_AGENTDENSITY();
}
template<unsigned dim>
inline shared_ptr<Cell<dim>> Patch<dim>::getCell(){
    shared_ptr<Cell<dim>> p = _cellPtr.lock();
    if (!p) {
        throw logic_error("Weak_ptr (_cellPtr inside patch) expired");
    }
    return p;
}
template <unsigned dim>
inline void Patch<dim>::setup_patches(std::shared_ptr<Model<dim>> modelPtr, settings_t configs_){
    _modelPtr() = weak_ptr<Model<dim>>(modelPtr);
    configs() = configs_;
    float x_l = configs()["domain"]["x_l"].cast<float>();
    float y_l = configs()["domain"]["y_l"].cast<float>(); 
    Mesh::setup_meshes(x_l,y_l, configs()["domain"]["patch_size"].cast<float>());
    /*** define patches on grids ***/
    for (int iter=0; iter<Mesh::meshes().size(); iter++)
    {
        auto mesh = Mesh::at(iter);
        auto pPtr = make_shared<Patch<dim>>();
        pPtr->initialize();
        pPtr->patchIndex = mesh->get_meshindex(); // relating mesh index to associated patch index

        auto coords = mesh->get_globalxyzcoordinates();
        pPtr->xyzcoords= coords;
        container().push_back(pPtr);

    }

    for (int iter=0; iter<Mesh::meshes().size(); iter++){
        auto mesh = Mesh::at(iter);
        auto neighbors = mesh->neighbor_indices; // relating mesh neighbors to associated patch neighbors
        auto patch = container()[iter];
        for (const auto &neighbor:neighbors){
            auto neighborPatch = container()[neighbor];
            patch->neighborPatches.push_back(neighborPatch);
        }
    }

}
template<unsigned dim>
inline std::shared_ptr<Model<dim>>  Patch<dim>::get_modelPtr(){
    shared_ptr<Model<dim>> p = _modelPtr().lock();
    if (!p){
        throw logic_error("Weak_ptr (_modelPtr inside patch) expired)");
    }
    return p;
}

template<unsigned dim>
inline bool Patch<dim>::hasCell() const{
    return _hasCell;
}

