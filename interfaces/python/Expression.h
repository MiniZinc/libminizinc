#ifndef __Mzn_EXPRESSIONOBJECT_H
#define __Mzn_EXPRESSIONOBJECT_H

#include "Object.h"

struct MznExpression: MznObject {
  Expression* e;
};

/*inline Expression* MznObject_get_e(MznExpression* self) { return self->e; }
inline void MznObject_set_e(MznExpression* self, Expression* e0) { self->e = e0; }*/


static PyObject* MznExpression_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
  MznExpression* self = reinterpret_cast<MznExpression*>(type->tp_alloc(type,0));
  reinterpret_cast<MznObject*>(self)->tid = MOC_EXPR;
  self->e = NULL;
  return reinterpret_cast<PyObject*>(self);
}

static void MznExpression_dealloc(MznExpression* self) {
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

static PyTypeObject MznExpression_Type = {
  PyVarObject_HEAD_INIT(NULL,0)
  "minizinc.Expression",       /* tp_name */
  sizeof(MznExpression),          /* tp_basicsize */
  0,                         /* tp_itemsize */
  (destructor)MznExpression_dealloc, /* tp_dealloc */
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
  "Minizinc Expression Object (derived from MznObject)",  /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  0,                         /* tp_iter */
  0,                         /* tp_iternext */
  0,            /* tp_methods */
  0,            /* tp_members */
  0,        /* tp_getset */
  &MznObject_Type,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  0,    /* tp_init */
  0,                         /* tp_alloc */
  MznExpression_new,                /* tp_new */
  0,                         /* tp_free */
};



#endif