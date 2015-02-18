/*  Python Interface for MiniZinc constraint modelling
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *  Supervisor:
 *     Guido Tack <guido.tack@monash.edu>
 */
 
#ifndef __MZNVARIABLE_H
#define __MZNVARIABLE_H



using namespace std;
using namespace MiniZinc;

struct MznVariable {
  PyObject_HEAD
  Expression* e;
  VarDecl* vd;
  bool isVar() {return vd != NULL;}     // expression or variable
  vector<pair<int, int> >* dimList;
  MznVariable(Expression* e): e(e), vd(vd), dimList(NULL) {}

  bool isSet;
};

static PyObject* MznVariable_new(PyTypeObject* type, PyObject* args, PyObject* kwds);
static int MznVariable_init(MznVariable* self, PyObject* args);
static void MznVariable_dealloc(MznVariable* self);

// WARNING: Segmentation fault 11 if used when model is destroyed.
PyObject* MznVariable_getValue(MznVariable* self);
PyObject* MznVariable_setValue(MznVariable* self, PyObject* args);
PyObject* MznVariable_at(MznVariable* self, PyObject* indexList);






static PyMemberDef MznVariable_members[] = {
  {NULL}
};

static PyMethodDef MznVariable_methods[] = {
  {"getValue", (PyCFunction)MznVariable_getValue, METH_NOARGS, "Return value of the variable"},
  {"setValue", (PyCFunction)MznVariable_setValue, METH_VARARGS, "Set value of the variable"},
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


#endif