#pragma once
//#include "base.h"
#include <cstdio>
#include <sys/stat.h>
// #include "beta_distribution.h"
#include "settings.h"
#include <numeric>
#include <fstream>
#include <random>
#include <iomanip>
//#include <gsl/gsl_sf_bessel.h>
#include <pybind11/embed.h> // everything needed for embedding
// #include <pybind11/stl.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
namespace py = pybind11;
// using namespace pybind11::literals;
using namespace std;

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
    float       max(matrix<T> &); //!<returns max value of a vector of values
    template<typename T>
    float       max(vector<T> &); //!<returns max value of a vector of values
    template<typename T>
    float       mean(matrix<T> &); //!<returns average value of a vector of values
    template<typename T>
    float       mean(vector<T> &); //!<returns average value of a vector of values
    void        create_directory(string); //!< Receives a folder address and creates the directory
    // void        remove_directory(string); //!< Receives a folder address and removes the directory
    float       create_random_value(float, float);         //!< Creates an stochastic value between two given magnitudes
    float       pick_random_from_trun_norm_dist(float mean_value, float std_value, float lower_bound, float upper_bound); //!< Creates a random number based on a truncated normal distribution
    void        truncate(float &value, float lower_bound, float upper_bound); //!< Truncates value to be between 0-1
    float       find_in_range(float &lower_bound, float &upper_bound, float &ration); //!< calculates the value that is located between two bounds for the given ration
    float       vector_distance(vector<float> &vec1, vector<float> &vec2); //!< Calculates the distance between two vector
    template <typename T>
    void        log_vector(vector<T> &vec); //!< Logs the given vector to terminal
    template <typename T>
    void        log_matrix(matrix<T> &matr); //!< Logs the given matrix to terminal
    template <typename T>
    void        log_matrix3(matrix3<T> &matr); //!< Logs the given matrix to terminal
    vector<float> dirichlet_dist(vector<float> & alphas); //!< Receives alpha coefficients and returns a vector with dirichlet distribution
    float       pick_random_from_discrete_dist(vector<float> &weights); //!< Picks a random number from a discrete distribution using the given weights
    matrix<float> average_over_matrices(vector<matrix<float>> &); //!< Receives a vector of matrices and returns the average matrix
    vector<float> average_over_vectors(vector<vector<float>> &); //!< Receives a vector of vectors and returns the average vector
    void command_python(string scriptname, vector<string> args); //!< runs the given python script with the given arguments
    float product_vector(vector<float> &v1, vector<float>& v2); //!< product two vectors
    vector<float> multiply(vector<float> & v, float scalar); //!< multiplies a vector by a scalar
    vector<float> product(matrix<float> &m, vector<float>&v); //!< products of matrix to a vector
    float pick_random_from_normal_distribution(float mean, float var); //!< generates a random number from guassian dist
    vector<float> slice(vector<float> &v, vector<int> &indices); //!< slice the given vector based on the given indices
    vector<float> sum(vector<float> &v1, vector<float>& v2); //!< sums two vectors
    vector<float> subtract (vector<float> &v1, vector<float> &v2); //!< subtracts the second vector from the first one
    vector<float> normalize_vector_by_vector(vector<float> &v1, vector<float> &v2); //!< normalizes v1 based on the values of v2
    vector<float> sum_over_vectors(matrix<float> &M); //!< receives a matrix and sumes on each vector
    vector<float> concatenate_vectors(vector<float> &v1, vector<float> &v2); //!< concatenate v1 with v2
    vector<float> dot_product_vector(vector<float> &v1, vector<float>& v2); //!< dot product two vectors
    template<typename T>
    void print_to_file(vector<T> & data, string filename,string param_tag); //!< receives data in a vector format and writes it down to a file with the given name
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args);
    vector<vector<float>> Latin_hypercube_sampling(int sample_n,int sample_factor); /// generates LHS using python
    // void plot_fitness(vector<float> fitness_values, string folder_to_write); //!<plots fitness distribution using embedded python
    // void plot_distribution(vector<float> distribution,string tag, string dir); //!< plot distribution using embedded python
    json operation(vector<json> list, string operation_type, vector<int> &  int_list); //!< does operation on the list of json file
    float calculate_percentage_total_sum_of_squares(vector<int> &lower_bound_indices,vector<float>& results_array); //!< calculates percentafe of total sum of squares for each parameter 
    void params_to_file(map<string,float> &data,string &file_name, bool beauty); //!< Receivs a parameters type and writes it to the given file
    map<string,float> file_to_params(const string &file_name); //!< receivs a file dir, process it and converts it to ABM parameters inputs
    void json_to_file(json & jj, string file_name); //!< Receives a data in json format and writes it to the given file dir
    json file_to_json(string file_name);        //!< Recives a file dir, reads it and converts it to json format

    template<typename Key, typename Value>
    std::ostream& operator<<(std::ostream& os, const std::pair<const Key, Value>& p)
    {
        os << p.first << " => " << p.second;
        return os;
    }

    template<typename Container>
    void log_map(int level, const Container& c) {
        if (level >= 0) {
            for(typename Container::const_iterator it = c.begin();
                it != c.end(); ++it)
            std::cout << *it << '\n';
        }
    }
}

