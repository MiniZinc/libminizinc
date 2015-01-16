#include <Python.h>
#include "structmember.h"

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <list>
#include <string.h>
#include <typeinfo>

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
static PyObject* mzn_load_error;
static PyObject* mzn_loadString_error;
static PyObject* mzn_solve_warning;
static PyObject* mzn_set_error;

void MznModelDestructor(PyObject* o) {
  const char* name = PyCapsule_GetName(o);
  delete[] name;
  Py_DECREF(o);
}

struct Range {
  long min;
  long max;
  Range(long min, long max):min(min),max(max){}
};

typedef struct {
  PyObject_HEAD
  list<Range>* ranges;
} MznSet;

static void
MznSet_dealloc(MznSet* self)
{
  delete self->ranges;
  self->ob_type->tp_free((PyObject*)self);
}

static PyObject*
MznSet_new(PyTypeObject *type, PyObject* args, PyObject* kwds)
{
  MznSet *self;
  self = (MznSet*)type->tp_alloc(type,0);
  if (self != NULL) {
    self->ranges = new list<Range>;
  }
  return (PyObject* )self;
}

static void push(MznSet* obj, long min, long max) {
  if (obj->ranges->empty()) {
    Range toAdd(min, max);
    obj->ranges->push_front(toAdd);
    return;
  }
  list<Range>::iterator it,last;
  for (it = obj->ranges->begin(); it!= obj->ranges->end(); ++it) {
    if (min <= it->max + 1) {
      if (it->min < min)
        min = it->min;
      if (max <= it->max)
        return;
      goto FOUNDMIN;
    }
    continue;
    FOUNDMIN:
    last = it;
    while (++it != obj->ranges->end()) {
      if (max < it->min-1) {
        break;
      } else {
        obj->ranges->erase(last);
        last = it;
        if (max <= it->max) {
          break;
        } else 
          ++it;
      }
    }
    last->min = min;
    last->max = max;
    return;
  }
  Range toAdd(min,max);
  obj->ranges->push_back(toAdd);
}

static void push(MznSet* obj, long v) {
  
  list<Range>::iterator it,last;
  last = obj->ranges->begin();
  if (obj->ranges->empty()) {
    Range toAdd(v,v);
    obj->ranges->push_front(toAdd);
    return;
  }
  for (it = last; it != obj->ranges->end(); ++it) {
    if (v < it->min) {
      if (v == it->min - 1) {
        it->min = v;
        if (last->max == v) {
          last->max = it->max;
          obj->ranges->erase(it);
          return;
        }
      } else {
        Range toAdd(v,v);
        obj->ranges->insert(it,toAdd);
        return;
      }
    }
    else if (v > it->max) {
      if (v == it->max + 1) {
        it->max = v;
      } 
      last = it;
      continue;
    }
    else {
      return;
    }
  }
  if (last->max<v) {
    Range toAdd(v,v);
    obj->ranges->insert(it, toAdd);
  }
  return;
}

static PyObject*
MznSet_push(MznSet* self, PyObject* args) {
  PyObject* isv=NULL;
  if (!PyArg_ParseTuple(args,"|O",&isv)) {
    PyErr_SetString(mzn_set_error, "Argument is not suitable");
    return NULL;
  }
  if (isv == NULL)
    Py_RETURN_TRUE;
  if (!PyList_Check(isv)) {
    PyErr_SetString(mzn_set_error, "Parameter must be a list");
    return NULL;
  }

  Py_ssize_t size = PyList_Size(isv);
  for (Py_ssize_t i = 0; i!=size; ++i) {
    PyObject* elem = PyList_GetItem(isv,i);
    if (PyList_Check(elem)) {
      if (PyList_Size(elem) == 1) {
        PyObject* value = PyList_GetItem(elem, 0);
        if (PyInt_Check(value))
          push(self, PyInt_AS_LONG(value));
        else {
          PyErr_SetString(mzn_set_error, "Value must be an integer");
          return NULL;
        }
      } else if (PyList_Size(elem) == 2) {
        PyObject* Pmin = PyList_GetItem(elem,0);
        PyObject* Pmax = PyList_GetItem(elem,1);
        if (PyInt_Check(Pmin) && PyInt_Check(Pmax)) {
          int min = PyInt_AS_LONG(Pmin);
          int max = PyInt_AS_LONG(Pmax);
          if (min < max)
            push(self, min, max);
          else if (min > max) 
            push(self, max, min);
          else push(self, min);
        } else {
          PyErr_SetString(mzn_set_error, "Both values must be integers");
          return NULL;
        }
      } else {
        PyErr_SetString(mzn_set_error, "The sublist size can only be 1 or 2");
        return NULL;
      }
    } else if (PyInt_Check(elem)) {
      push(self, PyInt_AS_LONG(elem));
    } else {
      PyErr_SetString(mzn_set_error, "Values must be an integer or a list of integers");
      return NULL;
    }
  }
  Py_RETURN_NONE;
}

