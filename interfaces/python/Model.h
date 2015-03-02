/*
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *  Supervisor:
 *     Guido Tack <guido.tack@monash.edu>
 */
#ifndef __MODEL_H
#define __MODEL_H

#include "global.h"

struct MznModel {
  PyObject_HEAD
  Model* _m;
  vector<string>* includePaths;


  enum SolverCode {
    SC_GECODE
  };
  SolverCode sc;

  unsigned long long timeLimit;
  bool loaded;

  MznModel();

  int load(PyObject *args, PyObject *keywds, bool fromFile);
  int addData(const char* const name, PyObject* value);
  PyObject* solve(PyObject* args);
};

void MznModelDestructor(PyObject* o);
static PyObject* MznModel_new(PyTypeObject* type, PyObject* args, PyObject* kwds);
static int MznModel_init(MznModel* self, PyObject* args);
static void MznModel_dealloc(MznModel* self);

static PyObject* MznModel_load(MznModel *self, PyObject *args, PyObject *keywds);
static PyObject* MznModel_loadFromString(MznModel *self, PyObject *args, PyObject *keywds);
static PyObject* MznModel_solve(MznModel *self, PyObject* args);
static PyObject* MznModel_setTimeLimit(MznModel* self, PyObject* args);
static PyObject* MznModel_setSolver(MznModel* self, PyObject* args);
static PyObject* MznModel_addData(MznModel* self, PyObject* args);
static PyObject* MznModel_Declaration(MznModel* self, PyObject* args);
static PyObject* MznModel_Constraint(MznModel* self, PyObject* args);
static PyObject* MznModel_SolveItem(MznModel* self, PyObject* args);
static PyObject* MznModel_Call(MznModel* self, PyObject* args);

static PyObject* MznModel_copy(MznModel* self);

static PyMethodDef MznModel_methods[] = {
  {"load", (PyCFunction)MznModel_load, METH_KEYWORDS, "Load MiniZinc model from MiniZinc file"},
  {"loadFromString", (PyCFunction)MznModel_loadFromString, METH_KEYWORDS, "Load MiniZinc model from standard input"},
  {"solve", (PyCFunction)MznModel_solve, METH_VARARGS, "Solve a loaded MiniZinc model"},
  {"setTimeLimit", (PyCFunction)MznModel_setTimeLimit, METH_VARARGS, "Limit the execution time of the model"},
  {"setSolver", (PyCFunction)MznModel_setSolver, METH_VARARGS, "Choose which model will be used to solve the model"},
  {"Declaration", (PyCFunction)MznModel_Declaration, METH_VARARGS, "Add a variable into the model"},
  {"Constraint", (PyCFunction)MznModel_Constraint, METH_VARARGS, "Add a constraint into the model"},
  {"SolveItem", (PyCFunction)MznModel_SolveItem, METH_VARARGS, "Add a solve item into the model"},

  {"copy", (PyCFunction)MznModel_copy, METH_NOARGS, "Returns a copy of current model"},
  {NULL} /* Sentinel */
};


static PyMemberDef MznModel_members[] = {
  {NULL} /* Sentinel */
};

static PyTypeObject MznModelType = {
  PyVarObject_HEAD_INIT(NULL,0)
  "minizinc_internal.Model",       /* tp_name */
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