/*
 *  Python Interface for MiniZinc constraint modelling
 *  Main authors:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *          under the supervision of Guido Tack <guido.tack@monash.edu>
 */

#include "pyinterface.h";

using namespace MiniZinc;
using namespace std;

static PyObject*
MznModel_Variable(MznModel* self, PyObject* args)
{
  self->loaded = true;
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
  unsigned int tid=999;
  unsigned int dim=0;
  PyObject* lb = NULL;
  PyObject* ub = NULL;
  Type t;
  GCLock Lock;
  Expression* domain = NULL;

  if (!PyArg_ParseTuple(args, "s|IIOO", &name, &tid, &dim, &lb, &ub)) {
    PyErr_SetString(PyExc_TypeError, "Requires a name for the variable");
    return NULL;
  }
  if (tid == 999)
    t = Type();
  else if (tid>17) {
    PyErr_SetString(PyExc_ValueError, "Type Id is not valid");
    return NULL;
  } else {
    switch (static_cast<TypeId>(tid)) {
      case PARINT: t = Type::parint(dim); break;
      case PARBOOL: t = Type::parbool(dim); break;
      case PARFLOAT: t = Type::parbool(dim); break;
      case PARSTRING: t = Type::parstring(dim); break;
      case ANN: t = Type::ann(dim); break;
      case PARSETINT: t = Type::parsetint(dim); break;
      case PARSETBOOL: t = Type::parsetbool(dim); break;
      case PARSETFLOAT: t = Type::parsetfloat(dim); break;
      case PARSETSTRING: t = Type::parsetstring(dim); break;
      case VARINT:
        {
          t = Type::varint(dim);
          if (!(PyInt_Check(lb) && PyInt_Check(ub))) {
            PyErr_SetString(PyExc_TypeError, "Domain of integer values needed");
            return NULL;
          }
          long lbvalue = PyInt_AS_LONG(lb);
          long ubvalue = PyInt_AS_LONG(ub);
          if (lbvalue > ubvalue)
            swap(lbvalue, ubvalue);
          Expression* e0 = new IntLit(Location(), IntVal(lbvalue));
          Expression* e1 = new IntLit(Location(), IntVal(ubvalue));
          domain = new BinOp(Location(), e0, BOT_DOTDOT, e1);
          break;
        }
      case VARBOOL: t = Type::varbool(dim); break;
      case VARFLOAT:
        {
          t = Type::varfloat(dim);
          if (!(PyFloat_Check(lb) && PyFloat_Check(ub))) {
            PyErr_SetString(PyExc_TypeError, "Domain of float values needed");
            return NULL;
          }
          double lbvalue = PyFloat_AS_DOUBLE(lb);
          double ubvalue = PyFloat_AS_DOUBLE(ub);
          if (lbvalue > ubvalue)
            swap(lbvalue, ubvalue);
          Expression* e0 = new FloatLit(Location(), FloatVal(lbvalue));
          Expression* e1 = new FloatLit(Location(), FloatVal(ubvalue));
          domain = new BinOp(Location(), e0, BOT_DOTDOT, e1);
          break;
        }
      case VARSETINT: t = Type::varsetint(dim); break;
      case VARBOT: t = Type::varbot(dim); break;
      case BOT: t = Type::bot(dim); break;
      case TOP: t = Type::top(dim); break;
      case VARTOP: t = Type::vartop(dim); break;
      case OPTVARTOP: t = Type::optvartop(dim); break;
    }
  }
  VarDecl* e = new VarDecl(Location(), new TypeInst(Location(), t, domain) , string(name), NULL);
  VarDeclI* i = new VarDeclI(Location(), e);
  self->_m->addItem(i);

  PyObject* var = MznVariable_new(&MznVariableType, NULL, NULL);
  ((MznVariable*)var)->e = e->id();

  return var;
}

