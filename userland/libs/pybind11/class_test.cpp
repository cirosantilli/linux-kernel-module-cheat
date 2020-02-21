#include <string>

#include <pybind11/pybind11.h>

struct ClassTest {
    ClassTest(const std::string &name) : name(name) { }
    void setName(const std::string &name_) { name = name_; }
    const std::string &getName() const { return name; }
    std::string name;
};

namespace py = pybind11;

PYBIND11_PLUGIN(class_test) {
    py::module m("my_module", "pybind11 example plugin");
    py::class_<ClassTest>(m, "ClassTest")
        .def(py::init<const std::string &>())
        .def("setName", &ClassTest::setName)
        .def("getName", &ClassTest::getName)
        .def_readwrite("name", &ClassTest::name);
    return m.ptr();
}
