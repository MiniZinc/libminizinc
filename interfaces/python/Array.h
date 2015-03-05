#ifndef __Mzn_ARRAYOBJECT_H
#define __Mzn_ARRAYOBJECT_H

#include "Declaration.h"

struct MznArray: MznDeclaration {
  vector<pair<int, int> > *dimList;
};

/*inline Expression* MznObject_get_e(MznArray* self) { return MznObject_get_e(reinterpret_cast<MznDeclaration*>(self)); }
inline void MznObject_set_e(MznArray* self, Expression* e0) { MznObject_set_e(reinterpret_cast<MznDeclaration*>(self), e0); }
inline VarDecl* MznObject_get_vd(MznArray* self) { return MznObject_get_vd(reinterpret_cast<MznDeclaration*>(self)); }
inline void MznObject_set_vd(MznArray* self, VarDecl* vd0) { MznObject_set_vd(reinterpret_cast<MznDeclaration*>(self), vd0); }*/

static PyObject* MznArray_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
  MznArray* self = reinterpret_cast<MznArray*>(type->tp_alloc(type,0));
  reinterpret_cast<MznObject*>(self)->tid = MOC_ARR;
  self->e = NULL;
  self->vd = NULL;
  self->dimList = NULL;
  return reinterpret_cast<PyObject*>(self);
}

static void MznArray_dealloc(MznArray* self) {
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

PyObject* MznArray_at(MznArray* self, PyObject* indexList);

static PyMethodDef MznArray_methods[] = {
  {"at", (PyCFunction)MznArray_at, METH_VARARGS, "Return an array access"},
  {NULL}
};


static PyTypeObject MznArray_Type = {
  PyVarObject_HEAD_INIT(NULL,0)
  "minizinc.Array",       /* tp_name */
  sizeof(MznArray),          /* tp_basicsize */
  0,                         /* tp_itemsize */
  (destructor)MznArray_dealloc, /* tp_dealloc */
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
  "Minizinc Array (derived from MznDeclaration)",  /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  0,                         /* tp_iter */
  0,                         /* tp_iternext */
  MznArray_methods,            /* tp_methods */
  0,            /* tp_members */
  0,/*MznArray_getseters,        /* tp_getset */
  &MznDeclaration_Type,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  0,    /* tp_init */
  0,                         /* tp_alloc */
  MznArray_new,                /* tp_new */
  0,                         /* tp_free */
};

PyObject* MznArray_at(MznArray* self, PyObject* args)
{
  PyObject* indexList;
  if (!PyArg_ParseTuple(args, "O", &indexList)) {
    PyErr_SetString(PyExc_AttributeError, "MiniZinc: ArrayAccess:  Accept 1 Python list object");
    return NULL;
  }
  if (!PyList_Check(indexList)) {
    PyErr_SetString(PyExc_TypeError, "MiniZinc: ArrayAccess:  Argument must be a list");
    return NULL;
  }
  if (self->dimList == NULL) {
    PyErr_SetString(PyExc_TypeError, "MiniZinc: ArrayAccess:  Array dim list not initialized");
    return NULL;
  }
  vector<long>::size_type n = self->dimList->size();
  if (n != PyList_GET_SIZE(indexList)) {
    MZN_PYERR_SET_STRING(PyExc_IndexError, "MiniZinc: ArrayAccess:  Mismatched number of dimension, expected %li, received %li", n, PyList_GET_SIZE(indexList));
    return NULL;
  }


  vector<Expression*> idx(n);
  for (vector<long>::size_type i = 0; i!=n; ++i) {
    PyObject* obj = PyList_GetItem(indexList, i);
#if Py_MAJOR_VERSION < 3
    if (PyInt_Check(obj)) {
      long index = PyInt_AS_LONG(obj);
      if (index<((*(self->dimList))[i]).first || index > ((*(self->dimList))[i]).second) {
        MZN_PYERR_SET_STRING(PyExc_IndexError, "MiniZinc: ArrayAccess:  Index at pos %li out of range", i);
        return NULL;
      }
      idx[i] = new IntLit(Location(), IntVal(index));
    } else
#endif
    if (PyLong_Check(obj)) {
      int overflow;
      long long index = PyLong_AsLongLongAndOverflow(obj, &overflow);
      if (overflow) {
        MZN_PYERR_SET_STRING(PyExc_OverflowError, "MiniZinc: ArrayAccess:  Index at pos %li overflowed", i);
        return NULL;
      }
      if (index<((*(self->dimList))[i]).first || index > ((*(self->dimList))[i]).second) {
        MZN_PYERR_SET_STRING(PyExc_IndexError, "MiniZinc: ArrayAccess:  Index at pos %li out of range", i);
        return NULL;
      }
      idx[i] = new IntLit(Location(), IntVal(index));
    } else if (PyObject_TypeCheck(obj,&MznArray_Type)) {
      idx[i] = reinterpret_cast<MznArray*>(obj)->e;
    } else {
      PyErr_SetString(PyExc_TypeError, "MiniZinc: ArrayAccess:  Indices must be integers, MiniZinc Expression or MiniZinc Array");
      return NULL;
    }
  }

  MznVariable* ret = reinterpret_cast<MznVariable*> (MznVariable_new(&MznVariable_Type, NULL, NULL));
  ret->e = new ArrayAccess(Location(), self->e, idx);
  return reinterpret_cast<PyObject*>(ret);
}

#endif