// inline void tools::remove_directory(string dir){
//     // cout<<"deletingg "<<endl;
//     // boost::filesystem::remove_all(dir);
// }
inline void tools::json_to_file(json & jj, string file_name){
    std::ofstream O;
    O.open(file_name, std::ofstream::out | std::ofstream::trunc);
    try{
        O<<jj;

    }catch(...){
        cerr<<"error in json_to_file"<<endl;
        terminate();
    }
    O.close();
}
inline json tools::file_to_json(string file_name){
    std::ifstream i(file_name);
    json jj;
    try{
        i>>jj;

    }catch(...){
        cerr<<"error in file_to_json"<<endl;
        cerr<<"couldn't read "<<file_name<<endl;
        terminate();
    }

    i.close();
    return jj;
}

inline map<string,float> tools::file_to_params(const string &file_name){


    auto dir = file_name + ".json";
    ifstream I(dir);
    json j;
    try{
        I >> j;

    }catch(...){
        cerr<<"error in file_to_params"<<endl;
        cerr<<"couldn't read "<<file_name<<endl;
        terminate();
    }

    map<string,float> params = j.get<map<string,float>>();
    return params;
}
inline void tools::params_to_file(map<string,float> &data,string &file_name,bool beauty){
    json jj (data);
    auto dir = file_name + ".json";
    ofstream O(dir);
    if (beauty == true) O<<setw(4)<<jj;
    else O<<jj;
    O.close();

}
inline json tools::operation(vector<json> list, string operation_type, vector<int> & int_list){
        if (list.size() == 0){
            return nullptr;
        }
        json scooped_results; // to store tag: value for the combined results
        json candidate = list[0];
        // cout<<setw(4)<<candidate<<endl;
        vector<float> average_per_ID; // just for fitness case
        for (auto &set_ID_:candidate["IDs"]){
            // cout<<"ID "<<set_ID_<<endl;
            // average_per_ID.clear(); // just for fitness case
            vector<float> average_tags; // just for fitness case
            string set_ID = set_ID_.get<string>(); // obtain the id of this set
            json set_results; // to store tag: value for the combined results
            for (auto &tag_:candidate[set_ID]["results"]["tags"]){
                string tag = tag_.get<string>();
                auto days = candidate[set_ID]["results"][tag]["days"];
                json tag_results; // to store day: value results for the combined results
                vector<float> days_results; //// just for fitness case
                for (auto &day_:days){ // just for fitness case
                    string day = day_.get<string>();
                    vector<float> day_values;
                    for (auto &item:list){
                        auto value = item[set_ID]["results"][tag][day];
                        day_values.push_back(value);
                    }
                    if (operation_type=="sum"){
                        float sum_value = 0;
                        for (auto &val:day_values) sum_value+=val;
                        tag_results[day] = sum_value;
                    }
                    else if (operation_type == "mean"){ // for replicas
                        float sum_value = 0;
                        for (auto &val:day_values) sum_value+=val;
                        float mean = sum_value / day_values.size();
                        tag_results[day] = mean;
                    }
                    else if (operation_type == "norm_abs_diff"){ // to calculate normalized absolute different.
                        // notice that list has only 2 items
                        auto exp_result = day_values[0];
                        auto sim_result = day_values[1];

                        auto difference = abs(sim_result - exp_result);
                        auto error = difference / exp_result;
                        // cout<<" sim "<<sim_result<<" exp "<< exp_result<<" error "<<error<<endl;
                        tag_results[day] = error;
                        // exit(2);
                    }
                    else if (operation_type == "norm_diff"){ // to calculate normalized  difference.
                        // notice that list has only 2 items
                        auto exp_result = day_values[0];
                        auto sim_result = day_values[1];

                        auto difference = (sim_result - exp_result);
                        auto error = difference / exp_result;
                        // cout<<" sim "<<sim_result<<" exp "<< exp_result<<" error "<<error<<endl;
                        tag_results[day] = error;
                        // exit(2);
                    }
                    else if (operation_type == "average_of_scheme"){ // averages over items of one scheme
                        days_results.push_back(day_values[0]);
                    }
                    else if (operation_type == "accumulate"){
                        tag_results[day] = day_values;
                    }
                    else if (operation_type == "PTSS"){
                        float PTSS = tools::calculate_percentage_total_sum_of_squares(int_list,day_values);
                        tag_results[day] = PTSS;
                    } else {
                        cerr<<"wait a moment, the operaton type: "<<operation_type<<" is not defined"<<endl;
                        exit(2);
                    }


                }
                // cout<<"days_results "<<tag<<endl;
                // tools::log_vector(days_results);
                average_tags.push_back(tools::mean(days_results));


                tag_results["days"] = days;
                set_results[tag] = tag_results;
                // cout<<tag<<tag_results<<endl;
            }
            // tools::log_vector(average_tags);
            average_per_ID.push_back(tools::mean(average_tags));
            // cout<<"average_per_tag"<<endl;
            // tools::log_vector(average_per_tag);
            set_results["tags"] = candidate[set_ID]["results"]["tags"];
            json hold;
            hold["results"] = set_results;
            scooped_results[set_ID] = hold;

        }
        // tools::log_vector(average_per_ID);
        auto average_fitness = tools::mean(average_per_ID);
        // cout<<"average_fitness "<<average_fitness<<endl;
        if (operation_type == "average_of_scheme"){
            scooped_results = nullptr;
            scooped_results["average_of_scheme"] = average_fitness;
        }else{
            scooped_results["IDs"] = candidate["IDs"];
        }


        return scooped_results;
};
inline float tools::calculate_percentage_total_sum_of_squares(vector<int> & lower_bound_indices,vector<float>& results_array){
    int total_iteration_n = results_array.size();
    vector<float> all_replicas;
    float mean_of_replocas;  //mean value of replicas
    int index;

    auto overall_mean = tools::mean(results_array); // The mean output
    float TTS = 0; //total sum of squares TSS
    for (const auto&item:results_array){
        TTS+= std::pow(item-overall_mean,2); //calculates TSS
    }

    float sum_lower_bound = 0; // the sum of outputs where the parameter holds its lower bound value
    float sum_upper_bound = 0;

    int count_lower_bound = 0; //counts the occurrence of the parameter in its lower bound value
    int count_upper_bound = 0;
    for (int Iter =0; Iter<results_array.size(); Iter++){
        if (lower_bound_indices[Iter] == -1){ // The parameter holds its lower bound value
            sum_lower_bound += results_array[Iter];
            count_lower_bound++;
            continue;
        }
        if (lower_bound_indices[Iter] == 1){ // The parameter holds it upper bound
            sum_upper_bound += results_array[Iter];
            count_upper_bound++;
            continue;
        }
        cout<<" value of lower index: "<<lower_bound_indices[Iter] <<endl;
        throw std::invalid_argument("Value of lower bound indices must be either -1 or 1");
    }

    float mean_lower_bound = sum_lower_bound/count_lower_bound;
    float mean_upper_bound = sum_upper_bound/count_upper_bound;

    auto SSF =(std::pow(mean_upper_bound-overall_mean,2)+std::pow(mean_lower_bound-overall_mean,2));
    float PTSS;
    if (TTS == 0) PTSS = 0;
    else PTSS = (SSF/TTS * 100);

//    cout<<"Mean: "<<overall_mean<<" lower mean: "<< mean_lower_bound<<" upper mean: "<<mean_upper_bound<<" FSS: "<<FSS<<" TTS: "<<TTS<<" FSS/TTS: "<<FSS/TTS<< " PTSS: "<<PTSS<<endl;
    return PTSS;
};

