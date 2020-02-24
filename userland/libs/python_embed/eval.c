/* https://cirosantilli.com/linux-kernel-module-cheat#embedding-python-in-another-application
 *
 * Adapted from: https://docs.python.org/3.7/extending/embedding.html#very-high-level-embedding
 */

#define PY_SSIZE_T_CLEAN
#include <Python.h>

int main(int argc, char *argv[]) {
    (void)argc;
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }
    Py_SetProgramName(program);
    Py_Initialize();
    PyRun_SimpleString(argv[1]);
    if (Py_FinalizeEx() < 0) {
        exit(120);
    }
    PyMem_RawFree(program);
    return 0;
}
