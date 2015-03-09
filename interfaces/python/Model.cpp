#include "Model.h"

using namespace MiniZinc;
using namespace std;


int
MznModel::addData(const char* const name, PyObject* value)
{
  GCLock Lock;
  if (value == NULL) {
    PyErr_SetString(PyExc_TypeError, "MiniZinc: Model.addData: Received a NULL value");
    return -1;
  }
  for (unsigned int i=0; i<_m->size(); i++) 
    if (VarDeclI* vdi = (*_m)[i]->dyn_cast<VarDeclI>()) {
      if (strcmp(vdi->e()->id()->str().c_str(), name) == 0) {
        vector<pair<int, int> > dimList;
        Type type;
        Expression* rhs = python_to_minizinc(value, vdi->e()->ti()->ranges());//, vdi->e()->type(), name);
        if (rhs == NULL)
          return -1;
        vdi->e()->e(rhs);
        return 0;
      }
    }
  MZN_PYERR_SET_STRING(PyExc_TypeError, "MiniZinc: Model.addData: Undefined name '%s'", name);
  return -1;
}

int 
MznModel::load(PyObject *args, PyObject *keywds, bool fromFile)
{
  GCLock Lock;
  Model* saveModel = _m;
  stringstream errorStream;
  vector<string> data;

  PyObject* obj = Py_None;
  char* options = NULL;
  const char* py_string;
  char* errorFile = "./error.txt";

  bool isDict = false;

  if (fromFile) {
    char *kwlist[] = {"file","data","options"};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s|Os", kwlist, &py_string, &obj, &options)) {
      PyErr_SetString(PyExc_TypeError, "MiniZinc: load: Parsing error");
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
              PyErr_SetString(PyExc_ValueError, "MiniZinc: Model.load: Time value must be a valid positive number");
              return -1;
            }
            timeLimit = t;
            t_flag = false;
          } else {
            PyErr_SetString(PyExc_ValueError, "MiniZinc: Model.load: Unknown option");
            return -1;
          }
        }
        pch = strtok(NULL, " ");
      }
    }
    if (obj != Py_None) {
      if (PyUnicode_Check(obj)) {
        data.push_back(string(PyUnicode_AS_DATA(obj)));
      } else if (PyList_Check(obj)) {
        Py_ssize_t n = PyList_GET_SIZE(obj);
        for (Py_ssize_t i = 0; i!=n; ++i) {
          const char* name = PyUnicode_AS_DATA(PyList_GET_ITEM(obj, i));
          if (name == NULL) {
            PyErr_SetString(PyExc_TypeError, "MiniZinc: Model.load: Element in the list must be a filename");
            return -1;
          }
          data.push_back(string(name));
        }
      } else if (PyDict_Check(obj)) {
        isDict = true;
      }

      PyErr_SetString(PyExc_TypeError, "MiniZinc: Model.load: The second argument must be either a filename, a list of filenames or a dictionary of data");
      return -1;
    }
    _m = parse(string(py_string), data, *includePaths, false, false, false, errorStream);
  } else {
    char *kwlist[] = {"string","error","options"};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s|Os", kwlist, &py_string, &errorFile, &options)) {
      PyErr_SetString(PyExc_TypeError, "MiniZinc: Model.load: Keyword parsing error");
      return -1;
    }
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
        const char* name = PyUnicode_AS_DATA(key);
        if (addData(name,value) == -1) {
          // addData handles the error message
          return -1;
        }
      }
    }
    loaded = true;
    return 0;
  } else {
    const std::string tmp = "MiniZinc: Model.load: " + errorStream.str();
    PyErr_SetString(PyExc_RuntimeError, tmp.c_str());
    return -1;
  }
}


