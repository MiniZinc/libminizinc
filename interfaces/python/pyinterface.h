/*
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *  Supervisor:
 *     Guido Tack <guido.tack@monash.edu>
 */
#ifndef __PYINTERFACE_H
#define __PYINTERFACE_H

#include "global.h"

#include "Object.h"
#include "Expression.h"
#include "Annotation.h"
#include "Declaration.h"
#include "Variable.h"
#include "Array.h"
#include "Set.h"
#include "VarSet.h"

#include "Model.h"
#include "Solver.h"

#include "global.cpp"
#include "Set.cpp"
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
  PyObject* v = reinterpret_cast<PyObject*>(&MznVariable_Type);
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

PyObject* eval_type(TypeInst* ti) {
  ASTExprVec<TypeInst> ranges = ti->ranges();
  if (ranges.size() == 0) {
    PyObject* v;
    switch (ti->type().bt()) {
      case Type::BT_BOOL:   v = (PyObject*)(&PyBool_Type); break;
      case Type::BT_INT:   
        if (ti->type().st() == Type::ST_SET)
          v = (PyObject*)(&MznVarSet_Type);
        else
          v = (PyObject*)(&PyInt_Type);
        break;
      case Type::BT_FLOAT:  v = (PyObject*)(&PyFloat_Type); break;
      case Type::BT_STRING: v = (PyObject*)(&PyString_Type); break;
      case Type::BT_ANN:    v = (PyObject*)(&MznAnnotation_Type); break;
      //case Type::BT_BOT:    v = (PyObject*)(&MznSet_Type); break;
      default: //v = (PyObject*)(&MznVariable_Type);
        //cout << ti->type().bt() << endl;
        v = (PyObject*)(&MznObject_Type); break;
        throw runtime_error("CollectBoolFunctionNames: unexpected type");
    }
    Py_INCREF(v);
    return v;
  } else {
    PyObject* args_tuple = PyList_New(ranges.size());
    for (int i=0; i!=ranges.size(); ++i)
      PyList_SET_ITEM(args_tuple, i, eval_type(ranges[i]));
    return args_tuple;
  }
}

void add_to_dictionary (FunctionI* fi, PyObject* toAdd)
{
  ASTExprVec<VarDecl> params = fi->params();
  const char* str = fi->id().str().c_str();
  PyObject* key = PyString_FromString(str);

  PyObject* args_and_return_type_tuple = PyTuple_New(2);
  PyObject* args_tuple = PyTuple_New(params.size());
  for (unsigned int i=0; i<params.size(); ++i) {
    PyTuple_SET_ITEM(args_tuple, i, eval_type(params[i]->ti()));
  }
  PyObject* return_type = eval_type(fi->ti());

  PyTuple_SET_ITEM(args_and_return_type_tuple, 0, args_tuple);
  PyTuple_SET_ITEM(args_and_return_type_tuple, 1, return_type);

  PyObject* toAdd_item = PyDict_GetItem(toAdd, key);
  if (toAdd_item == NULL) {
    toAdd_item = PyList_New(1);
    PyList_SET_ITEM(toAdd_item, 0, args_and_return_type_tuple);
    if (PyDict_SetItem(toAdd, key, toAdd_item) != 0)
      throw runtime_error("CollectBoolFunctionNames: cannot set new key to the dictionary");
    Py_DECREF(toAdd_item);
  } else {
    if (PyList_Append(toAdd_item, args_and_return_type_tuple) != 0)
      throw runtime_error("CollectBoolFunctionNames: cannot append item to the list");
  }
}

class CollectBoolFuncNames: public ItemVisitor {
protected:
  bool include_global_mzn;
	PyObject* _boolfuncs;
public:
	CollectBoolFuncNames(PyObject* boolfuncs, bool include_global_mzn0):
              _boolfuncs(boolfuncs), include_global_mzn(include_global_mzn0) {}
	bool enterModel(Model* m) {
		return m->filename()!="stdlib.mzn" && (include_global_mzn || m->filename()!="globals.mzn");
	}
	void vFunctionI(FunctionI* fi) {
    if (fi->ti()->type().isvarbool() == false)
      return;

    add_to_dictionary(fi, _boolfuncs);
	}
};

class CollectAnnNames: public ItemVisitor {
protected:
  PyObject* _annfuncs;
  PyObject* _annvars;
  bool include_global_mzn;
public:
  CollectAnnNames(PyObject* annfuncs, PyObject* annvars, bool include_global_mzn0):
              _annfuncs(annfuncs), _annvars(annvars), include_global_mzn(include_global_mzn0) {}
  bool enterModel(Model* m) {
    return include_global_mzn || (m->filename() != "globals.mzn" && m->filename() != "stdlib.mzn");
  }
  void vFunctionI(FunctionI* fi) {
    if (fi->ti()->type().isann() == false)
      return;

    add_to_dictionary(fi, _annfuncs);
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

  {"retrieveNames", (PyCFunction)Mzn_retrieveNames, METH_VARARGS, "Returns names of MiniZinc functions and variables"},
  {"lock", (PyCFunction)Mzn_lock, METH_NOARGS, "Internal: Create a lock for garbage collection"},
  {"unlock", (PyCFunction)Mzn_lock, METH_NOARGS, "Internal: Unlock a lock for garbage collection"},
  {NULL}
};

#endif