template <typename T>
inline float tools::max(vector<T> & data_array){

    return *std::max_element( std::begin(data_array), std::end(data_array) );
}
template <typename T>
inline float tools::max(matrix<T> & data_array){
    vector<float> maxs;
    for (auto &row:data_array) {
        maxs.push_back(tools::max(row));
    }
    return *std::max_element( std::begin(maxs), std::end(maxs) );
}
template <typename T>
inline float tools::mean(vector<T> & data_array){
    float sum=0;
    for (auto&item:data_array){
        sum+= item;
    }
    return sum/data_array.size();
}
template <typename T>
inline float tools::mean(matrix<T> & data_array){
    float sum=0;
    for (auto&item:data_array){

        sum+= mean(item);
    }
    return sum/data_array.size();
}


// inline void tools::plot_distribution(vector<float> distribution,string tag, string dir){
// //    log_vector(fitness_values);
//     if (distribution.size() <= 1){
//         cerr<<"distribution vector should contain more than one element"<<endl;
//         cerr<<"distribution plotting skipped"<<endl;
//         return;
//     }
//     py::list data = py::cast(distribution);
//     string image_name = dir + tag+".png";
//     auto sns = py::module::import("seaborn");
//     auto plt = py::module::import("matplotlib.pyplot");
//     auto norm = py::module::import("scipy.stats").attr("norm");
//     plt.attr("clf")();
//     sns.attr("distplot")(data);
//     plt.attr("xlabel")(tag);

