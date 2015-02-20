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

// Need an outer GC Lock
static PyObject* Mzn_Id(MznModel* self, PyObject* args);
static PyObject* Mzn_retrieveFunctions(MznModel* self, PyObject* args);
static PyObject* Mzn_retrieveAnnotations(MznModel* self, PyObject* args);

class CollectBoolFunctionNames: public ItemVisitor {
protected:
	std::vector<std::string>& _names;
public:
	CollectBoolFunctionNames(std::vector<std::string>& names): _names(names) {}
	bool enterModel(Model* m) {
		return m->filename()!="stdlib.mzn";
	}
	void vFunctionI(FunctionI* fi) {
		if (fi->ti()->type().isvarbool()) {
			string toAdd = fi->id().str();
			std::vector<std::string>::size_type i, n = _names.size();
			for (i=0; i!=n; ++i) {
				if (_names[i] == toAdd)
					return;
				else if (_names[i] > toAdd)
					break;
			}
			_names.insert(_names.begin()+i, toAdd);
		}
	}
};

class CollectAnnotationNames: public ItemVisitor {
protected:
	std::vector<std::string>& _variables;
	std::vector<std::string>& _functions;
public:
	CollectAnnotationNames(std::vector<std::string>& variables, std::vector<std::string>& functions): _variables(variables), _functions(functions) {}
	bool enterModel(Model* m) {
		return true; //m->filename()!="stdlib.mzn";
	}
	void vFunctionI(FunctionI* fi) {
		if (fi->ti()->type().isann()) {
			string toAdd = fi->id().str();
			std::vector<std::string>::size_type i, n = _functions.size();
			for (i=0; i!=n; ++i) {
				if (_functions[i] == toAdd)
					return;
				else if (_functions[i] > toAdd)
					break;
			}
			_functions.insert(_functions.begin()+i, toAdd);
		}
	}

	void vVarDeclI(VarDeclI* vdi) {
		if (vdi->e()->ti()->type().isann()) {
			string toAdd = vdi->e()->id()->str().str();
			std::vector<std::string>::size_type i, n = _variables.size();
			for (i=0; i!=n; ++i) {
				if (_variables[i] == toAdd)
					return;
				else if (_variables[i] > toAdd)
					break;
			}
			_variables.insert(_variables.begin()+i, toAdd);
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

  {"retrieveFunctions", (PyCFunction)Mzn_retrieveFunctions, METH_NOARGS, "Returns list of MiniZinc functions that returns Bool"},
  {"retrieveAnnotations", (PyCFunction)Mzn_retrieveAnnotations, METH_NOARGS, "Returns list of MiniZinc annotations"},
  {"lock", (PyCFunction)Mzn_lock, METH_NOARGS, "Internal: Create a lock for garbage collection"},
  {"unlock", (PyCFunction)Mzn_lock, METH_NOARGS, "Internal: Unlock a lock for garbage collection"},
  {NULL}
};

#endif