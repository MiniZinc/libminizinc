/*  Python Interface for MiniZinc constraint modelling
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *  Supervisor:
 *     Guido Tack <guido.tack@monash.edu>
 */

#include "solver.h"

PyObject* 
MznSolver_getValue(MznSolver* self, PyObject* args) {
  const char* name;
  if (!PyArg_ParseTuple(args, "s", &name)) {
    PyErr_SetString(PyExc_TypeError, "Argument must be a string");
    return NULL;
  }
  //Model* _m = self->env->output();
  if (self->_m) {
    for (unsigned int i=0; i<self->_m->size(); i++) {
      if (VarDeclI* vdi = (*(self->_m))[i]->dyn_cast<VarDeclI>()) {
        if (strcmp(vdi->e()->id()->str().c_str(), name) == 0) {
          GCLock Lock;
          if (PyObject* PyValue = minizinc_to_python(vdi->e()))
            return PyValue;
          else {
            return NULL;
          }
        }
      }
    }
    PyErr_SetString(PyExc_RuntimeError, "Name not found");
  } else
    PyErr_SetString(PyExc_RuntimeError, "No model (maybe you need to call Model.next() first");
  return NULL;
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