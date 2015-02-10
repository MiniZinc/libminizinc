/*
 *  Python Interface for MiniZinc constraint modelling
 *  Main authors:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *          under the supervision of Guido Tack <guido.tack@monash.edu>
 */

#include "pyinterface.h";

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
    PyErr_SetString(PyExc_ValueError, "Value is not set");
    return NULL;
  }
  if (vd->type().st() == Type::ST_SET) {
    IntSetVal* isv = eval_intset(vd->e());
    long numberOfElement = 0;
    MznSet* newSet = reinterpret_cast<MznSet*>(MznSet_new(&MznSetType,NULL,NULL));
    
    for (IntSetRanges isr(isv); isr(); ++isr) {
      newSet->push(isr.min().toInt(),isr.max().toInt());
    }
    return reinterpret_cast<PyObject*>(newSet);
  } else {
    if (vd->type().bt() == Type::BT_BOOL) {
      if (vd->type().dim() == 0) {
        return PyBool_FromLong(eval_bool(vd->e()));
      } else {
        ArrayLit* al = eval_par(vd->e())->cast<ArrayLit>();
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
          PyList_SetItem(p[i], d[i], PyBool_FromLong(eval_bool(al->v()[currentPos])));
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
        IntVal iv = eval_int(vd->e());
        return PyInt_FromLong(iv.toInt());
      } else {
        ArrayLit* al = eval_par(vd->e())->cast<ArrayLit>();
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
          PyList_SetItem(p[i], d[i], PyInt_FromLong(eval_int(al->v()[currentPos]).toInt()));
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
        string temp(eval_string(vd->e()));
        return PyString_FromString(temp.c_str());
      } else {
        ArrayLit* al = eval_par(vd->e())->cast<ArrayLit>();
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
          string temp(eval_string(al->v()[currentPos]));
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
        FloatVal fv = eval_float(vd->e());
        return PyFloat_FromDouble(fv);
      } else {
        ArrayLit* al = eval_par(vd->e())->cast<ArrayLit>();
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
          PyList_SetItem(p[i], d[i], PyFloat_FromDouble(eval_float(al->v()[currentPos])));
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
inline Expression*
one_dim_python_to_minizinc(PyObject* pvalue, Type::BaseType& code)
{
  /*enum BaseType { BT_TOP, BT_BOOL, BT_INT, BT_FLOAT, BT_STRING, BT_ANN,
                    BT_BOT, BT_UNKNOWN };*/
  switch (code) {
    case Type::BT_UNKNOWN:
      if (PyBool_Check(pvalue)) {
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
        return NULL;
      }
      break;
    case Type::BT_INT: 
      if (!PyInt_Check(pvalue)) throw std::invalid_argument("Object in an array must be of the same type: Expected an integer");
      goto BT_INTEGER_PROCESS;
    case Type::BT_FLOAT:
      if (!PyFloat_Check(pvalue)) throw std::invalid_argument("Object in an array must be of the same type: Expected a float");
      goto BT_FLOAT_PROCESS;
    case Type::BT_STRING:
      if (!PyString_Check(pvalue)) throw std::invalid_argument("Object in an array must be of the same type: Expected a string");
      goto BT_STRING_PROCESS;
    case Type::BT_BOOL:
      if (!PyBool_Check(pvalue)) throw std::invalid_argument("Object in an array must be of the same type: Expected a boolean");
      goto BT_BOOLEAN_PROCESS;
    default:
      throw std::invalid_argument("Internal Error: Received unexpected base type code");
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
  if (PyObject_TypeCheck(pvalue, &MznSetType)) {
    vector<IntSetVal::Range> ranges;
    MznSet* Set = reinterpret_cast<MznSet*>(pvalue);
    for (list<MznRange>::const_iterator it = Set->ranges->begin(); it != Set->ranges->end(); ++it) {
      ranges.push_back(IntSetVal::Range(IntVal(it->min),IntVal(it->max)));
    }
    Expression* rhs = new SetLit(Location(), IntSetVal::a(ranges));
    returnType = Type::parsetint();
    return rhs;
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


/*  
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
 *                  dimList[0] = 1,5
 *                  dimList[1] = 2,4
 *                  dimList[3] = 0,5
 *              means a 3d array [1..5,2..4,0..5]
 *        lower bound:
 *        upper bound:
 *
 *    2:    name, python value
 *        python value: Create a variable based on the existing python value
 *
 */
static PyObject*
MznModel_Variable(MznModel* self, PyObject* args)
{
  GCLock Lock;
  enum TypeId { 
        PARINT,         // 0
        PARBOOL,        // 1
        PARFLOAT,       // 2
        PARSTRING,      // 3
        ANN,            // 4
        PARSETINT,      // 5
        PARSETBOOL,     // 6
        PARSETFLOAT,    // 7
        PARSETSTRING,   // 8
        VARINT,         // 9
        VARBOOL,        //10
        VARFLOAT,       //11
        VARSETINT,      //12
        VARBOT,         //13
        BOT,            //14
        TOP,            //15
        VARTOP,         //16
        OPTVARTOP       //17
  };


  char* name;
  unsigned int tid;
  PyObject* pydim = NULL;
  PyObject* pylb = NULL;
  PyObject* pyub = NULL;
  PyObject* pyval = NULL;
  Type type;
  Expression* domain = NULL;
  Expression* initValue = NULL;
  Py_ssize_t dim;

  vector<pair<int, int> >* dimList = NULL;
  vector<TypeInst*> ranges;
  Type::BaseType code;

  if (!PyArg_ParseTuple(args, "sO|OOO", &name, &pyval, &pydim, &pylb, &pyub)) {
    PyErr_Print();
    PyErr_SetString(PyExc_TypeError, "Variable parsing error");
    return NULL;
  }

  if (PyInt_Check(pyval)) {
    tid = PyInt_AS_LONG(pyval);
    pyval = NULL;
  }


  if (pyval != NULL) {
    dimList = new vector<pair<int, int> >();
    initValue = python_to_minizinc(pyval, type, *dimList);
    dim = dimList->size();
    domain = NULL;
  } else {
    if (tid>17) {
      PyErr_SetString(PyExc_ValueError, "Type Id is not valid");
      return NULL;
    }



    if (!PyList_Check(pydim)) {
      PyErr_SetString(PyExc_TypeError, "3rd argument must be a list of size of each dimension");
      return NULL;
    }
    dim = PyList_GET_SIZE(pydim);
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

    // Create an array if dimList is not empty
    for (Py_ssize_t i=0; i!=dim; ++i) {
      Expression* e0 = new IntLit(Location(), IntVal((*dimList)[i].first));
      Expression* e1 = new IntLit(Location(), IntVal((*dimList)[i].second));
      domain = new BinOp(Location(), e0, BOT_DOTDOT, e1);
      ranges.push_back(new TypeInst(Location(), Type(), domain));
    }

    // Process different types
    switch (static_cast<TypeId>(tid)) {
      case PARINT:
          type = Type::parint(dim);
          goto INTEGER_VARIABLE;
      case VARINT:  
          type = Type::varint(dim);
          INTEGER_VARIABLE:
          code = Type::BT_INT;
          if (pyub == NULL) {
            Type tempType;
            vector<pair<int, int> > tempDimList;
            domain = python_to_minizinc(pylb, tempType, tempDimList);
            if (tempType.st() != Type::ST_SET)
              throw invalid_argument("If 5th argument does not exist, 4th argument must be a Minizinc Set");
          } else 
            domain = new BinOp(Location(),
                            one_dim_python_to_minizinc(pylb,code),
                            BOT_DOTDOT,
                            one_dim_python_to_minizinc(pyub,code) );
          break;
      case PARBOOL:
          type = Type::parbool(dim);
          goto BOOLEAN_PROCESS;
      case VARBOOL:
          type = Type::varbool(dim); 
          BOOLEAN_PROCESS:
          break;
      case PARFLOAT:
          type = Type::parfloat(dim);
          goto FLOAT_PROCESS;
      case VARFLOAT:
          type = Type::varfloat(dim);
          FLOAT_PROCESS:
          code = Type::BT_FLOAT;
          domain = new BinOp(Location(),
                          one_dim_python_to_minizinc(pylb,code),
                          BOT_DOTDOT,
                          one_dim_python_to_minizinc(pyub,code) );
          break;
      case PARSTRING: type = Type::parstring(dim); break;
      case ANN: type = Type::ann(dim); break;
      case PARSETINT: type = Type::parsetint(dim); break;
      case PARSETBOOL: type = Type::parsetbool(dim); break;
      case PARSETFLOAT: type = Type::parsetfloat(dim); break;
      case PARSETSTRING: type = Type::parsetstring(dim); break;
      case VARSETINT: type = Type::varsetint(dim); break;
      case VARBOT: type = Type::varbot(dim); break;
      case BOT: type = Type::bot(dim); break;
      case TOP: type = Type::top(dim); break;
      case VARTOP: type = Type::vartop(dim); break;
      case OPTVARTOP: type = Type::optvartop(dim); break;
    }
  }

  VarDecl* e = new VarDecl(Location(), new TypeInst(Location(), type, ranges, domain) , string(name), initValue);
  self->_m->addItem(new VarDeclI(Location(), e));
  self->loaded = true;

  MznVariable* var = reinterpret_cast<MznVariable*>(MznVariable_new(&MznVariableType, NULL, NULL));
  var->e = e->id();
  var->vd = e;
  var->dimList = dimList;

  return reinterpret_cast<PyObject*>(var);
}


/* 
 * Description: Converts a minizinc call
 * Note: Need an outer GCLock for this to work
 */
static PyObject*
Mzn_Call(MznModel* self, PyObject* args)
{
  const char* name;
  PyObject* variableTuple;
  if (!PyArg_ParseTuple(args, "sO", &name, &variableTuple)) {
    PyErr_SetString(PyExc_TypeError, "Accepts two values: a string and a tuple of minizinc variable");
    PyErr_Print();
    return NULL;
  }

  if (!PyList_Check(variableTuple)) {
    PyErr_SetString(PyExc_TypeError, "Second argument must be a tuple");
    return NULL;
  }

  long len = PyList_GET_SIZE(variableTuple);
  vector<Expression*> expressionList(len);
  for (long i = 0; i!=len; ++i) {
    PyObject* pyval = PyList_GET_ITEM(variableTuple, i);
    if (PyObject_TypeCheck(pyval, &MznVariableType)) {
      expressionList[i] = (reinterpret_cast<MznVariable*>(pyval)) -> e;
    } else {
      Type type;
      vector<pair<int, int> > dimList;
      expressionList[i] = python_to_minizinc(pyval, type, dimList);
      if (type.bt() == Type::BT_UNKNOWN) {
        PyErr_SetString(PyExc_TypeError, "List items must be of Minizinc Variable Type");
        return NULL;
      }
    }
  }
  MznVariable* var = reinterpret_cast<MznVariable*>(MznVariable_new(&MznVariableType, NULL, NULL));
  var->e = new Call(Location(), string(name), expressionList);
  var->dimList = NULL;

  return reinterpret_cast<PyObject*>(var);
}


/* 
 * Description: Creates a minizinc UnOp expression
 * Note: Need an outer GCLock for this to work
 */
static PyObject*
Mzn_UnOp(MznModel* self, PyObject* args)
{
  /*
  enum UnOpType {
    UOT_NOT,          // 0
    UOT_PLUS,         // 1
    UOT_MINUS         // 2
  };*/
  PyObject* r;
  unsigned int op;
  if (!PyArg_ParseTuple(args, "IO", &op, &r)) {
    PyErr_SetString(PyExc_TypeError, "Requires a MznVariable/MznConstraint object and an integer");
    return NULL;
  }
  Expression *rhs;

  if (PyObject_TypeCheck(r, &MznVariableType)) {
    rhs = (reinterpret_cast<MznVariable*>(r))->e;
  } else if (PyBool_Check(r)) {
    rhs = new BoolLit(Location(), PyInt_AS_LONG(r));
  } else if (PyInt_Check(r)) {
    rhs = new IntLit(Location(), IntVal(PyInt_AS_LONG(r)));
  } else if (PyFloat_Check(r)) {
    rhs = new FloatLit(Location(), PyFloat_AS_DOUBLE(r));
  } else if (PyString_Check(r)) {
    rhs = new StringLit(Location(), string(PyString_AS_STRING(r)));
  } else {
    PyErr_SetString(PyExc_TypeError, "Object must be of type MznVariable or MznConstraint");
    return NULL;
  }


  GCLock Lock;

  PyObject* var = MznVariable_new(&MznVariableType, NULL, NULL);
  (reinterpret_cast<MznVariable*>(var))->e = new UnOp(Location(), static_cast<UnOpType>(op), rhs);

  return var;
}


/* 
 * Description: Creates a minizinc BinOp expression
 * Note: Need an outer GCLock for this to work
 */
static PyObject* 
Mzn_BinOp(MznModel* self, PyObject* args)
{
  /*
  enum BinOpType {
    BOT_PLUS,         // 0
    BOT_MINUS,        // 1
    BOT_MULT,         // 2
    BOT_DIV,          // 3
    BOT_IDIV,         // 4
    BOT_MOD,          // 5
    BOT_LE,           // 6
    BOT_LQ,           // 7
    BOT_GR,           // 8
    BOT_GQ,           // 9
    BOT_EQ,           //10
    BOT_NQ,           //11
    BOT_IN,           //12
    BOT_SUBSET,       //13
    BOT_SUPERSET,     //14
    BOT_UNION,        //15
    BOT_DIFF,         //16
    BOT_SYMDIFF,      //17
    BOT_INTERSECT,    //18
    BOT_PLUSPLUS,     //19
    BOT_EQUIV,        //20
    BOT_IMPL,         //21
    BOT_RIMPL,        //22
    BOT_OR,           //23
    BOT_AND,          //24
    BOT_XOR,          //25
    BOT_DOTDOT        //26
  };*/
  PyObject* l;
  PyObject* r;
  unsigned int op;
  if (!PyArg_ParseTuple(args, "OIO", &l, &op, &r)) {
    PyErr_SetString(PyExc_TypeError, "Requires two MznVariable/MznConstraint objects and an integer");
    return NULL;
  }
  Expression *lhs, *rhs;
  if (PyObject_TypeCheck(l, &MznVariableType)) {
    lhs = (reinterpret_cast<MznVariable*>(l))->e;
  } else if (PyBool_Check(l)) {
    lhs = new BoolLit(Location(), PyInt_AS_LONG(l));
  } else if (PyInt_Check(l)) {
    lhs = new IntLit(Location(), IntVal(PyInt_AS_LONG(l)));
  } else if (PyFloat_Check(l)) {
    lhs = new FloatLit(Location(), PyFloat_AS_DOUBLE(l));
  } else if (PyString_Check(l)) {
    lhs = new StringLit(Location(), string(PyString_AS_STRING(l)));
  } else {
    PyErr_SetString(PyExc_TypeError, "Object must be of type MznVariable or MznConstraint");
    return NULL;
  }

  if (PyObject_TypeCheck(r, &MznVariableType)) {
      rhs = (reinterpret_cast<MznVariable*>(r))->e;
  } else if (PyBool_Check(r)) {
    rhs = new BoolLit(Location(), PyInt_AS_LONG(r));
  } else if (PyInt_Check(r)) {
    rhs = new IntLit(Location(), IntVal(PyInt_AS_LONG(r)));
  } else if (PyFloat_Check(r)) {
    rhs = new FloatLit(Location(), PyFloat_AS_DOUBLE(r));
  } else if (PyString_Check(r)) {
    rhs = new StringLit(Location(), string(PyString_AS_STRING(r)));
  } else {
    PyErr_SetString(PyExc_TypeError, "Object must be of type MznVariable or MznConstraint");
    return NULL;
  }


  GCLock Lock;

  PyObject* var = MznVariable_new(&MznVariableType, NULL, NULL);
  (reinterpret_cast<MznVariable*>(var))->e = new BinOp(Location(), lhs, static_cast<BinOpType>(op), rhs);

  return var;
}


/* 
 * Description: Creates a minizinc constraint
 * Note: Need an outer GCLock for this to work
 */
static PyObject* 
MznModel_Constraint(MznModel* self, PyObject* args)
{
  PyObject* obj;
  if (!PyArg_ParseTuple(args, "O", &obj)) {
    PyErr_SetString(PyExc_TypeError, "Requires an object of Minizinc Variable");
    return NULL;
  }

  GCLock Lock;
  ConstraintI* i;

  if (!PyObject_TypeCheck(obj, &MznVariableType)) {
    if (PyBool_Check(obj)) {
      bool val = PyObject_IsTrue(obj);
      i = new ConstraintI(Location(), new BoolLit(Location(), val));
    } else {
      PyErr_SetString(PyExc_TypeError, "Object must be a Minizinc Variable");
      return NULL;
    }
  } else
    i = new ConstraintI(Location(), (reinterpret_cast<MznVariable*>(obj))->e);
  self->_m->addItem(i);
  Py_RETURN_NONE;
}

/* 
 * Description: Defines the type of solution of the model
 */
static PyObject* 
MznModel_SolveItem(MznModel* self, PyObject* args)
{
  unsigned int solveType;
  PyObject* obj = NULL;
  Expression* e = NULL;

  if (!PyArg_ParseTuple(args, "I|O", &solveType, &obj)) {
    PyErr_SetString(PyExc_TypeError, "Requires a solver code and an optional expression");
    return NULL;
  }

  if (solveType > 2) {
    PyErr_SetString(PyExc_ValueError, "Invalid solver code");
    return NULL;
  }
  if (solveType) {
    if (obj == NULL) {
      PyErr_SetString(PyExc_TypeError, "Optimisation solver requires an addition constraint object");
      return NULL;
    } else if (PyObject_TypeCheck(obj, &MznVariableType))  {
      e = (reinterpret_cast<MznVariable*>(obj))->e;
    }
    else {
      PyErr_SetString(PyExc_TypeError, "Requires a Minizinc Constraint Object");
      return NULL;
    }
  }

  GCLock Lock;
  SolveI* i;
  switch (solveType) {
    case 0: i = SolveI::sat(Location()); break;
    case 1: i = SolveI::min(Location(),(e)); break;
    case 2: i = SolveI::max(Location(),(e)); break;
  }
  self->_m->addItem(i);
  Py_RETURN_NONE;
}


// Helper functions

int getList(PyObject* value, vector<Py_ssize_t>& dimensions, vector<PyObject*>& simpleArray, const int layer)
{
  for (Py_ssize_t i=0; i<PyList_Size(value); i++) {
    if (dimensions.size() <= layer) {
      dimensions.push_back(PyList_Size(value));
    } else if (dimensions[layer]!=PyList_Size(value)) {
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

static PyObject*
MznModel_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  MznModel* self = reinterpret_cast<MznModel*>(type->tp_alloc(type,0));
  self->includePaths = new vector<string>;
  return reinterpret_cast<PyObject*>(self);
}

static int
MznModel_init(MznModel* self, PyObject* args)
{
  self->loaded = false;
  string std_lib_dir;
  if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
    std_lib_dir = string(MZNSTDLIBDIR);
  } else {
    PyErr_SetString(PyExc_EnvironmentError, "No MiniZinc library directory MZN_STDLIB_DIR defined.");
    return -1;
  }
  self->timeLimit = 0;
  self->includePaths->push_back(std_lib_dir+"/gecode/");
  self->includePaths->push_back(std_lib_dir+"/std/");
  stringstream errorStream;
  self->_m = parseFromString("","error.txt",*(self->includePaths),false,false,false, errorStream);
  if (!(self->_m)) {
    const std::string& tmp = errorStream.str();
    const char* cstr = tmp.c_str();
    PyErr_SetString(PyExc_EnvironmentError, cstr);
    return -1;
  }
  return 0;
}

static void
MznModel_dealloc(MznModel* self)
{
  if (self->_m)
    delete self->_m;
  delete self->includePaths;
  self->ob_type->tp_free(reinterpret_cast<PyObject*>(self));
}

int
MznModel::addData(const char* const name, PyObject* value)
{
  GCLock Lock;
  if (value == NULL) {
    PyErr_SetString(PyExc_ValueError, "Cannot add NULL");
    return -1;
  }
  for (unsigned int i=0; i<_m->size(); i++) 
    if (VarDeclI* vdi = (*_m)[i]->dyn_cast<VarDeclI>()) {
      if (strcmp(vdi->e()->id()->str().c_str(), name) == 0) {
        vector<pair<int, int> > dimList;
        Type type;
        Expression* rhs = python_to_minizinc(value, type, dimList);//, vdi->e()->type(), name);
        if (rhs == NULL)
          return -1;
        vdi->e()->e(rhs);
        return 0;
      }
    }
  string ret = "Undefined name '" + string(name) + "'";
  PyErr_SetString(PyExc_TypeError, ret.c_str());
  return -1;
}

static PyObject* MznModel_addData(MznModel* self, PyObject* args)
{
  PyObject* obj;
  const char* name;
  if (!PyArg_ParseTuple(args, "sO", &name, &obj)) {
    PyErr_SetString(PyExc_RuntimeError, "Parsing error");
    return NULL;
  }
  if (self->addData(name,obj)==-1)
    return NULL;
  Py_RETURN_NONE;
}

int 
MznModel::load(PyObject *args, PyObject *keywds, bool fromFile)
{
  GCLock Lock;
  Model* saveModel = _m;
  stringstream errorStream;
  vector<string> data;

  PyObject* obj = Py_None;
  Py_ssize_t pos = 0;
  PyObject* key;
  PyObject* value;
  char* options = NULL;
  const char* py_string;
  char* errorFile = "./error.txt";

  bool isDict = false;

  if (fromFile) {
    char *kwlist[] = {"file","data","options"};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s|Os", kwlist, &py_string, &obj, &options)) {
      PyErr_SetString(PyExc_TypeError, "Parsing error");
      return -1;
    }
    if (options != NULL) {
      char* pch;
      bool t_flag = false;
      pch = strtok(options, " ");
      while (pch != NULL) {
        if (strcmp(pch,"-t")==0)
          t_flag = true;
        else {
          if (t_flag) {
            char* ptr;
            int t = strtol(pch,&ptr,10);
            if (t == 0) {
              PyErr_SetString(PyExc_ValueError, "Time value must be a valid positive number");
              return -1;
            }
            timeLimit = t;
            t_flag = false;
          } else {
            PyErr_SetString(PyExc_ValueError, "Unknown option");
            return -1;
          }
        }
        pch = strtok(NULL, " ");
      }
    }
    if (obj != Py_None) {
      if (PyString_Check(obj)) {
        data.push_back(string(PyString_AS_STRING(obj)));
      } else if (PyList_Check(obj)) {
        Py_ssize_t n = PyList_GET_SIZE(obj);
        for (Py_ssize_t i = 0; i!=n; ++i) {
          char* name = PyString_AsString(PyList_GET_ITEM(obj, i));
          if (name == NULL) {
            PyErr_SetString(PyExc_TypeError, "Element in the list must be a filename");
            return -1;
          }
          data.push_back(string(name));
        }
      } else if (!PyDict_Check(obj)) {
        PyErr_SetString(PyExc_TypeError, "The second argument must be either a filename, a list of filenames or a dictionary of data");
        return -1;
      }
      isDict = true;
    }
    _m = parse(string(py_string), data, *includePaths, false, false, false, errorStream);
  } else {
    char *kwlist[] = {"string","error","options"};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s|Os", kwlist, &py_string, &errorFile, &options))
      return -1;
    _m = parseFromString(string(py_string), errorFile, *includePaths, false, false, false, errorStream);
  }
  if (_m) {
    delete saveModel;
    if (isDict) {
      stringstream assignments;
      Py_ssize_t pos = 0;
      PyObject* key;
      PyObject* value;
      GCLock lock;
      while (PyDict_Next(obj, &pos, &key, &value)) {
        char* name = PyString_AS_STRING(key);
        if (addData(name,value) == -1)
          return -1;
      }
    }
    loaded = true;
    return 0;
  } else {
    const std::string& tmp = errorStream.str();
    const char* cstr = tmp.c_str();
    PyErr_SetString(MznModel_load_error, cstr);
    return -1;
  }
}


PyObject* MznModel::solve()
{

  if (!loaded) {
    PyErr_SetString(PyExc_RuntimeError, "No data has been loaded into the model");
    return NULL;
  }
  loaded = false;
  debugprint(_m);
  vector<TypeError> typeErrors;
  try {
    MiniZinc::typecheck(_m, typeErrors);
  } catch (LocationException& e) {
    stringstream errorLog;
    errorLog << e.what() << ": " << std::endl;
    errorLog << "  " << e.msg() << std::endl;
    const std::string& tmp = errorLog.str();
    const char* cstr = tmp.c_str();
    PyErr_SetString(PyExc_TypeError, cstr);
    return NULL;
  }
  if (typeErrors.size() > 0) {
    stringstream errorLog;
    for (unsigned int i=0; i<typeErrors.size(); i++) {
      errorLog << typeErrors[i].loc() << ":" << endl;
      errorLog << typeErrors[i].what() << ": " << typeErrors[i].msg() << "\n";
    }
    const std::string& tmp = errorLog.str();
    const char* cstr = tmp.c_str();
    PyErr_SetString(PyExc_TypeError, cstr);
    return NULL;
  }
  MiniZinc::registerBuiltins(_m);
  Env* env = new Env(_m);
  try {
    FlatteningOptions fopts;
    flatten(*env,fopts);
  } catch (LocationException& e) {
    stringstream errorLog;
    errorLog << e.what() << ": " << std::endl;
    env->dumpErrorStack(errorLog);
    errorLog << "  " << e.msg() << std::endl;
    const std::string& tmp = errorLog.str();
    const char* cstr = tmp.c_str();
    PyErr_SetString(PyExc_RuntimeError, cstr);
    return NULL;
  }
  if (env->warnings().size()!=0)
  {
    stringstream warningLog;
    for (unsigned int i=0; i<env->warnings().size(); i++) {
      warningLog << "Warning: " << env->warnings()[i];
    }
    const std::string& tmp = warningLog.str();
    const char* cstr = tmp.c_str();
    PyErr_WarnEx(PyExc_RuntimeWarning, cstr, 1);
  }
  optimize(*env);
  oldflatzinc(*env);
  GCLock lock;
  Options options;
  MznSolution* ret = reinterpret_cast<MznSolution*>(MznSolution_new(&MznSolutionType, NULL, NULL));
  ret->solver = new GecodeSolverInstance(*env, options);
  ret->solver->processFlatZinc();
  ret->env = env;
  return reinterpret_cast<PyObject*>(ret);
  //GecodeSolverInstance gecode(*env,options);
  //gecode.processFlatZinc();
}

PyObject* 
MznSolution_getValue(MznSolution* self, PyObject* args) {
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
          else 
            return NULL;
        }
      }
    }
    PyErr_SetString(PyExc_RuntimeError, "Name not found");
  } else
    PyErr_SetString(PyExc_RuntimeError, "No model (maybe you need to call Model.next() first");
  return NULL;
}


PyObject*
MznSolution::next()
{
  if (solver==NULL)
    throw runtime_error("Solver Object not found");
  GCLock lock;
  SolverInstance::Status status = solver->solve();
  if (!(status==SolverInstance::SAT || status==SolverInstance::OPT)) {
    PyErr_SetString(PyExc_RuntimeError,"Unsatisfied");
    return NULL;
  }  
  PyObject* solutions = PyList_New(0);
  PyObject* sol = PyDict_New();
  _m = env->output();
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
}


static PyObject*
MznModel_load(MznModel *self, PyObject *args, PyObject *keywds) {
  if (self->load(args, keywds, true) < 0)
    return NULL;
  Py_RETURN_NONE;
}

static PyObject*
MznModel_loadFromString(MznModel *self, PyObject *args, PyObject *keywds) {
  if (self->load(args, keywds, false) < 0)
    return NULL;
  Py_RETURN_NONE;
}

static PyObject*
MznModel_solve(MznModel *self)
{
  if (self->timeLimit != 0) {
    signal(SIGALRM, sig_alrm);
    alarm(self->timeLimit);
    if (sigsetjmp(jmpbuf, 1)) {
      PyErr_SetString(PyExc_RuntimeError, "Time out");
      return NULL;
    }
    PyObject* result = self->solve();
    alarm(0);
    return result;
  } else 
    return self->solve();
}

static PyObject*
MznModel_setTimeLimit(MznModel *self, PyObject *args)
{
  if (PyInt_Check(args)) {
    PyErr_SetString(PyExc_TypeError, "Argument must be an integer");
    return NULL;
  }
  self->timeLimit = PyInt_AS_LONG(args);
  return Py_None;
}





static PyObject* Mzn_load(PyObject* self, PyObject* args, PyObject* keywds) {
  PyObject* model = MznModel_new(&MznModelType, NULL, NULL);
  if (model == NULL)
    return NULL;
  if (MznModel_init(reinterpret_cast<MznModel*>(model), NULL) < 0)
    return NULL;
  if (MznModel_load(reinterpret_cast<MznModel*>(model), args, keywds)==NULL)
    return NULL;
  return model;
}

static PyObject* Mzn_loadFromString(PyObject* self, PyObject* args, PyObject* keywds) {
  PyObject* model = MznModel_new(&MznModelType, NULL, NULL);
  if (model == NULL)
    return NULL;
  if (MznModel_init(reinterpret_cast<MznModel*>(model), NULL) < 0)
    return NULL;
  if (MznModel_loadFromString(reinterpret_cast<MznModel*>(model), args, keywds)==NULL)
    return NULL;
  return model;
}


static void
MznSolution_dealloc(MznSolution* self)
{
  /*if (self->_m)
    delete self->_m;
  if (self->env)
    delete self->env;*/
  if (self->solver)
    delete self->solver;
  self->_m = NULL;
  self->env = NULL;
  self->ob_type->tp_free(reinterpret_cast<PyObject*>(self));
}

static PyObject*
MznSolution_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  MznSolution* self = reinterpret_cast<MznSolution*>(type->tp_alloc(type,0));
  self->solver = NULL;
  self->_m = NULL;
  self->env = NULL;
  return reinterpret_cast<PyObject*>(self);
}

static int
MznSolution_init(MznSolution* self, PyObject* args)
{
  return 0;
}

static PyObject*
MznSolution_next(MznSolution *self)
{
  return self->next();
}
