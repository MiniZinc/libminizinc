/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jason Nguyen <jason.nguyen@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/json_parser.hh>
#include <minizinc/param_config.hh>

#include <sstream>

namespace MiniZinc {

void ParamConfig::load(const std::string& filename) {
  if (JSONParser::fileIsJSON(filename)) {
    try {
      Env confenv;
      JSONParser jp(confenv.envi());
      Model m;
      GCLock lock;
      jp.parse(&m, filename);
      for (auto& i : m) {
        if (auto* ai = i->dynamicCast<AssignI>()) {
          addValue(ai->id(), ai->e());
        }
      }
    } catch (ParamException& e) {
      throw;
    } catch (Exception& e) {
      throw ParamException(e.what());
    }
  } else {
    throw ParamException("Invalid configuration file");
  }
}

void ParamConfig::addValue(const ASTString& flag_input, Expression* e) {
  std::stringstream flag_ss;
  if (!flag_input.beginsWith("-")) {
    flag_ss << "--";
  }
  flag_ss << flag_input;
  auto flag = flag_ss.str();

  if (_blacklist.count(flag) > 0) {
    throw ParamException("Parameter '" + flag + "' is not allowed in configuration file");
  }

  std::stringstream val_ss;
  switch (e->eid()) {
    case Expression::E_ARRAYLIT: {
      auto* al = e->cast<ArrayLit>();
      for (auto* exp : al->getVec()) {
        addValue(flag, exp);
      }
      break;
    }
    case Expression::E_BOOLLIT: {
      if (e->cast<BoolLit>()->v()) {
        _values.push_back(flag);
      } else {
        // If this flag has a negated version, use it
        auto it = _boolSwitches.find(flag);
        if (it != _boolSwitches.end()) {
          _values.push_back(it->second);
        }
      }
      break;
    }
    case Expression::E_STRINGLIT:
      _values.push_back(flag);
      val_ss << e->cast<StringLit>()->v();
      _values.push_back(val_ss.str());
      break;
    case Expression::E_INTLIT:
      _values.push_back(flag);
      val_ss << e->cast<IntLit>()->v();
      _values.push_back(val_ss.str());
      break;
    case Expression::E_FLOATLIT:
      _values.push_back(flag);
      val_ss << e->cast<FloatLit>()->v();
      _values.push_back(val_ss.str());
      break;
    default:
      throw ParamException("Unsupported parameter type for '" + flag + "'");
  }
}

const std::vector<std::string>& ParamConfig::argv() { return _values; }

void ParamConfig::blacklist(const std::string& disallowed) { _blacklist.insert(disallowed); }

void ParamConfig::blacklist(const std::vector<std::string>& disallowed) {
  for (const auto& param : disallowed) {
    _blacklist.insert(param);
  }
}

void ParamConfig::negatedFlag(const std::string& flag, const std::string& negated) {
  _boolSwitches.insert(std::make_pair(flag, negated));
}

}  // namespace MiniZinc
