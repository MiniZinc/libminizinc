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

using namespace MiniZinc;
using namespace std;

// Error Objects
static PyObject* MznModel_solve_error;
static PyObject* MznModel_load_error;
static PyObject* MznModel_loadString_error;
static PyObject* MznModel_solve_warning;
static PyObject* MznSet_error;


// Time alarm
sigjmp_buf jmpbuf;

void sig_alrm (int signo)
{
  siglongjmp(jmpbuf, 1);
}

//

#include "MznSet.h"

void MznModelDestructor(PyObject* o) {
  const char* name = PyCapsule_GetName(o);
  delete[] name;
  Py_DECREF(o);
}

string minizinc_set(long start, long end);

int getList(PyObject* value, vector<Py_ssize_t>& dimensions, vector<PyObject*>& simpleArray, const int layer);

struct MznModel {
  PyObject_HEAD
  Model* m;
  vector<string>* includePaths;
  bool loaded;

  //vector<string>* data;

  int load(PyObject *args, PyObject *keywds, bool fromFile);
  PyObject* solve();
};

static PyObject*
MznModel_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  MznModel* self = (MznModel*)type->tp_alloc(type,0);
  self->m = new Model;
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
  self->includePaths->push_back(std_lib_dir+"/gecode/");
  self->includePaths->push_back(std_lib_dir+"/std/");
  return 0;
}

static void
MznModel_dealloc(MznModel* self)
{
  delete self->m;
  delete self->includePaths;
  //delete self->data;
  self->ob_type->tp_free((PyObject*)self);
}