PyObject* MznModel::solve(PyObject* args)
{
  if (!loaded) {
    PyErr_SetString(PyExc_RuntimeError, "MiniZinc: Model.solve: No data has been loaded into the model");
    return NULL;
  }

  PyObject* dict = NULL;
  if (!PyArg_ParseTuple(args, "|O", &dict)) {
    PyErr_SetString(PyExc_RuntimeError, "MiniZinc: Model.solve: Parsing error");
    return NULL;
  }
  debugprint(_m);
  Model* saveModel;
  {
    GCLock lock;
    saveModel = copy(_m);
    Py_ssize_t pos = 0;
    PyObject* key;
    PyObject* value;
    if (dict) {
      while (PyDict_Next(dict, &pos, &key, &value)) {
        const char* name = PyUnicode_AS_DATA(key);
        if (addData(name,value) == -1) {
          delete _m;
          _m = saveModel;
          // addData handles the error message
          return NULL;
        }
      }
    }
  }
  vector<TypeError> typeErrors;
  try {
    MiniZinc::typecheck(_m, typeErrors);
  } catch (LocationException& e) {
    MZN_PYERR_SET_STRING(PyExc_RuntimeError, "MiniZinc: Model.solve:   %s: %s", e.what(), e.msg().c_str());
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
      warningLog << "MiniZinc: Model.solve: Warning:  " << env->warnings()[i];
    }
    const std::string& tmp = warningLog.str();
    const char* cstr = tmp.c_str();
    PyErr_WarnEx(PyExc_RuntimeWarning, cstr, 1);
  }
  optimize(*env);
  oldflatzinc(*env);
  GCLock lock;
  Options options;
  if (timeLimit != 0)
    options.setIntParam("time", timeLimit);
  delete _m;
  _m = saveModel;
  MznSolver* ret = reinterpret_cast<MznSolver*>(MznSolver_new(&MznSolver_Type, NULL, NULL));
  switch (sc) {
    case SC_GECODE: ret->solver = new GecodeSolverInstance(*env, options); break;
  }
  ret->solver->processFlatZinc();
  ret->env = env;
  return reinterpret_cast<PyObject*>(ret);
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
    PyErr_SetString(PyExc_TypeError, "MiniZinc: Model.Constraint:  Requires an object of Minizinc Variable");
    return NULL;
  }

  GCLock Lock;
  ConstraintI* i;
  if (PyObject_ExactTypeCheck(obj, &MznExpression_Type)) {
    // XXX: Need to support Boolean Variable here
    i = new ConstraintI(Location(), (reinterpret_cast<MznExpression*>(obj)->e));
  } else if (PyBool_Check(obj)) {
    bool val = PyObject_IsTrue(obj);
    i = new ConstraintI(Location(), new BoolLit(Location(), val));
  } else {
    PyErr_SetString(PyExc_TypeError, "MiniZinc: Model.Constraint:  Object must be a MiniZinc Variable or Python Boolean value");
    return NULL;
  }
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
  PyObject* PyExp = NULL;
  PyObject* PyAnn = NULL;
  Expression* e = NULL;
  Expression* ann = NULL;

  if (!PyArg_ParseTuple(args, "I|OO", &solveType, &PyAnn, &PyExp)) {
    PyErr_SetString(PyExc_TypeError, "MiniZinc: Model.SolveItem:  Requires a solver code, an annotation (can be NULL) and an optional expression (for optimisation)");
    return NULL;
  }

  if (solveType > 2) {
    PyErr_SetString(PyExc_ValueError, "MiniZinc: Model.SolveItem:  Invalid solver code (Must be a positive less than 3 integer)");
    return NULL;
  }
  if (solveType) {
    if (PyExp == NULL) {
      PyErr_SetString(PyExc_TypeError, "MiniZinc: Model.SolveItem:  Optimisation solver requires an addition constraint object");
      return NULL;
    } else if (PyObject_ExactTypeCheck(PyExp, &MznExpression_Type))  {
      e = reinterpret_cast<MznExpression*>(PyExp)->e;
    } else {
      PyErr_SetString(PyExc_TypeError, "MiniZinc: Model.SolveItem:  Expression must be a Minizinc Variable Object");
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
  if (PyObject_IsTrue(PyAnn)) {
    if (PyObject_TypeCheck(PyAnn, &MznAnnotation_Type)) {
      ann = reinterpret_cast<MznAnnotation*>(PyAnn)->e;
      i->ann().add(ann);
    } else if (PyList_Check(PyAnn)) {
      long n = PyList_GET_SIZE(PyAnn);
      for (long idx = 0; idx != n; ++idx) {
        PyObject* PyItem = PyList_GET_ITEM(PyAnn, idx);
        if (!PyObject_TypeCheck(PyItem, &MznAnnotation_Type)) {
          // XXX: CONSIDER REVIEW - should I delete i or it will be automatically deleted
          delete i;
          MZN_PYERR_SET_STRING(PyExc_TypeError, "MiniZinc: Model.SolveItem:  Item at position %ld must be a MiniZinc Variable", idx);
          return NULL;
        }
        ann = reinterpret_cast<MznAnnotation*>(PyItem)->e;
        i->ann().add(ann);
      }
    } else if (PyTuple_Check(PyAnn)) {
      long n = PyTuple_GET_SIZE(PyAnn);
      for (long idx = 0; idx != n; ++idx) {
        PyObject* PyItem = PyTuple_GET_ITEM(PyAnn, idx);
        if (!PyObject_TypeCheck(PyItem, &MznAnnotation_Type)) {
          // CONSIDER REVIEW
          delete i;
          MZN_PYERR_SET_STRING(PyExc_TypeError, "MiniZinc: Model.SolveItem:  Item at position %ld must be a MiniZinc Variable", idx);
          return NULL;
        }
        ann = reinterpret_cast<MznAnnotation*>(PyItem)->e;
        i->ann().add(ann);
      }
    } else {
      // CONSIDER REVIEW
      delete i;
      PyErr_SetString(PyExc_TypeError, "MiniZinc: Model.SolveItem:  Annotation must be a single value of or a list/tuple of MiniZinc Variable Object");
      return NULL;
    }
  }
  self->_m->addItem(i);
  Py_RETURN_NONE;
}




static PyObject*
MznModel_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  MznModel* self = reinterpret_cast<MznModel*>(type->tp_alloc(type,0));
  if (self == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "MiniZinc: Unable to create new model");
    return NULL;
  }
  self->includePaths = NULL;
  self->_m = NULL;
  return reinterpret_cast<PyObject*>(self);
}

