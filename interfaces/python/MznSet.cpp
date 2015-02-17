#include "MznSet.h"

static void
MznSet_dealloc(MznSet* self)
{
  delete self->ranges;
  self->ob_type->tp_free(reinterpret_cast<PyObject*>(self));
}

static PyObject*
MznSet_new(PyTypeObject *type, PyObject* args, PyObject* kwds)
{
  MznSet* self = reinterpret_cast<MznSet*>(type->tp_alloc(type,0));
  self->ranges = new list<MznRange>;
  return reinterpret_cast<PyObject*>(self);
}

static PyObject*
MznSet_iter(PyObject* self)
{
  MznSetIter* iter = reinterpret_cast<MznSetIter*>(MznSetIter_new(&MznSetIterType, NULL, NULL));
  iter->listBegin = reinterpret_cast<MznSet*>(self)->ranges->begin();
  iter->listEnd = reinterpret_cast<MznSet*>(self)->ranges->end();
  iter->listIndex = iter->listBegin;
  iter->currentValue = iter->listBegin->min;
  return reinterpret_cast<PyObject*>(iter);
}
/*
static PyObject*
MznSet_iternext(PyObject* self)
{
  MznSetIter* iter = reinterpret_cast<MznSetIter*>(self);
  if (iter->listIndex==iter->listEnd) {
    iter->listIndex = iter->listBegin;
    iter->currentValue = iter->listBegin->min;
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  } else {
    PyObject *result = PyInt_FromLong(iter->currentValue);
    iter->currentValue++;
    if (iter->currentValue > iter->listIndex->max) {
      iter->listIndex++;
      if (iter->listIndex!=iter->listEnd)
        iter->currentValue = iter->listIndex->min;
    }
    return result;
  }
}*/

bool MznSet::contains(long val) {
  list<MznRange>::iterator it = ranges->begin();
  list<MznRange>::iterator end = ranges->end();
  assert (it != end);
  for (;it++ != end;)
    if (val < it->min)
      return false;
    else if (val <= it->max)
      return true;
  return false; 
}

bool MznSet::continuous() {
  if (ranges->empty())
    throw length_error("Ranges cannot be empty");
  else {
    // returning ranges.size() == 1, with O(1) time complexity
    list<MznRange>::iterator it = ranges->begin();
    return ++it == ranges->end();
  }
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
        //
        if (last->max < v) {
          MznRange toAdd(v,v);
          ranges->insert(it,toAdd);
        }
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
  if (last->max < v) {
    MznRange toAdd(v,v);
    ranges->insert(it, toAdd);
  }
  return;
}

long MznSet::min() 
{
  assert (ranges->begin() != ranges->end());
  return ranges->front().min;
}

long MznSet::max() 
{
  assert (ranges->begin() != ranges->end());
  return ranges->back().max;
}

/*SetLit* MznSet::e() {

}*/

static PyObject*
MznSet_min(MznSet* self)
{
  return PyInt_FromLong(self->min());
}

static PyObject*
MznSet_max(MznSet* self)
{
  return PyInt_FromLong(self->max());
}

static PyObject*
MznSet_continuous(MznSet* self)
{
  return PyBool_FromLong(self->continuous());
}

static PyObject*
MznSet_contains(MznSet* self, PyObject* args)
{
  PyObject* val;
  if (!PyArg_ParseTuple(args,"O",&val)) {
    PyErr_SetString(PyExc_TypeError, "Parsing error");
    return NULL;
  }
  if (!PyInt_Check(val)) {
    PyErr_SetString(PyExc_TypeError, "Argument must be an integer");
    return NULL;
  }
  return PyBool_FromLong(self->contains(PyInt_AS_LONG(val)));
}


static PyObject*
MznSet_push(MznSet* self, PyObject* args) {
  PyObject* isv;
  if (!PyArg_ParseTuple(args,"O",&isv)) {
    PyErr_SetString(PyExc_TypeError, "Parsing error");
    return NULL;
  }
  if (isv == NULL)
    Py_RETURN_NONE;
  if (!PyList_Check(isv)) {
    PyErr_SetString(PyExc_TypeError, "Argument must be a list of integer values");
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
          PyErr_SetString(PyExc_TypeError, "Value must be an integer");
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
          PyErr_SetString(PyExc_TypeError, "Both values must be integers");
          return NULL;
        }
      } else {
        PyErr_SetString(PyExc_TypeError, "The sublist size can only be 1 or 2");
        return NULL;
      }
    } else if (PyInt_Check(elem)) {
      self->push(PyInt_AS_LONG(elem));
    } else {
      PyErr_SetString(PyExc_TypeError, "List values must be an integer or a list of integers");
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

static PyObject* MznSet_repr(PyObject* self) {
  stringstream output;
  list<MznRange>* r = (reinterpret_cast<MznSet*>(self))->ranges;
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





static void
MznSetIter_dealloc(MznSetIter* self)
{
  self->ob_type->tp_free(reinterpret_cast<PyObject*>(self));
}

static PyObject*
MznSetIter_new(PyTypeObject *type, PyObject* args, PyObject* kwds)
{
  return type->tp_alloc(type,0);
}



static PyObject*
MznSetIter_iternext(PyObject* self)
{
  MznSetIter* iter = reinterpret_cast<MznSetIter*>(self);
  if (iter->listIndex==iter->listEnd) {
    iter->listIndex = iter->listBegin;
    iter->currentValue = iter->listBegin->min;
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  } else {
    PyObject *result = PyInt_FromLong(iter->currentValue);
    iter->currentValue++;
    if (iter->currentValue > iter->listIndex->max) {
      iter->listIndex++;
      if (iter->listIndex!=iter->listEnd)
        iter->currentValue = iter->listIndex->min;
    }
    return result;
  }
}