static PyObject* 
MznModel_Expression(MznModel* self, PyObject* args)
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
  if (PyObject_TypeCheck(l, &MznVariableType))
    lhs = ((MznVariable*)l)->e;
  //else if (PyObject_TypeCheck(l, &MznConstraintType))
  //  lhs = ((MznConstraint*)l)->i->e();
  else {
    PyErr_SetString(PyExc_TypeError, "Object must be of type MznVariable or MznConstraint");
    return NULL;
  }

  if (PyObject_TypeCheck(r, &MznVariableType))
    rhs = ((MznVariable*)r)->e;
  //else if (PyObject_TypeCheck(r, &MznConstraintType))
  //  rhs = ((MznConstraint*)r)->i->e();
  else {
    PyErr_SetString(PyExc_TypeError, "Object must be of type MznVariable or MznConstraint");
    return NULL;
  }


  GCLock Lock;
  BinOp* binop = new BinOp(Location(), lhs, static_cast<BinOpType>(op), rhs); 

  PyObject* var = MznVariable_new(&MznVariableType, NULL, NULL);
  ((MznVariable*)var)->e = binop;

  return var;
}

static PyObject* 
MznModel_Constraint(MznModel* self, PyObject* args)
{
  PyObject* obj;
  if (!PyArg_ParseTuple(args, "O", &obj)) {
    PyErr_SetString(PyExc_TypeError, "Requires an object of Minizinc Variable");
    return NULL;
  }
  if (!PyObject_TypeCheck(obj, &MznVariableType)) {
    PyErr_SetString(PyExc_TypeError, "Object must be a Minizinc Variable");
    return NULL;
  }
  GCLock Lock;
  ConstraintI* i = new ConstraintI(Location(), ((MznVariable*)obj)->e);
  self->_m->addItem(i);
  Py_RETURN_NONE;
}


static PyObject* 
MznModel_SolveItem(MznModel* self, PyObject* args)
{
  unsigned int solveType;
  PyObject* obj = NULL;

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
    } else if (!PyObject_TypeCheck(obj, &MznVariableType)) {
      PyErr_SetString(PyExc_TypeError, "Requires a Minizinc Constraint Object");
      return NULL;
    }
  }
  GCLock Lock;
  SolveI* i;
  switch (solveType) {
    case 0: i = SolveI::sat(Location()); break;
    case 1: i = SolveI::min(Location(),((MznVariable*)obj)->e ); break;
    case 2: i = SolveI::max(Location(),((MznVariable*)obj)->e ); break;
  }
  self->_m->addItem(i);
  Py_RETURN_NONE;
}


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
  MznModel* self = (MznModel*)type->tp_alloc(type,0);
  //self->_m = new Model;
  self->includePaths = new vector<string>;
  //self->data = new vector<string>;
  return (PyObject* )self;
}

static int
MznModel_init(MznModel* self, PyObject* args)
{
  // add later
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
  delete self->_m;
  delete self->includePaths;
  //delete self->data;
  self->ob_type->tp_free((PyObject*)self);
}

