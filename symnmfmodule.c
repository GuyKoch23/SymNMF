#define _GNU_SOURCE
#define PY_SSIZE_T_CLEAN
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Python.h>
#include "symnmf.h"



static PyObject* convert_to_python_list(double **array, int rows, int cols) {
    int i, j;
    PyObject *outer_list, *inner_list, *value;
    outer_list = PyList_New(rows);
    if (outer_list == NULL) {
      return NULL;
    }

    for (i = 0; i < rows; i++) {
        inner_list = PyList_New(cols);
        if (inner_list == NULL) {
            Py_DECREF(outer_list);
            return NULL;
        }
        for (j = 0; j < cols; j++) {
            value = PyFloat_FromDouble(array[i][j]);
            if (value == NULL) {
                Py_DECREF(inner_list);
                Py_DECREF(outer_list);
                return NULL;
            }
            PyList_SET_ITEM(inner_list, j, value);
        }
        PyList_SET_ITEM(outer_list, i, inner_list);
    }

    return outer_list;
}

static double** parse_matrix(PyObject* mat, int N, int d){
  int i, j;
  double** X;
  PyObject* x;
  if (PyObject_Length(mat) < 0) {
      return NULL;
    }
    X = (double **)calloc(N, sizeof(double *));
    if (X == NULL) {
      return NULL;
    }
    for (i=0; i<N; i++) {
      x = PyList_GetItem(mat, i);
      X[i] = calloc(d, sizeof(double));
      if (X[i] == NULL) {
        Py_DECREF(x);
        Py_DECREF(mat);
        free_array_of_pointers(X, i);
        return NULL;
      }
      for (j=0; j<d; j++) {
        X[i][j] = PyFloat_AsDouble(PyList_GetItem(x, j));
      }
    }
  return X;
}

static PyObject* sym(PyObject *self, PyObject *args){
    double **X, **A;
    PyObject *X_obj;
    int N, d;
    if(!PyArg_ParseTuple(args, "Oii", &X_obj, &N, &d)){
         return NULL;
    }
    X = parse_matrix(X_obj, N, d);
    if(X == NULL){
      return NULL;
    }
    A =  sym_c(X, N, d);
    free_array_of_pointers(X, N, N);
    return convert_to_python_list(A, N, N);
}

static PyObject* ddg(PyObject *self, PyObject *args){
    double** X;
    int N, d;
    if(!PyArg_ParseTuple(args, "oii", &X, &N, &d)){
        return NULL;
    }
    return Py_BuildValue("O", ddg_c(X, N, d));
}

static PyObject* norm(PyObject *self, PyObject *args){
    double** X;
    int N, d;
    if(!PyArg_ParseTuple(args, "oii", &X, &N, &d)){
        return NULL;
    }
    return Py_BuildValue("O", norm_c(X, N, d));
}

static PyMethodDef symnmfmodMethods[] = {
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

static struct PyModuleDef symnmfModule = {
    PyModuleDef_HEAD_INIT,
    "symnmf",
    NULL,
    -1,
    symnmfmodMethods
};

PyMODINIT_FUNC PyInit_symnmf(void)
{
  PyObject *m;
  m = PyModule_Create(&symnmfModule);
  if (!m) {
    return NULL;
  }
  return m;
}