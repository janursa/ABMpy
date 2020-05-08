
#pragma once
#include <iostream>
#include <variant>
#include <string>
#include <pybind11/pybind11.h>
namespace py = pybind11;


#define DEBUG


#include <chrono>
struct _clock {
    void static start(){
        _clock::_begin() = std::chrono::steady_clock::now();
    }
    void static end(){
        _clock::_end() = std::chrono::steady_clock::now();
        std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(_end() - _begin()).count() << "[s]" << std::endl;
    }
    static std::chrono::steady_clock::time_point& _begin(){
        static std::chrono::steady_clock::time_point var{};
        return var;
    };
    static std::chrono::steady_clock::time_point& _end(){
        static std::chrono::steady_clock::time_point var{};
        return var;
    };

};



// *** constants ***// 
const unsigned DIM = 2; 
#ifdef DEBUG
#define LOG(str) do { std::cout << str << std::endl; } while( false )
#else
#define LOG(str) do { } while ( false )
#endif

using domain_data_t = std::map<std::string,std::map<std::string,std::vector<float>>>;
using domain_measurements_scheme_t = std::vector<std::map<std::string,std::variant<std::string,std::vector<std::string>>>>;
using settings_t = py::dict;

//** directories **//

const std::string main_output_folder = "outputs/";

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