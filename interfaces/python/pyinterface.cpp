#include <Python.h>
#include <iostream>
#include <cstdio>

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
static PyObject* mzn_solve_error;
static PyObject* mzn_solve_warning;

string minizinc_set(int start, int end) {
  stringstream ret;
  ret << start << ".." << end;
  return ret.str();
}

int getList(PyObject* value, vector<Py_ssize_t>& dimensions, vector<PyObject*>& simpleArray, const int layer) {
  for (Py_ssize_t i=0; i<PyList_Size(value); i++) {
    if (dimensions.size() <= layer) {
      dimensions.push_back(PyList_Size(value));
    } else if (dimensions[layer]!=PyList_Size(value))
      return -1; // Inconsistent size of array (should be the same)
    PyObject* li = PyList_GetItem(value, i);
    //Py_INCREF(li);
    if (PyList_Check(li)) {
        if (getList(li,dimensions,simpleArray,layer+1)==-1) {
        //Py_DECREF(li);
        return -1;
      }
      //Py_DECREF(li);
    } else {
      simpleArray.push_back(li);
    }
  }
  return 0;
}

string processArguments(PyObject* value, stringstream& assignments) {
  if (PyList_Check(value)) { 
    if (PyList_Size(value)==0)
      return "Objects must contain at least 1 value";
    vector<Py_ssize_t> dimensions;
    vector<PyObject*> simpleArray;
    if (getList(value, dimensions, simpleArray,0) == -1) {
      //for (int i=0; i!=simpleArray.size(); i++)
      //  Py_DECREF(simpleArray[i]);
      return "Inconsistency in size of multidimensional array";
    }
    if (dimensions.size()>6)
      return "Maximum dimension of a multidimensional array is 6";
    assignments << "array" << dimensions.size() << "d(";
    for (vector<Py_ssize_t>::size_type i=0; i!=dimensions.size(); i++) {
      assignments << minizinc_set(1,dimensions[i]) << ", ";
    }
    assignments << '[';
    if (PyBool_Check(simpleArray[0]))
      for (vector<PyObject*>::size_type i=0; i!=simpleArray.size(); i++) {
        if (i!=0)
          assignments << ", ";
        if (PyBool_Check(simpleArray[i])) {
          if (PyInt_AS_LONG(simpleArray[i]))
            assignments << "true";
          else assignments << "false";
        } else return "Inconsistency in values type";
      }
    else if (PyInt_Check(simpleArray[0]))
      for (vector<PyObject*>::size_type i=0; i!=simpleArray.size(); i++) {
        if (i!=0)
          assignments << ", ";
        if (PyInt_Check(simpleArray[i]))
          assignments << PyInt_AS_LONG(simpleArray[i]);
        else return "Inconsistency in values type";
      }
    else if (PyFloat_Check(simpleArray[0]))
      for (vector<PyObject*>::size_type i=0; i!=simpleArray.size(); i++) {
        if (i!=0)
          assignments << ", ";
        if (PyFloat_Check(simpleArray[i]))
          assignments << PyFloat_AS_DOUBLE(simpleArray[i]);
        else return "Inconsistency in values type";
      }
    else if (PyString_Check(simpleArray[0]))
      for (vector<PyObject*>::size_type i=0; i!=simpleArray.size(); i++) {
        if (i!=0)
          assignments << ", ";
        if (PyString_Check(simpleArray[i]))
          assignments << "\"" << PyString_AS_STRING(simpleArray[i]) << "\"";
        else return "Inconsistency in values type";
      }
    else return "Object must be an integer, float, boolean or string";
    assignments << "])";
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
      return "Object is neither a list or value";
    }
  }
  assignments << ";\n";
  return "";
}
/*
void processIntList(VarDecl* vdi, PyObject* sol) {
  if (dim()==0) {
    IntVal iv = eval_int(vdi->e()->e());
    PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(), PyInt_FromLong(iv.toInt()));
  } else if (dim()==1) {
    ArrayLit* al = eval_par(vdi->e()->e())->cast<ArrayLit>();
    PyObject* p = PyList_New(al->v.size());
    for (unsigned int i=al.v().size(); i--;)
      PyList_SetItem(pa, i, PyInt_FromLong(eval_int(al->v()[i]).toInt()));
    PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(), pa);
  } else {
    ArrayLit* al = eval_par(vdi->e()->e())->cast<ArrayLit>();
    
  }
}*/

/*static PyObject*
mzn_set(PyObject *self, PyObject *args)
{
  PyObject* obj;
  const char* args1 = PyString_AsString(args[0]);
  const char* args2 = PyString_AsString(args[1]);
  stringstream assignments;
  assignments << args1 << ".." << args2;
  
}*/