int
MznModel::addData(const char* const name, PyObject* value)
{
  for (unsigned int i=0; i<_m->size(); i++) 
    if (VarDeclI* vdi = (*_m)[i]->dyn_cast<VarDeclI>()) {
      if (strcmp(vdi->e()->id()->str().c_str(), name) == 0) {
        if (vdi->e()->type().st() == Type::ST_SET) {
          if (PyObject_TypeCheck(value, &MznSetType)) {
            vector<IntSetVal::Range> ranges;
            MznSet* Set = (MznSet*)value;
            for (list<MznRange>::const_iterator it = Set->ranges->begin(); it != Set->ranges->end(); ++it) {
              ranges.push_back(IntSetVal::Range(IntVal(it->min),IntVal(it->max)));
            }
            Expression* rhs = new SetLit(Location(), IntSetVal::a(ranges));
            vdi->e()->e(rhs);
          } else {
            string errorLog = "type-inst error: output expression " + string(name) + " has invalid type-inst: expected a set";
            PyErr_SetString(PyExc_TypeError, errorLog.c_str());
            return -1;
          }
        } else if (vdi->e()->type().bt() == Type::BT_INT) {
          if (vdi->e()->type().dim() == 0) {
            if (PyInt_Check(value)) {
              Expression* rhs = new IntLit(Location(), IntVal(PyInt_AS_LONG(value)));
              vdi->e()->e(rhs);
            } else {
              string errorLog = "type-inst error: output expression " + string(name) + " has invalid type-inst: expected an integer";
              PyErr_SetString(PyExc_TypeError, errorLog.c_str());
              return -1;
            }
          } else {
            if (PyList_Check(value)) {
              ArrayLit* al = vdi->e()->cast<ArrayLit>();
              vector<Py_ssize_t> dimensions;
              vector<PyObject*> simpleArray;
              if (getList(value, dimensions, simpleArray, 0) == -1) {
                PyErr_SetString(PyExc_TypeError, "Inconsistency in size of multidimensional array");
                return -1;
              }
              if (vdi->e()->type().dim() != dimensions.size()) {
                string errorLog = "type-inst error: output expression " + string(name) + " has invalid type-inst: number of dimension not conforms";
                PyErr_SetString(PyExc_TypeError, errorLog.c_str());
                return -1;
              }
              vector<Expression*> v;
              vector<pair<int,int> > dims;
              for (int i=0; i!=simpleArray.size(); ++i) {
                PyObject* temp= simpleArray[i];
                if (!PyInt_Check(temp)) {
                  string errorLog = "Expected integer value";
                  PyErr_SetString(PyExc_TypeError, errorLog.c_str());
                  return -1;
                }
                Expression* rhs = new IntLit(Location(), IntVal(PyInt_AS_LONG(temp)));
                v.push_back(rhs);
              }

              for (int i=0; i!=dimensions.size(); i++)
                dims.push_back(pair<Py_ssize_t,Py_ssize_t>(1,dimensions[i]));
              Expression* rhs = new ArrayLit(Location(), v, dims);
              vdi->e()->e(rhs);
            } else {
              string errorLog = "type-inst error: output expression " + string(name) + " has invalid type-inst: expected an array of integer";
              PyErr_SetString(PyExc_TypeError, errorLog.c_str());
              return -1;
            }
          }
        } else if (vdi->e()->type().bt() == Type::BT_BOOL) {
          if (PyBool_Check(value)) {
            Expression* rhs = new BoolLit(Location(), PyInt_AS_LONG(value));
            vdi->e()->e(rhs);
          } else if (PyList_Check(value)) {
            ArrayLit* al = vdi->e()->cast<ArrayLit>();
            vector<Py_ssize_t> dimensions;
            vector<PyObject*> simpleArray;
            if (getList(value, dimensions, simpleArray, 0) == -1) {
              PyErr_SetString(PyExc_TypeError, "Inconsistency in size of multidimensional array");
              return -1;
            }
            if (vdi->e()->type().dim() != dimensions.size()) {
              string errorLog = "type-inst error: output expression " + string(name) + " has invalid type-inst: number of dimension not conforms";
              PyErr_SetString(PyExc_TypeError, errorLog.c_str());
              return -1;
            }
            vector<Expression*> v;
            vector<pair<int,int> > dims;
            for (int i=0; i!=simpleArray.size(); ++i) {
              PyObject* temp= simpleArray[i];
              if (!PyInt_Check(temp)) {
                string errorLog = "Expected boolean value";
                PyErr_SetString(PyExc_TypeError, errorLog.c_str());
                return -1;
              }
              Expression* rhs = new BoolLit(Location(), PyInt_AS_LONG(temp));
              v.push_back(rhs);
            }

            for (int i=0; i!=dimensions.size(); i++)
              dims.push_back(pair<int,int>(1,dimensions[i]));
            Expression* rhs = new ArrayLit(Location(), v, dims);
            vdi->e()->e(rhs);
          } else {
          string errorLog = "type-inst error: output expression " + string(name) + " has invalid type-inst: expected a boolean";
          PyErr_SetString(PyExc_TypeError, errorLog.c_str());
          return -1;
          }
        } else if (vdi->e()->type().bt() == Type::BT_STRING) {
          if (PyString_Check(value)) {
            Expression* rhs = new StringLit(Location(), PyString_AS_STRING(value));
            vdi->e()->e(rhs);
          } else if (PyList_Check(value)) {
            ArrayLit* al = vdi->e()->cast<ArrayLit>();
            vector<Py_ssize_t> dimensions;
            vector<PyObject*> simpleArray;
            if (getList(value, dimensions, simpleArray, 0) == -1) {
              PyErr_SetString(PyExc_TypeError, "Inconsistency in size of multidimensional array");
              return -1;
            }
            if (vdi->e()->type().dim() != dimensions.size()) {
              string errorLog = "type-inst error: output expression " + string(name) + " has invalid type-inst: number of dimension not conforms";
              PyErr_SetString(PyExc_TypeError, errorLog.c_str());
              return -1;
            }
            vector<Expression*> v;
            vector<pair<int,int> > dims;
            for (int i=0; i!=simpleArray.size(); ++i) {
              PyObject* temp= simpleArray[i];
              if (!PyString_Check(temp)) {
                string errorLog = "Expected string value";
                PyErr_SetString(PyExc_TypeError, errorLog.c_str());
                return -1;
              }
              Expression* rhs = new StringLit(Location(), PyString_AS_STRING(temp));
              v.push_back(rhs);
            }

            for (int i=0; i!=dimensions.size(); i++)
              dims.push_back(pair<int,int>(1,dimensions[i]));
            Expression* rhs = new ArrayLit(Location(), v, dims);
            vdi->e()->e(rhs);
          } else {
            string errorLog = "type-inst error: output expression " + string(name) + " has invalid type-inst: expected a string";
            PyErr_SetString(PyExc_TypeError, errorLog.c_str());
            return -1;
          }
        } else if (vdi->e()->type().bt() == Type::BT_FLOAT) {
          if (PyFloat_Check(value)) {
            Expression* rhs = new FloatLit(Location(), PyFloat_AS_DOUBLE(value));
            vdi->e()->e(rhs);
          } else if (PyList_Check(value)) {
            ArrayLit* al = vdi->e()->cast<ArrayLit>();
            vector<Py_ssize_t> dimensions;
            vector<PyObject*> simpleArray;
            if (getList(value, dimensions, simpleArray, 0) == -1) {
              PyErr_SetString(PyExc_TypeError, "Inconsistency in size of multidimensional array");
              return -1;
            }
            if (vdi->e()->type().dim() != dimensions.size()) {
              string errorLog = "type-inst error: output expression '" + string(name) + "'' has invalid type-inst: number of dimension not conforms";
              PyErr_SetString(PyExc_TypeError, errorLog.c_str());
              return -1;
            }
            vector<Expression*> v;
            vector<pair<int,int> > dims;
            for (int i=0; i!=simpleArray.size(); ++i) {
              PyObject* temp= simpleArray[i];
              if (!PyFloat_Check(temp)) {
                string errorLog = "Expected float value";
                PyErr_SetString(PyExc_TypeError, errorLog.c_str());
                return -1;
              }
              Expression* rhs = new FloatLit(Location(), PyFloat_AS_DOUBLE(temp));
              v.push_back(rhs);
            }

            for (int i=0; i!=dimensions.size(); i++)
              dims.push_back(pair<int,int>(1,dimensions[i]));
            Expression* rhs = new ArrayLit(Location(), v, dims);
            vdi->e()->e(rhs);
          } else {
            string errorLog = "type-inst error: output expression " + string(name) + " has invalid type-inst: expected a float";
            PyErr_SetString(PyExc_TypeError, errorLog.c_str());
            return -1;
          }
        } else {
          PyErr_SetString(PyExc_TypeError, "Unhandled type");
          return -1;
        }
      }
    }
  return 0;
}

