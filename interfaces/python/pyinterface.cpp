/*  Python Interface for MiniZinc constraint modelling
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *  Supervisor:
 *     Guido Tack <guido.tack@monash.edu>
 */

#include "pyinterface.h"

using namespace MiniZinc;
using namespace std;


static PyObject*
Mzn_Call(PyObject* self, PyObject* args)
{
  const char* name;
  PyObject* variableTuple;
  PyTypeObject* returnType;
  if (!PyArg_ParseTuple(args, "sO|O", &name, &variableTuple, &returnType)) {
    PyErr_SetString(PyExc_TypeError, "MiniZinc: Mzn_Call: Accepts 2 values: a string, a list of minizinc variable");
    PyErr_Print();
    return NULL;
  }

  if (!PyList_Check(variableTuple)) {
    PyErr_SetString(PyExc_TypeError, "MiniZinc: Mzn_Call: Second argument must be a list");
    return NULL;
  }


  long len = PyList_GET_SIZE(variableTuple);
  vector<Expression*> expressionList(len);
  for (long i = 0; i!=len; ++i) {
    PyObject* pyval = PyList_GET_ITEM(variableTuple, i);
    if (PyObject_TypeCheck(pyval, &MznObject_Type)) {
      expressionList[i] = MznObject_get_e(reinterpret_cast<MznObject*>(pyval));
    } else {
      Type type;
      vector<pair<int, int> > dimList;
      expressionList[i] = python_to_minizinc(pyval, type, dimList);
      if (expressionList[i] == NULL) {
        MZN_PYERR_SET_STRING(PyExc_RuntimeError, "MiniZinc: MznCall: Second argument, item at position %li must be a MiniZinc Object or Python int/float/string/list/tuple", i);
        return NULL;
      }
    }
  }

  PyObject* ret = MznExpression_new(&MznExpression_Type, NULL, NULL);
  
  reinterpret_cast<MznExpression*>(ret)->e = new Call(Location(), string(name), expressionList);

  return ret;
}

static PyObject*
Mzn_Id(PyObject* self, PyObject* args)
{
  const char* name;
  if (!PyArg_ParseTuple(args, "s", &name)) {
    PyErr_SetString(PyExc_TypeError, "MiniZinc: Mzn_Id: Argument must be a string");
    return NULL;
  }
  PyObject* ret = MznExpression_new(&MznExpression_Type, NULL, NULL);
  reinterpret_cast<MznExpression*>(ret)->e = new Id(Location(), name, NULL);
  return ret;
}

static PyObject* Mzn_at(PyObject* self, PyObject* args)
{
  PyObject* Py_array;
  PyObject* Py_idx;
  if (!PyArg_ParseTuple(args, "OO", &Py_array, &Py_idx)) {
    PyErr_SetString(PyExc_TypeError, "MiniZinc: at:  Parsing error");
    return NULL;
  }

  if (!PyObject_TypeCheck(Py_array, &MznExpression_Type)) {
    PyErr_SetString(PyExc_TypeError, "MiniZinc: at:  First argument must be a MiniZinc Expression");
    return NULL;
  } 

  if (!PyList_Check(Py_idx)) {
    PyErr_SetString(PyExc_TypeError, "MiniZinc: at:  Second argument must be a list of indices");
    return NULL;
  }

  Py_ssize_t n = PyList_GET_SIZE(Py_idx);

  vector<Expression*> idx(n);
  for (Py_ssize_t i = 0; i!=n; ++i) {
    PyObject* obj = PyList_GetItem(Py_idx, i);
#if PY_MAJOR_VERSION < 3
    if (PyInt_Check(obj)) {
      long index = PyInt_AS_LONG(obj);
      idx[i] = IntLit::a(IntVal(index));
    } else
#endif
    if (PyLong_Check(obj)) {
      int overflow;
      long long index = PyLong_AsLongLongAndOverflow(obj, &overflow);
      if (overflow) {
        MZN_PYERR_SET_STRING(PyExc_OverflowError, "MiniZinc: at:  Index at pos %li overflowed", i);
        return NULL;
      }
      idx[i] = IntLit::a(IntVal(index));
    } else if (PyObject_TypeCheck(obj,&MznExpression_Type)) {
      idx[i] = reinterpret_cast<MznExpression*>(obj)->e;
    } else {
      PyErr_SetString(PyExc_TypeError, "MiniZinc: ArrayAccess:  Indices must be integers or MiniZinc Expression");
      return NULL;
    }
  }

  MznExpression* ret = reinterpret_cast<MznExpression*> (MznExpression_new(&MznExpression_Type, NULL, NULL));
  ret->e = new ArrayAccess(Location(), reinterpret_cast<MznExpression*>(Py_array)->e, idx);
  return reinterpret_cast<PyObject*>(ret);
}

static PyObject*
Mzn_UnOp(PyObject* self, PyObject* args)
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
    PyErr_SetString(PyExc_TypeError, "MiniZinc: Mzn_UnOp: Requires a MiniZinc object and an integer");
    return NULL;
  }
  Expression *rhs;

  if (PyObject_TypeCheck(r, &MznExpression_Type)) {
    rhs = reinterpret_cast<MznExpression*>(r)->e;
  } else if (PyBool_Check(r)) {
    rhs = new BoolLit(Location(), PyObject_IsTrue(r));
  } else
#if PY_MAJOR_VERSION < 3
  if (PyInt_Check(r)) {
    rhs = IntLit::a(IntVal(PyInt_AS_LONG(r)));
  } else
#endif
  if (PyLong_Check(r)) {
    int overflow;
    long long c_val = PyLong_AsLongLongAndOverflow(r, &overflow);
    if (overflow) {
      PyErr_SetString(PyExc_OverflowError, "MiniZinc: Mzn_UnOp:  Object is overflowed");
      return NULL;
    }
    rhs = IntLit::a(IntVal(c_val));
  } else if (PyFloat_Check(r)) {
    rhs = new FloatLit(Location(), PyFloat_AS_DOUBLE(r));
  } else if (PyUnicode_Check(r)) {
    rhs = new StringLit(Location(), string(PyUnicode_AsUTF8(r)));
  } else {
    PyErr_SetString(PyExc_TypeError, "MiniZinc: Mzn_UnOp: Object must be a Python value or a MiniZinc object");
    return NULL;
  }


  GCLock Lock;

  PyObject* ret = MznExpression_new(&MznExpression_Type, NULL, NULL);
  reinterpret_cast<MznExpression*>(ret)->e = new UnOp(Location(), static_cast<UnOpType>(op), rhs);
  return ret;
}


/* 
 * Description: Creates a minizinc BinOp expression
 * Note: Need an outer GCLock for this to work
 */
static PyObject* 
Mzn_BinOp(PyObject* self, PyObject* args)
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
  PyObject* PyPre[2];
  unsigned int op;
  if (!PyArg_ParseTuple(args, "OIO", &PyPre[0], &op, &PyPre[1])) {
    PyErr_SetString(PyExc_TypeError, "MiniZinc: Mzn_BinOp: Requires two MiniZinc objects and an integer");
    return NULL;
  }
  Expression *pre[2];
  // pre[0]: lhs;
  // pre[1]: rhs;
  for (int i=0; i!=2; ++i) {
    if (PyObject_TypeCheck(PyPre[i], &MznExpression_Type)) {
      pre[i] = reinterpret_cast<MznExpression*>(PyPre[i])->e;
    } else if (PyBool_Check(PyPre[i])) {
      pre[i] = new BoolLit(Location(), PyObject_IsTrue(PyPre[i]));
    } else 

#if PY_MAJOR_VERSION < 3
    if (PyInt_Check(PyPre[i])) {
      pre[i] = IntLit::a(IntVal(PyInt_AS_LONG(PyPre[i])));
    } else
#endif

    if (PyLong_Check(PyPre[i])) {
      int overflow;
      long long c_val = PyLong_AsLongLongAndOverflow(PyPre[i], &overflow);
      if (overflow) {
        PyErr_SetString(PyExc_OverflowError, "MiniZinc: Mzn_UnOp:  Object is overflowed");
        return NULL;
      }
      pre[i] = IntLit::a(IntVal(c_val));
    } else if (PyFloat_Check(PyPre[i])) {
      pre[i] = new FloatLit(Location(), PyFloat_AS_DOUBLE(PyPre[i]));
    } else if (PyUnicode_Check(PyPre[i])) {
      pre[i] = new StringLit(Location(), string(PyUnicode_AsUTF8(PyPre[i])));
    } else {
      if (i == 0)
        PyErr_SetString(PyExc_TypeError, "MiniZinc: Mzn_BinOp: Left hand side object must be a Python value or MiniZinc object");
      else
        PyErr_SetString(PyExc_TypeError, "MiniZinc: Mzn_BinOp: Right hand side object must be a Python value or MiniZinc object");
      return NULL;
    }
  }


  GCLock Lock;

  PyObject* ret = MznExpression_new(&MznExpression_Type, NULL, NULL);
  reinterpret_cast<MznExpression*>(ret)->e = (new BinOp(Location(), pre[0], static_cast<BinOpType>(op), pre[1]));
  return ret;
}


static PyObject* Mzn_load(PyObject* self, PyObject* args, PyObject* keywds) {
  PyObject* model = MznModel_new(&MznModel_Type, NULL, NULL);
  if (MznModel_init(reinterpret_cast<MznModel*>(model), NULL) < 0) 
    return NULL;
  if (MznModel_load(reinterpret_cast<MznModel*>(model), args, keywds)==NULL)
    return NULL;
  return model;
}

static PyObject* Mzn_load_from_string(PyObject* self, PyObject* args, PyObject* keywds) {
  PyObject* model = MznModel_new(&MznModel_Type, NULL, NULL);
  if (model == NULL)
    return NULL;
  if (MznModel_init(reinterpret_cast<MznModel*>(model), NULL) < 0)
    return NULL;
  if (MznModel_load_from_string(reinterpret_cast<MznModel*>(model), args, keywds)==NULL)
    return NULL;
  return model;
}

static PyObject* 
Mzn_retrieveNames(PyObject* self, PyObject* args) {
  PyObject* boolfuncs = PyDict_New();
  PyObject* annfuncs = PyDict_New();
  PyObject* annvars = PyList_New(0);
  PyObject* libName = NULL;

  {
    Py_ssize_t n = PyTuple_GET_SIZE(args);
    if (n > 1) {
      PyErr_SetString(PyExc_TypeError, "MiniZinc: Mzn_retrieveNames: accepts at most 1 argument");
      return NULL;
    } else if (n == 1) {
      libName = PyTuple_GET_ITEM(args, 0);
      if (PyObject_IsTrue(libName)) {
        if (!PyUnicode_Check(libName)) {
          PyErr_SetString(PyExc_TypeError, "MiniZinc: Mzn_retrieveNames: first argument must be a string");
          return NULL;
        }
      } else
        libName = NULL;
    }
  }

  // If a library name is specified here, it means that this function is called at least once already.
  // If that's the case, functions in globals.mzn and stdlib.mzn will be already defined, so we dont want to reinclude it
  bool include_global_mzn = (libName == NULL);

  MznModel* tempModel = reinterpret_cast<MznModel*>(MznModel_new(&MznModel_Type, NULL, NULL));

  if (MznModel_init(tempModel,libName) != 0) {
    return NULL;
  }
  CollectBoolFuncNames bool_fv(boolfuncs, include_global_mzn);
  CollectAnnNames ann_fv(annfuncs, annvars, include_global_mzn);
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

#if PY_MAJOR_VERSION >= 3

#define INITERROR return NULL

//PyObject*
PyMODINIT_FUNC
PyInit_minizinc_internal(void)

#else
#define INITERROR return

PyMODINIT_FUNC
initminizinc_internal(void)

#endif

{
#if PY_MAJOR_VERSION >= 3
  PyObject* module = PyModule_Create(&moduledef);
#else
  PyObject* module = Py_InitModule3("minizinc_internal", Mzn_methods, "A python interface for MiniZinc constraint modeling");
#endif

  if (module == NULL)
    INITERROR;

  if (PyType_Ready(&MznObject_Type) < 0)
    INITERROR;
  Py_INCREF(&MznObject_Type);
  PyModule_AddObject(module, "Object", reinterpret_cast<PyObject*>(&MznObject_Type));

  if (PyType_Ready(&MznSetIter_Type) < 0)
    INITERROR;
  Py_INCREF(&MznSetIter_Type);
  PyModule_AddObject(module, "Set_Iter", reinterpret_cast<PyObject*>(&MznSetIter_Type));

  if (PyType_Ready(&MznExpression_Type) < 0)
    INITERROR;
  Py_INCREF(&MznExpression_Type);
  PyModule_AddObject(module, "Expression", reinterpret_cast<PyObject*>(&MznExpression_Type));

  if (PyType_Ready(&MznAnnotation_Type) < 0)
    INITERROR;
  Py_INCREF(&MznAnnotation_Type);
  PyModule_AddObject(module, "Annotation", reinterpret_cast<PyObject*>(&MznAnnotation_Type));

  if (PyType_Ready(&MznSet_Type) < 0)
    INITERROR;
  Py_INCREF(&MznSet_Type);
  PyModule_AddObject(module, "Set", reinterpret_cast<PyObject*>(&MznSet_Type));

  if (PyType_Ready(&MznVarSet_Type) < 0)
    INITERROR;
  Py_INCREF(&MznVarSet_Type);
  PyModule_AddObject(module, "VarSet", reinterpret_cast<PyObject*>(&MznVarSet_Type));

  if (PyType_Ready(&MznModel_Type) < 0)
    INITERROR;
  Py_INCREF(&MznModel_Type);
  PyModule_AddObject(module, "Model", reinterpret_cast<PyObject*>(&MznModel_Type));

  if (PyType_Ready(&PyMznSolver_Type) < 0)
    INITERROR;
  Py_INCREF(&PyMznSolver_Type);
  PyModule_AddObject(module, "Solver", reinterpret_cast<PyObject*>(&PyMznSolver_Type));

#if PY_MAJOR_VERSION >= 3
  return module;
#endif
}