static int
MznModel_init(MznModel* self, PyObject* args = NULL)
{
  self->loaded = false;
  string std_lib_dir;
  if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
    std_lib_dir = string(MZNSTDLIBDIR);
  } else {
    PyErr_SetString(PyExc_EnvironmentError, "MiniZinc: Model.init:  No MiniZinc library directory MZN_STDLIB_DIR defined.");
    return -1;
  }
  stringstream libNames;
  libNames << "include \"globals.mzn\";";
  if (args != NULL) {
    PyObject* PyLibNames = NULL;
    if (PyUnicode_Check(args)) {
      libNames << "\ninclude \"" << PyUnicode_AS_DATA(args) << "\";";
    } else if (PyTuple_Check(args)) {
      Py_ssize_t n = PyTuple_GET_SIZE(args);
      if (n > 1) {
        PyErr_SetString(PyExc_TypeError, "MiniZinc: Model.init: Accept at most 1 argument");
        return -1;
      } else if (n == 1) {
        PyLibNames = PyTuple_GET_ITEM(args,0);
        if (PyObject_IsTrue(PyLibNames)) {
          if (PyUnicode_Check(PyLibNames)) {
            libNames << "\ninclude \"" << PyUnicode_AS_DATA(PyLibNames) << "\";";
          } else if (PyList_Check(PyLibNames)) {
            Py_ssize_t n = PyList_GET_SIZE(PyLibNames);
            for (Py_ssize_t i = 0; i!=n; ++i) {
              PyObject* temp = PyList_GET_ITEM(PyLibNames, i);
              if (!PyUnicode_Check(temp)) {
                PyErr_SetString(PyExc_TypeError, "MiniZinc: Model.init:  Items in parsing list must be strings");
                return -1;
              }
              libNames << "\ninclude \"" << PyUnicode_AS_DATA(temp) << "\";";
            }
          } else if (PyTuple_Check(PyLibNames)) {
            Py_ssize_t n = PyTuple_GET_SIZE(PyLibNames);
            for (Py_ssize_t i = 0; i!=n; ++i) {
              PyObject* temp = PyTuple_GET_ITEM(PyLibNames, i);
              if (!PyUnicode_Check(temp)) {
                PyErr_SetString(PyExc_TypeError, "MiniZinc: Model.init:  Items in parsing tuples must be strings");
                return -1;
              }
              libNames << "\ninclude \"" << PyUnicode_AS_DATA(temp) << "\";";
            }
          } else {
            PyErr_SetString(PyExc_TypeError, "MiniZinc: Model.init:  Parsing argument must be a string or list/tuple of strings");
            return -1;
          }
        }
      }
    }
  }
  const std::string& libNamesStr = libNames.str();
  self->timeLimit = 0;
  self->includePaths = new vector<string>;
  self->includePaths->push_back(std_lib_dir+"/gecode/");
  self->includePaths->push_back(std_lib_dir+"/std/");
  self->sc = self->SC_GECODE;
  stringstream errorStream;
  self->_m = parseFromString(libNamesStr,"error.txt",*(self->includePaths),false,false,false, errorStream);
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
  if (self->_m)
    delete self->includePaths;
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}



