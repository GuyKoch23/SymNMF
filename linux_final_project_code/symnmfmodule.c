#define _GNU_SOURCE
#define PY_SSIZE_T_CLEAN
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Python.h>
#include "symnmf.h"

/*
 * Function which converts array of c code to list of python code.
 * Parameters:
 *   array: the array to convert.
 *   rows: number of rows in the array.
 *   cols: number of columns in the array.
 * Returns:
 *   the parsed list.
 */
static PyObject* convert_array_to_python_list(double **array, int rows, int cols) {
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

/*
 * Function which converts list of python code to matrix of c code.
 * Parameters:
 *   max: the matrix to convert.
 *   N: number of rows in the matrix.
 *   d: number of columns in the matrix.
 * Returns:
 *   the parsed matrix.
 */
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

/*
 * Wrapper funtion for the sym_c function.
*/
static PyObject* sym(PyObject *self, PyObject *args){
    double **X, **A;
    PyObject *X_obj, *A_obj;
    int N, d;
    if(!PyArg_ParseTuple(args, "Oii", &X_obj, &N, &d)){
         return NULL;
    }
    X = parse_matrix(X_obj, N, d);
    if(X == NULL){
      Py_DECREF(X_obj);
      return NULL;
    }
    A = sym_c(X, N, d);
    free_array_of_pointers(X, N);
    if(A == NULL){
      Py_DECREF(X_obj);
      return NULL;
    }
    A_obj = convert_array_to_python_list(A, N, N);
    free_array_of_pointers(A, N);
    return A_obj;
}

/*
 * Wrapper funtion for the ddg_c function.
*/
static PyObject* ddg(PyObject *self, PyObject *args){
    double **X, **D;
    PyObject *X_obj, *D_obj;
    int N, d;
    if(!PyArg_ParseTuple(args, "Oii", &X_obj, &N, &d)){
         return NULL;
    }
    X = parse_matrix(X_obj, N, d);
    if(X == NULL){
      Py_DECREF(X_obj);
      return NULL;
    }
    D = ddg_c(X, N, d);
    free_array_of_pointers(X, N);
    if(D == NULL){
      Py_DECREF(X_obj);
      return NULL;
    }
    D_obj = convert_array_to_python_list(D, N, N);
    free_array_of_pointers(D, N);
    return D_obj;
}

/*
 * Wrapper funtion for the nurm_c function.
*/
static PyObject* norm(PyObject *self, PyObject *args){
    double **X, **W;
    PyObject *X_obj, *W_obj;
    int N, d;
    if(!PyArg_ParseTuple(args, "Oii", &X_obj, &N, &d)){
         return NULL;
    }
    X = parse_matrix(X_obj, N, d);
    if(X == NULL){
      Py_DECREF(X_obj);
      return NULL;
    }
    W = norm_c(X, N, d);
    free_array_of_pointers(X, N);
    if(W == NULL){
      Py_DECREF(X_obj);
      return NULL;
    }
    W_obj = convert_array_to_python_list(W, N, N);
    free_array_of_pointers(W, N);
    return W_obj;
}

/*
 * Symnmf implementation.
*/
static PyObject* symnmf(PyObject *self, PyObject *args){
    double **W, **H, **H_new;
    PyObject *W_obj, *H_obj, *H_new_obj;
    int N, d, K;
    if(!PyArg_ParseTuple(args, "OOiii", &W_obj, &H_obj, &N, &d, &K)){
         return NULL;
    }
    W = parse_matrix(W_obj, N, N);
    if(W == NULL){
      Py_DECREF(W_obj);
      Py_DECREF(H_obj);
      return NULL;
    }
    H = parse_matrix(H_obj, N, K);
    if(H == NULL){
      free_array_of_pointers(W, N);
      Py_DECREF(W_obj);
      Py_DECREF(H_obj);
      return NULL;
    }
    H_new = symnmf_c(W, H, N, K);
    free_array_of_pointers(W, N);
    free_array_of_pointers(H, N);
    if(H_new == NULL){
      Py_DECREF(W_obj);
      Py_DECREF(H_obj);
      return NULL;
    }
    H_new_obj = convert_array_to_python_list(H_new, N, K);
    free_array_of_pointers(H_new, N);
    return H_new_obj;
}

static PyMethodDef symnmfmodMethods[] = {
    {
      "sym",
      (PyCFunction) sym,
      METH_VARARGS,
      PyDoc_STR(
        "Executes Sym algorithm."
      )
    },
    {
      "ddg",
      (PyCFunction) ddg,
      METH_VARARGS,
      PyDoc_STR(
        "Executes ddg algorithm."
      )
    },
    {
      "norm",
      (PyCFunction) norm,
      METH_VARARGS,
      PyDoc_STR(
        "Executes norm algorithm."
      )
    },
    {
      "symnmf",
      (PyCFunction) symnmf,
      METH_VARARGS,
      PyDoc_STR(
        "Executes symnmf algorithm."
      )
    },
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef symnmfModule = {
    PyModuleDef_HEAD_INIT,
    "csymnmf",
    NULL,
    -1,
    symnmfmodMethods
};

PyMODINIT_FUNC PyInit_csymnmf(void)
{
  PyObject *m;
  m = PyModule_Create(&symnmfModule);
  if (!m) {
    return NULL;
  }
  return m;
}