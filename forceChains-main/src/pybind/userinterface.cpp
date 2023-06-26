#include "../userInterface.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

using namespace ForceChain;
using namespace std;



namespace py = pybind11;

PYBIND11_MODULE(forcechain, m) {
    py::class_<UserInterface>(m, "UserInterface")
        .def(py::init<string, string, string, double>())
        .def("Run", &UserInterface::Run)
        .def("WriteChainsOnTerminal", &UserInterface::WriteChainsOnTerminal)
        .def_readonly("stat", &UserInterface::stat);

        py::class_<Stat>(m, "Stat")
        .def("GetChainsAnglesWithBed", &Stat::GetChainsAnglesWithBed);
        
}

/*
 import UserInterface
add = "liggghtsResult"
a= add+"/d_3660000.liggghts"
b = add+"/pair3660000.txt"
c = add+"/wall_pair3660000.txt"
*/