int 
MznModel::load(PyObject *args, PyObject *keywds, bool fromFile)
{
  PyObject* obj = Py_None;
  Py_ssize_t pos = 0;
  PyObject* key;
  PyObject* value;
  PyObject* options = Py_None;
  const char* py_filename;

  static char *kwlist[] = {"name","data","options"};
  if (fromFile) {
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s|OO", kwlist, &py_filename, &obj, &options)) {
      PyErr_SetString(MznModel_load_error, "Parsing error");
      return -1;
    }
  } else {

  }
  stringstream errorStream;
  vector<string> data;
  if (m = parse(string(py_filename), data, *includePaths, false, false, false, errorStream))
  {
    loaded = true;
    if (obj != Py_None) {
      // Records the list of Set for special parsing
      list<string> nameOfSets;
      for (unsigned int i=0; i<m->size(); i++) {
        if (VarDeclI* vdi = (*m)[i]->dyn_cast<VarDeclI>())
          if (vdi->e()->type().is_set() && vdi->e()->type().ispar()) {
            nameOfSets.push_back(vdi->e()->id()->str().str());
          }
      }
      stringstream assignments;
      Py_ssize_t pos = 0;
      PyObject* key;
      PyObject* value;
      while (PyDict_Next(obj, &pos, &key, &value)) {
        assignments << PyString_AsString(key) << " = ";
        if (PyList_Check(value)) {
          vector<Py_ssize_t> dimensions;
          vector<PyObject*> simpleArray;
          if (PyList_Size(value)==0) {
            PyErr_SetString(MznModel_load_error,"Objects must contain at least 1 value");
            return -1;
          }
          bool is_set = false; // find out if it is a set or an array.
          std::list<string>::iterator findIter = std::find(nameOfSets.begin(),nameOfSets.end(),PyString_AsString(key));
          if (findIter!=nameOfSets.end()) {
            is_set = true;
            nameOfSets.erase(findIter);
          }
          if (getList(value, dimensions, simpleArray,0) == -1) {
            PyErr_SetString(MznModel_load_error, "Inconsistency in size of multidimensional array");
            return -1;
          }

          if (dimensions.size()>6) {
            PyErr_SetString(MznModel_load_error, "Maximum dimension of a multidimensional array is 6");
            return -1;
          }
          if (is_set)
            assignments << "{";
          else {
            assignments << "array" << dimensions.size() << "d(";
            for (vector<Py_ssize_t>::size_type i=0; i!=dimensions.size(); i++) {
              assignments << minizinc_set(1,dimensions[i]) << ", ";
            }
            assignments << '[';
          }
          if (PyBool_Check(simpleArray[0]))
            for (vector<PyObject*>::size_type i=0; i!=simpleArray.size(); i++) {
              if (i!=0)
                 assignments << ", ";
              if (PyBool_Check(simpleArray[i])) {
                if (PyInt_AS_LONG(simpleArray[i]))
                  assignments << "true";
                else assignments << "false";
              } else {
                PyErr_SetString(MznModel_load_error, "Inconsistency in values type");
                return -1;
              }
            }
          else if (PyInt_Check(simpleArray[0]))
            for (vector<PyObject*>::size_type i=0; i!=simpleArray.size(); i++) {
              if (i!=0)
                assignments << ", ";
              if (PyInt_Check(simpleArray[i]))
                assignments << PyInt_AS_LONG(simpleArray[i]);
              else {
                PyErr_SetString(MznModel_load_error, "Inconsistency in values type");
                return -1;
              }
            }
          else if (PyFloat_Check(simpleArray[0]))
            for (vector<PyObject*>::size_type i=0; i!=simpleArray.size(); i++) {
              if (i!=0)
                assignments << ", ";
              if (PyFloat_Check(simpleArray[i]))
                assignments << PyFloat_AS_DOUBLE(simpleArray[i]);
              else {
                PyErr_SetString(MznModel_load_error, "Inconsistency in values type");
                return -1;
              }
            }
          else if (PyString_Check(simpleArray[0]))
            for (vector<PyObject*>::size_type i=0; i!=simpleArray.size(); i++) {
              if (i!=0)
                assignments << ", ";
              if (PyString_Check(simpleArray[i]))
                assignments << "\"" << PyString_AS_STRING(simpleArray[i]) << "\"";
              else {
                PyErr_SetString(MznModel_load_error, "Inconsistency in values type");
                return -1;
              }
            }
          else {
            PyErr_SetString(MznModel_load_error, "Object must be an integer, float, boolean or string");
            return -1;
          }
          if (is_set)
            assignments << "}";
          else assignments << "])";
        } else {
          // no error checking currently
          if (PyBool_Check(value)) {
            if (PyInt_AS_LONG(value))
              assignments << "true";
            else assignments << "false";
          } else if (PyInt_Check(value))
            assignments << PyInt_AS_LONG(value);
          else if (PyFloat_Check(value)) {
            assignments << PyFloat_AS_DOUBLE(value);
          }
          else if (PyString_Check(value))
            assignments << PyString_AS_STRING(value);
          else {
            PyErr_SetString(MznModel_load_error, "Object must be a list or value");
            return -1;
          }
        }
        assignments << ";\n";
      }
      string asn = assignments.str();
      cout << asn << endl;
      if (asn.size() > 0)
        data.push_back("cmd:/"+assignments.str());
      stringstream errorStream;
      if (!parseData(m, data, *includePaths, true,
                         false, false, errorStream))
      {
        const std::string& tmp = errorStream.str();
        const char* cstr = tmp.c_str();
        PyErr_SetString(MznModel_load_error, cstr);
        return -1;
      }
    }
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

PyObject* MznModel::solve()
{
  if (!loaded) {
    PyErr_SetString(MznModel_solve_error, "No data has been loaded into the model");
    return NULL;
  }
  loaded = false;
  vector<TypeError> typeErrors;
  try {
    MiniZinc::typecheck(m, typeErrors);
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
  MiniZinc::registerBuiltins(m);
  Env env(m);
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
  SolverInstance::Status status = gecode.solve();
  if (status==SolverInstance::SAT || status==SolverInstance::OPT) {
    PyObject* solutions = PyList_New(0);
    PyObject* sol = PyDict_New();
    GCLock lock;
    Model* _m = env.output();
    for (unsigned int i=0; i<_m->size(); i++) {
      if (VarDeclI* vdi = (*_m)[i]->dyn_cast<VarDeclI>()) {
        if (vdi->e()->type().st() == Type::ST_SET) {
          IntSetVal* isv = eval_intset(vdi->e()->e());
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
          Py_DECREF(p);
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
  return self->solve();
}

static PyObject*
MznModel_solvewArgs(MznModel *self, PyObject *args)
{
  char* options;
  long t = 0;

  string s = "";
  if (!PyArg_ParseTuple(args, "s", &options)) {
    PyErr_SetString(MznModel_solve_error, "Parsing error");
    return NULL;
  } else {
    char* pch;
    bool t_flag = false;
    pch = strtok(options, " ");
    while (pch != NULL) {
      if (strcmp(pch,"-t")==0)
        t_flag = true;
      else {
        if (t_flag) {
          char* ptr;
          t = strtol(pch,&ptr,10);
          if (t == 0) {
            PyErr_SetString(MznModel_solve_error, "Time value must be a valid positive number");
            return NULL;
          }
          t_flag = false;
        } else {
          PyErr_SetString(MznModel_solve_error, "Unknown option");
          return NULL;
        }
      }
      pch = strtok(NULL, " ");
    }
  }
  if (t != 0) {
    signal(SIGALRM, sig_alrm);
    alarm(t);
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

static PyMemberDef MznModel_members[] = {
  {NULL} /* Sentinel */
};

static PyMethodDef MznModel_methods[] = {
  {"load", (PyCFunction)MznModel_load, METH_VARARGS, "Load MiniZinc model from MiniZinc file"},
  {"loadFromString", (PyCFunction)MznModel_loadString, METH_VARARGS, "Load MiniZinc model from standard input"},
  {"solve", (PyCFunction)MznModel_solve, METH_NOARGS, "Solve a loaded MiniZinc model"},
  {"solveO", (PyCFunction)MznModel_solvewArgs, METH_VARARGS, "Solve a loaded MiniZinc model with options"},
  {NULL} /* Sentinel */
};

static PyTypeObject MznModelType = {
  PyVarObject_HEAD_INIT(NULL,0)
  "minizinc.model",       /* tp_name */
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
  "Minizinc Model",  /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  0,                         /* tp_iter */
  0,                         /* tp_iternext */
  MznModel_methods,            /* tp_methods */
  MznModel_members,            /* tp_members */
  0,/*MznModel_getseters,        /* tp_getset */
  0,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  (initproc)MznModel_init,     /* tp_init */
  0,                         /* tp_alloc */
  MznModel_new,                /* tp_new */
};


PyMODINIT_FUNC
initminizinc(void) {
    GC::init();
    PyObject* model = Py_InitModule3("minizinc", NULL, "A python interface for minizinc constraint modeling");

    if (model == NULL)
      return;

    //Error Handling
    MznModel_load_error = PyErr_NewException("MznModel_load.error", NULL, NULL);
    if (MznModel_load_error == NULL)
      return;
    Py_INCREF(MznModel_load_error);
    PyModule_AddObject(model,"error",MznModel_load_error);

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