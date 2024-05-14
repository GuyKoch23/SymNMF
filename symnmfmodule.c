#define _GNU_SOURCE
#define PY_SSIZE_T_CLEAN
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Python.h>
#include <symnmf.h>


static PyObject* sym(PyObject *self, PyObject *args){
    double** X;
    int N, d;
    if(!PyArg_ParseTuple(args, "oii", &X, &N, &d)){
        return NULL;
    }
    return Py_BuildValue("O", sym(X, N, d));
}

static PyObject* ddg(PyObject *self, PyObject *args){
    double** X;
    int N, d;
    if(!PyArg_ParseTuple(args, "oii", &X, &N, &d)){
        return NULL;
    }
    return Py_BuildValue("O", ddg(X, N, d));
}

static PyObject* norm(PyObject *self, PyObject *args){
    double** X;
    int N, d;
    if(!PyArg_ParseTuple(args, "oii", &X, &N, &d)){
        return NULL;
    }
    return Py_BuildValue("O", norm(X, N, d));
}

static PyMethodDef symmnfmodMethods[] = {
    {
      "sym",
      (PyCFunction) sym,
      METH_VARARGS,
      PyDoc_STR(
        "Perform some operation using Sym algorithm."
      )
    },
    {
      "ddg",
      (PyCFunction) ddg,
      METH_VARARGS,
      PyDoc_STR(
        "Perform some operation using ddg algorithm."
      )
    },
    {
      "norm",
      (PyCFunction) norm,
      METH_VARARGS,
      PyDoc_STR(
        "Perform some operation using norm algorithm."
      )
    },
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef symmnfModule = {
    PyModuleDef_HEAD_INIT,
    "symmnf",
    NULL,
    -1,
    symmnfmodMethods
};

PyMODINIT_FUNC PyInit_symmnf(void)
{
    PyObject *m;
    m = PyModule_Create(&symmnfModule);
    if (!m) {
        return NULL;
    }
    return m;
}