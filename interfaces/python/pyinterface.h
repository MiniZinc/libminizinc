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
#include "Set.h"
#include "Expression.h"
#include "Annotation.h"
#include "VarSet.h"

#include "Model.h"
#include "Solver.h"

#include "global.cpp"
#include "Set.cpp"
#include "Model.cpp"
#include "Solver.cpp"


/* Helper functions that load data into a new model
 * For example: instead of 
        import minizinc_internal
        model = minizinc_internal.Model()
        model.load(...)
  we can do this:
        import minizinc_internal
        model = minizinc_internal.load(...)
 */
static PyObject* Mzn_load(PyObject* self, PyObject* args, PyObject* keywds);
static PyObject* Mzn_load_from_string(PyObject* self, PyObject* args, PyObject* keywds);


// GC::lock() and GC::unlock()
static PyObject* Mzn_lock(PyObject* self) {GC::lock(); Py_RETURN_NONE;}
static PyObject* Mzn_unlock(PyObject* self) {GC::unlock(); Py_RETURN_NONE;}


/************************************************************
    Groups of function that need an outer GCLock to work
 ************************************************************/
// Note: Please see Note 1 of the first python_to_minizinc declaration in global.h first


// Requires:
//    - An UnOpType integer opcode
//    - A MiniZinc expression or Python value
static PyObject* Mzn_UnOp(PyObject* self, PyObject* args);

// Requires:
//    - A left hand sided MiniZinc expression or Python value
//    - A BinOpType integer opcode
//    - A right hand sided MiniZinc expression or Python value
static PyObject* Mzn_BinOp(PyObject* self, PyObject* args);

// Requires:
//    - Name of the MiniZinc function to be called (string)
//    - List of MiniZinc object or Python value
//    - (Optional, deprecated) Return type of Mzn_Call
static PyObject* Mzn_Call(PyObject* self, PyObject* args);

// Requires a name(string)
static PyObject* Mzn_Id(PyObject* self, PyObject* args);
// Requires:
//    - A MiniZinc expression of array type
//    - A list of integer indices (can be MiniZinc expression as well)
static PyObject* Mzn_at(PyObject* self, PyObject* args);

// *****************  END OF GROUPS  *****************



/* Definition: Returns a dictionary of keys: "boolfuncs", "annfuncs" and "annvars" (currently)
    For each '*funcs':
      Itself is another dictionary:
        key: name of function
        value:  a list of all possible combination of parsing argument types and return type:
            for each item in that list:
                item[0] is a tuple argument types:
                    if a function accepts (int, int), it should be (<type 'long'>, <type 'long'>)

                    if a function accepts (array of [int, int]), it should be
                                    ( [<type 'long'>, <type 'long'>], )
                          (notice the appearance of a list here)
                item[1] is the return type:
                    MiniZinc Type -   Python Type
                        int       -     long
                        float     -     float
                        bool      -     bool 
                        string    -     str
                        VarSet    -   minizinc.VarSet
                        Ann       -   minizinc.Annotation
                        Top       -    NoneType   (accepts everything)
                        others    -   minizinc.Object (means an error)

    For 'annvars':
      Just a list of ann names

 */
static PyObject* Mzn_retrieveNames(PyObject* self, PyObject* args);



/********************* SKIP ******************************* */
PyObject* eval_type(TypeInst* ti) {
  ASTExprVec<TypeInst> ranges = ti->ranges();
  PyObject* v;
  switch (ti->type().bt()) {
    case Type::BT_BOOL:   v = reinterpret_cast<PyObject*>(&PyBool_Type); break;
    case Type::BT_INT:   
      if (ti->type().st() == Type::ST_SET)
        v = reinterpret_cast<PyObject*>(&MznVarSet_Type);
      else
        v = reinterpret_cast<PyObject*>(&PyLong_Type);
      break;
    case Type::BT_FLOAT:  v = reinterpret_cast<PyObject*>(&PyFloat_Type); break;
    case Type::BT_STRING: v = reinterpret_cast<PyObject*>(&PyUnicode_Type); break;
    case Type::BT_ANN:    v = reinterpret_cast<PyObject*>(&MznAnnotation_Type); break;
    case Type::BT_TOP:    v = Py_None; break;
    default: //v = reinterpret_cast<PyObject*>(&MznVariable_Type);
      //cout << ti->type().bt() << endl;
      v = reinterpret_cast<PyObject*>(&MznObject_Type); break;
      throw runtime_error("CollectBoolFunctionNames: unexpected type");
  }

  if (ranges.size() == 0) {
    Py_INCREF(v);
    return v;
  } else {
    PyObject* args_tuple = PyList_New(ranges.size());
    for (int i=0; i!=ranges.size(); ++i) {
      Py_INCREF(v);
      PyList_SET_ITEM(args_tuple, i, v);
    }
    return args_tuple;
  }
}

void add_to_dictionary (FunctionI* fi, PyObject* toAdd)
{
  ASTExprVec<VarDecl> params = fi->params();
  const char* str = fi->id().str().c_str();
  PyObject* key = PyUnicode_FromString(str);

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
      PyList_Append(_annvars, PyUnicode_FromString(vdi->e()->id()->str().c_str()));
    }
  }
};

/********************* END OF SKIP ******************************* */


static PyMethodDef Mzn_methods[] = {
  {"load", (PyCFunction)Mzn_load, METH_KEYWORDS, "Load MiniZinc model from MiniZinc file"},
  {"load_from_string", (PyCFunction)Mzn_load_from_string, METH_KEYWORDS, "Load MiniZinc model from stdin"},
  {"BinOp", (PyCFunction)Mzn_BinOp, METH_VARARGS, "Add a binary expression into the model"},
  {"UnOp", (PyCFunction)Mzn_UnOp, METH_VARARGS, "Add a unary expression into the model"},
  {"Id", (PyCFunction)Mzn_Id, METH_VARARGS, "Return a MiniZinc Variable containing the given name"},
  {"Call", (PyCFunction)Mzn_Call, METH_VARARGS, "MiniZinc Call"},

  {"at", (PyCFunction)Mzn_at, METH_VARARGS, "Array Access"},
  {"retrieveNames", (PyCFunction)Mzn_retrieveNames, METH_VARARGS, "Returns names of MiniZinc functions and variables"},
  {"lock", (PyCFunction)Mzn_lock, METH_NOARGS, "Internal: Create a lock for garbage collection"},
  {"unlock", (PyCFunction)Mzn_lock, METH_NOARGS, "Internal: Unlock a lock for garbage collection"},
  {NULL}
};

#if PY_MAJOR_VERSION >= 3

static struct PyModuleDef moduledef = {
  PyModuleDef_HEAD_INIT,
  "minizinc",
  "A python interface for MiniZinc constraint modeling",
  -1,
  Mzn_methods,
  NULL,
  NULL,
  NULL,
  NULL
};

#endif


#endif