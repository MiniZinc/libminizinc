#ifndef __Mzn_ANNOTATIONOBJECT_H
#define __Mzn_ANNOTATIONOBJECT_H

#include "Expression.h"

struct MznAnnotation: MznExpression {

};

/*inline Expression* MznObject_get_e(MznAnnotation* self) { return MznObject_get_e(reinterpret_cast<MznExpression*>(self)); }
inline void MznObject_set_e(MznAnnotation* self, Expression* e0) { MznObject_set_e(reinterpret_cast<MznExpression*>(self), e0); }*/


static PyObject* MznAnnotation_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
  MznAnnotation* self = reinterpret_cast<MznAnnotation*>(type->tp_alloc(type,0));
  reinterpret_cast<MznObject*>(self)->tid = MOC_ANN;
  self->e = NULL;
  return reinterpret_cast<PyObject*>(self);
}

static void MznAnnotation_dealloc(MznAnnotation* self) {
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

static PyTypeObject MznAnnotation_Type = {
  PyVarObject_HEAD_INIT(NULL,0)
  "minizinc.Annotation",       /* tp_name */
  sizeof(MznAnnotation),          /* tp_basicsize */
  0,                         /* tp_itemsize */
  (destructor)MznAnnotation_dealloc, /* tp_dealloc */
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
  Py_TPFLAGS_DEFAULT,        /* tp_flags */
  "Minizinc Variable (derived from MznExpression)",  /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  0,                         /* tp_iter */
  0,                         /* tp_iternext */
  0,            /* tp_methods */
  0,            /* tp_members */
  0,/*MznAnnotation_getseters,        /* tp_getset */
  &MznExpression_Type,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  0,    /* tp_init */
  0,                         /* tp_alloc */
  MznAnnotation_new,                /* tp_new */
  0,                         /* tp_free */
};



#endif