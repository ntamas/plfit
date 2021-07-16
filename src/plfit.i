/* plfit.i
 *
 * Copyright (C) 2010-2011 Tamas Nepusz
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
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
#include "plfit_error.h"
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

#define PLFIT_VERSION_MAJOR 0
#define PLFIT_VERSION_MINOR 8
#define PLFIT_VERSION_PATCH 2
#define PLFIT_VERSION_STRING "0.8.2"

/* Data types */
typedef unsigned short int plfit_bool_t;

typedef enum {
    PLFIT_LINEAR_ONLY,
    PLFIT_STRATIFIED_SAMPLING,
    PLFIT_GSS_OR_LINEAR,
    PLFIT_DEFAULT_CONTINUOUS_METHOD = PLFIT_STRATIFIED_SAMPLING
} plfit_continuous_method_t;

typedef enum {
    PLFIT_LBFGS,
    PLFIT_LINEAR_SCAN,
    PLFIT_PRETEND_CONTINUOUS,
    PLFIT_DEFAULT_DISCRETE_METHOD = PLFIT_LBFGS
} plfit_discrete_method_t;

typedef enum {
    PLFIT_P_VALUE_SKIP,
    PLFIT_P_VALUE_APPROXIMATE,
    PLFIT_P_VALUE_EXACT,
    PLFIT_DEFAULT_P_VALUE_METHOD = PLFIT_P_VALUE_EXACT
} plfit_p_value_method_t;

typedef struct _plfit_result_t {
    double alpha;
    double xmin;
    double L;
    double D;
    double p;

    %extend {
        char *__str__() {
            static char temp[512];
            sprintf(temp, "alpha = %lg, xmin = %lg, L = %lg, D = %lg, p = %lg",
                $self->alpha, $self->xmin, $self->L, $self->D, $self->p);
            return temp;
        }
    }
} plfit_result_t;

typedef struct _plfit_continuous_options_t {
    plfit_bool_t finite_size_correction;
    plfit_continuous_method_t xmin_method;
    plfit_p_value_method_t p_value_method;
    double p_value_precision;
    plfit_mt_rng_t* rng;

    %extend {
        _plfit_continuous_options_t() {
            plfit_continuous_options_t* obj = (plfit_continuous_options_t*)
                malloc(sizeof(plfit_continuous_options_t));
            plfit_continuous_options_init(obj);
            return obj;
        }

        char *__str__() {
            static char temp[512];
            sprintf(temp, "finite_size_correction = %d, xmin_method = %d, "
                "p_value_method = %d, p_value_precision = %lg",
                $self->finite_size_correction, $self->xmin_method,
                $self->p_value_method, $self->p_value_precision);
            return temp;
        }
    }
} plfit_continuous_options_t;

typedef struct _plfit_discrete_options_t {
    plfit_bool_t finite_size_correction;
    plfit_discrete_method_t alpha_method;
    struct {
        double min;
        double max;
        double step;
    } alpha;
    plfit_p_value_method_t p_value_method;
    double p_value_precision;
    plfit_mt_rng_t* rng;

    %extend {
        _plfit_discrete_options_t() {
            plfit_discrete_options_t* obj = (plfit_discrete_options_t*)
                malloc(sizeof(plfit_discrete_options_t));
            plfit_discrete_options_init(obj);
            return obj;
        }

        char *__str__() {
            static char temp[512];
            sprintf(temp, "finite_size_correction = %d, alpha_method = %d, "
                "alpha.min = %lg, alpha.step = %lg, alpha.max = %lg, "
                "p_value_method = %d, p_value_precision = %lg",
                $self->finite_size_correction, $self->alpha_method,
                $self->alpha.min, $self->alpha.step, $self->alpha.max,
                $self->p_value_method, $self->p_value_precision);
            return temp;
        }
    }
} plfit_discrete_options_t;

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

/* Typemap for incoming plfit_bool_t */
%typemap(in) plfit_bool_t {
    $1 = PyObject_IsTrue($input);
}

/* Typemap for outgoing plfit_result_t */
%typemap(in,numinputs=0) plfit_result_t *OUTPUT (plfit_result_t temp) {
    $1 = &temp;
}
%typemap(argout) (plfit_result_t* OUTPUT) {
    plfit_result_t *result_new;
    PyObject *o;
    result_new = (plfit_result_t*)calloc(1, sizeof(plfit_result_t));
    memmove(result_new, $1, sizeof(plfit_result_t));
    o = SWIG_NewPointerObj((void*)result_new, $1_descriptor, 0);
    $result = l_output_helper($result, o);
}

/* Module initialization */
%init {
    plfit_set_error_handler(plfit_error_handler_ignore);
}

/* Exception handler for return codes */
%typemap(out) int {};
%exception {
    $action
    if (result != 0) {
        SWIG_exception(SWIG_RuntimeError, plfit_strerror(result));
        goto fail;
    }
}

/********** continuous power law distribution fitting **********/

int plfit_log_likelihood_continuous(double* xs, size_t n, double alpha,
        double xmin, double* OUTPUT);
int plfit_estimate_alpha_continuous(double* xs, size_t n, double xmin,
        const plfit_continuous_options_t* options=0, plfit_result_t* OUTPUT);
int plfit_continuous(double* xs, size_t n,
        const plfit_continuous_options_t* options=0, plfit_result_t* OUTPUT);

/********** discrete power law distribution fitting **********/

int plfit_estimate_alpha_discrete(double* xs, size_t n, double xmin,
        const plfit_discrete_options_t* options=0, plfit_result_t* OUTPUT);
int plfit_log_likelihood_discrete(double* xs, size_t n, double alpha,
        double xmin, double* OUTPUT);
int plfit_discrete(double* xs, size_t n,
        const plfit_discrete_options_t* options=0,
        plfit_result_t* OUTPUT);

/***** resampling routines to generate synthetic replicates ****/

int plfit_resample_continuous(double* xs, size_t n, double alpha, double xmin,
        size_t num_samples, plfit_mt_rng_t* rng, double* OUTPUT);
int plfit_resample_discrete(double* xs, size_t n, double alpha, double xmin,
        size_t num_samples, plfit_mt_rng_t* rng, double* OUTPUT);

/******** calculating the p-value of a fitted model only *******/

int plfit_calculate_p_value_continuous(double* xs, size_t n,
        const plfit_continuous_options_t* options, plfit_bool_t xmin_fixed,
        plfit_result_t *OUTPUT);
int plfit_calculate_p_value_discrete(double* xs, size_t n,
        const plfit_discrete_options_t* options, plfit_bool_t xmin_fixed,
        plfit_result_t *OUTPUT);

/************* calculating descriptive statistics **************/

int plfit_moments(double* xs, size_t n, double* OUTPUT, double* OUTPUT,
        double* OUTPUT, double* OUTPUT);

%exception;

%pythoncode %{
__version__ = PLFIT_VERSION_STRING
%}
