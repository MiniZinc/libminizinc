/*
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *  Supervisor:
 *     Guido Tack <guido.tack@monash.edu>
 */
#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <Python.h>

#if PY_MAJOR_VERSION < 3
	#define PyUnicode_Check PyString_Check
	#define PyUnicode_AS_DATA PyString_AS_STRING	
 	#define PyUnicode_FromString PyString_FromString
 	#define PyUnicode_Type PyString_Type
   //#include <bytesobject.h>
#endif
 
#include "structmember.h"

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <list>
#include <string.h>
#include <typeinfo>
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>
#include <limits.h>

#include <minizinc/model.hh>
#include <minizinc/parser.hh>
#include <minizinc/model.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/flatten.hh>
#include <minizinc/optimize.hh>
#include <minizinc/builtins.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/solvers/gecode_solverinstance.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/copy.hh>

// some files are included at the end of the header
// XXX: to be changed later

using namespace std;
using namespace MiniZinc;

static PyObject* MznModel_new_error;
static PyObject* MznModel_init_error;
static PyObject* MznModel_solve_error;
static PyObject* MznModel_load_error;
static PyObject* MznModel_loadFromString_error;
static PyObject* MznModel_solve_warning;
static PyObject* MznVariable_init_error;
static PyObject* MznSet_error;

#define MZN_PYERR_SET_STRING(py_type_error, ...)	\
do {											\
	char buffer[150];						\
	snprintf(buffer, 150, __VA_ARGS__);		\
	PyErr_SetString(py_type_error, buffer);	\
} while (0)

inline PyObject* c_to_py_number(long long);
inline long long py_to_c_number(PyObject*);
inline long long py_to_c_number(PyObject*, int*);



// Nicely presenting the type, for example: set of int or array of [int, int]
string typePresentation(const Type& type);

// For internal use, only compare TypeInst, BaseType, SetType
bool compareType(const Type& type1, const Type& type2);

inline PyObject* one_dim_minizinc_to_python(Expression* e);

//Convert minizinc expression to python value
PyObject* minizinc_to_python(VarDecl* vd);


/*
 * Description: Helper function for python_to_minizinc
 *        converts a python value (not an array) to minizinc expression
 *        also returns the type of that value
 * Parameters: A python value - pvalue
 *             A minizinc BaseType code
 * Note:  If code == Type::BT_UNKNOWN, it will be changed to the corresponding type of pvalue
 *        If code is initialized to specific type, an error will be thrown if type mismatched 
 * Accepted code type:
 *      - Type::BT_UNKNOWN: Unknown type, will be changed later
 *      - Type::BT_INT
 *      - Type::BT_FLOAT
 *      - Type::BT_STRING
 *      - Type::BT_BOOL
 * Note 2: Need an outer GCLock for this to work
 */
inline Expression* one_dim_python_to_minizinc(PyObject* pvalue, Type::BaseType& code);

/* 
 * Description: Converts a python value to minizinc expression
 *              also returns the type of python value
 *              and the dimension list if it is an array
 * Note: Need an outer GCLock for this to work
 */
Expression* python_to_minizinc(PyObject* pvalue, Type& type, vector<pair<int, int> >& dimList);

// Special function used when loading Model from MiniZinc file
Expression* python_to_minizinc(PyObject* pvalue, const ASTExprVec<TypeInst>& ranges);


// Helper functions
string minizinc_set(long start, long end);
int getList(PyObject* value, vector<Py_ssize_t>& dimensions, vector<PyObject*>& simpleArray, const int layer);

bool PyObject_ExactTypeCheck(PyObject* ob, PyTypeObject* type) {
	return Py_TYPE(ob) == type;
}

#include "Object.h"
#include "Set.h"


#endif