/*
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *  Supervisor:
 *     Guido Tack <guido.tack@monash.edu>
 */
#ifndef __PYINTERFACE_H
#define __PYINTERFACE_H

#include "global.h"
#include "Set.h"
#include "Variable.h"
#include "Model.h"
#include "Solver.h"

#include "global.cpp"
#include "Set.cpp"
#include "Variable.cpp"
#include "Model.cpp"
#include "Solver.cpp"

static PyObject* Mzn_load(PyObject* self, PyObject* args, PyObject* keywds);
static PyObject* Mzn_loadFromString(PyObject* self, PyObject* args, PyObject* keywds);
static PyObject* Mzn_BinOp(MznModel* self, PyObject* args);
static PyObject* Mzn_UnOp(MznModel* self, PyObject* args);
static PyObject* Mzn_Call(MznModel* self, PyObject* args);
static PyObject* Mzn_lock(MznModel* self) {GC::lock(); Py_RETURN_NONE;}
static PyObject* Mzn_unlock(MznModel* self) {GC::unlock(); Py_RETURN_NONE;}

static PyMethodDef Mzn_methods[] = {
  {"load", (PyCFunction)Mzn_load, METH_KEYWORDS, "Load MiniZinc model from MiniZinc file"},
  {"loadFromString", (PyCFunction)Mzn_load, METH_KEYWORDS, "Load MiniZinc model from stdin"},
  {"BinOp", (PyCFunction)Mzn_BinOp, METH_VARARGS, "Add a binary expression into the model"},
  {"UnOp", (PyCFunction)Mzn_UnOp, METH_VARARGS, "Add a unary expression into the model"},
  {"Call", (PyCFunction)Mzn_Call, METH_VARARGS, ""},
  {"lock", (PyCFunction)Mzn_lock, METH_NOARGS, "Internal: Create a lock for garbage collection"},
  {"unlock", (PyCFunction)Mzn_lock, METH_NOARGS, "Internal: Unlock a lock for garbage collection"},
  {NULL}
};

#endif