int 
MznModel::load(PyObject *args, PyObject *keywds, bool fromFile)
{
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
  if (MznModel_init((MznModel*)model, NULL) < 0)
    return NULL;
  if (MznModel_load((MznModel*)model, args, keywds)==NULL)
    return NULL;
  return model;
}

static PyObject* Mzn_loadFromString(PyObject* self, PyObject* args, PyObject* keywds) {
  PyObject* model = MznModel_new(&MznModelType, NULL, NULL);
  if (model == NULL)
    return NULL;
  if (MznModel_init((MznModel*)model, NULL) < 0)
    return NULL;
  if (MznModel_loadFromString((MznModel*)model, args, keywds)==NULL)
    return NULL;
  return model;
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
  GecodeSolverInstance gecode(*env,options);
  gecode.processFlatZinc();
  SolverInstance::Status status = gecode.solve();
  if (status==SolverInstance::SAT || status==SolverInstance::OPT) {
    PyObject* result = MznSolution_new(&MznSolutionType, NULL, NULL);
    ((MznSolution*)result)->env = env;
    ((MznSolution*)result)->status = status;
    return result; 
  } else {
    PyErr_SetString(PyExc_RuntimeError,"Unknown status code");
    return NULL;
  }  
}


PyObject*
MznSolution::next()
{
  PyObject* solutions = PyList_New(0);
  PyObject* sol = PyDict_New();
  GCLock lock;
  Model* _m = env->output();
  for (unsigned int i=0; i<_m->size(); i++) {
    if (VarDeclI* vdi = (*_m)[i]->dyn_cast<VarDeclI>()) {
      if (vdi->e()->type().st() == Type::ST_SET) {
        IntSetVal* isv = eval_intset(vdi->e()->e());
        long numberOfElement = 0;
        MznSet* newSet = (MznSet*) MznSet_new(&MznSetType,NULL,NULL);
        
        for (IntSetRanges isr(isv); isr(); ++isr) {
          newSet->push(isr.min().toInt(),isr.max().toInt());
        }
        PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(),(PyObject*)newSet);
      } else {
        if (vdi->e()->type().bt() == Type::BT_BOOL) {
          if (vdi->e()->type().dim() == 0) {
            PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(), PyBool_FromLong(eval_bool(vdi->e()->e())));
          } else {
            ArrayLit* al = eval_par(vdi->e()->e())->cast<ArrayLit>();
            int dim = vdi->e()->type().dim();

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
            PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(),p[0]);
            for (int i=0; i<dim; i++)
              Py_DECREF(p[i]);
          }
        } else if (vdi->e()->type().bt() == Type::BT_INT) {
          if (vdi->e()->type().dim() == 0) {
            IntVal iv = eval_int(vdi->e()->e());
            PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(), PyInt_FromLong(iv.toInt()));
          } else {
            ArrayLit* al = eval_par(vdi->e()->e())->cast<ArrayLit>();
            int dim = vdi->e()->type().dim();

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
            PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(),p[0]);
            for (int i=0; i<dim; i++)
              Py_DECREF(p[i]);
          }
        } else if (vdi->e()->type().bt() == Type::BT_STRING) {
          if (vdi->e()->type().dim() == 0) {
            string temp(eval_string(vdi->e()->e()));
            PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(), PyString_FromString(temp.c_str()));
          } else {
            ArrayLit* al = eval_par(vdi->e()->e())->cast<ArrayLit>();
            int dim = vdi->e()->type().dim();

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
            PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(),p[0]);
            for (int i=0; i<dim; i++)
              Py_DECREF(p[i]);
          }
        } else if (vdi->e()->type().bt() == Type::BT_FLOAT) {
          if (vdi->e()->type().dim() == 0) {
            FloatVal fv = eval_float(vdi->e()->e());
            PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(), PyFloat_FromDouble(fv));
          } else {
            ArrayLit* al = eval_par(vdi->e()->e())->cast<ArrayLit>();
            int dim = vdi->e()->type().dim();

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
            PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(),p[0]);
            for (int i=0; i<dim; i++) 
              Py_DECREF(p[i]);
          }
        }
      }
    }
  }
  PyList_Append(solutions, sol);
  PyObject* ret = Py_BuildValue("iO", status, solutions);
  Py_DECREF(sol);
  Py_DECREF(solutions);
  return ret; 
}

static void
MznSolution_dealloc(MznSolution* self)
{
  if (self->env!=NULL)
    delete self->env;
  self->ob_type->tp_free((PyObject*)self);
}

static PyObject*
MznSolution_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  MznSolution* self = (MznSolution*)(type->tp_alloc(type,0));
  self->env = NULL;
  return (PyObject*) self;
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