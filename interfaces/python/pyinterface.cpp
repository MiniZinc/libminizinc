/*  Python Interface for MiniZinc constraint modelling
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *  Supervisor:
 *     Guido Tack <guido.tack@monash.edu>
 */

#include "pyinterface.h";

using namespace MiniZinc;
using namespace std;


/* 
 * Description: Converts a minizinc call
 * Note: Need an outer GCLock for this to work
 */
static PyObject*
Mzn_Call(MznModel* self, PyObject* args)
{
  const char* name;
  PyObject* variableTuple;
  if (!PyArg_ParseTuple(args, "sO", &name, &variableTuple)) {
    PyErr_SetString(PyExc_TypeError, "Accepts two values: a string and a tuple of minizinc variable");
    PyErr_Print();
    return NULL;
  }

  if (!PyList_Check(variableTuple)) {
    PyErr_SetString(PyExc_TypeError, "Second argument must be a list");
    return NULL;
  }

  long len = PyList_GET_SIZE(variableTuple);
  vector<Expression*> expressionList(len);
  for (long i = 0; i!=len; ++i) {
    PyObject* pyval = PyList_GET_ITEM(variableTuple, i);
    if (PyObject_TypeCheck(pyval, &MznVariableType)) {
      expressionList[i] = (reinterpret_cast<MznVariable*>(pyval)) -> e;
    } else {
      Type type;
      vector<pair<int, int> > dimList;
      expressionList[i] = python_to_minizinc(pyval, type, dimList);
      if (expressionList[i] == NULL) {
        char buffer[100];
        sprintf(buffer, "Item at position %ld must be a MiniZinc Variable or Python int/float/string/list/tuple", i);
        PyErr_SetString(PyExc_RuntimeError, buffer);
        return NULL;
      }
    }
  }
  MznVariable* var = reinterpret_cast<MznVariable*>(MznVariable_new(&MznVariableType, NULL, NULL));
  var->e = new Call(Location(), string(name), expressionList);
  var->dimList = NULL;

  return reinterpret_cast<PyObject*>(var);
}


/* 
 * Description: Creates a minizinc UnOp expression
 * Note: Need an outer GCLock for this to work
 */
static PyObject*
Mzn_UnOp(MznModel* self, PyObject* args)
{
  /*
  enum UnOpType {
    UOT_NOT,          // 0
    UOT_PLUS,         // 1
    UOT_MINUS         // 2
  };*/
  PyObject* r;
  unsigned int op;
  if (!PyArg_ParseTuple(args, "IO", &op, &r)) {
    PyErr_SetString(PyExc_TypeError, "Requires a MznVariable/MznConstraint object and an integer");
    return NULL;
  }
  Expression *rhs;

  if (PyObject_TypeCheck(r, &MznVariableType)) {
    rhs = (reinterpret_cast<MznVariable*>(r))->e;
  } else if (PyBool_Check(r)) {
    rhs = new BoolLit(Location(), PyInt_AS_LONG(r));
  } else if (PyInt_Check(r)) {
    rhs = new IntLit(Location(), IntVal(PyInt_AS_LONG(r)));
  } else if (PyFloat_Check(r)) {
    rhs = new FloatLit(Location(), PyFloat_AS_DOUBLE(r));
  } else if (PyString_Check(r)) {
    rhs = new StringLit(Location(), string(PyString_AS_STRING(r)));
  } else {
    PyErr_SetString(PyExc_TypeError, "Object must be of type MznVariable or MznConstraint");
    return NULL;
  }


  GCLock Lock;

  PyObject* var = MznVariable_new(&MznVariableType, NULL, NULL);
  (reinterpret_cast<MznVariable*>(var))->e = new UnOp(Location(), static_cast<UnOpType>(op), rhs);

  return var;
}

