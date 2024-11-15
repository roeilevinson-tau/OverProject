#include <Python.h>
#include "symnmf.h"

/* Helper function to convert a Python list to a Matrix struct */
Matrix* python_list_to_matrix(PyObject* list) {
    if (!PyList_Check(list)) {
        PyErr_SetString(PyExc_TypeError, "Input must be a list of lists.");
        return NULL;
    }

    int rows = PyList_Size(list);
    if (rows == 0) {
        PyErr_SetString(PyExc_ValueError, "Input list cannot be empty.");
        return NULL;
    }

    int cols = PyList_Size(PyList_GetItem(list, 0));
    Matrix* matrix = initialize_matrix_with_zeros(rows, cols);
    if (matrix == NULL) {
        PyErr_NoMemory();
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        PyObject* row = PyList_GetItem(list, i);
        if (!PyList_Check(row) || PyList_Size(row) != cols) {
            free_matrix(matrix);
            PyErr_SetString(PyExc_ValueError, "All rows must be lists of the same length.");
            return NULL;
        }

        for (int j = 0; j < cols; j++) {
            PyObject* item = PyList_GetItem(row, j);
            if (!PyFloat_Check(item)) {
                free_matrix(matrix);
                PyErr_SetString(PyExc_TypeError, "All elements must be floats.");
                return NULL;
            }
            matrix->data[i][j] = PyFloat_AsDouble(item);
        }
    }

    return matrix;
}

/* Helper function to convert a Matrix struct to a Python list */
PyObject* matrix_to_python_list(Matrix* matrix) {
    PyObject* list = PyList_New(matrix->rows);
    for (int i = 0; i < matrix->rows; i++) {
        PyObject* row = PyList_New(matrix->cols);
        for (int j = 0; j < matrix->cols; j++) {
            PyList_SetItem(row, j, PyFloat_FromDouble(matrix->data[i][j]));
        }
        PyList_SetItem(list, i, row);
    }
    return list;
}

/* Wrapper function for sym */
static PyObject* py_sym(PyObject* self, PyObject* args) {
    PyObject* input_list;
    if (!PyArg_ParseTuple(args, "O", &input_list)) {
        return NULL;
    }

    Matrix* input_matrix = python_list_to_matrix(input_list);
    if (input_matrix == NULL) {
        return NULL;  /* Exception already set */
    }

    Matrix* result_matrix = sym(input_matrix);
    free_matrix(input_matrix);

    if (result_matrix == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to compute the symmetric matrix.");
        return NULL;
    }

    PyObject* result_list = matrix_to_python_list(result_matrix);
    free_matrix(result_matrix);

    return result_list;
}

/* Wrapper function for ddg */
static PyObject* py_ddg(PyObject* self, PyObject* args) {
    PyObject* input_list;
    if (!PyArg_ParseTuple(args, "O", &input_list)) {
        return NULL;
    }

    Matrix* input_matrix = python_list_to_matrix(input_list);
    if (input_matrix == NULL) {
        return NULL; 
    }

    Matrix* result_matrix = ddg(input_matrix);
    free_matrix(input_matrix);

    if (result_matrix == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to compute the diagonal degree matrix.");
        return NULL;
    }

    PyObject* result_list = matrix_to_python_list(result_matrix);
    free_matrix(result_matrix);

    return result_list;
}

/* Wrapper function for norm */
static PyObject* py_norm(PyObject* self, PyObject* args) {
    PyObject* input_list;
    if (!PyArg_ParseTuple(args, "O", &input_list)) {
        return NULL;
    }

    Matrix* input_matrix = python_list_to_matrix(input_list);
    if (input_matrix == NULL) {
        return NULL;  /* Exception already set */
    }

    Matrix* result_matrix = norm(input_matrix);
    free_matrix(input_matrix);

    if (result_matrix == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to compute the normalized similarity matrix.");
        return NULL;
    }

    PyObject* result_list = matrix_to_python_list(result_matrix);
    free_matrix(result_matrix);

    return result_list;
}

/* Wrapper function for symnmf */
static PyObject* py_symnmf(PyObject* self, PyObject* args) {
    PyObject* H_list;
    PyObject* W_list;
    if (!PyArg_ParseTuple(args, "OO", &H_list, &W_list)) {
        return NULL;
    }

    Matrix* H_matrix = python_list_to_matrix(H_list);
    Matrix* W_matrix = python_list_to_matrix(W_list);
    if (H_matrix == NULL || W_matrix == NULL) {
        return NULL;  /* Exception already set */
    }

    Matrix* result_matrix = symnmf(H_matrix, W_matrix);
    free_matrix(H_matrix);
    free_matrix(W_matrix);

    if (result_matrix == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to compute the symnmf matrix.");
        return NULL;
    }

    PyObject* result_list = matrix_to_python_list(result_matrix);
    free_matrix(result_matrix);

    return result_list;
}

/* Method definitions */
static PyMethodDef SymnmfMethods[] = {
    {"sym", py_sym, METH_VARARGS, "Calculate the symmetric normalized similarity matrix."},
    {"ddg", py_ddg, METH_VARARGS, "Calculate the diagonal degree matrix."},
    {"norm", py_norm, METH_VARARGS, "Calculate the normalized similarity matrix."},
    {"symnmf", py_symnmf, METH_VARARGS, "Calculate the symnmf matrix."},
    {NULL, NULL, 0, NULL}
};

/* Module definition */
static struct PyModuleDef symnmfmodule = {
    PyModuleDef_HEAD_INIT,
    "mysymnmf",
    NULL,
    -1,
    SymnmfMethods
};

/* Module initialization function */
PyMODINIT_FUNC PyInit_mysymnmf(void) {
    return PyModule_Create(&symnmfmodule);
}
