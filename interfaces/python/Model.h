/*
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *  Supervisor:
 *     Guido Tack <guido.tack@monash.edu>
 */
#ifndef __MODEL_H
#define __MODEL_H

#include "global.h"

struct MznModel {
  PyObject_HEAD
  Model* _m;
  vector<string>* includePaths;


  enum SolverCode {
    SC_GECODE
  };
  SolverCode sc;

  long long timeLimit;
  bool loaded;

  MznModel();

  int load(PyObject *args, PyObject *keywds, bool fromFile);
  int addData(const char* const name, PyObject* value);
  PyObject* solve(PyObject* args);
};

void MznModelDestructor(PyObject* o);
static PyObject* MznModel_new(PyTypeObject* type, PyObject* args, PyObject* kwds);
static int MznModel_init(MznModel* self, PyObject* args);
static void MznModel_dealloc(MznModel* self);

/************************************************************
            Groups of function general functions
 ************************************************************/
static PyObject* MznModel_solve(MznModel *self, PyObject* args);
static PyObject* MznModel_set_time_limit(MznModel* self, PyObject* args);
static PyObject* MznModel_set_solver(MznModel* self, PyObject* args);


/************************************************************
 Groups of function functions to interact with MiniZinc files
 ************************************************************/
static PyObject* MznModel_load(MznModel *self, PyObject *args, PyObject *keywds);
static PyObject* MznModel_load_from_string(MznModel *self, PyObject *args, PyObject *keywds);
// Take in a string and a Python value
static PyObject* MznModel_addData(MznModel* self, PyObject* args);



/***************************************************************
  Groups of function functions to interact with Python Interface
 ***************************************************************/
/* Function:  MznModel_Declaration
 * Description: Take in a tuple of arguments, create a Variable in the Minizinc Model self
 * Arguments: 
 *    1:    name, TypeId, dimension vector, lower bound, upper bound
 *        name: string
 *        TypeId: see enum TypeId below
 *        dimension vector: empty if Variable is not an array
 *                          holds value if it is an array
 *            Syntax: vector<pair<int, int> >, called dimList
 *            - dimList[i] is the lower bound and upper bound of dimension i
 *            - for example:
 *                  dimList[0] = <1,5>
 *                  dimList[1] = <2,4>
 *                  dimList[2] = <0,5>
 *              means a 3d array [1..5,2..4,0..5]
 *        lower bound: can be MiniZinc Set, int or float
 *        upper bound: None if lower bound is MiniZinc Set,
 *                     type(lower bound) otherwise
 *
 *    2:    name, python value
 *        python value: Create a variable based on the existing python value
 * Return: 
 *      Id of the declared variable
 * Note 1: 
 *      Don't reuse this declaration if the model with this declaration has been destroyed
 * Note 2:
 *      Python Interface holds the responsibility of checking which type of value is declared.
 */
static PyObject* MznModel_Declaration(MznModel* self, PyObject* args);

/* Function:  MznModel_Constraint
 * Description: Take in a Python boolean or MiniZinc Expression, add it to the MiniZinc Model self
 * Return:
 *      None
 * Note:  It is the Python Interface responsibility to check if parsed argument is of type bool or not
 */
static PyObject* MznModel_Constraint(MznModel* self, PyObject* args);

/* Function:  MznModel_SolveItem
 * Description: Add a solve item into the model
 * Arguments:
 *    Arg1: Solve Type: 0 ~ sat, 1 ~ min, 2 ~ max
 *    Arg2 (opt): Annotation
 *    Arg3 (opt): Expression to be minimize / maximize if Arg1 != 0
 * Return: 
 *      None
 * Note:
 *    Arg2 accepts all MiniZinc expression, but the Python Interface should ensures that 
 *    Arg2 is an expression of MiniZinc Annotation.
 */
static PyObject* MznModel_SolveItem(MznModel* self, PyObject* args);

// Returns an exact copy of the current model
static PyObject* MznModel_copy(MznModel* self);
// Print the MiniZinc representation of the model
static PyObject* MznModel_debugprint(MznModel* self);

static PyMethodDef MznModel_methods[] = {
  {"load", (PyCFunction)MznModel_load, METH_KEYWORDS, "Load MiniZinc model from MiniZinc file"},
  {"load_from_string", (PyCFunction)MznModel_load_from_string, METH_KEYWORDS, "Load MiniZinc model from standard input"},
  {"addData", (PyCFunction)MznModel_addData, METH_VARARGS, "Add data to a MiniZinc model"},
  {"solve", (PyCFunction)MznModel_solve, METH_VARARGS, "Solve a loaded MiniZinc model"},
  {"set_time_limit", (PyCFunction)MznModel_set_time_limit, METH_VARARGS, "Limit the execution time of the model"},
  {"set_solver", (PyCFunction)MznModel_set_solver, METH_VARARGS, "Choose which model will be used to solve the model"},
  {"Declaration", (PyCFunction)MznModel_Declaration, METH_VARARGS, "Add a variable into the model"},
  {"Constraint", (PyCFunction)MznModel_Constraint, METH_VARARGS, "Add a constraint into the model"},
  {"SolveItem", (PyCFunction)MznModel_SolveItem, METH_VARARGS, "Add a solve item into the model"},

  {"copy", (PyCFunction)MznModel_copy, METH_NOARGS, "Returns a copy of current model"},
  {"debugprint", (PyCFunction)MznModel_debugprint, METH_NOARGS, "Print the MiniZinc representation of the current model"},
  {NULL} /* Sentinel */
};


static PyMemberDef MznModel_members[] = {
  {NULL} /* Sentinel */
};

static PyTypeObject MznModel_Type = {
  PyVarObject_HEAD_INIT(NULL,0)
  "minizinc_internal.Model", /* tp_name */
  sizeof(MznModel),          /* tp_basicsize */
  0,                         /* tp_itemsize */
  (destructor)MznModel_dealloc, /* tp_dealloc */
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
  "Minizinc Model",          /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  0,                         /* tp_iter */
  0,                         /* tp_iternext */
  MznModel_methods,          /* tp_methods */
  MznModel_members,          /* tp_members */
  0,                         /* tp_getset */
  0,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  (initproc)MznModel_init,   /* tp_init */
  0,                         /* tp_alloc */
  MznModel_new,              /* tp_new */
};

#endif