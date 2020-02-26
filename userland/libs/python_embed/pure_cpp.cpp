/* C++ version, the main goal is to show how to interface with C++ classes. */

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"

#include <string>

/* Define a function in C to be accessible from the Python code.
 *
 * ....
 * def my_native_method(i, j):
 *     return i * j
 * ....
 */
static PyObject*
my_native_module_my_native_method(PyObject *self, PyObject *args) {
    (void)self;
    int i, j;
    if(!PyArg_ParseTuple(args, "ii", &i, &j))
        return NULL;
    /* "i" stands for integer. */
    return Py_BuildValue("i", i * j);
}

static PyMethodDef my_native_methods[] = {
    {
        "my_native_method",
        my_native_module_my_native_method,
        METH_VARARGS,
        "always returns the integer 2, boring"
    },
    /* NULL terminator sentinel denotes the end of the defined methods. */
    {NULL}
};

/* Define a class in C to be accessible from the Python code.
 * and bind data and methods it to a C++ class!
 * ....
 */

struct MyNativeClass {
    std::string first;
    std::string last;
    int number;

    MyNativeClass(
        const std::string& first,
        const std::string& last,
        int number
    ) : first(first), last(last), number(number) {}

    std::string name() {
        return first + " " + last;
    }
};

struct MyDerivedNativeClass : public MyNativeClass {
    std::string first2;
    std::string last2;
    int number2;

    MyDerivedNativeClass(
        const std::string& first,
        const std::string& last,
        int number,
        const std::string& first2,
        const std::string& last2,
        int number2
    ) : MyNativeClass(first, last, number),
      first2(first2), last2(last2), number2(number2) {}

    std::string name() {
        return first + " " + last + " " + first2 + " " + last2;
    }
};

typedef struct {
    PyObject_HEAD
    PyObject *first;
    PyObject *last;
    int number;
} my_native_module_MyNativeClass;

static void
my_native_module_MyNativeClass_dealloc(my_native_module_MyNativeClass *self) {
    Py_XDECREF(self->first);
    Py_XDECREF(self->last);
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
my_native_module_MyNativeClass_traverse(my_native_module_MyNativeClass *self, visitproc visit, void *arg)
{
    Py_VISIT(self->first);
    Py_VISIT(self->last);
    return 0;
}

static int
my_native_module_MyNativeClass_clear(my_native_module_MyNativeClass *self)
{
    Py_CLEAR(self->first);
    Py_CLEAR(self->last);
    return 0;
}

static PyObject *
my_native_module_MyNativeClass_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;
    my_native_module_MyNativeClass *self;
    self = (my_native_module_MyNativeClass *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->first = PyUnicode_FromString("");
        if (self->first == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->last = PyUnicode_FromString("");
        if (self->last == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->number = 0;
    }
    return (PyObject *) self;
}

static int
my_native_module_MyNativeClass_init(my_native_module_MyNativeClass *self, PyObject *args, PyObject *kwds)
{
    static const char *kwlist[] = {"first", "last", "number", NULL};
    PyObject *first = NULL, *last = NULL, *tmp;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OOi", (char**)kwlist,
            &first, &last, &self->number))
        return -1;
    if (first) {
        tmp = self->first;
        Py_INCREF(first);
        self->first = first;
        Py_XDECREF(tmp);
    }
    if (last) {
        tmp = self->last;
        Py_INCREF(last);
        self->last = last;
        Py_XDECREF(tmp);
    }
    return 0;
}

static PyMemberDef my_native_module_MyNativeClass_members[] = {
    {(char*)"number", T_INT, offsetof(my_native_module_MyNativeClass, number), 0,
     (char*)"custom number"},
    {NULL}
};

static PyObject *
my_native_module_MyNativeClass_getfirst(my_native_module_MyNativeClass *self, void *closure)
{
    (void)closure;
    Py_INCREF(self->first);
    return self->first;
}

static int
my_native_module_MyNativeClass_setfirst(my_native_module_MyNativeClass *self, PyObject *value, void *closure)
{
    (void)closure;
    PyObject *tmp;
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the first attribute");
        return -1;
    }
    if (!PyUnicode_Check(value)) {
        PyErr_SetString(PyExc_TypeError,
                        "The first attribute value must be a string");
        return -1;
    }
    tmp = self->first;
    Py_INCREF(value);
    self->first = value;
    Py_DECREF(tmp);
    return 0;
}

