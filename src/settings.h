//
// Created by nourisaj on 8/7/19.
//
// directives //*
#pragma once
#include <variant>
#include <any>
#include <string>
#include <pybind11/pybind11.h>
namespace py = pybind11;

#define MIGRATION
//#define AUTO_REMOVAL_DEADCELLS
#define PYTHON
#define DEBUG



// *** constants ***// 
#define _unused(x) ((void)(x))
const unsigned DIM = 2; 
#ifdef DEBUG
#define LOG(str) do { std::cout << str << std::endl; } while( false )
#else
#define LOG(str) do { } while ( false )
#endif


using input_t = std::map<std::string,std::variant<bool,int,double,std::string>>; //!< inputs of controllers
using settings_t = py::dict;
using output_t = std::map<std::string,std::map<std::string,float>>;             //!< outputs of controllers

//** types **/
template <typename T>
using matrix                             =  std::vector<std::vector<T>>;       //!< 2nd order vector containing T
template <typename T>
using matrix3                             =  std::vector<std::vector<std::vector<T>>>;       //!< 2nd order vector containing T


//** directories **//

const std::string root_dir = "/Users/matin/Downloads/testProjs/ABM/";
const std::string settings_dir= root_dir + "settings.json";
const std::string param_dir = root_dir+"parameters";
const std::string scheme_dir = root_dir+ "schemes/scheme_helvia.json";


const std::string main_output_folder = "outputs/";
const std::string NN_output_folder = main_output_folder;
const std::string cell_dist_file = main_output_folder + "cell_dist.csv";
const std::string cell_count_file = main_output_folder + "cell_count.csv";
const std::string cell_dist_output_folder = main_output_folder+"cell_dist/";           //!< dir of folder to write cell dist into
const std::string cell_dist_output_file =  cell_dist_output_folder+ "cell_dist";


struct invalid_directory{
    invalid_directory(std::string msg):message(msg){}
    std::string message;
};
struct convergence_error{
    convergence_error(std::string msg):message(msg){}
    std::string message;
    const char *what() const throw() {
        return message.c_str();
    }
};

struct no_available_patch{
	no_available_patch(){}
};