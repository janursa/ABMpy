
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
#include <CA/model.h>
using std::cout;
struct CA{
    CA() {}
    explicit CA(py::dict agent_modelObjs,py::object patch_model,settings_t settings) {
        model = make_shared<Model<DIM>> (agent_modelObjs,patch_model,settings);
    }
    std::shared_ptr<Model<DIM>> model;
    py::dict run() {
        model->setup();
        py::dict returns = model->run();
        model->reset();
        return returns;
    }
};

PYBIND11_MODULE(CA, m) {
    py::class_<CA>(m, "CA")
            .def(py::init<py::dict,py::object,settings_t>()) //receives NN as input
            .def("run", &CA::run);
};