static int
MznSet_init(MznSet* self, PyObject* args)
{
  self->ranges->clear();
  if (MznSet_push(self, args) == NULL)
    return -1;
  return 0;
}

static PyObject*
MznSet_output(MznSet* self)
{
  stringstream s;
  cout << typeid(*self).name() << endl;
  //cout << (self->ranges.begin()) << endl << (self->ranges.end()) << endl;
  for (list<Range>::iterator it = self->ranges->begin(); it!=self->ranges->end(); ++it) {
    s << it->min << ".." << it->max << " ";
  }
  const std::string& tmp = s.str();
  const char* cstr = tmp.c_str();
  PyObject* result = PyString_FromString(cstr);
  return result;
}

static PyMemberDef MznSet_members[] = {
  {NULL} /* Sentinel */
};

static PyMethodDef MznSet_methods[] = {
  {"output", (PyCFunction)MznSet_output, METH_NOARGS, "Return all values in the set"},
  {"output", (PyCFunction)MznSet_output, METH_VARARGS, "Return all values in the set"},
  {"push", (PyCFunction)MznSet_push, METH_VARARGS, "Expand the set"},
  {NULL}    /* Sentinel */
};

static PyTypeObject MznSetType = {
  PyVarObject_HEAD_INIT(NULL,0)
  "minizinc.type_set",       /* tp_name */
  sizeof(MznSet),          /* tp_basicsize */
  0,                         /* tp_itemsize */
  0,/*(destructor)MznModel_dealloc, /* tp_dealloc */
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
  "Minizinc Set",  /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  0,                         /* tp_iter */
  0,                         /* tp_iternext */
  MznSet_methods,            /* tp_methods */
  MznSet_members,            /* tp_members */
  0,/*MznModel_getseters,        /* tp_getset */
  0,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  (initproc)MznSet_init,     /* tp_init */
  0,                         /* tp_alloc */
  MznSet_new,                /* tp_new */
};

class MznObject {
private:
  Model* _m;
public:
  MznObject(Model* m): _m(m) {}
  const Model* getModel() const {return _m;};
  void setModel(Model* m) {_m = m;}
};


string minizinc_set(long long start, long long end);

int getList(PyObject* value, vector<Py_ssize_t>& dimensions, vector<PyObject*>& simpleArray, const int layer);



