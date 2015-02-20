/*  Python Interface for MiniZinc constraint modelling
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *  Supervisor:
 *     Guido Tack <guido.tack@monash.edu>
 */

#include "solver.h"

static PyObject*
MznSolver_getValueHelper(MznSolver* self, const char* const name)
{
  for (unsigned int i=0; i<self->_m->size(); ++i) {
    if (VarDeclI* vdi = (*(self->_m))[i]->dyn_cast<VarDeclI>()) {
      if (strcmp(vdi->e()->id()->str().c_str(), name) == 0) {
        GCLock Lock;
        if (PyObject* PyValue = minizinc_to_python(vdi->e()))
          return PyValue;
        else {
          char buffer[50];
          sprintf(buffer, "Cannot retrieve the value of '%s'", name);
          PyErr_SetString(PyExc_RuntimeError, buffer);
          return NULL;
        }
      }
    }
  }
  char buffer[50];
  sprintf(buffer, "'%s' not found", name);
  PyErr_SetString(PyExc_RuntimeError, buffer);
  return NULL;
}

static PyObject* 
MznSolver_getValue(MznSolver* self, PyObject* args) {
  const char* name;
  PyObject* obj;
  if (!(self->_m)) {
    PyErr_SetString(PyExc_RuntimeError, "No model (maybe you need to call Model.next() first");
    return NULL;
  }
  if (!PyArg_ParseTuple(args, "O", &obj)) {
    PyErr_SetString(PyExc_TypeError,"Accept 1 argument of strings or list/tuple of strings");
    return NULL;
  }
  if (PyString_Check(obj)) {
    name = PyString_AS_STRING(obj);
    return MznSolver_getValueHelper(self, name);;
  } else 
  // INEFFICIENT function to retrieve values, consider optimize it later
    if (PyList_Check(obj)) {
      Py_ssize_t n = PyList_GET_SIZE(obj);
      PyObject* ret = PyList_New(n);
      for (Py_ssize_t i=0; i!=n; ++i) {
        PyObject* item = PyList_GET_ITEM(obj, i);
        if (!PyString_Check(item)) {
          Py_DECREF(ret);
          PyErr_SetString(PyExc_RuntimeError,"Elements must be strings");
          return NULL;
        }
        name = PyString_AS_STRING(item);
        PyObject* value = MznSolver_getValueHelper(self, name);
        if (value == NULL) {
          Py_DECREF(ret);
          return NULL;
        }
        PyList_SET_ITEM(ret,i,value);
      }
      return ret;
    } else if (PyTuple_Check(obj)) {
      Py_ssize_t n = PyTuple_GET_SIZE(obj);
      PyObject* ret = PyTuple_New(n);
      for (Py_ssize_t i=0; i!=n; ++i) {
        PyObject* item = PyTuple_GET_ITEM(obj, i);
        if (!PyString_Check(item)) {
          Py_DECREF(ret);
          PyErr_SetString(PyExc_RuntimeError,"Elements must be strings");
          return NULL;
        }
        name = PyString_AS_STRING(item);
        PyObject* value = MznSolver_getValueHelper(self, name);
        if (value == NULL) {
          Py_DECREF(ret);
          return NULL;
        }
        PyTuple_SET_ITEM(ret,i,value);
      }
      return ret;
    } else {
      PyErr_SetString(PyExc_TypeError, "Object must be a string or a list/tuple of strings");
      return NULL;
    }
}


PyObject*
MznSolver::next()
{
  if (solver==NULL)
    throw runtime_error("Solver Object not found");
  GCLock lock;
  SolverInstance::Status status = solver->solve();
  if (status == SolverInstance::SAT || status == SolverInstance::OPT) {
    _m = env->output();

    /* DEPRECATED - use Solution.getValue(name) instead
    if (loaded_from_minizinc) {
      PyObject* solutions = PyList_New(0);
      PyObject* sol = PyDict_New();
      for (unsigned int i=0; i < _m->size(); i++) {
        if (VarDeclI* vdi = (*_m)[i]->dyn_cast<VarDeclI>()) {
          PyObject* PyValue = minizinc_to_python(vdi->e());
          if (PyValue == NULL)
            return NULL;
          PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(), PyValue);
        }
      }
      PyList_Append(solutions, sol);
      PyObject* ret = Py_BuildValue("iO", status, solutions);
      Py_DECREF(sol);
      Py_DECREF(solutions);
      return ret;
    }*/
    Py_RETURN_NONE; 
  }
  if (_m == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "Unsatisfied");
    return NULL;
  } else {
    PyErr_SetString(PyExc_RuntimeError, "Reached last solution");
    return NULL;
  }
}


static void
MznSolver_dealloc(MznSolver* self)
{
  if (self->env)
    delete self->env;
  if (self->solver)
    delete self->solver;
  self->ob_type->tp_free(reinterpret_cast<PyObject*>(self));
}

static PyObject*
MznSolver_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  MznSolver* self = reinterpret_cast<MznSolver*>(type->tp_alloc(type,0));
  self->solver = NULL;
  self->_m = NULL;
  self->env = NULL;
  return reinterpret_cast<PyObject*>(self);
}

static int
MznSolver_init(MznSolver* self, PyObject* args)
{
  return 0;
}

static PyObject*
MznSolver_next(MznSolver *self)
{
  return self->next();
}