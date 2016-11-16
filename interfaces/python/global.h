/*
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *  Supervisor:
 *     Guido Tack <guido.tack@monash.edu>
 */

// This header must be included above all other files
 
#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <Python.h>

#if PY_MAJOR_VERSION < 3
	#define PyUnicode_Check PyString_Check
	#define PyUnicode_AsUTF8 PyString_AS_STRING	
 	#define PyUnicode_FromString PyString_FromString
 	#define PyUnicode_Type PyString_Type
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

// These two files are included at the end of the header
//#include "Object.h"
//#include "Set.h"

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



// Macro that combines snprintf and PyErr_SetString into one.
// First argument is PyExc_*Error, Second argument is like ordinary printf function
#define MZN_PYERR_SET_STRING(py_type_error, ...)	\
do {											\
	char buffer[150];						\
	snprintf(buffer, 150, __VA_ARGS__);		\
	PyErr_SetString(py_type_error, buffer);	\
} while (0)

// Converts C++ long long to appropriate Python integer type
inline PyObject* c_to_py_number(long long);
// Converts Python integer type to C++ value, returns -1 if error occurred
inline long long py_to_c_number(PyObject*);
// Converts Python integer type to C++ value, set int* to -1 or 1 if overflown, returns -1 if other errors occurred
inline long long py_to_c_number(PyObject*, int*);



// Nicely presenting the type, for example: set of int or array of [int, int]
string typePresentation(const Type& type);

// For internal use, only compare TypeInst, BaseType, SetType
bool compareType(const Type& type1, const Type& type2);



// Converts a minizinc expression of dim()==0 to python value.
// Note 1: e = vd->e(), type = vd->type()
// Note 2: e must be not NULL
inline PyObject* one_dim_minizinc_to_python(Expression* e, const Type& type);

//Convert minizinc expression to python value, return a Python list if vd is an array
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
 * Return:  MiniZinc expression, NULL if error occurred
 *
 * Note 1: If you parse a list of MiniZinc Object, remember to parse the object only, not its wrapper class
 *         For current implementation, the python wrapper class must be replaced with a call to
 *         minizinc_internal.Id(wrapper.name)   (remember to wrap this function with lock and unlock())
 * Note 2: Need an outer GCLock for this to work
 * Note 3: if dimList is empty, any array created by this function will have its index start from 0
 *		  else, the array created will have the dimension correspondingly to dimList, 
 *		  and it will complain if dimension sizes mismatched
 */
 Expression* python_to_minizinc(PyObject* pvalue, Type& type, vector<pair<int, int> >& dimList);

/*
 * Used only when importing model from MiniZinc file
 * Return: MiniZinc expression, NULL if error occurred
 * Note: Need an outer GCLock
 */
Expression* python_to_minizinc(PyObject* pvalue, const ASTExprVec<TypeInst>& ranges);





/* 
 * A Python dimension list is like this
 *      [ [1,5],  [2,5],  [3,5] ]
 *   (1st dimension)
 *           (2nd dimension)
 *                   (3rd dimension)
 *
 * This function converts such dimension list to minizinc ranges list
 *      [ <1..5>, <2..5>, <3..5> ]
 *
 * Returns: If error, return empty vector, also set errorOccurred to 1
 *          Else, return a vector of MiniZinc dimension range
 */
vector<TypeInst*> pydim_to_minizinc_ranges(PyObject* pydim);


// Helper functions
// XXX: change the function name to a more meaningful name later
// starting layer should be 0
/* converts a python array(list) to:
        dimensions: an array holding the size of each dimension
        simpleArray: a 1d-array of Python value
      return 0 if success, -1 if it is an incomplete array
        ( incomplete array example:
          X X X X X X
          X X X X X
          X X X X X X
          X X X X X X
        )
*/
int getList(PyObject* value, vector<Py_ssize_t>& dimensions, vector<PyObject*>& simpleArray, const int layer);

inline bool PyObject_ExactTypeCheck(PyObject* ob, PyTypeObject* type) {
	return Py_TYPE(ob) == type;
}

#include "Object.h"
#include "Set.h"


#endif