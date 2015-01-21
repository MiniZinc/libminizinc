#ifndef __PYINTERFACE_H
#define __PYINTERFACE_H

#include <Python.h>
#include "structmember.h"

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <list>
#include <string.h>
#include <typeinfo>
#include <cstdlib>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>

#include <minizinc/parser.hh>
#include <minizinc/model.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/flatten.hh>
#include <minizinc/optimize.hh>
#include <minizinc/builtins.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/solvers/gecode/gecode_solverinstance.hh>

using namespace MiniZinc;
using namespace std;

// Error Objects
static PyObject* MznModel_new_error;
static PyObject* MznModel_init_error;
static PyObject* MznModel_solve_error;
static PyObject* MznModel_load_error;
static PyObject* MznModel_loadString_error;
static PyObject* MznModel_solve_warning;
static PyObject* MznSet_error;

#include "MznSet.h"

// Time alarm
sigjmp_buf jmpbuf;

string minizinc_set(long start, long end);
int getList(PyObject* value, vector<Py_ssize_t>& dimensions, vector<PyObject*>& simpleArray, const int layer);


struct MznModel {
  PyObject_HEAD
  Model* _m;
  vector<string>* includePaths;

  int timeLimit;
  bool loaded;

  //vector<string>* data;

  int load(PyObject *args, PyObject *keywds, bool fromFile);
  //int loadData(PyObject *args);
  PyObject* solve();
};

void MznModelDestructor(PyObject* o);
static PyObject* MznModel_new(PyTypeObject* type, PyObject* args, PyObject* kwds);
static int MznModel_init(MznModel* self, PyObject* args);
static void MznModel_dealloc(MznModel* self);

static PyObject* MznModel_load(MznModel *self, PyObject *args, PyObject *keywds);
static PyObject* MznModel_loadString(MznModel *self, PyObject *args, PyObject *keywds);
//static PyObject* MznModel_loadData(MznModel* self, PyObject *args);
static PyObject* MznModel_solve(MznModel *self);
static PyObject* MznModel_setTimeLimit(MznModel* self, PyObject* args);
//static PyObject* MznModel_solvewArgs(MznModel *self, PyObject *args);

static PyObject* Mzn_load(PyObject* self, PyObject* args, PyObject* keywds);

static PyMemberDef MznModel_members[] = {
  {NULL} /* Sentinel */
};

static PyMethodDef MznModel_methods[] = {
  {"load", (PyCFunction)MznModel_load, METH_KEYWORDS, "Load MiniZinc model from MiniZinc file"},
  //{"loadData", (PyCFunction)MznModel_load, METH_VARARGS, "Load data into the loaded model"},
  {"loadFromString", (PyCFunction)MznModel_loadString, METH_KEYWORDS, "Load MiniZinc model from standard input"},
  {"solve", (PyCFunction)MznModel_solve, METH_NOARGS, "Solve a loaded MiniZinc model"},
  {"setTimeLimit", (PyCFunction)MznModel_setTimeLimit, METH_VARARGS, "Limit the execution time of the model"},
  //{"solveO", (PyCFunction)MznModel_solvewArgs, METH_VARARGS, "Solve a loaded MiniZinc model with options"},
  {NULL} /* Sentinel */
};

static PyMethodDef Mzn_methods[] = {
  {"load", (PyCFunction)Mzn_load, METH_KEYWORDS, "Load MiniZinc model from MiniZinc file"},
  {NULL}
};

static PyTypeObject MznModelType = {
  PyVarObject_HEAD_INIT(NULL,0)
  "minizinc.model",       /* tp_name */
  sizeof(MznModel),          /* tp_basicsize */
  0,                         /* tp_itemsize */
  (destructor)MznModel_dealloc, /* tp_dealloc */
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
  "Minizinc Model",  /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  0,                         /* tp_iter */
  0,                         /* tp_iternext */
  MznModel_methods,            /* tp_methods */
  MznModel_members,            /* tp_members */
  0,/*MznModel_getseters,        /* tp_getset */
  0,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  (initproc)MznModel_init,     /* tp_init */
  0,                         /* tp_alloc */
  MznModel_new,                /* tp_new */
};


#endif