static PyObject *
my_native_module_MyNativeClass_getlast(my_native_module_MyNativeClass *self, void *closure)
{
    (void)closure;
    Py_INCREF(self->last);
    return self->last;
}

static int
my_native_module_MyNativeClass_setlast(my_native_module_MyNativeClass *self, PyObject *value, void *closure)
{
    (void)closure;
    PyObject *tmp;
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the last attribute");
        return -1;
    }
    if (!PyUnicode_Check(value)) {
        PyErr_SetString(PyExc_TypeError,
                        "The last attribute value must be a string");
        return -1;
    }
    tmp = self->last;
    Py_INCREF(value);
    self->last = value;
    Py_DECREF(tmp);
    return 0;
}

static PyObject *
my_native_module_MyNativeClass_name(my_native_module_MyNativeClass *self, PyObject *Py_UNUSED(ignored))
{
    if (self->first == NULL) {
        PyErr_SetString(PyExc_AttributeError, "first");
        return NULL;
    }
    if (self->last == NULL) {
        PyErr_SetString(PyExc_AttributeError, "last");
        return NULL;
    }
    return PyUnicode_FromFormat("%S %S %d", self->first, self->last, self->number + 1);
}

static PyMethodDef my_native_module_MyNativeClass_methods[] = {
    {
        "name",
        (PyCFunction)my_native_module_MyNativeClass_name,
        METH_NOARGS,
        "Return the name, combining the first and last name"
    },
    {NULL}
};

static PyGetSetDef my_native_module_MyNativeClass_getsetters[] = {
    {(char *)"first", (getter) my_native_module_MyNativeClass_getfirst, (setter) my_native_module_MyNativeClass_setfirst,
     (char *)"first name", NULL},
    {(char *)"last", (getter) my_native_module_MyNativeClass_getlast, (setter) my_native_module_MyNativeClass_setlast,
     (char *)"last name", NULL},
    {NULL}  /* Sentinel */
};

static PyTypeObject my_native_module_MyNativeClassType = {
    PyVarObject_HEAD_INIT(NULL, 0)
};

typedef struct {
    my_native_module_MyNativeClass base;
    PyObject *first2;
    PyObject *last2;
    int number2;
} my_native_module_MyDerivedNativeClass;

static void
my_native_module_MyDerivedNativeClass_dealloc(my_native_module_MyDerivedNativeClass *self) {
    Py_XDECREF(self->first2);
    Py_XDECREF(self->last2);
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
my_native_module_MyDerivedNativeClass_traverse(my_native_module_MyDerivedNativeClass *self, visitproc visit, void *arg)
{
    Py_VISIT(self->first2);
    Py_VISIT(self->last2);
    return 0;
}

static int
my_native_module_MyDerivedNativeClass_clear(my_native_module_MyDerivedNativeClass *self)
{
    Py_CLEAR(self->first2);
    Py_CLEAR(self->last2);
    return 0;
}

static PyObject *
my_native_module_MyDerivedNativeClass_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;
    my_native_module_MyDerivedNativeClass *self;
    self = (my_native_module_MyDerivedNativeClass *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->first2 = PyUnicode_FromString("");
        if (self->first2 == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->last2 = PyUnicode_FromString("");
        if (self->last2 == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->number2 = 0;
    }
    return (PyObject *) self;
}

static int
my_native_module_MyDerivedNativeClass_init(my_native_module_MyDerivedNativeClass *self, PyObject *args, PyObject *kwds)
{
    static const char *kwlist[] = {"first", "last", "number", "first2", "last2", "number2", NULL};
    PyObject *first = NULL, *last = NULL, *first2 = NULL, *last2 = NULL, *tmp;
    int ret;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OOOOOi", (char**)kwlist,
            &first, &last, &tmp, &first2, &last2, &self->number2))
        return -1;
    /* args is a PyTuple, extract the first 3 arguments into a new
     * tuple to serve as arguments of the base class. */
    PyObject *base_args = PySequence_GetSlice(args, 0, 3);
    ret = my_native_module_MyNativeClassType.tp_init((PyObject *) self, base_args, kwds);
    Py_DECREF(base_args);
    if (ret < 0)
        return -1;
    if (first2) {
        tmp = self->first2;
        Py_INCREF(first2);
        self->first2 = first2;
        Py_XDECREF(tmp);
    }
    if (last2) {
        tmp = self->last2;
        Py_INCREF(last2);
        self->last2 = last2;
        Py_XDECREF(tmp);
    }
    return 0;
}

