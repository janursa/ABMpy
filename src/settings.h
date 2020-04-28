//
// Created by nourisaj on 8/7/19.
//
// directives //*
#pragma once
#include <variant>
#include <any>
#include <string>
#include <pybind11/pybind11.h>
#include "cpp_logs.h"
namespace py = pybind11;


#define DEBUG



// *** constants ***// 
#define _unused(x) ((void)(x))
const unsigned DIM = 2; 
#ifdef DEBUG
#define LOG(str) do { std::cout << str << std::endl; } while( false )
#else
#define LOG(str) do { } while ( false )
#endif

using domain_data_t = std::map<std::string,std::map<std::string,std::vector<float>>>;
using domain_measurements_scheme_t = std::vector<std::map<std::string,std::variant<std::string,std::vector<std::string>>>>;
using settings_t = py::dict;

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

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */