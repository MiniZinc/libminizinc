/*
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *  Supervisor:
 *     Guido Tack <guido.tack@monash.edu>
 */
#ifndef __PYINTERFACE_H
#define __PYINTERFACE_H

#include "global.h"
#include "Set.h"
#include "Variable.h"
#include "Model.h"
#include "Solver.h"

#include "global.cpp"
#include "Set.cpp"
#include "Variable.cpp"
#include "Model.cpp"
#include "Solver.cpp"

static PyObject* Mzn_load(PyObject* self, PyObject* args, PyObject* keywds);
static PyObject* Mzn_loadFromString(PyObject* self, PyObject* args, PyObject* keywds);
static PyObject* Mzn_BinOp(MznModel* self, PyObject* args);
static PyObject* Mzn_UnOp(MznModel* self, PyObject* args);
static PyObject* Mzn_Call(MznModel* self, PyObject* args);
static PyObject* Mzn_lock(MznModel* self) {GC::lock(); Py_RETURN_NONE;}
static PyObject* Mzn_unlock(MznModel* self) {GC::unlock(); Py_RETURN_NONE;}
static PyObject* Mzn_typeVariable(MznModel* self) {
  PyObject* v = reinterpret_cast<PyObject*>(&MznVariableType);
  Py_INCREF(v);
  return v;
}

// Need an outer GC Lock
static PyObject* Mzn_Id(MznModel* self, PyObject* args);
static PyObject* Mzn_retrieveNames(MznModel* self, PyObject* args);

// This struct take over pointers to passing arguments. Be aware!
struct MznFunction {
  std::string name;
  std::vector<PyObject*> args;
  MznFunction(const std::string &name0): name(name0) {}
  MznFunction(const std::string &name0, int size): name(name0) { args = vector<PyObject*>(size);}
  MznFunction(const std::string &name0, const std::vector<PyObject*>& args0): name(name0), args(args0) {}
};

PyObject* eval_arguments(TypeInst* ti) {
  ASTExprVec<TypeInst> ranges = ti->ranges();
  if (ranges.size() == 0) {
    PyObject* v;
    switch (ti->type().bt()) {
      case Type::BT_BOOL:   v = (PyObject*)(&PyBool_Type); break;
      case Type::BT_INT:    v = (PyObject*)(&PyInt_Type); break;
      case Type::BT_FLOAT:  v = (PyObject*)(&PyFloat_Type); break;
      case Type::BT_STRING: v = (PyObject*)(&PyString_Type); break;
      case Type::BT_ANN:    v = (PyObject*)(&MznVariableType); break;
      default: v = (PyObject*)(&MznVariableType);
        //throw runtime_error("CollectBoolFunctionNames: unexpected type");
    }
    Py_INCREF(v);
    return v;
  } else {
    PyObject* args_tuple = PyList_New(ranges.size());
    for (int i=0; i!=ranges.size(); ++i)
      PyList_SET_ITEM(args_tuple, i, eval_arguments(ranges[i]));
    return args_tuple;
  }
}

class CollectBoolFuncNames: public ItemVisitor {
protected:
	PyObject* _boolfuncs;
public:
	CollectBoolFuncNames(PyObject* boolfuncs): _boolfuncs(boolfuncs){}
	bool enterModel(Model* m) {
		return m->filename()!="stdlib.mzn";
	}
	void vFunctionI(FunctionI* fi) {
    PyObject* toAdd = _boolfuncs;
    ASTExprVec<VarDecl> params = fi->params();
    const char* str = fi->id().str().c_str();
    PyObject* key = PyString_FromString(str);
    PyObject* args_tuple = PyTuple_New(params.size());
    for (unsigned int i=0; i<params.size(); ++i) {
      PyTuple_SET_ITEM(args_tuple, i, eval_arguments(params[i]->ti()));
    }
    PyObject* toAdd_item = PyDict_GetItem(toAdd, key);
    if (toAdd_item == NULL) {
      toAdd_item = PyList_New(1);
      PyList_SET_ITEM(toAdd_item, 0, args_tuple);
      if (PyDict_SetItem(toAdd, key, toAdd_item) != 0)
        throw runtime_error("CollectBoolFunctionNames: cannot set new key to the dictionary");
      Py_DECREF(toAdd_item);
    } else {
      if (PyList_Append(toAdd_item, args_tuple) != 0)
        throw runtime_error("CollectBoolFunctionNames: cannot append item to the list");
    }
	}
};

class CollectAnnNames: public ItemVisitor {
protected:
  PyObject* _annfuncs;
  PyObject* _annvars;
public:
  CollectAnnNames(PyObject* annfuncs, PyObject* annvars): _annfuncs(annfuncs), _annvars(annvars) {}
  bool enterModel(Model* m) {
    return true;
  }
  void vFunctionI(FunctionI* fi) {
    PyObject* toAdd = _annfuncs;
    ASTExprVec<VarDecl> params = fi->params();
    const char* str = fi->id().str().c_str();
    PyObject* key = PyString_FromString(str);
    PyObject* args_tuple = PyTuple_New(params.size());
    for (unsigned int i=0; i<params.size(); ++i) {
      PyTuple_SET_ITEM(args_tuple, i, eval_arguments(params[i]->ti()));
    }
    PyObject* toAdd_item = PyDict_GetItem(toAdd, key);
    if (toAdd_item == NULL) {
      toAdd_item = PyList_New(1);
      PyList_SET_ITEM(toAdd_item, 0, args_tuple);
      if (PyDict_SetItem(toAdd, key, toAdd_item) != 0)
        throw runtime_error("CollectBoolFunctionNames: cannot set new key to the dictionary");
      Py_DECREF(toAdd_item);
    } else {
      if (PyList_Append(toAdd_item, args_tuple) != 0)
        throw runtime_error("CollectBoolFunctionNames: cannot append item to the list");
    }
  }
  void vVarDeclI(VarDeclI* vdi) {
    if (vdi->e()->ti()->type().isann()) {
      PyList_Append(_annvars, PyString_FromString(vdi->e()->id()->str().c_str()));
    }
  }
};




static PyMethodDef Mzn_methods[] = {
  {"load", (PyCFunction)Mzn_load, METH_KEYWORDS, "Load MiniZinc model from MiniZinc file"},
  {"loadFromString", (PyCFunction)Mzn_load, METH_KEYWORDS, "Load MiniZinc model from stdin"},
  {"BinOp", (PyCFunction)Mzn_BinOp, METH_VARARGS, "Add a binary expression into the model"},
  {"UnOp", (PyCFunction)Mzn_UnOp, METH_VARARGS, "Add a unary expression into the model"},
  {"Id", (PyCFunction)Mzn_Id, METH_VARARGS, "Return a MiniZinc Variable containing the given name"},
  {"Call", (PyCFunction)Mzn_Call, METH_VARARGS, "MiniZinc Call"},
  {"TypeVariable", (PyCFunction)Mzn_typeVariable, METH_NOARGS, "Type of MiniZinc Variable"},

  {"retrieveNames", (PyCFunction)Mzn_retrieveNames, METH_NOARGS, "Returns names of MiniZinc functions and variables"},
  {"lock", (PyCFunction)Mzn_lock, METH_NOARGS, "Internal: Create a lock for garbage collection"},
  {"unlock", (PyCFunction)Mzn_lock, METH_NOARGS, "Internal: Unlock a lock for garbage collection"},
  {NULL}
};

#endif