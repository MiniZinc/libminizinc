#ifndef __Mzn_DECLARATIONOBJECT_H
#define __Mzn_DECLARATIONOBJECT_H

#include "Object.h"

struct MznDeclaration: MznObject {
  Expression* e;
  VarDecl* vd;
};

/*inline Expression* MznObject_get_e(MznDeclaration* self) { return self->e; }
inline void MznObject_set_e(MznDeclaration* self, Expression* e0) { self->e = e0; }
inline VarDecl* MznObject_get_vd(MznDeclaration* self) { return self->vd; }
inline void MznObject_set_vd(MznDeclaration* self, VarDecl* vd0) { self->vd = vd0; }*/


static PyObject* MznDeclaration_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
  MznDeclaration* self = reinterpret_cast<MznDeclaration*>(type->tp_alloc(type,0));
  reinterpret_cast<MznObject*>(self)->tid = MOC_DECL;
  self->e = NULL;
  self->vd = NULL;
  return reinterpret_cast<PyObject*>(self);
}

static void MznDeclaration_dealloc(MznDeclaration* self) {
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

// WARNING: Segmentation fault 11 if used when model is destroyed.
PyObject* MznDeclaration_getValue(MznDeclaration* self);

PyObject* MznDeclaration_setValue(MznDeclaration* self, PyObject* args);

static PyMethodDef MznDeclaration_methods[] = {
  {"getValue", (PyCFunction)MznDeclaration_getValue, METH_NOARGS, "Return value of the declaration"},
  {"setValue", (PyCFunction)MznDeclaration_setValue, METH_VARARGS, "Set value of the declaration"},
  {NULL}
};

static PyTypeObject MznDeclaration_Type = {
  PyVarObject_HEAD_INIT(NULL,0)
  "minizinc.Declaration",       /* tp_name */
  sizeof(MznDeclaration),          /* tp_basicsize */
  0,                         /* tp_itemsize */
  (destructor)MznDeclaration_dealloc, /* tp_dealloc */
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
  "Minizinc Declaration (derived from MznObject)",  /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  0,                         /* tp_iter */
  0,                         /* tp_iternext */
  MznDeclaration_methods,            /* tp_methods */
  0,            /* tp_members */
  0,        /* tp_getset */
  &MznObject_Type,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  0,    /* tp_init */
  0,                         /* tp_alloc */
  MznDeclaration_new,                /* tp_new */
  0,                         /* tp_free */
};



PyObject* MznDeclaration_getValue(MznDeclaration* self)
{
  if (self->vd->e() == NULL)
    Py_RETURN_NONE;

  PyObject* PyValue = minizinc_to_python(self->vd);
  if (PyValue == NULL)
    return NULL;
  return PyValue;
}

PyObject* MznDeclaration_setValue(MznDeclaration* self, PyObject* args)
{
  PyObject* pyval;
  if (!PyArg_ParseTuple(args, "O", &pyval)) {
    PyErr_SetString(PyExc_TypeError, "Parsing error: Set value accepts 1 argument");
    return NULL;
  } else  {
    GCLock Lock;
    Type valueType;
    Type declaredType = self->e->cast<Id>()->type();
    vector<pair<int, int> > dimList;
    Expression* e = python_to_minizinc(pyval, valueType, dimList);
    if (!compareType(declaredType,valueType)) {
      string err = "Value does not match declared variable: expected " + typePresentation(declaredType) +
                    + ", received " + typePresentation(valueType);
      PyErr_SetString(PyExc_ValueError, err.c_str());
      return NULL;
    }
    self->vd->e(e);
    Py_RETURN_NONE;
  }
}


#endif