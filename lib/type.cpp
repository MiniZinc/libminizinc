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

std::string Type::toString(EnvI& env) const {
  std::ostringstream oss;
  if (_dim > 0) {
    oss << "array[";
    if (_enumId != 0U) {
      const std::vector<unsigned int>& arrayEnumIds = env.getArrayEnum(_enumId);
      for (unsigned int i = 0; i < arrayEnumIds.size() - 1; i++) {
        if (i != 0) {
          oss << ",";
        }
        unsigned int enumId = arrayEnumIds[i];
        if (enumId == 0) {
          oss << "int";
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
      if (_enumId != 0U && _dim > 0) {
        const std::vector<unsigned int>& arrayEnumIds = env.getArrayEnum(_enumId);
        enumId = arrayEnumIds[arrayEnumIds.size() - 1];
      } else {
        enumId = _enumId;
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
  switch (static_cast<TypeInst>(_ti)) {
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