static PyMemberDef my_native_module_MyDerivedNativeClass_members[] = {
    {(char*)"number2", T_INT, offsetof(my_native_module_MyDerivedNativeClass, number2), 0,
     (char*)"custom number2"},
    {NULL}
};

static PyObject *
my_native_module_MyDerivedNativeClass_getfirst2(my_native_module_MyDerivedNativeClass *self, void *closure)
{
    (void)closure;
    Py_INCREF(self->first2);
    return self->first2;
}

static int
my_native_module_MyDerivedNativeClass_setfirst2(my_native_module_MyDerivedNativeClass *self, PyObject *value, void *closure)
{
    (void)closure;
    PyObject *tmp;
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the first2 attribute");
        return -1;
    }
    if (!PyUnicode_Check(value)) {
        PyErr_SetString(PyExc_TypeError,
                        "The first2 attribute value must be a string");
        return -1;
    }
    tmp = self->first2;
    Py_INCREF(value);
    self->first2 = value;
    Py_DECREF(tmp);
    return 0;
}

static PyObject *
my_native_module_MyDerivedNativeClass_getlast2(my_native_module_MyDerivedNativeClass *self, void *closure)
{
    (void)closure;
    Py_INCREF(self->last2);
    return self->last2;
}

static int
my_native_module_MyDerivedNativeClass_setlast2(my_native_module_MyDerivedNativeClass *self, PyObject *value, void *closure)
{
    (void)closure;
    PyObject *tmp;
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the last2 attribute");
        return -1;
    }
    if (!PyUnicode_Check(value)) {
        PyErr_SetString(PyExc_TypeError,
                        "The last2 attribute value must be a string");
        return -1;
    }
    tmp = self->last2;
    Py_INCREF(value);
    self->last2 = value;
    Py_DECREF(tmp);
    return 0;
}

static PyObject *
my_native_module_MyDerivedNativeClass_name2(my_native_module_MyDerivedNativeClass *self, PyObject *Py_UNUSED(ignored))
{
    if (self->base.first == NULL) {
        PyErr_SetString(PyExc_AttributeError, "first");
        return NULL;
    }
    if (self->base.last == NULL) {
        PyErr_SetString(PyExc_AttributeError, "last");
        return NULL;
    }
    if (self->first2 == NULL) {
        PyErr_SetString(PyExc_AttributeError, "first2");
        return NULL;
    }
    if (self->last2 == NULL) {
        PyErr_SetString(PyExc_AttributeError, "last2");
        return NULL;
    }
    return PyUnicode_FromFormat("%S %S %d %S %S %d",
        self->base.first, self->base.last, self->base.number + 1,
        self->first2, self->last2, self->number2 + 2);
}

static PyGetSetDef my_native_module_MyDerivedNativeClass_getsetters[] = {
    {(char *)"first2", (getter) my_native_module_MyDerivedNativeClass_getfirst2, (setter) my_native_module_MyDerivedNativeClass_setfirst2,
     (char *)"first2 name", NULL},
    {(char *)"last2", (getter) my_native_module_MyDerivedNativeClass_getlast2, (setter) my_native_module_MyDerivedNativeClass_setlast2,
     (char *)"last2 name", NULL},
    {NULL}  /* Sentinel */
};

static PyMethodDef my_native_module_MyDerivedNativeClass_methods[] = {
    {
        "name2",
        (PyCFunction)my_native_module_MyDerivedNativeClass_name2,
        METH_NOARGS,
        "Return the name2, combining the first2 and last2 name2"
    },
    {NULL}
};

static PyTypeObject my_native_module_MyDerivedNativeClassType = {
    PyVarObject_HEAD_INIT(NULL, 0)
};

static PyModuleDef my_native_module = {
    PyModuleDef_HEAD_INIT,
    "my_native_module",
    "My native module",
    -1,
    my_native_methods,
};

