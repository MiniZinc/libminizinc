/*  Python Interface for MiniZinc constraint modelling
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *  Supervisor:
 *     Guido Tack <guido.tack@monash.edu>
 */

#ifndef __SOLVER_H
#define __SOLVER_H

#include "global.h"

struct MznSolver {
  PyObject_HEAD
  MiniZinc::SolverInstanceBase* solver;
  MiniZinc::Env* env;
  MiniZinc::Model* _m;

  PyObject* next();
};

static PyObject* MznSolver_new(PyTypeObject* type, PyObject* args, PyObject* kwds);
static int MznSolver_init(MznSolver* self, PyObject* args);
static void MznSolver_dealloc(MznSolver* self);


// Ask the solver to give the next available solution
//  Return Py_None if no error
//  Return a string with "Unsatisfied" or "Reached last solution" if error
static PyObject* MznSolver_next(MznSolver *self);


static PyObject* MznSolver_get_value_helper(MznSolver* self, PyObject* args);
// get_value accepts 1 string or a list of strings,
//   which are the name of values to be retrieved.
// returns a value or a tuple of value depending on which type argument was parsed.
static PyObject* MznSolver_get_value(MznSolver* self, PyObject* args);



static PyMemberDef MznSolver_members[] = {
  {NULL} /* Sentinel */
};


//static PyMethodDef MznSolver_methods[3];

static PyMethodDef MznSolver_methods[] = {
  {"next", (PyCFunction)MznSolver_next, METH_NOARGS, "Next Solution"},
  {"get_value",(PyCFunction)MznSolver_get_value, METH_VARARGS, "Get value of a variable"},
  {NULL} /* Sentinel */
};


static PyTypeObject MznSolver_Type = {
  PyVarObject_HEAD_INIT(NULL,0)
  "minizinc_internal.solver",/* tp_name */
  sizeof(MznSolver),         /* tp_basicsize */
  0,                         /* tp_itemsize */
  (destructor)MznSolver_dealloc, /* tp_dealloc */
  0,                         /* tp_print */
  0,                         /* tp_getattr */
  0,                         /* tp_setattr */
  0,                         /* tp_reserved */
  0,                         /* tp_repr */
  0,                         /* tp_as_number */
  0,                         /* tp_as_sequence */
  0,                         /* tp_as_mapping */
  0,                         /* tp_hash  */
  0,                         /* tp_call */
  0,                         /* tp_str */
  0,                         /* tp_getattro */
  0,                         /* tp_setattro */
  0,                         /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
  "Minizinc Solver",         /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  0,                         /* tp_iter */
  0,                         /* tp_iternext */
  MznSolver_methods,         /* tp_methods */
  MznSolver_members,         /* tp_members */
  0,                         /* tp_getset */
  0,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  (initproc)MznSolver_init,  /* tp_init */
  0,                         /* tp_alloc */
  MznSolver_new,             /* tp_new */
};

#endif