string processModel(Model*& m, PyObject* obj, vector<string>& includePaths, PyObject*& capsule) {
  vector<string> data;
  // Records the list of Set for special parsing
  list<string> nameOfSets;
  for (unsigned int i=0; i<m->size(); i++) {
    if (VarDeclI* vdi = (*m)[i]->dyn_cast<VarDeclI>())
      if (vdi->e()->type().is_set() && vdi->e()->type().ispar()) {
        nameOfSets.push_back(vdi->e()->id()->str().str());
      }
  }
  // Parsing values
  stringstream assignments;
  Py_ssize_t pos = 0;
  PyObject* key;
  PyObject* value;
  while (PyDict_Next(obj, &pos, &key, &value)) {
    assignments << PyString_AsString(key) << " = ";
    if (PyList_Check(value)) {
      vector<Py_ssize_t> dimensions;
      vector<PyObject*> simpleArray;
      if (PyList_Size(value)==0)
        return "Objects must contain at least 1 value";
      bool is_set = false; // find out if it is a set or an array.
      std::list<string>::iterator findIter = std::find(nameOfSets.begin(),nameOfSets.end(),PyString_AsString(key));
      if (findIter!=nameOfSets.end()) {
        is_set = true;
        nameOfSets.erase(findIter);
      }
      if (getList(value, dimensions, simpleArray,0) == -1)
        return "Inconsistency in size of multidimensional array";

      if (dimensions.size()>6)
        return "Maximum dimension of a multidimensional array is 6";
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
          } else
            return "Inconsistency in values type";
        }
      else if (PyInt_Check(simpleArray[0]))
        for (vector<PyObject*>::size_type i=0; i!=simpleArray.size(); i++) {
          if (i!=0)
            assignments << ", ";
          if (PyInt_Check(simpleArray[i]))
            assignments << PyInt_AS_LONG(simpleArray[i]);
          else
            return "Inconsistency in values type";
        }
      else if (PyFloat_Check(simpleArray[0]))
        for (vector<PyObject*>::size_type i=0; i!=simpleArray.size(); i++) {
          if (i!=0)
            assignments << ", ";
          if (PyFloat_Check(simpleArray[i]))
            assignments << PyFloat_AS_DOUBLE(simpleArray[i]);
          else
            return "Inconsistency in values type";
        }
      else if (PyString_Check(simpleArray[0]))
        for (vector<PyObject*>::size_type i=0; i!=simpleArray.size(); i++) {
          if (i!=0)
            assignments << ", ";
          if (PyString_Check(simpleArray[i]))
            assignments << "\"" << PyString_AS_STRING(simpleArray[i]) << "\"";
          else
            return "Inconsistency in values type";
        }
      else
        return "Object must be an integer, float, boolean or string";
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
      else 
        return "Object must be a list or value";
    }
    assignments << ";\n";
  }
  string asn = assignments.str();
  cout << asn << endl;
  if (asn.size() > 0)
    data.push_back("cmd:/"+assignments.str());
  stringstream errorStream;
  if (!parseData(m, data, includePaths, true,
                     false, false, errorStream))
  {
    const std::string& tmp = errorStream.str();
    return tmp;
  }

mzn_processModel_return:
  Model* o = m;
  char* name = new char[20];
  strcpy(name, "MiniZinc Object");
  capsule = PyCapsule_New((void*)o, name, MznModelDestructor);
  return "";
}

static PyObject*
mzn_loadString(PyObject *self, PyObject *args, PyObject *keywds) {
  PyObject* obj = Py_None;
  PyObject* options = Py_None;
  const char* py_string;

  static char *kwlist[] = {"string","data","options"};

  if (!PyArg_ParseTupleAndKeywords(args, keywds, "s|OO", kwlist, &py_string, &obj, &options)) {
    PyErr_SetString(mzn_load_error, "Parsing error");
    return NULL;
  }

  string std_lib_dir;
  if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
    std_lib_dir = string(MZNSTDLIBDIR);
  } else {
    PyErr_SetString(mzn_load_error, "No MiniZinc library directory MZN_STDLIB_DIR defined.");
    return NULL;
  }

  vector<string> includePaths;
  includePaths.push_back(std_lib_dir+"/gecode/");
  includePaths.push_back(std_lib_dir+"/std/");


  const char* data;
  stringstream errorStream;
  if (Model* m = parseFromString(string(py_string), data, includePaths, false, false, false, errorStream))
  {
    PyObject* capsule;
    string errorLog = processModel(m, obj, includePaths, capsule);
    if (errorLog!="") {
      PyErr_SetString(mzn_loadString_error,errorLog.c_str());
      return NULL;
    }
    return capsule;
  } else {
    const std::string& tmp = errorStream.str();
    const char* cstr = tmp.c_str();
    PyErr_SetString(mzn_load_error, cstr);
    return NULL;
  }
  PyErr_SetString(mzn_load_error, "Unknown Error");
  return NULL;
}

static PyObject*
mzn_load(PyObject *self, PyObject *args, PyObject *keywds) {
  PyObject* obj = Py_None;
  Py_ssize_t pos = 0;
  PyObject* key;
  PyObject* value;
  PyObject* options = Py_None;
  const char* py_filename;

  static char *kwlist[] = {"name","data","options"};

  if (!PyArg_ParseTupleAndKeywords(args, keywds, "s|OO", kwlist, &py_filename, &obj, &options)) {
    PyErr_SetString(mzn_load_error, "Parsing error");
    return NULL;
  }

  string std_lib_dir;
  if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
    std_lib_dir = string(MZNSTDLIBDIR);
  } else {
    PyErr_SetString(mzn_load_error, "No MiniZinc library directory MZN_STDLIB_DIR defined.");
    return NULL;
  }

  vector<string> includePaths;
  includePaths.push_back(std_lib_dir+"/gecode/");
  includePaths.push_back(std_lib_dir+"/std/");



  stringstream errorStream;
  // Empty string (leave out the parse in argument first)
  vector<string> data;
  if (Model* m = parse(string(py_filename), data, includePaths, false, false, false, errorStream))
  {
    PyObject* capsule;
    string errorLog = processModel(m, obj, includePaths, capsule);
    if (errorLog!="") {
      PyErr_SetString(mzn_load_error,errorLog.c_str());
      return NULL;
    }
    return capsule;
  } else {
    const std::string& tmp = errorStream.str();
    const char* cstr = tmp.c_str();
    PyErr_SetString(mzn_load_error, cstr);
    return NULL;
  }
  PyErr_SetString(mzn_load_error, "Unknown Error");
  return NULL;
}