PyMODINIT_FUNC
my_native_module_init_func(void) {
    PyObject *m;

    /* Create the module */
    m = PyModule_Create(&my_native_module);
    if (m == NULL)
        return NULL;

    /* Create MyNativeClass */
    if (PyType_Ready(&my_native_module_MyNativeClassType) < 0)
        return NULL;
    Py_INCREF(&my_native_module_MyNativeClassType);
    if (PyModule_AddObject(m, "MyNativeClass", (PyObject *) &my_native_module_MyNativeClassType) < 0) {
        Py_DECREF(&my_native_module_MyNativeClassType);
        Py_DECREF(m);
        return NULL;
    }

    /* Create MyDerivedNativeClass */
    my_native_module_MyDerivedNativeClassType.tp_base = &my_native_module_MyNativeClassType;
    if (PyType_Ready(&my_native_module_MyDerivedNativeClassType) < 0)
        return NULL;
    Py_INCREF(&my_native_module_MyDerivedNativeClassType);
    if (PyModule_AddObject(m, "MyDerivedNativeClass", (PyObject *) &my_native_module_MyDerivedNativeClassType) < 0) {
        Py_DECREF(&my_native_module_MyDerivedNativeClassType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}

int
main(int argc, char *argv[])
{
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;
    int i;

    if (argc < 3) {
        fprintf(stderr,"Usage: call pythonfile funcname [args]\n");
        return 1;
    }

    my_native_module_MyNativeClassType.tp_name = "my_native_module.MyNativeClass";
    my_native_module_MyNativeClassType.tp_doc = "My native class";
    my_native_module_MyNativeClassType.tp_basicsize = sizeof(my_native_module_MyNativeClass);
    my_native_module_MyNativeClassType.tp_itemsize = 0;
    my_native_module_MyNativeClassType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC;
    my_native_module_MyNativeClassType.tp_new = my_native_module_MyNativeClass_new;
    my_native_module_MyNativeClassType.tp_init = (initproc) my_native_module_MyNativeClass_init;
    my_native_module_MyNativeClassType.tp_dealloc = (destructor) my_native_module_MyNativeClass_dealloc;
    my_native_module_MyNativeClassType.tp_traverse = (traverseproc) my_native_module_MyNativeClass_traverse;
    my_native_module_MyNativeClassType.tp_clear = (inquiry) my_native_module_MyNativeClass_clear;
    my_native_module_MyNativeClassType.tp_members = my_native_module_MyNativeClass_members;
    my_native_module_MyNativeClassType.tp_methods = my_native_module_MyNativeClass_methods;
    my_native_module_MyNativeClassType.tp_getset = my_native_module_MyNativeClass_getsetters;

    my_native_module_MyDerivedNativeClassType.tp_name = "my_native_module.MyDerivedNativeClass";
    my_native_module_MyDerivedNativeClassType.tp_doc = "My native class";
    my_native_module_MyDerivedNativeClassType.tp_basicsize = sizeof(my_native_module_MyDerivedNativeClass);
    my_native_module_MyDerivedNativeClassType.tp_itemsize = 0;
    my_native_module_MyDerivedNativeClassType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC;
    my_native_module_MyDerivedNativeClassType.tp_new = my_native_module_MyDerivedNativeClass_new;
    my_native_module_MyDerivedNativeClassType.tp_init = (initproc) my_native_module_MyDerivedNativeClass_init;
    my_native_module_MyDerivedNativeClassType.tp_dealloc = (destructor) my_native_module_MyDerivedNativeClass_dealloc;
    my_native_module_MyDerivedNativeClassType.tp_traverse = (traverseproc) my_native_module_MyDerivedNativeClass_traverse;
    my_native_module_MyDerivedNativeClassType.tp_clear = (inquiry) my_native_module_MyDerivedNativeClass_clear;
    my_native_module_MyDerivedNativeClassType.tp_members = my_native_module_MyDerivedNativeClass_members;
    my_native_module_MyDerivedNativeClassType.tp_methods = my_native_module_MyDerivedNativeClass_methods;
    my_native_module_MyDerivedNativeClassType.tp_getset = my_native_module_MyDerivedNativeClass_getsetters;

    PyImport_AppendInittab("my_native_module", &my_native_module_init_func);
    Py_Initialize();
    pName = PyUnicode_DecodeFSDefault(argv[1]);
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, argv[2]);
        if (pFunc && PyCallable_Check(pFunc)) {
            pArgs = PyTuple_New(argc - 3);
            for (i = 0; i < argc - 3; ++i) {
                pValue = PyLong_FromLong(atoi(argv[i + 3]));
                if (!pValue) {
                    Py_DECREF(pArgs);
                    Py_DECREF(pModule);
                    fprintf(stderr, "Cannot convert argument\n");
                    return 1;
                }
                PyTuple_SetItem(pArgs, i, pValue);
            }
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if (pValue != NULL) {
                printf("%ld\n", PyLong_AsLong(pValue));
                Py_DECREF(pValue);
            } else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return 1;
            }
        } else {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", argv[2]);
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    } else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", argv[1]);
        return 1;
    }
    if (Py_FinalizeEx() < 0) {
        return 120;
    }
    return 0;
}
