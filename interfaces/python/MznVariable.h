/*
 *  Python Interface for MiniZinc constraint modelling
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *          under the supervision of Guido Tack <guido.tack@monash.edu>
 */

#ifndef __MZNVARIABLE_H
#define __MZNVARIABLE_H

using namespace std;

struct MznVariable {
  PyObject_HEAD
  Expression* e;
};

static PyObject* MznVariable_new(PyTypeObject* type, PyObject* args, PyObject* kwds);
static int MznVariable_init(MznVariable* self, PyObject* args);
static void MznVariable_dealloc(MznVariable* self);

static PyMemberDef MznVariable_members[] = {
  {NULL}
};

static PyMethodDef MznVariable_methods[] = {
  {NULL}
};

static PyTypeObject MznVariableType = {
  PyVarObject_HEAD_INIT(NULL,0)
  "minizinc.Variable",       /* tp_name */
  sizeof(MznVariable),          /* tp_basicsize */
  0,                         /* tp_itemsize */
  (destructor)MznVariable_dealloc, /* tp_dealloc */
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
  "Minizinc Variable",  /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  0,                         /* tp_iter */
  0,                         /* tp_iternext */
  MznVariable_methods,            /* tp_methods */
  MznVariable_members,            /* tp_members */
  0,/*MznVariable_getseters,        /* tp_getset */
  0,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  (initproc)MznVariable_init,     /* tp_init */
  0,                         /* tp_alloc */
  MznVariable_new,                /* tp_new */
};

static PyObject* MznVariable_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
  MznVariable* self = (MznVariable*)type->tp_alloc(type,0);
  self->e = NULL;
  return (PyObject*)self;
}
static int MznVariable_init(MznVariable* self, PyObject* args) {
  PyErr_SetString(PyExc_TypeError, "This object does not support user declaration");
  return -1;
}
static void MznVariable_dealloc(MznVariable* self) {
  self->ob_type->tp_free((PyObject*)self);
}





#endif