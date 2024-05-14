#define _GNU_SOURCE
#define PY_SSIZE_T_CLEAN
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Python.h>
#include <symnmf.h>


static PyObject* sym(PyObject *self, PyObject *args);

static PyMethodDef mykmeansspMethods[] = {
    {
      "fit",
      (PyCFunction) fit,
      METH_VARARGS,
      PyDoc_STR(
        "Perform some operation using K-means algorithm.\n \
        Input:\n \
        K : int - The number of centroids.\n \
        N : int - The number of data points.\n \
        d : int - The dimensionality of the data points.\n \
        iter : int - The maximum number of iterations for the algorithm.\n \
        eps : float - The threshold for convergence.\n \
        vectors : double** - A 2D list representing the input data points. Each inner list represents a data point and should have 'd' elements.\n \
        centroids : double** - A 2D list representing the initial centroids for the clusters. It should have 'K' inner lists, each representing a centroid point and should have 'd' elements.\n \
        "
      )
    },
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef mykmeansspModule = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",
    NULL,
    -1,
    mykmeansspMethods
};

PyMODINIT_FUNC PyInit_mykmeanssp(void)
{
    PyObject *m;
    m = PyModule_Create(&mykmeansspModule);
    if (!m) {
        return NULL;
    }
    return m;
}