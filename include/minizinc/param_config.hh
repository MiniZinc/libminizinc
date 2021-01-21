/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jason Nguyen <jason.nguyen@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/ast.hh>
#include <minizinc/solver_config.hh>

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace MiniZinc {
/// Configuration for solver parameters
class ParamConfig {
protected:
  std::vector<std::string> _values;
  std::unordered_set<std::string> _blacklist;
  std::unordered_map<std::string, std::string> _boolSwitches;
  void addValue(const ASTString& flag, Expression* e);
  static std::string flagName(const ASTString& flag);
  static std::string modelToString(Model& model);

public:
  ParamConfig() {}
  /// Load a configuration from a JSON file
  void load(const std::string& filename);
  /// Add given parameter to blacklist
  void blacklist(const std::string& disallowed);
  /// Add given parameters to blacklist
  void blacklist(const std::vector<std::string>& disallowed);
  /// Add boolean switch
  /// When the key is found in the config, then if it's false the negated flag is used
  void negatedFlag(const std::string& flag, const std::string& negated);
  /// Return the arguments represented by this configuration
  const std::vector<std::string>& argv();
};

class ParamException : public Exception {
public:
  /// Construct with message \a msg
  ParamException(const std::string& msg) : Exception(msg) {}
  /// Destructor
  ~ParamException() throw() override {}
  /// Return description
  const char* what() const throw() override { return "MiniZinc: solver parameter error"; }
};
}  // namespace MiniZinc
