/*
 *  Python Interface for MiniZinc constraint modelling
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *          under the supervision of Guido Tack <guido.tack@monash.edu>
 */

#ifndef __MZNSET_H
#define __MZNSET_H


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

  long size() {return ranges->size();}
};

static PyObject* MznSet_new(PyTypeObject *type, PyObject* args, PyObject* kwds);
static void MznSet_dealloc(MznSet* self);
static PyObject* MznSet_new(PyTypeObject *type, PyObject* args, PyObject* kwds);
static PyObject* MznSet_push(MznSet* self, PyObject* args);
static int MznSet_init(MznSet* self, PyObject* args);
static PyObject* MznSet_output(MznSet* self);
static PyObject* MznSet_repr(PyObject* self);

static PyMethodDef MznSet_methods[] = {
  {"output", (PyCFunction)MznSet_output, METH_NOARGS, "Return all values in the set"},
  {"output", (PyCFunction)MznSet_output, METH_VARARGS, "Return all values in the set"},
  {"push", (PyCFunction)MznSet_push, METH_VARARGS, "Expand the set"},
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
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
  "Minizinc Set",  /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  0,                         /* tp_iter */
  0,                         /* tp_iternext */
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

#endif