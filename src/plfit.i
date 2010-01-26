/* plfit.i
 * 
 * Copyright (C) 2010 Tamas Nepusz
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

%module plfit
%include "exception.i"
%include "typemaps.i"

%{
#include "plfit.h"
#include "error.h"
%}

/* Helper function for Python list output */
%{
static PyObject* l_output_helper(PyObject* target, PyObject* o) {
    PyObject *o2;
    if (!target) {
        target = o;
    } else if (target == Py_None) {
        Py_DECREF(Py_None);
        target = o;
    } else {
        if (!PyList_Check(target)) {
            o2 = target;
            target = PyList_New(0);
            PyList_Append(target, o2);
            Py_XDECREF(o2);
        }
        PyList_Append(target, o);
        Py_XDECREF(o);
    }
    return target;
}
%}

/* Data types */
typedef struct _plfit_result_t {
    double alpha;
    double xmin;
    double L;
    double p;

    %extend {
        char *__str__() {
            static char temp[512];
            sprintf(temp, "alpha = %lg, xmin = %lg, L = %lg, p = %lg",
                $self->alpha, $self->xmin, $self->L, $self->p);
            return temp;
        }
    }
} plfit_result_t;

/* Typemap for incoming samples */
%typemap(in) (double* xs, size_t n) {
    int i;
    if (!PyList_Check($input)) {
        PyErr_SetString(PyExc_ValueError, "expecting a list");
        return NULL;
    }
    $2 = PyList_Size($input);
    $1 = (double*) malloc(sizeof(double) * $2);
    for (i = 0; i < $2; i++) {
        PyObject *obj = PyList_GetItem($input, i);
        if (PyInt_Check(obj)) {
            $1[i] = (double)PyInt_AsLong(obj);
        } else if (PyFloat_Check(obj)) {
            $1[i] = PyFloat_AsDouble(obj);
        } else {
            free($1);
            PyErr_SetString(PyExc_ValueError, "list items must be numbers");
            return NULL;
        }
    }
}
%typemap(freearg) (double* xs, size_t n) {
    if ($1) free($1);
}

/* Typemap for outgoing plfit_result_t */
%typemap(in,numinputs=0) plfit_result_t *result (plfit_result_t temp) {
    $1 = &temp;
}
%typemap(argout) (plfit_result_t* result) {
    plfit_result_t *result_new;
    PyObject *o;
    result_new = (plfit_result_t*)calloc(1, sizeof(plfit_result_t));
    memmove(result_new, $1, sizeof(plfit_result_t));
    o = SWIG_NewPointerObj((void*)result_new, $1_descriptor, 0);
    $result = l_output_helper($result, o);
}

/* Exception handler for return codes */
%typemap(out) int {};
%exception plfit_continuous {
    $action
    if (result != 0) {
        SWIG_exception(SWIG_RuntimeError, plfit_strerror(result));
        goto fail;
    }
}

/* Module initialization */
%init {
    plfit_set_error_handler(plfit_error_handler_ignore);
}

/********** continuous power law distribution fitting **********/

int plfit_estimate_alpha_continuous(double* xs, size_t n, double xmin, double* OUTPUT);
int plfit_log_likelihood_continuous(double* xs, size_t n, double alpha, double xmin, double* OUTPUT);
int plfit_continuous(double* xs, size_t n, unsigned short int finite_size_correction = 0,
    plfit_result_t* result = 0);

/********** discrete power law distribution fitting **********/

%rename(plfit_discrete) plfit_discrete_in_range;
%rename(plfit_estimate_alpha_discrete) plfit_estimate_alpha_discrete_in_range;

int plfit_estimate_alpha_discrete_in_range(double* xs, size_t n, double xmin,
        double alpha_min = 1.5, double alpha_max = 3.5, double alpha_step = 0.01,
        double* OUTPUT);
int plfit_estimate_alpha_discrete_fast(double* xs, size_t n, double xmin, double* OUTPUT);
int plfit_log_likelihood_discrete(double* xs, size_t n, double alpha, double xmin, double* OUTPUT);
int plfit_discrete_in_range(double* xs, size_t n, double alpha_min = 1.5,
        double alpha_max = 3.5, double alpha_step = 0.01,
        unsigned short int finite_size_correction = 0,
        plfit_result_t* result = 0);
