#include "global.h"

using namespace MiniZinc;
using namespace std;

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
  if (vd->type().st() == Type::ST_SET) {
    Env env(NULL);
    IntSetVal* isv = eval_intset(env.envi(),vd->e());
    long numberOfElement = 0;
    MznSet* newSet = reinterpret_cast<MznSet*>(MznSet_new(&MznSet_Type,NULL,NULL));
    for (IntSetRanges isr(isv); isr(); ++isr) {
      newSet->push(isr.min().toInt(),isr.max().toInt());
    }
    return reinterpret_cast<PyObject*>(newSet);
  } else {
    if (vd->type().bt() == Type::BT_BOOL) {
      if (vd->type().dim() == 0) {
        Env env(NULL);
        return PyBool_FromLong(eval_bool(env.envi(),vd->e()));
      } else {
        Env env(NULL);
        ArrayLit* al = eval_par(env.envi(), vd->e())->cast<ArrayLit>();
        int dim = vd->type().dim();

        // Maximum size of each dimension
        vector<long long int> dmax;
        // Current size of each dimension
        vector<long long int> d;
        // p[0] holds the final array, p[1+] builds up p[0]
        vector<PyObject*> p(dim);
        for (int i=0; i<dim; i++) {
          d.push_back(0);
          Py_ssize_t dtemp = al->max(i) - al->min(i) + 1;
          dmax.push_back(dtemp);
          p[i] = PyList_New(dtemp);
        }
        int i = dim - 1;
        // next item to be put onto the final array.
        unsigned int currentPos = 0;
        do {
          Env env(NULL);
          PyList_SetItem(p[i], d[i], PyBool_FromLong(eval_bool(env.envi(),al->v()[currentPos])));
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
    } else if (vd->type().bt() == Type::BT_INT) {
      if (vd->type().dim() == 0) {
        Env env(NULL);
        IntVal iv = eval_int(env.envi(), vd->e());
        return PyInt_FromLong(iv.toInt());
      } else {
        Env env(NULL);
        ArrayLit* al = eval_par(env.envi(),vd->e())->cast<ArrayLit>();
        int dim = vd->type().dim();

        // Maximum size of each dimension
        vector<long long int> dmax;
        // Current size of each dimension
        vector<long long int> d;
        // p[0] holds the final array, p[1+] builds up p[0]
        vector<PyObject*> p(dim);
        for (int i=0; i<dim; i++) {
          d.push_back(0);
          Py_ssize_t dtemp = al->max(i) - al->min(i) + 1;
          dmax.push_back(dtemp);
          p[i] = PyList_New(dtemp);
        }
        int i = dim - 1;
        // next item to be put onto the final array.
        unsigned int currentPos = 0;
        do {
          Env env(NULL);
          PyList_SetItem(p[i], d[i], PyInt_FromLong(eval_int(env.envi(),al->v()[currentPos]).toInt()));
          currentPos++;
          d[i]++;
          while (d[i]>=dmax[i] && i>0) {
            PyList_SetItem(p[i-1],d[i-1],p[i]);
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
    } else if (vd->type().bt() == Type::BT_STRING) {
      if (vd->type().dim() == 0) {
        Env env(NULL);
        string temp(eval_string(env.envi(), vd->e()));
        return PyString_FromString(temp.c_str());
      } else {
        Env env(NULL);
        ArrayLit* al = eval_par(env.envi(), vd->e())->cast<ArrayLit>();
        int dim = vd->type().dim();

        // Maximum size of each dimension
        vector<long long int> dmax;
        // Current size of each dimension
        vector<long long int> d;
        // p[0] holds the final array, p[1+] builds up p[0]
        vector<PyObject*> p(dim);
        for (int i=0; i<dim; i++) {
          d.push_back(0);
          Py_ssize_t dtemp = al->max(i) - al->min(i) + 1;
          dmax.push_back(dtemp);
          p[i] = PyList_New(dtemp);
        }
        int i = dim - 1;
        // next item to be put onto the final array.
        unsigned int currentPos = 0;
        do {
          Env env(NULL);
          string temp(eval_string(env.envi(),al->v()[currentPos]));
          PyList_SetItem(p[i], d[i], PyString_FromString(temp.c_str()));
          currentPos++;
          d[i]++;
          while (d[i]>=dmax[i] && i>0) {
            PyList_SetItem(p[i-1],d[i-1],p[i]);
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
    } else if (vd->type().bt() == Type::BT_FLOAT) {
      if (vd->type().dim() == 0) {
        Env env(NULL);
        FloatVal fv = eval_float(env.envi(),vd->e());
        return PyFloat_FromDouble(fv);
      } else {
        Env env(NULL);
        ArrayLit* al = eval_par(env.envi(),vd->e())->cast<ArrayLit>();
        int dim = vd->type().dim();

        // Maximum size of each dimension
        vector<long long int> dmax;
        // Current size of each dimension
        vector<long long int> d;
        // p[0] holds the final array, p[1+] builds up p[0]
        vector<PyObject*> p(dim);
        for (int i=0; i<dim; i++) {
          d.push_back(0);
          Py_ssize_t dtemp = al->max(i) - al->min(i) + 1;
          dmax.push_back(dtemp);
          p[i] = PyList_New(dtemp);
        }
        int i = dim - 1;
        // next item to be put onto the final array.
        unsigned int currentPos = 0;
        do {
          Env env(NULL);
          PyList_SetItem(p[i], d[i], PyFloat_FromDouble(eval_float(env.envi(),al->v()[currentPos])));
          currentPos++;
          d[i]++;
          while (d[i]>=dmax[i] && i>0) {
            PyList_SetItem(p[i-1],d[i-1],p[i]);
            //Py_DECREF(p[i]);
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
        Expression* rhs = new BoolLit(Location(), PyInt_AS_LONG(pvalue));
        code = Type::BT_BOOL;
        return rhs;
      } else if (PyInt_Check(pvalue)) {
        BT_INTEGER_PROCESS:
        Expression* rhs = new IntLit(Location(), IntVal(PyInt_AS_LONG(pvalue)));
        code = Type::BT_INT;
        return rhs;
      } else if (PyFloat_Check(pvalue)) {
        BT_FLOAT_PROCESS:
        Expression* rhs = new FloatLit(Location(), PyFloat_AS_DOUBLE(pvalue));
        code = Type::BT_FLOAT;
        return rhs;
      } else if (PyString_Check(pvalue)) {
        BT_STRING_PROCESS:
        Expression* rhs = new StringLit(Location(), PyString_AS_STRING(pvalue));
        code = Type::BT_STRING;
        return rhs;
      } else {
        PyErr_SetString(PyExc_TypeError, "Unexpected python type");
        return NULL;
      }
      break;

    case Type::BT_INT: 
      if (!PyInt_Check(pvalue)) {
        PyErr_SetString(PyExc_TypeError,"Object in an array must be of the same type: Expected an integer");
        return NULL;
      }
      goto BT_INTEGER_PROCESS;

    case Type::BT_FLOAT:
      if (!PyFloat_Check(pvalue)) {
        PyErr_SetString(PyExc_TypeError,"Object in an array must be of the same type: Expected an float");
        return NULL;
      }
      goto BT_FLOAT_PROCESS;

    case Type::BT_STRING:
      if (!PyString_Check(pvalue)) {
        PyErr_SetString(PyExc_TypeError,"Object in an array must be of the same type: Expected an string");
        return NULL;
      }
      goto BT_STRING_PROCESS;

    case Type::BT_BOOL:
      if (!PyBool_Check(pvalue)) {
        PyErr_SetString(PyExc_TypeError,"Object in an array must be of the same type: Expected an boolean");
        return NULL;
      }
      goto BT_BOOLEAN_PROCESS;

    default:
      throw std::invalid_argument("Internal Error: Received unexpected base type code");
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
    //return reinterpret_cast<MznObject*>(pvalue)->e();
  } else if (PyList_Check(pvalue)) {
    vector<Py_ssize_t> dimensions;
    vector<PyObject*> simpleArray;
    if (getList(pvalue, dimensions, simpleArray, 0) == -1)
      // need to be changed later
      throw invalid_argument("Inconsistency in size of multidimensional array");
    if (ranges.size()!=dimensions.size())
      throw invalid_argument("Size of declared array and data array not conform");
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
    if (getList(pvalue, dimensions, simpleArray, 0) == -1)
      throw invalid_argument("Inconsistency in size of multidimensional array");
    if (dimList.empty())
      for (int i=0; i!=dimensions.size(); i++)
        dimList.push_back(pair<Py_ssize_t,Py_ssize_t>(0,dimensions[i]-1));
    else {
      if (dimList.size()!=dimensions.size())
        throw invalid_argument("Size of declared array and data array not conform");
      for (int i=0; i!=dimensions.size(); i++) {
        //cout << dimList[i].first << " - " << dimList[i].second << " ---- " << dimensions[i] << endl; 
        if ( (dimList[i].second - (dimList[i].first) + 1) != dimensions[i] )
          throw invalid_argument("Size of each dimension not conform");
      }
    }
    vector<Expression*> v;
    for (int i=0; i!=simpleArray.size(); ++i) {
      PyObject* temp= simpleArray[i];
      Expression* rhs = one_dim_python_to_minizinc(temp, code);
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
    PyErr_SetString(PyExc_TypeError, "3rd argument must be a list of size of each dimension");
    return NULL;
  }
  Py_ssize_t dim = PyList_GET_SIZE(pydim);
  dimList = new vector<pair<int, int> >(dim);
  for (Py_ssize_t i=0; i!=dim; ++i) {
    PyObject* temp = PyList_GET_ITEM(pydim, i);
    if (!PyList_Check(temp)) {
      PyErr_SetString(PyExc_TypeError, "Objects in the dimension list must be of integer value");
      return NULL;
    }
    PyObject *tempLb = PyList_GetItem(temp, 0);
    PyObject *tempUb = PyList_GetItem(temp, 1);
    if (PyErr_Occurred()) {

      PyErr_SetString(PyExc_TypeError, "Range list must consist of two integer values");
      return NULL;
    }

    long c_lb = PyInt_AsLong(tempLb);
    long c_ub = PyInt_AsLong(tempUb);
    if (PyErr_Occurred()) {
      PyErr_SetString(PyExc_TypeError, "Range values must be integers");
      return NULL;
    }
    if (c_lb > c_ub)
      swap(c_lb, c_ub);
    (*dimList)[i] = make_pair(c_lb, c_ub);
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
      PyErr_SetString(PyExc_RuntimeError,"Inconsistency in size of multidimensional array");
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
