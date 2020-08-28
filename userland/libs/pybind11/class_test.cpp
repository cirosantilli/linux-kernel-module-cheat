#include <string>

#include <pybind11/pybind11.h>

struct ClassTest {
    ClassTest(const std::string &name) : name(name) {}
    void setName(const std::string &name_) { name = name_; }
    const std::string &getName() const { return name; }
    std::string name;
};

struct ClassTestDerived : ClassTest {
    ClassTestDerived(const std::string &name, const std::string &name2) :
        ClassTest(name), name2(name2) {}
    std::string getName2() { return name + name2 + "2"; }
    std::string name2;
};

namespace py = pybind11;

PYBIND11_MODULE(class_test, m) {
    m.doc() = "pybind11 example plugin";
    py::class_<ClassTest>(m, "ClassTest")
        .def(py::init<const std::string &>())
        .def("setName", &ClassTest::setName)
        .def("getName", &ClassTest::getName)
        .def_readwrite("name", &ClassTest::name);
    py::class_<ClassTestDerived, ClassTest>(m, "ClassTestDerived")
        .def(py::init<const std::string &, const std::string &>())
        .def("getName2", &ClassTestDerived::getName2)
        .def_readwrite("name", &ClassTestDerived::name);
}