static PyObject* MznModel_addData(MznModel* self, PyObject* args)
{
  PyObject* obj;
  const char* name;
  if (!PyArg_ParseTuple(args, "sO", &name, &obj)) {
    PyErr_SetString(PyExc_RuntimeError, "MiniZinc: Model.addData:  Parsing error");
    return NULL;
  }
  if (self->addData(name,obj)==-1) {
    // addData set error string already
    return NULL;
  }
  Py_RETURN_NONE;
}


static PyObject*
MznModel_copy(MznModel* self)
{
  MznModel* ret = reinterpret_cast<MznModel*>(MznModel_new(&MznModel_Type, NULL, NULL));
  GCLock lock;
  ret->_m = copy(self->_m);
  ret->includePaths = new vector<string>(*(self->includePaths));

  ret->timeLimit = self->timeLimit;
  ret->loaded = self->loaded;
  return reinterpret_cast<PyObject*>(ret);
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
MznModel_solve(MznModel *self, PyObject* args)
{
  return self->solve(args);
}

static PyObject*
MznModel_setTimeLimit(MznModel *self, PyObject *args)
{
  unsigned long long t;
  if (!PyArg_ParseTuple(args, "K", &t)) {
    PyErr_SetString(PyExc_TypeError, "MiniZinc: Model.setTimeLimit:  Time limit must be an integer");
    return NULL;
  }
  self->timeLimit = t;
  return Py_None;
}

static PyObject*
MznModel_setSolver(MznModel *self, PyObject *args)
{
  const char* s;
  if (!PyArg_ParseTuple(args, "s", &s)) {
    PyErr_SetString(PyExc_TypeError, "MiniZinc: Model.setSolver:  Solver name must be a string");
    return NULL;
  }
  std::string name(s);
  // lower characters in name
  for (std::string::iterator i = name.begin(); i!=name.end(); ++i)
    if (*i<='Z' && *i>='A')
      *i = *i - ('Z'-'z');
  if (name == "gecode")
    self->sc = self->SC_GECODE;
  else {
    MZN_PYERR_SET_STRING(PyExc_ValueError, "MiniZinc: Model.setSolver: Unexpected solver name: %s", name.c_str());
    return NULL;
  }
  return Py_None;
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
MznModel_Declaration(MznModel* self, PyObject* args)
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
  long tid;
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
  Type::BaseType code = Type::BT_UNKNOWN;

  if (!PyArg_ParseTuple(args, "sO|OOO", &name, &pyval, &pydim, &pylb, &pyub)) {
    MZN_PYERR_SET_STRING(PyExc_TypeError, "MiniZinc: MznModel.Declaration:  Variable parsing error");
    return NULL;
  }
  // if only 2 arguments, second value is the initial value
  if (pydim == NULL) {
    dimList = new vector<pair<int, int> >();
    initValue = python_to_minizinc(pyval, type, *dimList);
    dim = dimList->size();
    domain = NULL;
  } 
  else 
  // else if > 2 arguments, create a MiniZinc Variable
  {
#if PY_MAJOR_VERSION < 3
    if (PyInt_Check(pyval)) {
      tid = PyInt_AS_LONG(pyval);
      pyval = NULL;
    } else
#endif
    if (PyLong_Check(pyval)) {
      tid = PyLong_AsLong(pyval);
      pyval = NULL;
    } else {
      PyErr_SetString(PyExc_TypeError, "MiniZinc: MznModel.Declaration:  Type Id must be an integer");
      return NULL;
    }
    if (tid>17 || tid<0) {
      PyErr_SetString(PyExc_ValueError, "MiniZinc: MznModel.Declaration:  Type Id is from 0 to 17");
      return NULL;
    }
    dimList = pydim_to_dimList(pydim);
    if (dimList == NULL)
      return NULL;
    dim = dimList->size();

    // Create an array if dimList is not empty
    for (Py_ssize_t i=0; i!=dim; ++i) {
      Expression* e0 = new IntLit(Location(), IntVal((*dimList)[i].first));
      Expression* e1 = new IntLit(Location(), IntVal((*dimList)[i].second));
      domain = new BinOp(Location(), e0, BOT_DOTDOT, e1);
      ranges.push_back(new TypeInst(Location(), Type(), domain));
    }

    // Process different types
    switch (static_cast<TypeId>(tid)) {
      case VARINT:  
          type = Type::varint(dim);
          code = Type::BT_INT;
          if (pyub == NULL) {
            Type tempType;
            vector<pair<int, int> > tempDimList;
            domain = python_to_minizinc(pylb, tempType, tempDimList);
            if (tempType.st() != Type::ST_SET) {
              PyErr_SetString(PyExc_TypeError, "MiniZinc: MznModel.Declaration:  If 5th argument does not exist, 4th argument must be a Minizinc Set");
              return NULL;
            }
          } else 
            domain = new BinOp(Location(),
                            one_dim_python_to_minizinc(pylb,code),
                            BOT_DOTDOT,
                            one_dim_python_to_minizinc(pyub,code) );
          break;
      case VARBOOL:
          type = Type::varbool(dim); 
          break;
      case VARFLOAT:
          type = Type::varfloat(dim);
          code = Type::BT_FLOAT;
          domain = new BinOp(Location(),
                          one_dim_python_to_minizinc(pylb,code),
                          BOT_DOTDOT,
                          one_dim_python_to_minizinc(pyub,code) );
          break;
      case VARSETINT:
          type = Type::varsetint(dim);
          if (pyub == NULL) {
            Type tempType;
            vector<pair<int, int> > tempDimList;
            domain = python_to_minizinc(pylb, tempType, tempDimList);
            if (tempType.st() != Type::ST_SET) {
              PyErr_SetString(PyExc_TypeError, "MiniZinc: MznModel.Declaration:  If 5th argument does not exist, 4th argument must be a Minizinc Set");
              return NULL;
            }
          } else 
            domain = new BinOp(Location(),
                            one_dim_python_to_minizinc(pylb,code),
                            BOT_DOTDOT,
                            one_dim_python_to_minizinc(pyub,code) );
          break;
      default:
          MZN_PYERR_SET_STRING(PyExc_ValueError, "MiniZinc: MznModel.Declaration:  Value code %li not supported", tid);
          return NULL;
    }
  }

  VarDecl* vd = new VarDecl(Location(), new TypeInst(Location(), type, ranges, domain) , string(name), initValue);
  self->_m->addItem(new VarDeclI(Location(), vd));
  self->loaded = true;

  PyObject* ret;

  if (static_cast<TypeId>(tid) == VARSETINT) {
    ret = MznVarSet_new(&MznVarSet_Type, NULL, NULL);
    reinterpret_cast<MznVarSet*>(ret)-> e = vd->id();
    reinterpret_cast<MznVarSet*>(ret)->vd = vd;
  } else {
    if (dimList->size() == 0) {
      ret = MznVariable_new(&MznVariable_Type, NULL, NULL);
      reinterpret_cast<MznVariable*>(ret)->e = vd->id();
      reinterpret_cast<MznVariable*>(ret)->vd= vd;
    } else {
      ret = MznArray_new(&MznArray_Type, NULL, NULL);
      reinterpret_cast<MznArray*>(ret)->e = vd->id();
      reinterpret_cast<MznArray*>(ret)->vd= vd;
      reinterpret_cast<MznArray*>(ret)->dimList = dimList;
    }
  }
  return ret;
}