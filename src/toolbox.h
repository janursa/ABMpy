#pragma once

#include "settings.h"
#include <fstream>
#include <random>

using namespace std;



namespace tools{
    string      concatenate(const std::string c_part, const string n_part); //!< Adds the given number to the given string by a _
    float       normalize(const float &value, const float&max, const float& min=0); //!< Normalize value based on the given max and min values
    vector<double>  nomalize_vector( vector<float> &vector); //!< normalize the items of the given vector
    float       str_to_num(const string );     //!<
    void        remove_space( string&);         //!< Removes space from a string
    // int         day_to_tick_convertor(float day);  //!< Converts day to ticks

    // int         tick_to_day_convertor(int tick);  //!< Converts ticks to day
    // void        day_to_tick_convertor_array(vector<int>& arrayValue);  //!< Converts ticks to day for an array of values
    void        ltrim(std::string &s); //!<removes space from left side of a string
    void        rtrim(std::string &s); //!<removes space from righ side of a string
    void        trim(std::string &s); //!< removes space from both sides
    template<typename T>
    float       mean(vector<T> &); //!<returns average value of a vector of values
    void        create_directory(string); //!< Receives a folder address and creates the directory
    float       create_random_value(float, float);         //!< Creates an stochastic value between two given magnitudes
    template <typename T>
    void        log_vector(vector<T> &vec); //!< Logs the given vector to terminal
}

template <typename T>
inline void  tools::log_vector(vector<T> &vec){
    for (const auto &item:vec){
        cout<<item<<" ";
    }
    cout<<endl;
}



inline string tools::concatenate(const std::string c_part, const string n_part){
    return c_part +"_"+n_part;
}


inline float tools::normalize(const float &value, const float&max, const float& min  ){
    float nominator_range = value - min;
    float denominator_range = max - min;
    float norm_value = nominator_range/denominator_range;
    (norm_value > 1)?(norm_value=1):(true);
    return norm_value;
}

inline float tools::str_to_num(const string str){

    return std::atof(str.c_str());


}

inline void tools::remove_space( string &str){
    std::string::iterator end_pos = std::remove(str.begin(),str.end(),' ');
    str.erase(end_pos, str.end());
}

inline void tools::ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c);}));
}

inline void tools::rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](int c) {return !std::isspace(c);}).base(), s.end());
}

inline void tools::trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}


inline void tools::create_directory(string folder){
    auto opts = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
    mkdir(folder.c_str(), opts);
}

inline float tools::create_random_value(float min, float max){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min,max);
    return dis(gen);

}



inline vector<double>  tools::nomalize_vector(vector<float> &vec){
    float sum = accumulate(vec.begin(),vec.end(),0.0);
    vector<double> normalized_vec(vec.size(),0);
    std::transform(vec.begin(), vec.end(), normalized_vec.begin(),std::bind(std::divides<float>(),std::placeholders::_1,sum));
    return normalized_vec;
}

