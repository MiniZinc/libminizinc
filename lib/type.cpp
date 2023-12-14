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
      if (env.getTupleType(t0)->isSubtypeOf(env, *env.getTupleType(t1), strictEnums)) {
        return true;
      }
      return false;
    }
    if (t0.bt() == BT_RECORD) {
      if (t0.typeId() == t1.typeId()) {
        return true;
      }
      if (env.getRecordType(t0)->isSubtypeOf(env, *env.getRecordType(t1), strictEnums)) {
        return true;
      }
      return false;
    }
    if (!strictEnums || t0.dim() != 0 || (t0.typeId() == t1.typeId() || t1.typeId() == 0)) {
      return true;
    }
  }
  switch (t0.bt()) {
    case BT_BOOL:
      return (t1.bt() == Type::BT_INT || t1.bt() == BT_FLOAT);
    case BT_INT:
      return t1.bt() == BT_FLOAT;
    case BT_BOT:
      return true;
    default:
      return false;
  }
}

void Type::mkPar(EnvI& env) {
  if (!structBT()) {
    ti(TI_PAR);
    cv(false);
    return;
  }
  if (!cv()) {
    return;
  }
  std::vector<unsigned int> arrayEnumIds;
  unsigned int tId = typeId();
  if (dim() != 0) {
    arrayEnumIds = env.getArrayEnum(tId);
    tId = arrayEnumIds[arrayEnumIds.size() - 1];
  }
  StructType* st = env.getStructType(tId, bt());
  std::vector<Type> pt(st->size());
  for (int i = 0; i < st->size(); ++i) {
    pt[i] = (*st)[i];
    if (pt[i].structBT()) {
      pt[i].mkPar(env);
    } else {
      pt[i].ti(TI_PAR);
      pt[i].cv(false);
    }
  }
  typeId(0);
  cv(false);
  ti(TI_PAR);
  unsigned int regId = bt() == BT_TUPLE ? env.registerTupleType(pt)
                                        : env.registerRecordType(static_cast<RecordType*>(st), pt);
  if (dim() != 0) {
    arrayEnumIds[arrayEnumIds.size() - 1] = regId;
    typeId(env.registerArrayEnum(arrayEnumIds));
  } else {
    typeId(regId);
  }
}

void Type::mkVar(EnvI& env) {
  if (!structBT()) {
    ti(TI_VAR);
    return;
  }
  if (ti() == TI_VAR) {
    return;
  }
  std::vector<unsigned int> arrayEnumIds;
  unsigned int tId = typeId();
  if (dim() != 0) {
    arrayEnumIds = env.getArrayEnum(tId);
    tId = arrayEnumIds[arrayEnumIds.size() - 1];
  }
  StructType* st = env.getStructType(tId, bt());
  std::vector<Type> pt(st->size());
  for (int i = 0; i < st->size(); ++i) {
    pt[i] = (*st)[i];
    if (pt[i].structBT()) {
      pt[i].mkVar(env);
    } else {
      pt[i].ti(TI_VAR);
    }
  }
  typeId(0);
  cv(true);
  ti(TI_VAR);
  unsigned int regId = bt() == BT_TUPLE ? env.registerTupleType(pt)
                                        : env.registerRecordType(static_cast<RecordType*>(st), pt);
  if (dim() != 0) {
    arrayEnumIds[arrayEnumIds.size() - 1] = regId;
    typeId(env.registerArrayEnum(arrayEnumIds));
  } else {
    typeId(regId);
  }
}

void Type::mkOpt(EnvI& env) {
  assert(st() == Type::ST_PLAIN);
  if (!structBT()) {
    ot(OT_OPTIONAL);
    return;
  }
  std::vector<unsigned int> arrayEnumIds;
  unsigned int tId = typeId();
  if (dim() != 0) {
    arrayEnumIds = env.getArrayEnum(tId);
    tId = arrayEnumIds[arrayEnumIds.size() - 1];
  }
  StructType* strt = env.getStructType(tId, bt());
  std::vector<Type> pt(strt->size());
  bool changed = false;
  for (int i = 0; i < strt->size(); ++i) {
    pt[i] = (*strt)[i];
    if (pt[i].structBT()) {
      pt[i].mkOpt(env);
      changed = changed || (*strt)[i].typeId() != pt[i].typeId();
    } else if (st() == Type::ST_PLAIN) {
      changed = changed || pt[i].ot() != OT_OPTIONAL;
      pt[i].ot(OT_OPTIONAL);
    }
  }
  if (changed) {
    unsigned int regId = bt() == BT_TUPLE
                             ? env.registerTupleType(pt)
                             : env.registerRecordType(static_cast<RecordType*>(strt), pt);
    if (dim() != 0) {
      arrayEnumIds[arrayEnumIds.size() - 1] = regId;
      typeId(env.registerArrayEnum(arrayEnumIds));
    } else {
      typeId(regId);
    }
  }
}

void Type::mkPresent(EnvI& env) {
  if (!structBT()) {
    ot(OT_PRESENT);
    return;
  }
  std::vector<unsigned int> arrayEnumIds;
  unsigned int tId = typeId();
  if (dim() != 0) {
    arrayEnumIds = env.getArrayEnum(tId);
    tId = arrayEnumIds[arrayEnumIds.size() - 1];
  }
  StructType* st = env.getStructType(tId, bt());
  std::vector<Type> pt(st->size());
  bool changed = false;
  for (int i = 0; i < st->size(); ++i) {
    pt[i] = (*st)[i];
    if (pt[i].structBT()) {
      pt[i].mkOpt(env);
      changed = changed || (*st)[i].typeId() != pt[i].typeId();
    } else {
      changed = changed || pt[i].ot() != OT_PRESENT;
      pt[i].ot(OT_PRESENT);
    }
  }
  if (changed) {
    unsigned int regId = bt() == BT_TUPLE
                             ? env.registerTupleType(pt)
                             : env.registerRecordType(static_cast<RecordType*>(st), pt);
    if (dim() != 0) {
      arrayEnumIds[arrayEnumIds.size() - 1] = regId;
      typeId(env.registerArrayEnum(arrayEnumIds));
    } else {
      typeId(regId);
    }
  }
}

