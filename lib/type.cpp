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

#include <cassert>
#include <vector>

namespace MiniZinc {

bool Type::btSubtype(const EnvI& env, const Type& t0, const Type& t1, bool strictEnums) {
  if (t0.bt() == t1.bt()) {
    if (t0.bt() == BT_TUPLE) {
      if (t0.typeId() == t1.typeId()) {
        return true;
      }
      if (env.getTupleType(t0)->isSubtypeOf(env, *env.getTupleType(t0), strictEnums)) {
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

void Type::mkPar(EnvI& env) {
  if (bt() != BT_TUPLE) {
    ti(TI_PAR);
    return;
  }
  if (!cv()) {
    return;
  }
  std::vector<unsigned int> arrayEnumIds;
  TupleType* tt = nullptr;
  if (dim() > 0) {
    arrayEnumIds = env.getArrayEnum(typeId());
    tt = env.getTupleType(arrayEnumIds[arrayEnumIds.size() - 1]);
  } else {
    tt = env.getTupleType(typeId());
  }
  std::vector<Type> pt(tt->size());
  for (int i = 0; i < tt->size(); ++i) {
    pt[i] = (*tt)[i];
    if (pt[i].bt() == BT_TUPLE) {
      pt[i].mkPar(env);
    } else {
      pt[i].ti(TI_PAR);
    }
  }
  typeId(0);
  cv(false);
  ti(TI_PAR);
  if (dim() > 0) {
    arrayEnumIds[arrayEnumIds.size() - 1] = env.registerTupleType(pt);
    typeId(env.registerArrayEnum(arrayEnumIds));
  } else {
    typeId(env.registerTupleType(pt));
  }
}

void Type::mkVar(EnvI& env) {
  if (bt() != BT_TUPLE) {
    ti(TI_VAR);
    return;
  }
  if (ti() == TI_VAR) {
    return;
  }
  std::vector<unsigned int> arrayEnumIds;
  TupleType* tt = nullptr;
  if (dim() > 0) {
    arrayEnumIds = env.getArrayEnum(typeId());
    tt = env.getTupleType(arrayEnumIds[arrayEnumIds.size() - 1]);
  } else {
    tt = env.getTupleType(typeId());
  }
  std::vector<Type> pt(tt->size());
  for (int i = 0; i < tt->size(); ++i) {
    pt[i] = (*tt)[i];
    if (pt[i].bt() == BT_TUPLE) {
      pt[i].mkVar(env);
    } else {
      pt[i].ti(TI_VAR);
    }
  }
  typeId(0);
  cv(true);
  ti(TI_VAR);
  if (dim() > 0) {
    arrayEnumIds[arrayEnumIds.size() - 1] = env.registerTupleType(pt);
    typeId(env.registerArrayEnum(arrayEnumIds));
  } else {
    typeId(env.registerTupleType(pt));
  }
}

void Type::mkOpt(EnvI& env) {
  assert(st() == Type::ST_PLAIN);
  if (bt() != BT_TUPLE) {
    ot(OT_OPTIONAL);
    return;
  }
  std::vector<unsigned int> arrayEnumIds;
  TupleType* tt = nullptr;
  if (dim() > 0) {
    arrayEnumIds = env.getArrayEnum(typeId());
    tt = env.getTupleType(arrayEnumIds[arrayEnumIds.size() - 1]);
  } else {
    tt = env.getTupleType(typeId());
  }
  std::vector<Type> pt(tt->size());
  bool changed = false;
  for (int i = 0; i < tt->size(); ++i) {
    pt[i] = (*tt)[i];
    if (pt[i].bt() == BT_TUPLE) {
      pt[i].mkOpt(env);
      changed = changed || (*tt)[i].typeId() != pt[i].typeId();
    } else if (st() == Type::ST_PLAIN) {
      changed = changed || pt[i].ot() != OT_OPTIONAL;
      pt[i].ot(OT_OPTIONAL);
    }
  }
  if (changed) {
    if (dim() > 0) {
      arrayEnumIds[arrayEnumIds.size() - 1] = env.registerTupleType(pt);
      typeId(env.registerArrayEnum(arrayEnumIds));
    } else {
      typeId(env.registerTupleType(pt));
    }
  }
}

void Type::mkPresent(EnvI& env) {
  if (bt() != BT_TUPLE) {
    ot(OT_PRESENT);
    return;
  }
  std::vector<unsigned int> arrayEnumIds;
  TupleType* tt = nullptr;
  if (dim() > 0) {
    arrayEnumIds = env.getArrayEnum(typeId());
    tt = env.getTupleType(arrayEnumIds[arrayEnumIds.size() - 1]);
  } else {
    tt = env.getTupleType(typeId());
  }
  std::vector<Type> pt(tt->size());
  bool changed = false;
  for (int i = 0; i < tt->size(); ++i) {
    pt[i] = (*tt)[i];
    if (pt[i].bt() == BT_TUPLE) {
      pt[i].mkOpt(env);
      changed = changed || (*tt)[i].typeId() != pt[i].typeId();
    } else {
      changed = changed || pt[i].ot() != OT_PRESENT;
      pt[i].ot(OT_PRESENT);
    }
  }
  if (changed) {
    if (dim() > 0) {
      arrayEnumIds[arrayEnumIds.size() - 1] = env.registerTupleType(pt);
      typeId(env.registerArrayEnum(arrayEnumIds));
    } else {
      typeId(env.registerTupleType(pt));
    }
  }
}

bool Type::decrement(EnvI& env) {
  if (bt() != BT_TUPLE) {
    if (ot() == Type::OT_OPTIONAL) {
      // this is var or par opt, turn into just par or var
      ot(Type::OT_PRESENT);
      return true;
    }
    if (ti() == Type::TI_VAR) {
      // var, turn into par opt
      if (st() == Type::ST_PLAIN) {
        ot(Type::OT_OPTIONAL);
      }
      ti(Type::TI_PAR);
      return true;
    }
    return false;
  }
  std::vector<unsigned int> arrayEnumIds;
  TupleType* tt = nullptr;
  if (dim() > 0) {
    arrayEnumIds = env.getArrayEnum(typeId());
    tt = env.getTupleType(arrayEnumIds[arrayEnumIds.size() - 1]);
  } else {
    tt = env.getTupleType(typeId());
  }
  // Copy types
  std::vector<Type> pt(tt->size());
  for (int i = 0; i < tt->size(); ++i) {
    pt[i] = (*tt)[i];
  }
  int changed = static_cast<int>(tt->size()) - 1;
  for (; changed >= 0; --changed) {
    if (pt[changed].decrement(env)) {
      break;
    }
  }
  if (changed < 0) {
    return false;
  }
  for (int i = changed + 1; i < tt->size(); ++i) {
    pt[i].mkVar(env);
    pt[i].mkOpt(env);
  }
  if (dim() > 0) {
    arrayEnumIds[arrayEnumIds.size() - 1] = env.registerTupleType(pt);
    typeId(env.registerArrayEnum(arrayEnumIds));
  } else {
    typeId(env.registerTupleType(pt));
  }
  return true;
}

Type Type::elemType(EnvI& env) const {
  Type elemTy = *this;
  if (elemTy.typeId() == 0 || dim() == 0) {
    elemTy.dim(0);
    return elemTy;
  }
  const std::vector<unsigned int>& arrayEnumIds = env.getArrayEnum(typeId());
  elemTy.typeId(0);
  elemTy.dim(0);
  elemTy.typeId(arrayEnumIds[arrayEnumIds.size() - 1]);
  return elemTy;
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
      oss << "tuple(";
      TupleType* tt = env.getTupleType(*this);
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

std::string Type::simpleToString() const {
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

}  // namespace MiniZinc