//     plt.attr("tick_params")("axis"_a="both","labelsize"_a="18");

//     plt.attr("savefig")(image_name);

// }
template <typename T>
inline void  tools::log_vector(vector<T> &vec){
    for (const auto &item:vec){
        cout<<item<<" ";
    }
    cout<<endl;
}
template <typename T>
inline void tools::log_matrix(matrix <T> &matr) {
    for ( auto &row:matr){
        cout<<"  ";tools::log_vector(row);
    }
}
template <typename T>
inline void tools::log_matrix3(matrix3 <T> &matr3) {
    for ( auto &matrix:matr3){
        for (auto &row:matrix){
            for (auto &item:row){
                cout<<item<<" ";
            }
            cout<<" pp ";
        }
        cout<<"\n";
    }
}
template<typename T, typename... Args>
inline std::unique_ptr<T> tools::make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

inline vector<vector<float>> tools::Latin_hypercube_sampling(int sample_n,int param_count){
    vector<vector<float>> samples;
    vector<float> sample_row;
    if (sample_n == 1){
        sample_row.push_back(1);
        sample_row.push_back(0);
        samples.push_back(sample_row);

    }else{
//        py::scoped_interpreter guard{}; // start the interpreter and keep it alive
        py::module sys = py::module::import("diversipy");
        auto lhd_matrix = sys.attr("lhd_matrix");
        auto transform_spread_out = sys.attr("transform_spread_out");
        auto lhd_sample = lhd_matrix(sample_n,param_count);
        auto uniform_lhd_sample = transform_spread_out(lhd_sample);

        for (unsigned i = 0; i<sample_n; i++){
            sample_row.clear();
            for (unsigned j=0; j<param_count; j++){
                auto row = uniform_lhd_sample.attr("__getitem__")(i);
                auto item = row.attr("__getitem__")(j).cast<float>();
                sample_row.push_back(item);
            }
            samples.push_back(sample_row);
        }
    }

    return samples;
}
template<typename T>
inline void tools::print_to_file(vector<T> & data, string filename, string param_tag){
    ofstream fileID(filename);
    if (fileID.is_open()){
        fileID<<param_tag<<endl;
        for (auto &item:data){
            fileID<<item<<"\t";
        }
        fileID<<endl;
        fileID.close();
    }else{
        cerr<<"Couldn't open the file "<<filename<<endl;
        exit(2);
    }
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

inline vector<float> tools::slice(vector<float> &v, vector<int> &indices){
    vector<float> res;
    for (auto &index:indices) res.push_back(v[index]);
    return res;
}
inline vector<float> tools::sum(vector<float> &v1, vector<float>& v2){
    vector<float> res;
    if (v1.size()!=v2.size()) {
        cerr<<"Size of two vectors doesnt match"<<endl;
        exit(2);
    }
    for (int iter=0; iter<v1.size(); iter++){
        res.push_back(v1[iter]+v2[iter]);
    }
    return res;
}
inline void tools::remove_space( string &str){
    std::string::iterator end_pos = std::remove(str.begin(),str.end(),' ');
    str.erase(end_pos, str.end());
}
// inline int tools::day_to_tick_convertor(float day){

//     return day*24 /tick_to_hour;
// }
// inline void tools::day_to_tick_convertor_array(vector<int>& arrayValue){
//     for (auto &item:arrayValue){
//         item = day_to_tick_convertor(item);
//     }
// }
inline vector<float> tools::multiply(vector<float> & v, float scalar){
    vector<float> res;
    for (auto &item:v){
        res.push_back(item*scalar);
    }
    return res;
}
inline float tools::pick_random_from_normal_distribution(float mean, float var){
    std::random_device rd;
    std::mt19937 gen(rd());

    std::normal_distribution<double> distribution(mean,var);
    return distribution(gen);
}
// inline int tools::tick_to_day_convertor(int tick){
//     float tick_f = static_cast<float>(tick);
//     return static_cast<int>(tick_f*tick_to_hour/24);
// }
inline void tools::ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c);}));
}