static PyObject*
mzn_solve(PyObject *self, PyObject *args)
{
  PyObject* options = Py_None;
  PyObject* obj;

  if (!PyArg_ParseTuple(args, "O|O", &obj, &options)) {
    PyErr_SetString(mzn_solve_error, "Parsing error");
    return NULL;
  }

  if (PyCapsule_CheckExact(obj)) {
    char name[] = "MiniZinc Object";
    Model* m;
    if (void* tempObj = PyCapsule_GetPointer(obj,name))
      m = (Model*)tempObj;
    else {
      PyErr_SetString(mzn_solve_error, "Parsing argument is not a MiniZinc object");
      return NULL;
    }
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
                  //cout << d[0] << " " << dmax[0] << endl;
                  PyList_SetItem(p[i], d[i], PyInt_FromLong(eval_int(al->v()[currentPos]).toInt()));
                  //cout << "REACHED HERE" << endl;
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
                //cout << "REACHED HERE" << endl;
                PyDict_SetItemString(sol, vdi->e()->id()->str().c_str(),p[0]);
                for (int i=0; i<dim; i++)
                  Py_DECREF(p[i]);
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
      PyErr_SetString(mzn_solve_error,"Unknown status code");
      return NULL;
    }
  }
  PyErr_SetString(mzn_solve_error,"Passed argument must be a MiniZinc object");
  return NULL;
}

static PyMethodDef MiniZincMethods[] = {
    {"solve",  mzn_solve, METH_VARARGS, "Solve a MiniZinc model"},
    {"load", (PyCFunction)mzn_load, METH_VARARGS | METH_KEYWORDS, "Load MiniZinc file into Python"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


PyMODINIT_FUNC
initminizinc(void) {
    GC::init();
    PyObject* model = Py_InitModule3("minizinc", MiniZincMethods, "A python interface for minizinc constraint modeling");

    if (model == NULL)
      return;

    //Error Handling
    mzn_load_error = PyErr_NewException("mzn_load.error", NULL, NULL);
    if (mzn_load_error == NULL)
      return;
    Py_INCREF(mzn_load_error);
    PyModule_AddObject(model,"error",mzn_load_error);

    mzn_loadString_error = PyErr_NewException("mzn_loadString.error", NULL, NULL);
    if (mzn_loadString_error == NULL)
      return;
    Py_INCREF(mzn_loadString_error);
    PyModule_AddObject(model,"error",mzn_loadString_error);


    mzn_solve_error = PyErr_NewException("mzn_solve.error", NULL, NULL);
    if (mzn_solve_error == NULL)
      return;
    Py_INCREF(mzn_solve_error);
    PyModule_AddObject(model,"error",mzn_solve_error);

    mzn_solve_warning = PyErr_NewException("mzn_solve.warning", NULL, NULL);
    if (mzn_solve_error == NULL)
      return;
    Py_INCREF(mzn_solve_warning);
    PyModule_AddObject(model,"warning",mzn_solve_warning);

    mzn_set_error = PyErr_NewException("mzn_set.error", NULL, NULL);
    if (mzn_set_error == NULL)
      return;
    Py_INCREF(mzn_set_error);
    PyModule_AddObject(model,"error",mzn_set_error);


    // Minizinc Set Initialization
    Py_INCREF(&MznSetType);
    PyModule_AddObject(model, "Set", (PyObject *)&MznSetType);

    if (PyType_Ready(&MznSetType) < 0)
      return;

    Py_INCREF(&MznSetType);
    PyModule_AddObject(model, "Set", (PyObject *)&MznSetType);
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


string minizinc_set(long long start, long long end) {
  stringstream ret;
  ret << start << ".." << end;
  string asn(ret.str());
  return asn;
}