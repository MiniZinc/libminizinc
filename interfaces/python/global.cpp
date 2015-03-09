#include "global.h"

using namespace MiniZinc;
using namespace std;

inline PyObject* c_to_py_number(long long c_val)
{
#if PY_MAJOR_VERSION < 3
  if (c_val > LONG_MAX || c_val < LONG_MIN)
    return PyInt_FromLong( static_cast<long>(c_val) );
  else
#endif
    return PyLong_FromLongLong(c_val);
}

inline long long py_to_c_number(PyObject* py_val)
{
#if PY_MAJOR_VERSION < 3
  if (PyInt_Check(py_val)) {
    return PyInt_AS_LONG(py_val);
  } else
#endif
  if (PyLong_Check(py_val)) {
    int overflow;
    long long c_val = PyLong_AsLongLongAndOverflow(py_val, &overflow);
    if (overflow) {
      PyErr_SetString(PyExc_OverflowError, "Python value is overflown");
      return -1;
    }
    return c_val;
  } else {
    PyErr_SetString(PyExc_TypeError, "Python value must be an integer");
    return -1;
  }
}

inline long long py_to_c_number(PyObject* py_val, int* overflow)
{
#if PY_MAJOR_VERSION < 3
  if (PyInt_Check(py_val)) {
    *overflow = 0;
    return PyInt_AS_LONG(py_val);
  } else
#endif
  if (PyLong_Check(py_val)) {
    long long c_val = PyLong_AsLongLongAndOverflow(py_val, overflow);
    if (*overflow) {
      PyErr_SetString(PyExc_OverflowError, "Python value is overflown");
      return -1;
    }
    *overflow = 0;
    return c_val;
  } else {
    *overflow = 0;
    PyErr_SetString(PyExc_TypeError, "Python value must be an integer");
    return -1;
  }
}

// For internal use, only compare TypeInst, BaseType, SetType
bool compareType(const Type& type1, const Type& type2)
{
  return (type1.bt() == type2.bt() &&
          type1.st() == type2.st() && type1.dim() == type2.dim());
}

// Nicely presenting the type, for example: set of int or array of [int, int]
string typePresentation(const Type& type)
{
  string baseTypeString;
  switch (type.bt()) {
    case Type::BT_BOOL: baseTypeString = "bool"; break;
    case Type::BT_INT: baseTypeString = "int"; break;
    case Type::BT_FLOAT: baseTypeString = "float"; break;
    case Type::BT_STRING: baseTypeString = "string"; break;
    default: baseTypeString = "UNSET";
  }
  if (type.st() == Type::ST_SET)
    return "a set of " + baseTypeString;
  else if (type.dim() == 0) {
    if (type.bt() == Type::BT_INT)
      return "an " + baseTypeString;
    else 
      return "a " + baseTypeString;
  } else {
    string ret = "an array of [";
    for (int i=type.dim(); i--;) {
      ret += baseTypeString;
      if (i-1>0)
        ret += ", ";
    }
    ret += ']';
    return ret;
  }
}

inline PyObject*
one_dim_minizinc_to_python(Expression* e, const Type& type)
{
  Env env(NULL);
  /*if (vd==NULL) {
    PyErr_SetString(PyExc_ValueError, "MiniZinc_to_Python: Value is not set");
    return NULL;
  } else */
  // Here we assume that vd is not NULL

  if (type.st() == Type::ST_SET) {
    IntSetVal* isv = eval_intset(env.envi(),e);
    MznSet* newSet = reinterpret_cast<MznSet*>(MznSet_new(&MznSet_Type,NULL,NULL));
    for (IntSetRanges isr(isv); isr(); ++isr) {
      newSet->push(isr.min().toInt(),isr.max().toInt());
    }
    return reinterpret_cast<PyObject*>(newSet);
  } else switch (type.bt()) {
    case Type::BT_BOOL:
      return PyBool_FromLong(eval_bool(env.envi(),e));
    case Type::BT_INT:
      return c_to_py_number(eval_int(env.envi(), e).toInt());
    case Type::BT_FLOAT:
      return PyFloat_FromDouble(eval_float(env.envi(),e));
    case Type::BT_STRING:
    {
      string temp(eval_string(env.envi(), e));
      return PyUnicode_FromString(temp.c_str());
    }
    default:
      throw logic_error("MiniZinc: one_dim_minizinc_to_python: Unexpected type code");
  }
}

/* 
 * Convert minizinc expression to python value
 */
