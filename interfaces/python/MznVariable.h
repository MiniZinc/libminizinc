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
  bool isExp;     // expression or variable
  vector<pair<int,int> >* dimList;
  MznVariable(Expression* e, bool isExp): e(e), isExp(isExp), dimList(NULL) {}
};

static PyObject* MznVariable_new(PyTypeObject* type, PyObject* args, PyObject* kwds);
static int MznVariable_init(MznVariable* self, PyObject* args);
static void MznVariable_dealloc(MznVariable* self);
PyObject* MznVariable_getValue(MznVariable* self);
PyObject* MznVariable_at(MznVariable* self, PyObject* indexList);






static PyMemberDef MznVariable_members[] = {
  {NULL}
};

static PyMethodDef MznVariable_methods[] = {
  {"getValue", (PyCFunction)MznVariable_getValue, METH_NOARGS, "Return value of the variable"},
  {"at", (PyCFunction)MznVariable_at, METH_VARARGS, "Return an array access"},
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
  (initproc)MznVariable_init,    /* tp_init */
  0,                         /* tp_alloc */
  MznVariable_new,                /* tp_new */
};

static PyObject* MznVariable_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
  MznVariable* self = (MznVariable*)type->tp_alloc(type,0);
  self->e = NULL;
  self->dimList = NULL;
  return (PyObject*)self;
}
static int MznVariable_init(MznVariable* self, PyObject* args) {
  PyErr_SetString(PyExc_TypeError, "This object doesn't support user declaration");
  return -1;
}
static void MznVariable_dealloc(MznVariable* self) {
  if (self->dimList)
    delete self->dimList;
  self->ob_type->tp_free((PyObject*)self);
}



// WARNING: Segmentation fault 11 if used when model is destroyed.
PyObject* MznVariable_getValue(MznVariable* self)
{
  if (self->isExp) {
    PyErr_SetString(PyExc_ValueError, "Cannot evaluate an expression");
    return NULL;
  } else {
    if (((VarDecl*)(self->e))->e() == NULL) {
      Py_RETURN_NONE;
    }
    cout << ((VarDecl*)(self->e))->id()->str().c_str() << endl;
    PyObject* PyValue = minizinc_to_python( (VarDecl*)(self->e) );
    if (PyValue == NULL)
      return NULL;
    return PyValue;
  }
}

PyObject* MznVariable_at(MznVariable* self, PyObject* args)
{
  PyObject* indexList;
  if (!PyArg_ParseTuple(args, "O", &indexList)) {
    PyErr_SetString(PyExc_AttributeError, "Accept 1 tuple object");
    return NULL;
  }
  if (!PyTuple_Check(indexList)) {
    PyErr_SetString(PyExc_TypeError, "Argument must be a tuple");
    return NULL;
  }
  if (self->dimList == NULL) {
    PyErr_SetString(PyExc_TypeError, "This object is not an array");
    return NULL;
  }
  vector<long>::size_type n = self->dimList->size();
  if (n != PyTuple_GET_SIZE(indexList)) {
    PyErr_SetString(PyExc_IndexError, "Mismatched number of dimension");
    return NULL;
  }


  vector<Expression*> idx(n);
  for (vector<long>::size_type i = 0; i!=n; ++i) {
    PyObject* obj = PyTuple_GetItem(indexList, i);
    if (!PyInt_Check(obj)) {
      PyErr_SetString(PyExc_TypeError, "Indices must be integers");
      return NULL;
    }

    long index = PyInt_AS_LONG(obj);
    //if ( (*(self->dimList))[i]    )
    Expression* e = new IntLit(Location(), IntVal(index));
    idx[i] = e;
  }

  MznVariable* ret = (MznVariable*) MznVariable_new(&MznVariableType, NULL, NULL);
  ret->e = new ArrayAccess(Location(), ((VarDecl*)(self->e))->id(), idx);
  ret->isExp = true;
  return (PyObject*) ret;
}


#endif