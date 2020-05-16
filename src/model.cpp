#include <CA/frame.h>
//!  A pybind interface to Frame
/*!
  Provides a pybind interface to Frame. It receives agent models, patch model, and settings and passes to Frame::Frame to construct a model. 
*/
struct ABM{
    explicit ABM(py::dict agent_modelObjs,py::object patch_model,settings_t settings) {
        model = make_shared<Frame<DIM>> (agent_modelObjs,patch_model,settings);
    }
    std::shared_ptr<Frame<DIM>> model; //!< A pointer to Frame. It's create in ABM::ABM and used in ABM::run.
    py::dict run() { //! Calls Frame::setup to setup ABM and then Frame::run to run the model. It receives the results as a py::dict and returns.

        model->setup();
        py::dict returns = model->run();
        return returns;
    }
};

PYBIND11_MODULE(ABM, m) {
    py::class_<ABM>(m, "ABM")
            .def(py::init<py::dict,py::object,settings_t>()) //receives NN as input
            .def("run", &ABM::run);
};

