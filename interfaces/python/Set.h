/*  Python Interface for MiniZinc constraint modelling
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *  Supervisor:
 *     Guido Tack <guido.tack@monash.edu>
 */


#ifndef __MZNSET_H
#define __MZNSET_H

#include "Object.h"
#include <vector>
#include <list>
#include <exception>

#if PY_MAJOR_VERSION >= 3
#  define Py_TPFLAGS_HAVE_ITER 0
#endif

using namespace std;
using namespace MiniZinc;

struct MznRange {
  long long min;
  long long max;
  MznRange(long long min, long long max):min(min),max(max){}
};


/* 
 *  My custom defined Set
 *
 *  MznSet_init requires arguments, basically we cannot call minizinc_internal.Set()  (no argument)
 *  The reason is if the set is empty, a call to Expression* e() would fail.
 *  
 *  The input to MznSet_push take 1 list argument:
 *        [item1, item2, item3]
 *   For each item, it can be either a value or a list of min and max values:
 *          item? = 1,4 or 5 ....
 *       or item? = [1,10] or [5,6]
 *   For example, a Set of 1..5, 7, 10..15 can be defined like this:
 *        minizinc_internal.Set( [ [1,5], 7, [10,15] ])
 *   The set will automatically merge ranges if it can.
 *
 *   XXX: should a Set of {1,2,6,35} be written as 1..2, 6, 35
 *                                              or 1, 2, 6, 35?
 */

struct MznSet: MznObject {
  list<MznRange>* ranges;

  void clear() {ranges->clear();}

  void push(long long min, long long max);
  void push(long long v);

  long long min();
  long long max();


  long long size() {return ranges->size();}
  bool continuous();
  bool contains(long long val);


  // Requires an outer GC Lock
  Expression* e() {
    vector<IntSetVal::Range> setRanges;
    for (list<MznRange>::const_iterator it = ranges->begin(); it != ranges->end(); ++it) {
      setRanges.push_back(IntSetVal::Range(IntVal(it->min),IntVal(it->max)));
    }
    Expression* rhs = new SetLit(Location(), IntSetVal::a(setRanges));
    return rhs;
  }
};

// Set representation on terminal command
// XXX: should be redefined as MznSet_str
static PyObject* MznSet_repr(PyObject* self);

static PyObject* MznSet_push(MznSet* self, PyObject* args);
static PyObject* MznSet_output(MznSet* self);
static PyObject* MznSet_min(MznSet* self);
static PyObject* MznSet_max(MznSet* self);
static PyObject* MznSet_continuous(MznSet* self);
static PyObject* MznSet_contains(MznSet* self, PyObject* args);

static int MznSet_init(MznSet* self, PyObject* args);
static PyObject* MznSet_new(PyTypeObject *type, PyObject* args, PyObject* kwds);
static void MznSet_dealloc(MznSet* self);
static PyObject* MznSet_iter(PyObject* self);

static PyMethodDef MznSet_methods[] = {
  {"output", (PyCFunction)MznSet_output, METH_NOARGS, "Return all values in the set"},
  {"push", (PyCFunction)MznSet_push, METH_VARARGS, "Expand the set"},
  {"min", (PyCFunction)MznSet_min, METH_NOARGS, "Lower bound of the set"},
  {"max", (PyCFunction)MznSet_max, METH_NOARGS, "Upper bound of the set"},
  {"continuous", (PyCFunction)MznSet_continuous, METH_NOARGS, "Check whether the set is continous"},
  {"contains", (PyCFunction)MznSet_contains, METH_VARARGS, "Check whether a python value is in the Set"},
  {NULL}    /* Sentinel */
};

static PyMemberDef MznSet_members[] = {
  {NULL} /* Sentinel */
};



static PyTypeObject MznSet_Type = {
  PyVarObject_HEAD_INIT(NULL,0)
  "minizinc.Set",            /* tp_name */
  sizeof(MznSet),            /* tp_basicsize */
  0,                         /* tp_itemsize */
  (destructor)MznSet_dealloc,/* tp_dealloc */
  0,                         /* tp_print */
  0,                         /* tp_getattr */
  0,                         /* tp_setattr */
  0,                         /* tp_reserved */
  MznSet_repr,               /* tp_repr */
  0,                         /* tp_as_number */
  0,                         /* tp_as_sequence */
  0,                         /* tp_as_mapping */
  0,                         /* tp_hash  */
  0,                         /* tp_call */
  0,                         /* tp_str */
  0,                         /* tp_getattro */
  0,                         /* tp_setattro */
  0,                         /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT  | Py_TPFLAGS_HAVE_ITER,        /* tp_flags */
  "Minizinc Set Object",     /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  MznSet_iter,               /* tp_iter */
  0,                         /* tp_iternext */
  MznSet_methods,            /* tp_methods */
  MznSet_members,            /* tp_members */
  0,                         /* tp_getset */
  &MznObject_Type,           /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  (initproc)MznSet_init,     /* tp_init */
  0,                         /* tp_alloc */
  MznSet_new,                /* tp_new */
};


struct MznSetIter {
  // To support python iteration
  PyObject_HEAD
  list<MznRange>::const_iterator listIndex;
  list<MznRange>::const_iterator listBegin;
  list<MznRange>::const_iterator listEnd;
  long long currentValue;
};

static PyObject* MznSetIter_new(PyTypeObject *type, PyObject* args, PyObject* kwds);
static void MznSetIter_dealloc(MznSetIter* self);
static PyObject* MznSetIter_iternext(PyObject* self);

static PyTypeObject MznSetIter_Type = {
  PyVarObject_HEAD_INIT(NULL,0)
  "minizinc.SetIterator",       /* tp_name */
  sizeof(MznSetIter),          /* tp_basicsize */
  0,                         /* tp_itemsize */
  (destructor)MznSetIter_dealloc, /* tp_dealloc */
  0,                         /* tp_print */
  0,                         /* tp_getattr */
  0,                         /* tp_setattr */
  0,                         /* tp_reserved */
  0,               /* tp_repr */
  0,                         /* tp_as_number */
  0,                         /* tp_as_sequence */
  0,                         /* tp_as_mapping */
  0,                         /* tp_hash  */
  0,                         /* tp_call */
  0,                         /* tp_str */
  0,                         /* tp_getattro */
  0,                         /* tp_setattro */
  0,                         /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_ITER,        /* tp_flags */
  "Minizinc Set Iterator",  /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  PyObject_SelfIter,                         /* tp_iter */
  (iternextfunc)MznSetIter_iternext,                         /* tp_iternext */
  0,            /* tp_methods */
  0,            /* tp_members */
  0,/*MznModel_getseters,        /* tp_getset */
  0,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  0,     /* tp_init */
  0,                         /* tp_alloc */
  0,                /* tp_new */
};

#endif