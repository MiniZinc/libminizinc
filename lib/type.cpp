/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/flatten_internal.hh>
#include <minizinc/type.hh>

namespace MiniZinc {

bool Type::btSubtype(const EnvI& env, const Type& t0, const Type& t1, bool strictEnums) {
  if (t0.bt() == t1.bt()) {
    if (t0.bt() == BT_TUPLE) {
      if (t0.typeId() == t1.typeId()) {
        return true;
      }
      auto t0id = t0.typeId();
      if (t0.dim() != 0) {
        t0id = env.getArrayEnum(t0id)[t0.dim()];
      }
      auto t1id = t1.typeId();
      if (t1.dim() != 0) {
        t1id = env.getArrayEnum(t1id)[t1.dim()];
      }
      if (env.getTupleType(t0id)->isSubtypeOf(env, *env.getTupleType(t1id), strictEnums)) {
        return true;
      }
    }
    if ((!strictEnums || t0.dim() != 0 || (t0.typeId() == t1.typeId() || t1.typeId() == 0))) {
      return true;
    }
  }
  switch (t0.bt()) {
    case BT_BOOL:
      return (t1.bt() == Type::BT_INT || t1.bt() == BT_FLOAT);
    case BT_INT:
      return t1.bt() == BT_FLOAT;
    default:
      return false;
  }
}

unsigned int Type::commonTuple(EnvI& env, unsigned int tupleId1, unsigned int tupleId2) {
  if (tupleId1 == tupleId2) {
    return tupleId1;
  }
  if (tupleId1 == 0 || tupleId2 == 0) {
    return 0;
  }
  TupleType* tt1 = env.getTupleType(tupleId1);
  TupleType* tt2 = env.getTupleType(tupleId2);
  unsigned int size = std::min(tt1->size(), tt2->size());

  std::vector<Type> common(size);
  for (unsigned int i = 0; i < size; i++) {
    if ((*tt1)[i].bt() == BT_TUPLE) {
      common[i] = (*tt1)[i];
      if ((*tt1)[i].typeId() != (*tt2)[i].typeId()) {
        common[i].typeId(commonTuple(env, (*tt1)[i].typeId(), (*tt2)[i].typeId()));
      }
      if (common[i].typeId() == 0) {
        return 0;
      }
    } else {
      if (btSubtype(env, (*tt2)[i], (*tt1)[i], false)) {
        common[i] = (*tt1)[i];
      } else if (btSubtype(env, (*tt1)[i], (*tt2)[i], false)) {
        common[i] = (*tt2)[i];
      } else {
        return 0;
      }
      if ((*tt1)[i].typeId() != (*tt2)[i].typeId()) {
        common[i].typeId(0);
      }
    }
  }
  return env.registerTupleType(common);
}

std::string Type::toString(const EnvI& env) const {
  std::ostringstream oss;
  if (_dim > 0) {
    oss << "array[";
    if (_typeId != 0U) {
      const std::vector<unsigned int>& arrayEnumIds = env.getArrayEnum(_typeId);
      for (unsigned int i = 0; i < arrayEnumIds.size() - 1; i++) {
        if (i != 0) {
          oss << ",";
        }
        unsigned int enumId = arrayEnumIds[i];
        if (enumId == 0) {
          oss << "_";
        } else {
          oss << *env.getEnum(enumId)->e()->id();
        }
      }
    } else {
      for (int i = 0; i < _dim; i++) {
        oss << (i == 0 ? "" : ",") << "int";
      }
    }
    oss << "] of ";
  }
  if (_dim < 0) {
    oss << "array[$_] of ";
  }
  switch (static_cast<int>(_ti)) {
    case TI_PAR:
      break;
    case TI_VAR:
      oss << "var ";
      break;
  }
  if (static_cast<OptType>(_ot) == OT_OPTIONAL) {
    oss << "opt ";
  }
  if (static_cast<SetType>(_st) == ST_SET) {
    oss << "set of ";
  }
  switch (static_cast<BaseType>(_bt)) {
    case BT_INT: {
      unsigned int enumId;
      if (_typeId != 0U && _dim > 0) {
        const std::vector<unsigned int>& arrayEnumIds = env.getArrayEnum(_typeId);
        enumId = arrayEnumIds[arrayEnumIds.size() - 1];
      } else {
        enumId = _typeId;
      }
      if (enumId == 0) {
        oss << "int";
      } else {
        oss << *env.getEnum(enumId)->e()->id();
      }
    } break;
    case BT_BOOL:
      oss << "bool";
      break;
    case BT_FLOAT:
      oss << "float";
      break;
    case BT_STRING:
      oss << "string";
      break;
    case BT_ANN:
      oss << "ann";
      break;
    case BT_TUPLE: {
      unsigned int typeId;
      oss << "tuple(";
      if (_typeId != 0U && _dim > 0) {
        const std::vector<unsigned int>& arrayEnumIds = env.getArrayEnum(_typeId);
        typeId = arrayEnumIds[arrayEnumIds.size() - 1];
      } else {
        typeId = _typeId;
      }
      TupleType* tt = env.getTupleType(typeId);
      for (size_t i = 0; i < tt->size(); ++i) {
        oss << (*tt)[i].toString(env);
        if (i < tt->size() - 1) {
          oss << ", ";
        }
      }
      oss << ")";
    } break;
    case BT_BOT:
      oss << "bot";
      break;
    case BT_TOP:
      oss << "top";
      break;
    case BT_UNKNOWN:
      oss << "??? ";
      break;
  }
  return oss.str();
}

std::string Type::nonEnumToString() const {
  std::ostringstream oss;
  if (_dim > 0) {
    oss << "array[int";
    for (int i = 1; i < _dim; i++) {
      oss << ",int";
    }
    oss << "] of ";
  }
  if (_dim < 0) {
    oss << "array[$_] of ";
  }
  switch (static_cast<Inst>(_ti)) {
    case TI_PAR:
      break;
    case TI_VAR:
      oss << "var ";
      break;
  }
  if (static_cast<OptType>(_ot) == OT_OPTIONAL) {
    oss << "opt ";
  }
  if (static_cast<SetType>(_st) == ST_SET) {
    oss << "set of ";
  }
  switch (static_cast<BaseType>(_bt)) {
    case BT_INT:
      oss << "int";
      break;
    case BT_BOOL:
      oss << "bool";
      break;
    case BT_FLOAT:
      oss << "float";
      break;
    case BT_STRING:
      oss << "string";
      break;
    case BT_ANN:
      oss << "ann";
      break;
    case BT_TUPLE:
      oss << "tuple(...)";
      break;
    case BT_BOT:
      oss << "bot";
      break;
    case BT_TOP:
      oss << "top";
      break;
    case BT_UNKNOWN:
      oss << "??? ";
      break;
  }
  return oss.str();
}

bool Type::parTuple(const EnvI& env, const Type& t) {
  assert(t.bt() == BT_TUPLE);
  TupleType* tt = env.getTupleType(t.typeId());
  for (size_t i = 0; i < tt->size(); ++i) {
    if (!(*tt)[i].isPar() || (*tt)[i].cv() || (*tt)[i].bt() != Type::BT_ANN ||
        ((*tt)[i].bt() == Type::BT_TUPLE && !parTuple(env, (*tt)[i]))) {
      return false;
    }
  }
  return true;
}

}  // namespace MiniZinc
