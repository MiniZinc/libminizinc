#include <Python.h>
#include "structmember.h"

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <list>
#include <string.h>
#include <typeinfo>
#include <cstdlib>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>

#include <minizinc/parser.hh>
#include <minizinc/model.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/flatten.hh>
#include <minizinc/optimize.hh>
#include <minizinc/builtins.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/solvers/gecode/gecode_solverinstance.hh>

#include "pyinterface.h";

using namespace MiniZinc;
using namespace std;


void sig_alrm (int signo)
{
  siglongjmp(jmpbuf, 1);
}

//



void MznModelDestructor(PyObject* o) {
  const char* name = PyCapsule_GetName(o);
  delete[] name;
  Py_DECREF(o);
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
    PyErr_SetString(MznModel_load_error, "No MiniZinc library directory MZN_STDLIB_DIR defined.");
    return -1;
  }
  self->timeLimit = 0;
  self->includePaths->push_back(std_lib_dir+"/gecode/");
  self->includePaths->push_back(std_lib_dir+"/std/");
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
MznModel::load(PyObject *args, PyObject *keywds, bool fromFile)
{
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
      PyErr_SetString(MznModel_load_error, "Parsing error");
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
              PyErr_SetString(MznModel_solve_error, "Time value must be a valid positive number");
              return NULL;
            }
            timeLimit = t;
            t_flag = false;
          } else {
            PyErr_SetString(MznModel_solve_error, "Unknown option");
            return NULL;
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
            PyErr_SetString(MznModel_load_error, "Element in the list must be a filename");
            return -1;
          }
          data.push_back(string(name));
        }
      } else if (!PyDict_Check(obj)) {
        PyErr_SetString(MznModel_load_error, "The second argument must be either a filename, a list of filenames or a dictionary of data");
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
    if (isDict) {
      stringstream assignments;
      Py_ssize_t pos = 0;
      PyObject* key;
      PyObject* value;
      GCLock lock;
      while (PyDict_Next(obj, &pos, &key, &value)) {
        char* name = PyString_AS_STRING(key);
        for (unsigned int i=0; i<_m->size(); i++) 
          if (VarDeclI* vdi = (*_m)[i]->dyn_cast<VarDeclI>()) {
            if (strcmp(vdi->e()->id()->str().c_str(), name) == 0) {
              if (vdi->e()->type().st() == Type::ST_SET) {
                if (PyObject_TypeCheck(value, &MznSetType)) {
                  vector<IntSetVal::Range> ranges;
                  MznSet* Set = (MznSet*)value;
                  for (list<MznSet::Range>::const_iterator it = Set->ranges->begin(); it != Set->ranges->end(); ++it) {
                    ranges.push_back(IntSetVal::Range(IntVal(it->min),IntVal(it->max)));
                  }
                  Expression* rhs = new SetLit(Location(), IntSetVal::a(ranges));
                  vdi->e()->e(rhs);
                } else {
                  string errorLog = "type-inst error: output expression " + string(name) + " has invalid type-inst: expected a set";
                  PyErr_SetString(MznModel_solve_error, errorLog.c_str());
                  return -1;
                }
              } else if (vdi->e()->type().bt() == Type::BT_INT) {
                if (vdi->e()->type().dim() == 0) {
                  if (PyInt_Check(value)) {
                    Expression* rhs = new IntLit(Location(), IntVal(PyInt_AS_LONG(value)));
                    vdi->e()->e(rhs);
                  } else {
                    string errorLog = "type-inst error: output expression " + string(name) + " has invalid type-inst: expected an integer";
                    PyErr_SetString(MznModel_solve_error, errorLog.c_str());
                    return -1;
                  }
                } else {
                  if (PyList_Check(value)) {
                    ArrayLit* al = vdi->e()->cast<ArrayLit>();
                    vector<Py_ssize_t> dimensions;
                    vector<PyObject*> simpleArray;
                    if (getList(value, dimensions, simpleArray, 0) == -1) {
                      PyErr_SetString(MznModel_load_error, "Inconsistency in size of multidimensional array");
                      return -1;
                    }
                    if (vdi->e()->type().dim() != dimensions.size()) {
                      string errorLog = "type-inst error: output expression " + string(name) + " has invalid type-inst: number of dimension not conforms";
                      PyErr_SetString(MznModel_load_error, errorLog.c_str());
                      return -1;
                    }
                    /*
                    for (int i=0; i!=dimensions.size(); i++) {
                      cout << i << endl;
                      if (dimensions[i] != al->max(i) - al->min(i) + 1) {
                        string errorLog = "type-inst error: output expression " + string(name) + " has invalid type-inst: size of each dimension not conforms";
                        PyErr_SetString(MznModel_load_error, errorLog.c_str());
                        return -1;
                      }
                    }
                    */
                    vector<Expression*> v;
                    vector<pair<int,int> > dims;
                    for (int i=0; i!=simpleArray.size(); ++i) {
                      PyObject* temp= simpleArray[i];
                      if (!PyInt_Check(temp)) {
                        string errorLog = "Expected integer value";
                        PyErr_SetString(MznModel_load_error, errorLog.c_str());
                        return -1;
                      }
                      Expression* rhs = new IntLit(Location(), IntVal(PyInt_AS_LONG(temp)));
                      v.push_back(rhs);
                    }

                    for (int i=0; i!=dimensions.size(); i++)
                      dims.push_back(pair<int,int>(1,dimensions[i]));
                    Expression* rhs = new ArrayLit(Location(), v, dims);
                    vdi->e()->e(rhs);
                  } else {
                    string errorLog = "type-inst error: output expression " + string(name) + " has invalid type-inst: expected an array of integer";
                    PyErr_SetString(MznModel_solve_error, errorLog.c_str());
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
                    PyErr_SetString(MznModel_load_error, "Inconsistency in size of multidimensional array");
                    return -1;
                  }
                  if (vdi->e()->type().dim() != dimensions.size()) {
                    string errorLog = "type-inst error: output expression " + string(name) + " has invalid type-inst: number of dimension not conforms";
                    PyErr_SetString(MznModel_load_error, errorLog.c_str());
                    return -1;
                  }
                  vector<Expression*> v;
                  vector<pair<int,int> > dims;
                  for (int i=0; i!=simpleArray.size(); ++i) {
                    PyObject* temp= simpleArray[i];
                    if (!PyInt_Check(temp)) {
                      string errorLog = "Expected boolean value";
                      PyErr_SetString(MznModel_load_error, errorLog.c_str());
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
                PyErr_SetString(MznModel_solve_error, errorLog.c_str());
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
                    PyErr_SetString(MznModel_load_error, "Inconsistency in size of multidimensional array");
                    return -1;
                  }
                  if (vdi->e()->type().dim() != dimensions.size()) {
                    string errorLog = "type-inst error: output expression " + string(name) + " has invalid type-inst: number of dimension not conforms";
                    PyErr_SetString(MznModel_load_error, errorLog.c_str());
                    return -1;
                  }
                  vector<Expression*> v;
                  vector<pair<int,int> > dims;
                  for (int i=0; i!=simpleArray.size(); ++i) {
                    PyObject* temp= simpleArray[i];
                    if (!PyString_Check(temp)) {
                      string errorLog = "Expected string value";
                      PyErr_SetString(MznModel_load_error, errorLog.c_str());
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
                  PyErr_SetString(MznModel_solve_error, errorLog.c_str());
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
                    PyErr_SetString(MznModel_load_error, "Inconsistency in size of multidimensional array");
                    return -1;
                  }
                  if (vdi->e()->type().dim() != dimensions.size()) {
                    string errorLog = "type-inst error: output expression '" + string(name) + "'' has invalid type-inst: number of dimension not conforms";
                    PyErr_SetString(MznModel_load_error, errorLog.c_str());
                    return -1;
                  }
                  vector<Expression*> v;
                  vector<pair<int,int> > dims;
                  for (int i=0; i!=simpleArray.size(); ++i) {
                    PyObject* temp= simpleArray[i];
                    if (!PyFloat_Check(temp)) {
                      string errorLog = "Expected float value";
                      PyErr_SetString(MznModel_load_error, errorLog.c_str());
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
                  PyErr_SetString(MznModel_solve_error, errorLog.c_str());
                  return -1;
                }
              } else {
                PyErr_SetString(MznModel_load_error, "Unhandled type");
                return -1;
              }
            }
          }
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
  PyErr_SetString(MznModel_load_error, "Unknown Error");
  return -1;
}

static PyObject*
MznModel_load(MznModel *self, PyObject *args, PyObject *keywds) {
  if (self->load(args, keywds, true) < 0)
    return NULL;
  Py_RETURN_NONE;
}

static PyObject*
MznModel_loadString(MznModel *self, PyObject *args, PyObject *keywds) {
  if (self->load(args, keywds, false) < 0)
    return NULL;
  Py_RETURN_NONE;
}

/*static PyObject*
MznModel_loadData(MznModel* self, PyObject *args) {
  if (self->loadData(args) < 0)
    return NULL;
  Py_RETURN_NONE;
}*/

PyObject* MznModel::solve()
{
  if (!loaded) {
    PyErr_SetString(MznModel_solve_error, "No data has been loaded into the model");
    return NULL;
  }
  loaded = false;
  vector<TypeError> typeErrors;
  try {
    MiniZinc::typecheck(_m, typeErrors);
  } catch (LocationException& e) {
    stringstream errorLog;
    errorLog << e.what() << ": " << std::endl;
    errorLog << "  " << e.msg() << std::endl;
    const std::string& tmp = errorLog.str();
    const char* cstr = tmp.c_str();
    PyErr_SetString(MznModel_solve_error, cstr);
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
    PyErr_SetString(MznModel_solve_error, cstr);
    return NULL;
  }
  MiniZinc::registerBuiltins(_m);
  Env env(_m);
  try {
    FlatteningOptions fopts;
    flatten(env,fopts);
  } catch (LocationException& e) {
    stringstream errorLog;
    errorLog << e.what() << ": " << std::endl;
    env.dumpErrorStack(errorLog);
    errorLog << "  " << e.msg() << std::endl;
    const std::string& tmp = errorLog.str();
    const char* cstr = tmp.c_str();
    PyErr_SetString(MznModel_solve_error, cstr);
    return NULL;
  }
  if (env.warnings().size()!=0)
  {
    stringstream warningLog;
    for (unsigned int i=0; i<env.warnings().size(); i++) {
      warningLog << "Warning: " << env.warnings()[i];
    }
    const std::string& tmp = warningLog.str();
    const char* cstr = tmp.c_str();
    PyErr_WarnEx(MznModel_solve_warning, cstr, 1);
  }
  optimize(env);
  oldflatzinc(env);
  GCLock lock;
  Options options;
  GecodeSolverInstance gecode(env,options);
  gecode.processFlatZinc();
  //cout << "1" << endl;
  SolverInstance::Status status = gecode.solve();
  //cout << "2" << endl;
  if (status==SolverInstance::SAT || status==SolverInstance::OPT) {
    PyObject* solutions = PyList_New(0);
    PyObject* sol = PyDict_New();
    GCLock lock;
    Model* _m = env.output();
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
          /*IntSetVal* isv = eval_intset(vdi->e()->e());
          long long int numberOfElement = 0;
          for (IntSetRanges isr(isv); isr(); ++isr) {
            numberOfElement += (isr.max() - isr.min()).toInt() + 1;
          }

          PyObject* p = PyList_New(numberOfElement);
          Py_ssize_t count = 0;
          for (IntSetRanges isr(isv); isr(); ++isr) {
            for (IntVal j=isr.min(); j<=isr.max(); j++)
              PyList_SetItem(p,count++,PyInt_FromLong(j.toInt()));
          }
          PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(), p);
          Py_DECREF(p);*/
        } else {
          if (vdi->e()->type().bt() == Type::BT_BOOL) {
            if (vdi->e()->type().dim() == 0) {
              //IntVal iv = eval_bool(vdi->e()->e());
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
  } else {
    PyErr_SetString(MznModel_solve_error,"Unknown status code");
    return NULL;
  }  
}

static PyObject*
MznModel_solve(MznModel *self)
{
  if (self->timeLimit != 0) {
    signal(SIGALRM, sig_alrm);
    alarm(self->timeLimit);
    if (sigsetjmp(jmpbuf, 1)) {
      PyErr_SetString(MznModel_solve_error, "Time out");
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


PyMODINIT_FUNC
initminizinc(void) {
    GC::init();
    PyObject* model = Py_InitModule3("minizinc", Mzn_methods, "A python interface for minizinc constraint modeling");

    if (model == NULL)
      return;

    //Error Handling
    MznModel_load_error = PyErr_NewException("MznModel_load.error", NULL, NULL);
    if (MznModel_load_error == NULL)
      return;
    Py_INCREF(MznModel_load_error);
    PyModule_AddObject(model,"error",MznModel_load_error);

    /*MznModel_loadData_error = PyErr_NewException("MznModel_loadData.error", NULL, NULL);
    if (MznModel_loadData_error == NULL)
      return;
    Py_INCREF(MznModel_loadData_error);
    PyModule_AddObject(model, "error", MznModel_loadData_error);*/

    MznModel_loadString_error = PyErr_NewException("MznModel_loadString.error", NULL, NULL);
    if (MznModel_loadString_error == NULL)
      return;
    Py_INCREF(MznModel_loadString_error);
    PyModule_AddObject(model,"error",MznModel_loadString_error);


    MznModel_solve_error = PyErr_NewException("MznModel_solve.error", NULL, NULL);
    if (MznModel_solve_error == NULL)
      return;
    Py_INCREF(MznModel_solve_error);
    PyModule_AddObject(model,"error",MznModel_solve_error);

    MznModel_solve_warning = PyErr_NewException("MznModel_solve.warning", NULL, NULL);
    if (MznModel_solve_error == NULL)
      return;
    Py_INCREF(MznModel_solve_warning);
    PyModule_AddObject(model,"warning",MznModel_solve_warning);

    MznSet_error = PyErr_NewException("MznSet.error", NULL, NULL);
    if (MznSet_error == NULL)
      return;
    Py_INCREF(MznSet_error);
    PyModule_AddObject(model,"error",MznSet_error);


    // Minizinc Set Initialization
    if (PyType_Ready(&MznSetType) < 0)
      return;
    Py_INCREF(&MznSetType);
    PyModule_AddObject(model, "Set", (PyObject *)&MznSetType);

    if (PyType_Ready(&MznModelType) < 0)
      return;
    Py_INCREF(&MznModelType);
    PyModule_AddObject(model, "Model", (PyObject *)&MznModelType);

}










// Implementation
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