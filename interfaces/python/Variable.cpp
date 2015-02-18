/*  Python Interface for MiniZinc constraint modelling
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *  Supervisor:
 *     Guido Tack <guido.tack@monash.edu>
 */

#include "Variable.h"

using namespace std;
using namespace MiniZinc;

static PyObject* MznVariable_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
  MznVariable* self = reinterpret_cast<MznVariable*>(type->tp_alloc(type,0));
  self->e = NULL;
  self->vd = NULL;
  self->dimList = NULL;
  self->isSet = false;
  return reinterpret_cast<PyObject*>(self);
}
static int MznVariable_init(MznVariable* self, PyObject* args) {
  PyErr_SetString(PyExc_TypeError, "This object doesn't support user declaration");
  return -1;
}
static void MznVariable_dealloc(MznVariable* self) {
  if (self->dimList)
    delete self->dimList;
  self->ob_type->tp_free(reinterpret_cast<PyObject*>(self));
}


// WARNING: Segmentation fault 11 if used when model is destroyed.
PyObject* MznVariable_getValue(MznVariable* self)
{

  if (self->isVar() == false) {
    PyErr_SetString(PyExc_ValueError, "Cannot evaluate an expression");
    return NULL;
  } else {
    if (self->vd->e() == NULL) {
      //PyErr_SetString(PyExc_ValueError, "Value is not set or it's model is not solved");
      //return NULL;
      Py_RETURN_NONE;
    }
    PyObject* PyValue = minizinc_to_python( self->vd );
    if (PyValue == NULL)
      return NULL;
    return PyValue;
  }
}

PyObject* MznVariable_setValue(MznVariable* self, PyObject* args)
{
  PyObject* pyval;
  if (self->isVar() == false) {
    PyErr_SetString(PyExc_ValueError, "Cannot set value to an expression");
    return NULL;
  } else if (!PyArg_ParseTuple(args, "O", &pyval)) {
    PyErr_SetString(PyExc_AttributeError, "Parsing error: Require an int, float, string or minizinc set");
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
    //self->e->cast<Id>()->type().ti(Type::TI_PAR);
    Py_RETURN_NONE;
  }
}

PyObject* MznVariable_at(MznVariable* self, PyObject* args)
{
  PyObject* indexList;
  if (!PyArg_ParseTuple(args, "O", &indexList)) {
    PyErr_SetString(PyExc_AttributeError, "Accept 1 Python list object");
    return NULL;
  }
  if (!PyList_Check(indexList)) {
    PyErr_SetString(PyExc_TypeError, "Argument must be a list");
    return NULL;
  }
  if (self->dimList == NULL) {
    PyErr_SetString(PyExc_TypeError, "This object is not an array");
    return NULL;
  }
  vector<long>::size_type n = self->dimList->size();
  if (n != PyList_GET_SIZE(indexList)) {
    PyErr_SetString(PyExc_IndexError, "Mismatched number of dimension");
    return NULL;
  }


  vector<Expression*> idx(n);
  for (vector<long>::size_type i = 0; i!=n; ++i) {
    PyObject* obj = PyList_GetItem(indexList, i);
    if (PyInt_Check(obj)) {
      long index = PyInt_AS_LONG(obj);
      if (index<((*(self->dimList))[i]).first || index > ((*(self->dimList))[i]).second) {
        PyErr_SetString(PyExc_IndexError, "Index is out of range");
        return NULL;
      }
      idx[i] = new IntLit(Location(), IntVal(index));
    } else if (PyObject_TypeCheck(obj,&MznVariableType)) {
      idx[i] = reinterpret_cast<MznVariable*>(obj)->e;
    } else {
      PyErr_SetString(PyExc_TypeError, "Indices must be integers, MiniZinc Expression or MiniZinc Variable");
      return NULL;
    }
  }

  MznVariable* ret = reinterpret_cast<MznVariable*> (MznVariable_new(&MznVariableType, NULL, NULL));
  ret->e = new ArrayAccess(Location(), self->e, idx);
  ret->vd = NULL;
  return reinterpret_cast<PyObject*>(ret);
}