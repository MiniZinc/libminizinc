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
#include <minizinc/prettyprinter.hh>

#include <sstream>

namespace MiniZinc {

void ParamConfig::load(const std::string& filename) {
  try {
    Env confenv;
    JSONParser jp(confenv.envi());
    Model m;
    GCLock lock;
    jp.parse(&m, filename, false);
    for (auto& i : m) {
      if (auto* ai = i->dynamicCast<AssignI>()) {
        addValue(ai->id(), ai->e());
      } else if (auto* ii = i->dynamicCast<IncludeI>()) {
        auto flag = ParamConfig::flagName(ii->f());
        if (_blacklist.count(flag) > 0) {
          throw ParamException("Parameter '" + flag + "' is not allowed in configuration file");
        }
        _values.push_back(flag);
        _values.push_back(ParamConfig::modelToString(*(ii->m())));
      }
    }
  } catch (ParamException&) {
    throw;
  } catch (JSONError& e) {
    throw ParamException(e.msg());
  }
}

void ParamConfig::addValue(const ASTString& flag_input, Expression* e) {
  auto flag = ParamConfig::flagName(flag_input);
  if (_blacklist.count(flag) > 0) {
    throw ParamException("Parameter '" + flag + "' is not allowed in configuration file");
  }
  std::stringstream val_ss;
  switch (Expression::eid(e)) {
    case Expression::E_ARRAYLIT: {
      auto* al = Expression::cast<ArrayLit>(e);
      for (auto* exp : al->getVec()) {
        addValue(ASTString(flag), exp);
      }
      break;
    }
    case Expression::E_BOOLLIT: {
      if (Expression::cast<BoolLit>(e)->v()) {
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
      val_ss << Expression::cast<StringLit>(e)->v();
      _values.push_back(val_ss.str());
      break;
    case Expression::E_INTLIT:
      _values.push_back(flag);
      val_ss << IntLit::v(Expression::cast<IntLit>(e));
      _values.push_back(val_ss.str());
      break;
    case Expression::E_FLOATLIT:
      _values.push_back(flag);
      val_ss << FloatLit::v(Expression::cast<FloatLit>(e));
      _values.push_back(val_ss.str());
      break;
      break;
    default:
      throw ParamException("Unsupported parameter type for '" + flag + "'");
  }
}

std::string ParamConfig::flagName(const ASTString& flag_input) {
  std::stringstream flag_ss;
  if (!flag_input.beginsWith("-")) {
    flag_ss << "--";
  }
  flag_ss << flag_input;
  return flag_ss.str();
}

std::string ParamConfig::modelToString(Model& model) {
  std::stringstream ss;
  for (auto& i : model) {
    if (auto* ai = i->dynamicCast<AssignI>()) {
      auto flag = ParamConfig::flagName(ai->id());
      auto* e = ai->e();
      switch (Expression::eid(e)) {
        case Expression::E_ARRAYLIT: {
          auto* al = Expression::cast<ArrayLit>(e);
          for (auto* exp : al->getVec()) {
            ss << " " << flag;
            ss << " " << exp;
          }
          break;
        }
        case Expression::E_BOOLLIT:
          if (Expression::cast<BoolLit>(e)->v()) {
            ss << " " << flag;
          }
          break;
        case Expression::E_STRINGLIT:
          ss << " " << flag;
          ss << " " << Expression::cast<StringLit>(ai->e())->v();
          break;
        case Expression::E_INTLIT:
          ss << " " << flag;
          ss << " " << IntLit::v(Expression::cast<IntLit>(ai->e()));
          break;
        case Expression::E_FLOATLIT:
          ss << " " << flag;
          ss << " " << FloatLit::v(Expression::cast<FloatLit>(ai->e()));
          break;
        default:
          throw ParamException("Unsupported parameter type for '" + flag + "'");
      }
    } else if (auto* ii = i->dynamicCast<IncludeI>()) {
      ss << " " << ParamConfig::flagName(ii->f());
      ss << " \"" << Printer::escapeStringLit(modelToString(*(ii->m()))) << "\"";
    }
  }
  return ss.str().substr(1);
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
