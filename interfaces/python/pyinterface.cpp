#include <Python.h>
#include <iostream>

#include <minizinc/parser.hh>
#include <minizinc/model.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/flatten.hh>
#include <minizinc/optimize.hh>
#include <minizinc/builtins.hh>
#include <minizinc/file_utils.hh>
#include <solvers/gecode/gecode_solverinstance.hh>

using namespace MiniZinc;
using namespace std;

static PyObject*
mzn_solve(PyObject *self, PyObject *args)
{
  PyObject* obj;
  Py_ssize_t pos;
  PyObject* key;
  PyObject* value;
  const char* py_filename;

  if (!PyArg_ParseTuple(args, "sO", &py_filename, &obj))
    return NULL;

  Py_INCREF(obj);

  stringstream assignments;
  while (PyDict_Next(obj, &pos, &key, &value)) {
    if (PyList_Check(value)) {
      assignments << PyString_AsString(key) << " = [";
      for (Py_ssize_t i=0; i<PyList_Size(value); i++) {
        PyObject* li = PyList_GetItem(value,i);
        Py_INCREF(li);
        if (!PyInt_Check(li))
          std::cerr << "not an integer for " << PyString_AsString(key) << " at index " << i << std::endl;
        else
          assignments << PyInt_AsLong(li);
        Py_DECREF(li);
        if (i < PyList_Size(value)-1)
          assignments << ",";
      }
      assignments << "];\n";
    } else {
      assignments << PyString_AsString(key) << " = ";
      assignments << PyInt_AsLong(value) << ";\n";
    }
  }
  vector<string> data;
  data.push_back("cmd:/"+assignments.str());

  string std_lib_dir;

  vector<string> includePaths;
  includePaths.push_back(std_lib_dir+"/gecode/");
  includePaths.push_back(std_lib_dir+"/std/");

  GC::init();
  if (Model* m = parse(string(py_filename), data, includePaths, false,
                       false, false, std::cerr)) {
    vector<TypeError> typeErrors;
    MiniZinc::typecheck(m, typeErrors);
    if (typeErrors.size() > 0) {
     for (unsigned int i=0; i<typeErrors.size(); i++) {
       std::cerr << typeErrors[i].loc() << ":" << std::endl;
       std::cerr << typeErrors[i].what() << ": " << typeErrors[i].msg() << std::endl;
     }
     return NULL;
    }
    MiniZinc::registerBuiltins(m);

    Env env(m);
    try {
      FlatteningOptions fopts;
      flatten(env,fopts);
    } catch (LocationException& e) {
      std::cerr << e.what() << ": " << std::endl;
      env.dumpErrorStack(std::cerr);
      std::cerr << "  " << e.msg() << std::endl;
      return NULL;
    }
    for (unsigned int i=0; i<env.warnings().size(); i++) {
      std::cerr << "Warning: " << env.warnings()[i];
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
          if (vdi->e()->type() == Type::parint()) {
            IntVal iv = eval_int(vdi->e()->e());
            PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(), PyInt_FromLong(iv.toInt()));
          } else if (vdi->e()->type() == Type::parint(1)) {
            ArrayLit* al = eval_par(vdi->e()->e())->cast<ArrayLit>();
            PyObject* pa = PyList_New(al->v().size());
            for (unsigned int i=al->v().size(); i--;)
              PyList_SetItem(pa, i, PyInt_FromLong(eval_int(al->v()[i]).toInt()));
            PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(), pa);
          } else if (vdi->e()->type() == Type::parint(2)) {
            ArrayLit* al = eval_par(vdi->e()->e())->cast<ArrayLit>();
            int d1 = al->max(0)-al->min(0)+1;
            int d2 = al->max(1)-al->min(1)+1;
            PyObject* pa = PyList_New(d1);
            for (unsigned int i=0; i < d1; i++) {
              PyObject* pb = PyList_New(d2);
              for (unsigned int j=0; j<d2; j++) {
                PyList_SetItem(pb, j, PyInt_FromLong(eval_int(al->v()[i*d2+j]).toInt()));
              }
              PyList_SetItem(pa, i, pb);
            }
            PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(), pa);
          }
        }
      }
      PyList_Append(solutions, sol);
      Py_DECREF(obj);
      return Py_BuildValue("iO", status, solutions);
      
    }
    
  }

  return NULL;
}

static PyMethodDef MiniZincMethods[] = {
    {"solve",  mzn_solve, METH_VARARGS, "Solve a MiniZinc model"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initminizinc(void) {
    (void) Py_InitModule("minizinc", MiniZincMethods);
}
