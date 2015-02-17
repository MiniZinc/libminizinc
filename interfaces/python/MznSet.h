/*
 *  Python Interface for MiniZinc constraint modelling
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *          under the supervision of Guido Tack <guido.tack@monash.edu>
 */

#ifndef __MZNSET_H
#define __MZNSET_H

#include <exception>

struct MznRange {
  long min;
  long max;
  MznRange(long min, long max):min(min),max(max){}
};


struct MznSet {
  PyObject_HEAD
  list<MznRange>* ranges;

  void clear() {ranges->clear();}

  void push(long min, long max);
  void push(long v);

  long min();
  long max();


  // 
  long size() {return ranges->size();}
  bool continuous();
  bool contains(long val);

  SetLit* e() {};
};

static PyObject* MznSet_new(PyTypeObject *type, PyObject* args, PyObject* kwds);
static void MznSet_dealloc(MznSet* self);
static PyObject* MznSet_push(MznSet* self, PyObject* args);
static int MznSet_init(MznSet* self, PyObject* args);
static PyObject* MznSet_output(MznSet* self);
static PyObject* MznSet_repr(PyObject* self);
static PyObject* MznSet_iter(PyObject* self);
static PyObject* MznSet_min(MznSet* self);
static PyObject* MznSet_max(MznSet* self);
static PyObject* MznSet_continuous(MznSet* self);
static PyObject* MznSet_contains(MznSet* self, PyObject* args);

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

static PyTypeObject MznSetType = {
  PyVarObject_HEAD_INIT(NULL,0)
  "minizinc.set",       /* tp_name */
  sizeof(MznSet),          /* tp_basicsize */
  0,                         /* tp_itemsize */
  (destructor)MznSet_dealloc, /* tp_dealloc */
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
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_ITER,        /* tp_flags */
  "Minizinc Set",  /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  MznSet_iter,                         /* tp_iter */
  //MznSet_iternext,                         /* tp_iternext */
  0,
  MznSet_methods,            /* tp_methods */
  MznSet_members,            /* tp_members */
  0,/*MznModel_getseters,        /* tp_getset */
  0,                         /* tp_base */
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
  long currentValue;
};

static PyObject* MznSetIter_new(PyTypeObject *type, PyObject* args, PyObject* kwds);
static void MznSetIter_dealloc(MznSetIter* self);
static PyObject* MznSetIter_iternext(PyObject* self);

static PyTypeObject MznSetIterType = {
  PyVarObject_HEAD_INIT(NULL,0)
  "minizinc.setiterator",       /* tp_name */
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
  MznSet_new,                /* tp_new */
};

#endif