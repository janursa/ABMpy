
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

    static void setup_patches(std::shared_ptr<Model<dim>> modelPtr, json & configs, float downscale_factor);
    static void run(unsigned const &iCount); 
    static void visualize(const unsigned &, const json &);
    static void update();

    static vector<shared_ptr<Patch<dim>>>& container(){static vector<shared_ptr<Patch<dim>>> var{}; return var;}

    static shared_ptr<Patch<dim>>& find_an_empty_patch();
    static shared_ptr<Patch<dim>> find_an_empty_patch(shared_ptr<Patch<dim>>& refPatch);
    static std::shared_ptr<Model<dim>> get_modelPtr();              //!< Returns #_modelPtr
    static json& configs(){ static json var{}; return var;};


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
inline void Patch<dim>::visualize(const unsigned &iCount,const json &specs){
      if (specs["flag"] == false) return;
      unsigned interval = specs["interval"];
      if (iCount != 0) {
        if (iCount % interval != 0) return;  // interval is not met
      }
      string dir = specs["dir"];
      ofstream O(dir);
      // header
      vector<string> tags = specs["tags"];
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
    for (auto &item:configs()["attrs"]) {
        for (auto& el : item.items()) {
          string name = el.key();
          float value = el.value();
          // cout<<name<<" "<<value<<endl;
          this->data.insert(std::pair<string,float>(name,value));
        }
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
inline void Patch<dim>::setup_patches(std::shared_ptr<Model<dim>> modelPtr, json & configs_,float downscale_factor){
    _modelPtr() = weak_ptr<Model<dim>>(modelPtr);
    configs() = configs_;
    float x_l = configs()["x_l"]; x_l = x_l/downscale_factor;
    float y_l = configs()["y_l"]; y_l = y_l/downscale_factor;
    Mesh::setup_meshes(x_l,y_l, configs()["patch_size"]);
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