// Need an outer GCLock
static PyObject*
Mzn_Id(MznModel* self, PyObject* args)
{
  const char* name;
  if (!PyArg_ParseTuple(args, "s", &name)) {
    PyErr_SetString(PyExc_TypeError, "Argument must be a string");
    return NULL;
  }
  MznVariable* var = reinterpret_cast<MznVariable*>(MznVariable_new(&MznVariableType, NULL, NULL));
  var->e = new Id(Location(), name, NULL);
  return reinterpret_cast<PyObject*>(var);
}


/* 
 * Description: Creates a minizinc BinOp expression
 * Note: Need an outer GCLock for this to work
 */
static PyObject* 
Mzn_BinOp(MznModel* self, PyObject* args)
{
  /*
  enum BinOpType {
    BOT_PLUS,         // 0
    BOT_MINUS,        // 1
    BOT_MULT,         // 2
    BOT_DIV,          // 3
    BOT_IDIV,         // 4
    BOT_MOD,          // 5
    BOT_LE,           // 6
    BOT_LQ,           // 7
    BOT_GR,           // 8
    BOT_GQ,           // 9
    BOT_EQ,           //10
    BOT_NQ,           //11
    BOT_IN,           //12
    BOT_SUBSET,       //13
    BOT_SUPERSET,     //14
    BOT_UNION,        //15
    BOT_DIFF,         //16
    BOT_SYMDIFF,      //17
    BOT_INTERSECT,    //18
    BOT_PLUSPLUS,     //19
    BOT_EQUIV,        //20
    BOT_IMPL,         //21
    BOT_RIMPL,        //22
    BOT_OR,           //23
    BOT_AND,          //24
    BOT_XOR,          //25
    BOT_DOTDOT        //26
  };*/
  PyObject* l;
  PyObject* r;
  unsigned int op;
  if (!PyArg_ParseTuple(args, "OIO", &l, &op, &r)) {
    PyErr_SetString(PyExc_TypeError, "Requires two MznVariable/MznConstraint objects and an integer");
    return NULL;
  }
  Expression *lhs, *rhs;
  if (PyObject_TypeCheck(l, &MznVariableType)) {
    lhs = (reinterpret_cast<MznVariable*>(l))->e;
  } else if (PyBool_Check(l)) {
    lhs = new BoolLit(Location(), PyInt_AS_LONG(l));
  } else if (PyInt_Check(l)) {
    lhs = new IntLit(Location(), IntVal(PyInt_AS_LONG(l)));
  } else if (PyFloat_Check(l)) {
    lhs = new FloatLit(Location(), PyFloat_AS_DOUBLE(l));
  } else if (PyString_Check(l)) {
    lhs = new StringLit(Location(), string(PyString_AS_STRING(l)));
  } else {
    PyErr_SetString(PyExc_TypeError, "Object must be of type MznVariable or MznConstraint");
    return NULL;
  }

  if (PyObject_TypeCheck(r, &MznVariableType)) {
      rhs = (reinterpret_cast<MznVariable*>(r))->e;
  } else if (PyBool_Check(r)) {
    rhs = new BoolLit(Location(), PyInt_AS_LONG(r));
  } else if (PyInt_Check(r)) {
    rhs = new IntLit(Location(), IntVal(PyInt_AS_LONG(r)));
  } else if (PyFloat_Check(r)) {
    rhs = new FloatLit(Location(), PyFloat_AS_DOUBLE(r));
  } else if (PyString_Check(r)) {
    rhs = new StringLit(Location(), string(PyString_AS_STRING(r)));
  } else {
    PyErr_SetString(PyExc_TypeError, "Object must be of type MznVariable or MznConstraint");
    return NULL;
  }


  GCLock Lock;

  PyObject* var = MznVariable_new(&MznVariableType, NULL, NULL);
  (reinterpret_cast<MznVariable*>(var))->e = new BinOp(Location(), lhs, static_cast<BinOpType>(op), rhs);

  return var;
}


