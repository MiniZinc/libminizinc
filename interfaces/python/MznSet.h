/*
 *  Python Interface for MiniZinc constraint modelling
 *  Author:
 *     Tai Tran <tai.tran@student.adelaide.edu.au>
 *          under the supervision of Guido Tack <guido.tack@monash.edu>
 */

#ifndef __MZNSET_H
#define __MZNSET_H


struct MznRange {
  long min;
  long max;
  MznRange(long min, long max):min(min),max(max){}
};


struct MznSet {
  PyObject_HEAD
  list<MznRange>* ranges;

  void clear() {ranges->clear();}

  void push(long min, long max);
  void push(long v);

  int size() {return ranges->size();}
};

static PyObject* MznSet_new(PyTypeObject *type, PyObject* args, PyObject* kwds);
static void MznSet_dealloc(MznSet* self);

static void
MznSet_dealloc(MznSet* self)
{
  delete self->ranges;
  self->ob_type->tp_free((PyObject*)self);
}

static PyObject*
MznSet_new(PyTypeObject *type, PyObject* args, PyObject* kwds)
{
  MznSet* self = (MznSet*)type->tp_alloc(type,0);
  self->ranges = new list<MznRange>;
  return (PyObject* )self;
}

void MznSet::push(long min, long max) {
  if (ranges->empty()) {
    MznRange toAdd(min, max);
    ranges->push_front(toAdd);
    return;
  }
  list<MznRange>::iterator it,last;
  for (it = ranges->begin(); it!= ranges->end(); ++it) {
    if (min <= it->max + 1) {
      if (it->min < min)
        min = it->min;
      if (max <= it->max) {
        ranges->insert(it,MznRange(min,max));
        return;
      }
      goto FOUNDMIN;
    }
    continue;
    FOUNDMIN:
    last = it;
    while (++it != ranges->end()) {
      if (max < it->min-1) {
        break;
      } else {
        ranges->erase(last);
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
  ranges->push_back(MznRange(min,max));
}

void MznSet::push(long v) {
  
  list<MznRange>::iterator it,last;
  last = ranges->begin();
  if (ranges->empty()) {
    MznRange toAdd(v,v);
    ranges->push_front(toAdd);
    return;
  }
  for (it = last; it != ranges->end(); ++it) {
    if (v < it->min) {
      if (v == it->min - 1) {
        it->min = v;
        if (last->max == v) {
          last->max = it->max;
          ranges->erase(it);
          return;
        }
      } else {
        MznRange toAdd(v,v);
        ranges->insert(it,toAdd);
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
    MznRange toAdd(v,v);
    ranges->insert(it, toAdd);
  }
  return;
}

static PyObject*
MznSet_push(MznSet* self, PyObject* args) {
  PyObject* isv=NULL;
  if (!PyArg_ParseTuple(args,"|O",&isv)) {
    PyErr_SetString(MznSet_error, "Argument is not suitable");
    return NULL;
  }
  if (isv == NULL)
    Py_RETURN_TRUE;
  if (!PyList_Check(isv)) {
    PyErr_SetString(MznSet_error, "Parameter must be a list");
    return NULL;
  }

  Py_ssize_t size = PyList_Size(isv);
  for (Py_ssize_t i = 0; i!=size; ++i) {
    PyObject* elem = PyList_GetItem(isv,i);
    if (PyList_Check(elem)) {
      if (PyList_Size(elem) == 1) {
        PyObject* value = PyList_GetItem(elem, 0);
        if (PyInt_Check(value))
          self->push(PyInt_AS_LONG(value));
        else {
          PyErr_SetString(MznSet_error, "Value must be an integer");
          return NULL;
        }
      } else if (PyList_Size(elem) == 2) {
        PyObject* Pmin = PyList_GetItem(elem,0);
        PyObject* Pmax = PyList_GetItem(elem,1);
        if (PyInt_Check(Pmin) && PyInt_Check(Pmax)) {
          int min = PyInt_AS_LONG(Pmin);
          int max = PyInt_AS_LONG(Pmax);
          if (min < max)
            self->push(min, max);
          else if (min > max) 
            self->push(max, min);
          else self->push(min);
        } else {
          PyErr_SetString(MznSet_error, "Both values must be integers");
          return NULL;
        }
      } else {
        PyErr_SetString(MznSet_error, "The sublist size can only be 1 or 2");
        return NULL;
      }
    } else if (PyInt_Check(elem)) {
      self->push(PyInt_AS_LONG(elem));
    } else {
      PyErr_SetString(MznSet_error, "Values must be an integer or a list of integers");
      return NULL;
    }
  }
  Py_RETURN_NONE;
}

static int
MznSet_init(MznSet* self, PyObject* args)
{
  if (MznSet_push(self, args) == NULL)
    return -1;
  return 0;
}

static PyObject*
MznSet_output(MznSet* self)
{
  stringstream s;
  for (list<MznRange>::iterator it = self->ranges->begin(); it!=self->ranges->end(); ++it) {
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

static PyObject* MznSet_repr(PyObject* self) {
  stringstream output;
  list<MznRange>* r = ((MznSet*)self)->ranges;
  if (r->begin() == r->end())
    output << "Empty Set";
  else for (list<MznRange>::iterator it = r->begin();;) {
    if (it->min == it->max)
      output << it->min;
    else output << it->min << ".." << it->max;
    if (++it == r->end())
      break;
    else output << ", ";
  }
  const std::string& tmp = output.str();
  return PyString_FromString(tmp.c_str());
}

static PyMethodDef MznSet_methods[] = {
  {"output", (PyCFunction)MznSet_output, METH_NOARGS, "Return all values in the set"},
  {"output", (PyCFunction)MznSet_output, METH_VARARGS, "Return all values in the set"},
  {"push", (PyCFunction)MznSet_push, METH_VARARGS, "Expand the set"},
  {NULL}    /* Sentinel */
};

static PyTypeObject MznSetType = {
  PyVarObject_HEAD_INIT(NULL,0)
  "minizinc.set",       /* tp_name */
  sizeof(MznSet),          /* tp_basicsize */
  0,                         /* tp_itemsize */
  (destructor)MznSet_dealloc, /* tp_dealloc */
  0,                         /* tp_print */
  0,                         /* tp_getattr */
  0,                         /* tp_setattr */
  0,                         /* tp_reserved */
  MznSet_repr,               /* tp_repr */
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

#endif