bool Type::decrement(EnvI& env) {
  if (!structBT()) {
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
  unsigned int tId = typeId();
  if (dim() != 0) {
    arrayEnumIds = env.getArrayEnum(tId);
    tId = arrayEnumIds[arrayEnumIds.size() - 1];
  }
  StructType* st = env.getStructType(tId, bt());

  // Copy types
  std::vector<Type> pt(st->size());
  for (int i = 0; i < st->size(); ++i) {
    pt[i] = (*st)[i];
  }
  int changed = static_cast<int>(st->size()) - 1;
  for (; changed >= 0; --changed) {
    if (pt[changed].decrement(env)) {
      break;
    }
  }
  if (changed < 0) {
    return false;
  }
  for (int i = changed + 1; i < st->size(); ++i) {
    pt[i].mkVar(env);
    if (pt[i].st() == Type::ST_PLAIN) {
      pt[i].mkOpt(env);
    }
  }
  unsigned int regId = bt() == BT_TUPLE ? env.registerTupleType(pt)
                                        : env.registerRecordType(static_cast<RecordType*>(st), pt);
  bool all_var = true;
  bool any_var = false;
  for (auto& t : pt) {
    all_var = all_var && t.ti() == TI_VAR;
    any_var = any_var || t.cv();
  }
  typeId(0);
  cv(any_var);
  ti(all_var ? TI_VAR : TI_PAR);
  if (dim() != 0) {
    arrayEnumIds[arrayEnumIds.size() - 1] = regId;
    typeId(env.registerArrayEnum(arrayEnumIds));
  } else {
    typeId(regId);
  }
  return true;
}

Type Type::elemType(const EnvI& env) const {
  Type elemTy = *this;
  if (dim() == 0) {
    return elemTy;
  }
  elemTy.typeId(0);
  elemTy.dim(0);
  if (typeId() == 0) {
    return elemTy;
  }
  const std::vector<unsigned int>& arrayEnumIds = env.getArrayEnum(typeId());
  elemTy.typeId(arrayEnumIds[arrayEnumIds.size() - 1]);
  return elemTy;
}

Type Type::arrType(EnvI& env, const Type& dimTy, const Type& elemTy) {
  assert(dimTy.dim() != 0);
  Type ret = elemTy;
  ret.typeId(0);
  ret.dim(dimTy.dim());
  if (dimTy.typeId() == 0) {
    if (elemTy.typeId() == 0) {
      return ret;
    }
    if (dimTy.dim() == elemTy.dim()) {
      return elemTy;
    }
  }
  std::vector<unsigned int> arrayEnumIds = dimTy.typeId() != 0
                                               ? env.getArrayEnum(dimTy.typeId())
                                               : std::vector<unsigned int>(dimTy.dim() + 1, 0);
  unsigned int elemTypeId = elemTy.typeId();
  if (elemTy.dim() != 0 && elemTypeId != 0) {
    const std::vector<unsigned int>& elemArrayIds = env.getArrayEnum(elemTypeId);
    elemTypeId = elemArrayIds.back();
  }
  arrayEnumIds.back() = elemTypeId;
  ret.typeId(env.registerArrayEnum(arrayEnumIds));
  return ret;
}

std::string Type::toString(const EnvI& env) const {
  std::ostringstream oss;
  if (dim() > 0) {
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
      for (int i = 0; i < dim(); i++) {
        oss << (i == 0 ? "" : ",") << "int";
      }
    }
    oss << "] of ";
  }
  if (dim() < 0) {
    oss << "array[$_] of ";
  }
  switch (static_cast<Inst>(_ti)) {
    case TI_PAR:
      if (static_cast<ExplicitType>(_tiExplicit) == EXPL_YES) {
        oss << "par ";
      }
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
      if (_typeId != 0U && dim() > 0) {
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
      if (typeId() == COMP_INDEX) {
        oss << "???";
      } else {
        TupleType* tt = env.getTupleType(*this);
        for (size_t i = 0; i < tt->size(); ++i) {
          oss << (*tt)[i].toString(env);
          if (i < tt->size() - 1) {
            oss << ", ";
          }
        }
      }
      oss << ")";
    } break;
    case BT_RECORD: {
      oss << "record(";
      RecordType* rt = env.getRecordType(*this);
      for (size_t i = 0; i < rt->size(); ++i) {
        oss << (*rt)[i].toString(env) << ": " << rt->fieldName(i);
        if (i < rt->size() - 1) {
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
  if (dim() > 0) {
    oss << "array[int";
    for (int i = 1; i < dim(); i++) {
      oss << ",int";
    }
    oss << "] of ";
  }
  if (dim() < 0) {
    oss << "array[$_] of ";
  }
  switch (static_cast<Inst>(_ti)) {
    case TI_PAR:
      if (static_cast<ExplicitType>(_tiExplicit) == EXPL_YES) {
        oss << "par ";
      }
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
      oss << "tuple(\?\?\?)";
      break;
    case BT_RECORD:
      oss << "record(\?\?\?)";
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