PyObject*
minizinc_to_python(VarDecl* vd)
{
  GCLock Lock;
  if (vd==NULL) {
    PyErr_SetString(PyExc_ValueError, "MiniZinc_to_Python: Value is not set");
    return NULL;
  }
  Type type = vd->type();
  if (type.dim() == 0) {
    return one_dim_minizinc_to_python(vd->e(), type);
  } else {
    Env env(NULL);
    ArrayLit *al = eval_par(env.envi(), vd->e())->cast<ArrayLit>();
    int dim = vd->type().dim();

    // Maximum size of each dimension
    vector<Py_ssize_t> dmax;
    // Current size of each dimension
    vector<Py_ssize_t> d;
    // p[0] holds the final array, p[1+] builds up p[0]
    vector<PyObject*> p(dim);

    for (int i=0; i<dim; ++i) {
      d.push_back(0);
      Py_ssize_t dtemp = al->max(i) - al->min(i) + 1;
      dmax.push_back(dtemp);
      p[i] = PyList_New(dtemp);
    }

    int i = dim - 1;

    // next item to be put onto the final array
    unsigned int currentPos = 0;
    do {
      PyList_SetItem(p[i], d[i], one_dim_minizinc_to_python(al->v()[currentPos], type));
      currentPos++;
      d[i]++;
      while (d[i]>=dmax[i] && i>0) {
        PyList_SetItem(p[i-1],d[i-1],p[i]);
        Py_DECREF(p[i]);
        d[i]=0;
        p[i]=PyList_New(dmax[i]);
        i--;
        d[i]++;
      }
      i = dim - 1;
    } while (d[0]<dmax[0]);
    for (int i=1; i<dim; i++)
      Py_DECREF(p[i]);
    return p[0];
  }
  
}


/*
 * Description: Helper function for python_to_minizinc
 *        converts a python value (not an array) to minizinc expression
 *        also returns the type of that value
 * Parameters: A python value - pvalue
 *             A minizinc BaseType code
 * Returns: appropriate expression or NULL if cannot convert
 * Note:  If code == Type::BT_UNKNOWN, it will be changed to the corresponding type of pvalue
 *        If code is initialized to specific type, an error will be thrown if type mismatched 
 * Accepted code type:
 *      - Type::BT_UNKNOWN: Unknown type, will be changed later to corresponding type
 *      - Type::BT_INT
 *      - Type::BT_FLOAT
 *      - Type::BT_STRING
 *      - Type::BT_BOOL
 * Note 2: Need an outer GCLock for this to work
 */
inline Expression*
one_dim_python_to_minizinc(PyObject* pvalue, Type::BaseType& code)
{
  /*enum BaseType { BT_TOP, BT_BOOL, BT_INT, BT_FLOAT, BT_STRING, BT_ANN,
                    BT_BOT, BT_UNKNOWN };*/
  switch (code) {
    case Type::BT_UNKNOWN:
      if (PyObject_TypeCheck(pvalue, &MznObject_Type)) {
        return MznObject_get_e(reinterpret_cast<MznObject*>(pvalue));
        //return reinterpret_cast<MznObject*>(pvalue)->e();
      } else if (PyBool_Check(pvalue)) {
        BT_BOOLEAN_PROCESS:
        Expression* rhs = new BoolLit(Location(), PyObject_IsTrue(pvalue));
        code = Type::BT_BOOL;
        return rhs;
      } else 
#if PY_MAJOR_VERSION < 3
      if (PyInt_Check(pvalue)) {
        BT_INTEGER_PROCESS_2X_VERSION:
        Expression* rhs = new IntLit(Location(), IntVal(PyInt_AS_LONG(pvalue)));
        code = Type::BT_INT;
        return rhs;
      } else
#endif
      if (PyLong_Check(pvalue)) {
        BT_INTEGER_PROCESS:
        int overflow;
        Expression* rhs = new IntLit(Location(), IntVal(PyLong_AsLongLongAndOverflow(pvalue, &overflow)));
        if (overflow) {
          if (overflow > 0)
            PyErr_SetString(PyExc_OverflowError, "MiniZinc: Python integer value is larger than 2^63-1");
          else
            PyErr_SetString(PyExc_OverflowError, "MiniZinc: Python integer value is smaller than -2^63");
          return NULL;
        }
        code = Type::BT_INT;
        return rhs;
      } else if (PyFloat_Check(pvalue)) {
        BT_FLOAT_PROCESS:
        Expression* rhs = new FloatLit(Location(), PyFloat_AS_DOUBLE(pvalue));
        code = Type::BT_FLOAT;
        return rhs;
      } else if (PyUnicode_Check(pvalue)) {
        BT_STRING_PROCESS:
        Expression* rhs = new StringLit(Location(), PyUnicode_AsUTF8(pvalue));
        code = Type::BT_STRING;
        return rhs;
      } else {
        PyErr_SetString(PyExc_TypeError, "MiniZinc: Unexpected python type");
        return NULL;
      }
      break;

    case Type::BT_INT: 
#if PY_MAJOR_VERSION < 3
      if (PyInt_Check(pvalue))
        goto BT_INTEGER_PROCESS_2X_VERSION;
      else
#endif
      if (!PyLong_Check(pvalue)) {
        PyErr_SetString(PyExc_TypeError,"MiniZinc: Object in an array must be of the same type: Expected an integer");
        return NULL;
      }
      goto BT_INTEGER_PROCESS;

    case Type::BT_FLOAT:
      if (!PyFloat_Check(pvalue)) {
        PyErr_SetString(PyExc_TypeError,"MiniZinc: Object in an array must be of the same type: Expected an float");
        return NULL;
      }
      goto BT_FLOAT_PROCESS;

    case Type::BT_STRING:
      if (!PyUnicode_Check(pvalue)) {
        PyErr_SetString(PyExc_TypeError,"MiniZinc: Object in an array must be of the same type: Expected an string");
        return NULL;
      }
      goto BT_STRING_PROCESS;

    case Type::BT_BOOL:
      if (!PyBool_Check(pvalue)) {
        PyErr_SetString(PyExc_TypeError,"MiniZinc: Object in an array must be of the same type: Expected an boolean");
        return NULL;
      }
      goto BT_BOOLEAN_PROCESS;

    default:
      throw std::invalid_argument("MiniZinc: Internal Error: Received unexpected base type code");
  }
}