static PyObject* Mzn_load(PyObject* self, PyObject* args, PyObject* keywds) {
  PyObject* model = MznModel_new(&MznModelType, NULL, NULL);
  if (model == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "Model cannot be created");
    return NULL;
  }
  if (MznModel_init(reinterpret_cast<MznModel*>(model), NULL) < 0) 
    return NULL;
  if (MznModel_load(reinterpret_cast<MznModel*>(model), args, keywds)==NULL)
    return NULL;
  return model;
}

static PyObject* Mzn_loadFromString(PyObject* self, PyObject* args, PyObject* keywds) {
  PyObject* model = MznModel_new(&MznModelType, NULL, NULL);
  if (model == NULL)
    return NULL;
  if (MznModel_init(reinterpret_cast<MznModel*>(model), NULL) < 0)
    return NULL;
  if (MznModel_loadFromString(reinterpret_cast<MznModel*>(model), args, keywds)==NULL)
    return NULL;
  return model;
}

static PyObject* 
Mzn_retrieveNames(MznModel* self, PyObject* args) {
  PyObject* boolfuncs = PyDict_New();
  PyObject* annfuncs = PyDict_New();
  PyObject* annvars = PyList_New(0);
  CollectBoolFuncNames bool_fv(boolfuncs);
  CollectAnnNames ann_fv(annfuncs, annvars);

  MznModel* tempModel = reinterpret_cast<MznModel*>(MznModel_new(&MznModelType, NULL, NULL));
  if (MznModel_init(tempModel,NULL) != 0) {
    return NULL;
  }
  iterItems(bool_fv, tempModel->_m);
  iterItems(ann_fv, tempModel->_m);
  MznModel_dealloc(tempModel);

  PyObject* dict = PyDict_New();
  PyDict_SetItemString(dict, "boolfuncs", boolfuncs);
  PyDict_SetItemString(dict, "annfuncs", annfuncs);
  PyDict_SetItemString(dict, "annvars", annvars);

  Py_DECREF(boolfuncs);
  Py_DECREF(annfuncs);
  Py_DECREF(annvars);
  return dict;
}


PyMODINIT_FUNC
initminizinc_internal(void) {
  PyObject* model = Py_InitModule3("minizinc_internal", Mzn_methods, "A python interface for minizinc constraint modeling");

  if (model == NULL)
    return;

  if (PyType_Ready(&MznSetType) < 0)
    return;
  Py_INCREF(&MznSetType);
  PyModule_AddObject(model, "Set", reinterpret_cast<PyObject*>(&MznSetType));

  if (PyType_Ready(&MznSetIterType) < 0)
    return;
  Py_INCREF(&MznSetIterType);
  PyModule_AddObject(model, "SetIterator", reinterpret_cast<PyObject*>(&MznSetIterType));

  if (PyType_Ready(&MznVariableType) < 0)
    return;
  Py_INCREF(&MznVariableType);
  PyModule_AddObject(model, "Variable", reinterpret_cast<PyObject*>(&MznVariableType));

  if (PyType_Ready(&MznModelType) < 0)
    return;
  Py_INCREF(&MznModelType);
  PyModule_AddObject(model, "Model", reinterpret_cast<PyObject*>(&MznModelType));

  /*PyMethodDef temp;

  temp.ml_name = "next";
  temp.ml_meth = (PyCFunction)MznSolver_next;
  temp.ml_flags = METH_NOARGS;
  temp.ml_doc = "Next Solution";
  MznSolver_methods[0] = temp;
  temp.ml_name = "getValue";
  temp.ml_meth = (PyCFunction)MznSolver_getValue;
  temp.ml_flags = METH_VARARGS;
  temp.ml_doc = "Get value";
  MznSolver_methods[1] = temp;
  temp.ml_name = NULL;
  temp.ml_meth = NULL;
  temp.ml_flags = NULL;
  temp.ml_doc = NULL;
  MznSolver_methods[2] = temp;*/

  if (PyType_Ready(&MznSolverType) < 0)
    return;
  Py_INCREF(&MznSolverType);
  PyModule_AddObject(model, "Solver", reinterpret_cast<PyObject*>(&MznSolverType));
}