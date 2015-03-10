#ifndef __Mzn_OBJECT_H
#define __Mzn_OBJECT_H

enum Mzn_Object_Code {
  MOC_BASE, MOC_EXPR, MOC_ANN,
                      MOC_VARSET,

            MOC_DECL, MOC_VAR,
                      MOC_ARR,
            MOC_SET
};

struct MznObject {
  PyObject_HEAD
  Mzn_Object_Code tid;
};

static PyObject* MznObject_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
  PyObject* self = type->tp_alloc(type,0);
  reinterpret_cast<MznObject*>(self)->tid = MOC_BASE;
  return self;
}


static void MznObject_dealloc(MznObject* self) {
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

static PyTypeObject MznObject_Type = {
  PyVarObject_HEAD_INIT(NULL,0)
  "minizinc.Object",       /* tp_name */
  sizeof(MznObject),          /* tp_basicsize */
  0,                         /* tp_itemsize */
  (destructor)MznObject_dealloc, /* tp_dealloc */
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
  Py_TPFLAGS_BASETYPE,        /* tp_flags */
  "Minizinc Object",  /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  0,                         /* tp_iter */
  0,                         /* tp_iternext */
  0,            /* tp_methods */
  0,            /* tp_members */
  0,        /* tp_getset */
  0,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  0,    /* tp_init */
  0,                         /* tp_alloc */
  MznObject_new,                /* tp_new */
  0,                         /* tp_free */
};


#include "Set.h"
#include "Expression.h"
#include "Annotation.h"
#include "Declaration.h"
#include "Variable.h"
#include "Array.h"
#include "VarSet.h"

// Note: I have tried adding a function Expression* e() to MznObject,
//      but it didn't work well. The derived objects, after being initialized by PyObject_INIT
//      returns unusable derived functions.
// While this maybe a little bit annoying, it is the best solution that I can come up with.
//
// Returns: MiniZinc expression Expression* 
static Expression* MznObject_get_e(MznObject* self) {
  switch (self->tid) {
    case MOC_BASE: 
      return NULL;
    case MOC_SET:
      return reinterpret_cast<MznSet*>(self)->e();
    case MOC_EXPR:
    case MOC_ANN:
      return reinterpret_cast<MznExpression*>(self)->e;
    case MOC_DECL:
    case MOC_VAR:
    case MOC_ARR:
    case MOC_VARSET:
      return reinterpret_cast<MznDeclaration*>(self)->e;
    default:
      throw logic_error("Unhandled type");
  }
}


#endif