/*
 * Used only when importing model from MiniZinc file
 */
Expression*
python_to_minizinc(PyObject* pvalue, const ASTExprVec<TypeInst>& ranges)
{
  if (PyObject_TypeCheck(pvalue, &MznObject_Type)) {
    return MznObject_get_e(reinterpret_cast<MznObject*>(pvalue));
  } else if (PyList_Check(pvalue)) {
    vector<Py_ssize_t> dimensions;
    vector<PyObject*> simpleArray;
    if (getList(pvalue, dimensions, simpleArray, 0) == -1)
      // getList should already set the error string
      return NULL;
    if (ranges.size()!=dimensions.size()) {
      PyErr_SetString(PyExc_ValueError, "MiniZinc: python_to_minizinc: size of declared array and actual array not matched");
      return NULL;
    }
    vector<Expression*> callArgument(dimensions.size()+1);
    vector<Expression*> onedArray(simpleArray.size());

    stringstream buffer;
    buffer << "array" << dimensions.size() << "d";
    string callName(buffer.str());
    for (int i=0; i!=dimensions.size(); ++i) {
      Expression* domain = ranges[i]->domain();
      if (domain == NULL) {
        Expression* e0 = new IntLit(Location(), IntVal(1));
        Expression* e1 = new IntLit(Location(), IntVal(dimensions[i]));
        callArgument[i] = new BinOp(Location(), e0, BOT_DOTDOT, e1);
      } else {
        callArgument[i] = domain;
      }
    }
    Type::BaseType code = Type::BT_UNKNOWN;
    for (int i=0; i!=simpleArray.size(); ++i) {
      PyObject* temp= simpleArray[i];
      Expression* rhs = one_dim_python_to_minizinc(temp, code);
      if (rhs == NULL)
        return NULL;
      onedArray[i] = rhs;
    }
    callArgument[dimensions.size()] = new ArrayLit(Location(), onedArray);
    Expression* rhs = new Call(Location(), callName, callArgument);
    return rhs;
  } else {
    Type::BaseType code = Type::BT_UNKNOWN;
    Expression* rhs = one_dim_python_to_minizinc(pvalue, code); 
    return rhs;
  }
}


/* 
 * Description: Converts a python value to minizinc expression
 *              also returns the type of python value
 *              and the dimension list if it is an array
 * Note: Need an outer GCLock for this to work
 */
Expression*
python_to_minizinc(PyObject* pvalue, Type& returnType, vector<pair<int, int> >& dimList)
{
  Type::BaseType code = Type::BT_UNKNOWN;
  if (PyObject_TypeCheck(pvalue, &MznObject_Type)) {
    returnType = Type::parsetint();
    return MznObject_get_e(reinterpret_cast<MznObject*>(pvalue));
  } else if (PyList_Check(pvalue)) {
    vector<Py_ssize_t> dimensions;
    vector<PyObject*> simpleArray;
    if (getList(pvalue, dimensions, simpleArray, 0) == -1) {
      // getList should set error string already
      return NULL;
      //throw invalid_argument("Inconsistency in size of multidimensional array");
    }
    if (dimList.empty())
      for (int i=0; i!=dimensions.size(); i++)
        dimList.push_back(pair<Py_ssize_t,Py_ssize_t>(0,dimensions[i]-1));
    else {
      if (dimList.size()!=dimensions.size()) {
        PyErr_SetString(PyExc_ValueError, "MiniZinc: python_to_minizinc: size of declared and actual array not matched");
        return NULL;
      }
      for (int i=0; i!=dimensions.size(); i++) {
        if ( (dimList[i].second - (dimList[i].first) + 1) != dimensions[i] ) {
          PyErr_SetString(PyExc_ValueError, "MiniZinc: python_to_minizinc: size of each dimension of python array not matched");
          return NULL;
        }
      }
    }
    vector<Expression*> v;
    for (int i=0; i!=simpleArray.size(); ++i) {
      PyObject* temp= simpleArray[i];
      Expression* rhs = one_dim_python_to_minizinc(temp, code);
      if (rhs == NULL)
        return NULL;
      v.push_back(rhs);
    }
    returnType = Type();
    returnType.bt(code);
    returnType.dim(dimList.size());
    Expression* rhs = new ArrayLit(Location(), v, dimList);
    return rhs;
  } else {
    Expression* rhs = one_dim_python_to_minizinc(pvalue, code); 
    returnType = Type();
    returnType.bt(code);
    return rhs;
  }
}