inline void tools::rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](int c) {return !std::isspace(c);}).base(), s.end());
}
inline vector<float> tools::normalize_vector_by_vector(vector<float> &v1, vector<float> &v2){
    vector<float> normalized;
    for (int Iter=0; Iter<v1.size(); Iter++){
        normalized.push_back(v1[Iter]/v2[Iter]);
    }
    return normalized;
}
inline vector<float> tools::sum_over_vectors(matrix<float> &M){
    vector<float> sum;
    for (auto &row:M){
        sum.push_back(accumulate(row.begin(),row.end(),0.0));
    }
    return sum;
}
inline vector<float> tools::concatenate_vectors(vector<float> &v1, vector<float> &v2){
    vector<float> result = v1;
    for (auto &item:v2) result.push_back(item);
    return result;
}
inline void tools::command_python(string scriptname, vector<string> args){
    string compiler = "python";
    string command = compiler +" "+ scriptname;
    for (const auto &arg:args) command+=" "+arg;
    system(command.c_str());
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



inline float tools::pick_random_from_trun_norm_dist(float mean_value, float std_value, float lower_bound, float upper_bound){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> distribution(mean_value, std_value);
    double sample;
    do {
        sample = distribution(gen);
    } while (sample < lower_bound || sample>upper_bound);
    return sample;
}
inline void   tools::truncate(float &value, float lower_bound, float upper_bound){
    (value < lower_bound)?(value = lower_bound):(true);
    (value > upper_bound)?(value = upper_bound):(true);
}
inline float tools::find_in_range(float &lower_bound, float &upper_bound, float &ration){
    auto range = upper_bound - lower_bound;
    return lower_bound+ration*range;
}
inline float tools::vector_distance(vector<float> &vec1, vector<float> &vec2){
    if (vec1.size() != vec2.size()){
        cerr<<"Two vectors should have the same size"<<endl;
        exit(2);
    };
    float sum = 0;
    for (int Iter = 0; Iter<vec1.size(); Iter++){
        sum += pow(vec1[Iter]-vec2[Iter],2);
    }
    return pow(sum, 0.5);
}

inline vector<float> tools::dirichlet_dist(vector<float> & alphas){
    cerr<<"fix dirichlet_dist before using"<<endl;
    exit(2);

//    const gsl_rng_type * T;
//
//    gsl_rng_env_setup();
//
//    T = gsl_rng_default;
//    struct timeval tv; // Seed generation based on time
//    gettimeofday(&tv,0);
//    unsigned long mySeed = tv.tv_sec + tv.tv_usec;
//    auto r = gsl_rng_alloc (T);
//    gsl_rng_set(r, mySeed);
////    r = gsl_rng_alloc (T);
//
//    vector<double> doubleAlphas(alphas.begin(),alphas.end());
//    vector<double> results(alphas.size(),0);
//    gsl_ran_dirichlet(r,doubleAlphas.size(),&doubleAlphas[0],&results[0]);
//    vector<float> results_d(results.begin(),results.end());
//    return results_d;
}
inline vector<float> tools::subtract (vector<float> &v1, vector<float> &v2){
    vector<float> diff;
    for (int Iter=0; Iter<v1.size(); Iter++){
        diff.push_back(v1[Iter]-v2[Iter]);
    }
    return diff;
};
inline float tools::pick_random_from_discrete_dist(vector<float> &weights_d){
    std::random_device rd;
    std::mt19937 gen(rd());
    vector<double> weights(weights_d.begin(),weights_d.end());
    std::discrete_distribution<> d(weights.begin(),weights.end());
    return d(gen);
}
inline vector<float> tools::average_over_vectors(matrix<float> & vectors_container){
    size_t matrix_size = vectors_container.size();
    size_t vector_size = vectors_container[0].size();
    vector<float> sum_vector(vector_size,0);
    for (auto &vector_data:vectors_container){
        std::transform(sum_vector.begin(), sum_vector.end(), vector_data.begin(), sum_vector.begin(), std::plus<float> ());
    }
//    log_vector(sum_vector);
    vector<float> ave_vector(vector_size,0);
    std::transform(sum_vector.begin(), sum_vector.end(), ave_vector.begin(),std::bind(std::divides<float>(),std::placeholders::_1,matrix_size));
    return ave_vector;

}
inline matrix<float> tools::average_over_matrices(vector<matrix<float>> & matrix_container){
    matrix<float> aver_matrix;
    size_t row_n = matrix_container[0].size();
    matrix<float> all_rows;
    for (int Iter=0; Iter<row_n; Iter ++){
        all_rows.clear();
        for (auto &matrix_data:matrix_container){
            all_rows.push_back(matrix_data[Iter]);
        }
        auto aver_over_rows = tools::average_over_vectors(all_rows);
        aver_matrix.push_back(aver_over_rows);
    }
    return aver_matrix;
}
inline vector<double>  tools::nomalize_vector(vector<float> &vec){
    float sum = accumulate(vec.begin(),vec.end(),0.0);
    vector<double> normalized_vec(vec.size(),0);
    std::transform(vec.begin(), vec.end(), normalized_vec.begin(),std::bind(std::divides<float>(),std::placeholders::_1,sum));
    return normalized_vec;
}
inline float tools::product_vector(vector<float> &v1, vector<float> &v2){
    if (v1.size()!=v2.size()) {
        cerr<<"Two vectors do not have the same size"<<endl;
        cerr<<"V1: "; log_vector(v1);
        cerr<<"V2: "; log_vector(v2);
        exit(2);
    }
    float sum = 0;
    for (int i=0;i<v1.size();i++ ){
        sum += v1[i]*v2[i];
    }
    return sum;
}
inline vector<float> tools::product(matrix<float> &m, vector<float>&v){
    for (auto &row:m) {
        if (row.size()!=v.size()) {
            cerr<<"The dimensions of matrix and vector are not compatible"<<endl;
            exit(2);
        }
    }
    vector<float> result;
    for (auto &row:m){
        result.push_back(tools::product_vector(row,v));
    }
    return result;
}
inline vector<float> tools::dot_product_vector(vector<float> &v1, vector<float>& v2){
    if (v1.size()!=v2.size()) {
        cerr<<"Two vectors do not have the same size"<<endl;
        cerr<<"V1: "; log_vector(v1);
        cerr<<"V2: "; log_vector(v2);
        exit(2);
    }
    vector<float> res;
    for (int ii=0; ii<v1.size(); ii++){
        res.push_back(v1[ii]*v2[ii]);
    }
    return res;
}
