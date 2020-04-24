/* Copyright (C) 2019 Jalil Nourisa - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */
/* Copyright (C) 2019 Jalil Nourisa - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */
// #include <pybind11/pybind11.h>
#include <iostream>
#include "model.h"
using std::cout;
#ifdef PYTHON
struct CA{
    CA() {}
    explicit CA(py::dict agent_modelObjs) {
    	// add the controllers to controllers() in ctr by taging them based on cell type
    	auto keys = agent_modelObjs.attr("keys")();
    	for (auto &key:keys) {
    		py::object agent_model = agent_modelObjs[key];
    		std::string agent_type = key.cast<string>();
    		interface::agent_models().insert(std::pair<std::string,py::object>(agent_type,agent_model));
    	}
    }
    bool run(py::str settings_dir) {
    	// setup the settings
    	std::string dir = settings_dir.cast<std::string>();
    	json settings = tools::file_to_json(dir);
    	tools::create_directory(main_output_folder);
    	auto model = make_shared<Model<DIM>> (settings);
		model->setup();
		model->run();
        return true;
    }
};

PYBIND11_MODULE(CA, m) {
    py::class_<CA>(m, "CA")
            .def(py::init<py::dict>()) //receives NN as input
            .def("run", &CA::run);
};

#endif //PYTHON