vector<pair<int, int> >*
pydim_to_dimList(PyObject* pydim)
{
  vector<pair<int, int> >* dimList;
  if (!PyList_Check(pydim)) {
    PyErr_SetString(PyExc_TypeError, "MiniZinc: python_to_dimList: argument must be a python list");
    return NULL;
  }
  Py_ssize_t dim = PyList_GET_SIZE(pydim);
  dimList = new vector<pair<int, int> >(dim);
  for (Py_ssize_t i=0; i!=dim; ++i) {
    PyObject* temp = PyList_GET_ITEM(pydim, i);
    if (!PyList_Check(temp)) {
      PyErr_SetString(PyExc_TypeError, "MiniZinc: python_to_dimList: objects in the list must be range lists");
      return NULL;
    }
    PyObject* Py_bound[2];
    Py_bound[0] = PyList_GetItem(temp, 0);
    Py_bound[1] = PyList_GetItem(temp, 1);
    if (PyErr_Occurred()) {
      PyErr_SetString(PyExc_TypeError, "MiniZinc: python_to_dimList: a range must consist of 2 values");
      return NULL;
    }

    long long c_bound[2];
    for (int i=0; i!=2; ++i) {
#if PY_MAJOR_VERSION < 3
      if (PyInt_Check(Py_bound[i]))
        c_bound[i] = PyInt_AS_LONG(Py_bound[i]);
      else 
#endif
      if (PyLong_Check(Py_bound[i])) {
        int overflow;
        c_bound[i] = PyLong_AsLongLongAndOverflow(Py_bound[i], &overflow);
        if (overflow) {
          switch (i) {
            case 0: MZN_PYERR_SET_STRING(PyExc_OverflowError, "MiniZinc: python_to_dimList:  Range at pos %i: First argument is overflown", i); break;
            case 1: MZN_PYERR_SET_STRING(PyExc_OverflowError, "MiniZinc: python_to_dimList:  Range at pos %i: Second argument is overflown", i); break;
            default:
              throw logic_error("pydim_to_dimList: Unexpected iterator value");
          }
          return NULL;
        }
      } else {
        switch (i) {
          case 0: MZN_PYERR_SET_STRING(PyExc_OverflowError, "MiniZinc: python_to_dimList: Range at pos %i: First argument type is mismatched: expected a number", i); break;
          case 1: MZN_PYERR_SET_STRING(PyExc_OverflowError, "MiniZinc: python_to_dimList: Range at pos %i: Second argument type is mismatched: expected a number", i); break;
          default:
            throw logic_error("pydim_to_dimList: Unexpected iterator value");
        }
      }
    }
    if (c_bound[0] > c_bound[1])
      swap(c_bound[0], c_bound[1]);
    (*dimList)[i] = make_pair(c_bound[0], c_bound[1]);
  }
  return dimList;
}


// Helper functions
int getList(PyObject* value, vector<Py_ssize_t>& dimensions, vector<PyObject*>& simpleArray, const int layer)
{
  for (Py_ssize_t i=0; i<PyList_Size(value); i++) {
    if (dimensions.size() <= layer) {
      dimensions.push_back(PyList_Size(value));
    } else if (dimensions[layer]!=PyList_Size(value)) {
      PyErr_SetString(PyExc_ValueError,"MiniZinc: Inconsistency in size of multidimensional array");
      return -1; // Inconsistent size of array (should be the same)
    }
    PyObject* li = PyList_GetItem(value, i);
    if (PyList_Check(li)) {
      if (getList(li,dimensions,simpleArray,layer+1)==-1) {
        return -1;
      }
    } else {
      simpleArray.push_back(li);
    }
  }
  return 0;
}


string minizinc_set(long start, long end) {
  stringstream ret;
  ret << start << ".." << end;
  string asn(ret.str());
  return asn;
}