static PyObject*
mzn_solve(PyObject *self, PyObject *args)
{
  PyObject* obj;
  Py_ssize_t pos = 0;
  PyObject* key;
  PyObject* value;
  const char* py_filename;

  if (!PyArg_ParseTuple(args, "sO", &py_filename, &obj)) {
    //std::cerr << "usage: minizinc.solve('filename', {dictionary})\n";
    PyErr_SetString(mzn_solve_error, "Parsing error");
    return NULL;
  }

  //Py_INCREF(obj);

  stringstream assignments;
  while (PyDict_Next(obj, &pos, &key, &value)) {
    //cout << PyString_AsString(key) << " - " << PyInt_AsLong(value) << endl;
    assignments << PyString_AsString(key) << " = ";
    string errorLog = processArguments(value,assignments);
    if (errorLog!="") {
      PyErr_SetString(mzn_solve_error, errorLog.c_str());
      return NULL;
    }
  }
  vector<string> data;
  string asn = assignments.str();
  cout << asn << endl;
  if (asn.size() > 0)
    data.push_back("cmd:/"+assignments.str());

  string std_lib_dir;
  if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
    std_lib_dir = string(MZNSTDLIBDIR);
  } else {
    PyErr_SetString(mzn_solve_error, "No MiniZinc library directory MZN_STDLIB_DIR defined.");
    return NULL;
  }

  vector<string> includePaths;
  includePaths.push_back(std_lib_dir+"/gecode/");
  includePaths.push_back(std_lib_dir+"/std/");

  GC::init();
  stringstream errorStream;
  if (Model* m = parse(string(py_filename), data, includePaths, false,
                       false, false, errorStream)) {
    vector<TypeError> typeErrors;
    try {
      MiniZinc::typecheck(m, typeErrors);
    } catch (LocationException& e) {
      stringstream errorLog;
      errorLog << e.what() << ": " << std::endl;
      errorLog << "  " << e.msg() << std::endl;
      const std::string& tmp = errorLog.str();
      const char* cstr = tmp.c_str();
      PyErr_SetString(mzn_solve_error, cstr);
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
      PyErr_SetString(mzn_solve_error, cstr);
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
      PyErr_SetString(mzn_solve_error, cstr);
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
      PyErr_WarnEx(mzn_solve_warning, cstr, 1);
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
      //GCLock lock;
      Model* _m = env.output();
      for (unsigned int i=0; i<_m->size(); i++) {
        if (VarDeclI* vdi = (*_m)[i]->dyn_cast<VarDeclI>()) {
          if (vdi->e()->type().bt() == Type::BT_BOOL) {
            if (vdi->e()->type().dim() == 0) {
              IntVal iv = eval_int(vdi->e()->e());
              PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(), PyBool_FromLong(iv.toInt()));
            } else {
              ArrayLit* al = eval_par(vdi->e()->e())->cast<ArrayLit>();
              int dim = vdi->e()->type().dim();

              // Maximum size of each dimension
              vector<int> dmax;
              // Current size of each dimension
              vector<int> d;
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
              int currentPos = 0;
              do {
                PyList_SetItem(p[i], d[i], PyBool_FromLong(eval_int(al->v()[currentPos]).toInt()));
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
            }
          } else if (vdi->e()->type().bt() == Type::BT_INT) {
            if (vdi->e()->type().dim() == 0) {
              //IntVal iv = eval_int(vdi->e()->e());
              PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(), PyInt_FromLong(eval_bool(vdi->e()->e())));
            } else {
              ArrayLit* al = eval_par(vdi->e()->e())->cast<ArrayLit>();
              int dim = vdi->e()->type().dim();

              // Maximum size of each dimension
              vector<int> dmax;
              // Current size of each dimension
              vector<int> d;
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
              int currentPos = 0;
              do {
                PyList_SetItem(p[i], d[i], PyInt_FromLong(eval_bool(al->v()[currentPos])));
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
            }
          } else if (vdi->e()->type().bt() == Type::BT_STRING) {
            if (vdi->e()->type().dim() == 0) {
              //IntVal iv = eval_int(vdi->e()->e());
              string temp(eval_string(vdi->e()->e()));
              PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(), PyString_FromString(temp.c_str()));
            } else {
              ArrayLit* al = eval_par(vdi->e()->e())->cast<ArrayLit>();
              int dim = vdi->e()->type().dim();

              // Maximum size of each dimension
              vector<int> dmax;
              // Current size of each dimension
              vector<int> d;
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
              int currentPos = 0;
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
            }
          }
        }
      }
      PyList_Append(solutions, sol);
      //Py_DECREF(obj);
      return Py_BuildValue("iO", status, solutions);  
    } else {
      //Py_DECREF(obj);
      PyErr_SetString(mzn_solve_error,"Unknown status code");
      return NULL;
    }
    
  }
  const std::string& tmp = errorStream.str();
  const char* cstr = tmp.c_str();
  PyErr_SetString(mzn_solve_error, cstr);
  return NULL;
}

static PyMethodDef MiniZincMethods[] = {
    {"solve",  mzn_solve, METH_VARARGS, "Solve a MiniZinc model"},
    //{"mzn_set", mzn_set, METH_VARARGS, "Create a MiniZinc set: args[1]..args[2]"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initminizinc(void) {
    PyObject* model = Py_InitModule("minizinc", MiniZincMethods);

    if (model == NULL)
      return;

    //Error Handling
    mzn_solve_error = PyErr_NewException("mzn_solve.error", NULL, NULL);
    if (mzn_solve_error == NULL)
      return;
    Py_XINCREF(mzn_solve_error);
    PyModule_AddObject(model,"error",mzn_solve_error);

    mzn_solve_warning = PyErr_NewException("mzn_solve.warning", NULL, NULL);
    if (mzn_solve_error == NULL)
      return;
    Py_XINCREF(mzn_solve_error);
    PyModule_AddObject(model,"warning",mzn_solve_warning);
}
