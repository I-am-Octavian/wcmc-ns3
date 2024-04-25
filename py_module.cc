// for ml model communication
#include <pybind11/pybind11.h>
#include "ns3/ai-module.h"
#include "mcs-model.h"

using namespace ns3;

namespace py = pybind11;


PYBIND11_MODULE(ns3ai_rnn_py, m)
{
    py::class_<MCSFeature>(m, "PyEnvStruct")
        .def(py::init<>())
        .def_readwrite("posX", &MCSFeature::posX)
        .def_readwrite("posY", &MCSFeature::posY);

    py::class_<MCSPredicted>(m, "PyActStruct")
        .def(py::init<>()).def_readwrite("mcsPredicted", &MCSPredicted::mcsPredicted);

    py::class_<ns3::Ns3AiMsgInterfaceImpl<MCSFeature, MCSPredicted>>(m, "Ns3AiMsgInterfaceImpl")
        .def(py::init<bool,
                      bool,
                      bool,
                      uint32_t,
                      const char*,
                      const char*,
                      const char*,
                      const char*>())
        .def("PyRecvBegin", &ns3::Ns3AiMsgInterfaceImpl<MCSFeature, MCSPredicted>::PyRecvBegin)
        .def("PyRecvEnd", &ns3::Ns3AiMsgInterfaceImpl<MCSFeature, MCSPredicted>::PyRecvEnd)
        .def("PySendBegin", &ns3::Ns3AiMsgInterfaceImpl<MCSFeature, MCSPredicted>::PySendBegin)
        .def("PySendEnd", &ns3::Ns3AiMsgInterfaceImpl<MCSFeature, MCSPredicted>::PySendEnd)
        .def("PyGetFinished", &ns3::Ns3AiMsgInterfaceImpl<MCSFeature, MCSPredicted>::PyGetFinished)
        .def("GetCpp2PyStruct",
             &ns3::Ns3AiMsgInterfaceImpl<MCSFeature, MCSPredicted>::GetCpp2PyStruct,
             py::return_value_policy::reference)
        .def("GetPy2CppStruct",
             &ns3::Ns3AiMsgInterfaceImpl<MCSFeature, MCSPredicted>::GetPy2CppStruct,
             py::return_value_policy::reference);
}
