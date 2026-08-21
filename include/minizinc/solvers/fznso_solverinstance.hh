/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/flattener.hh>
#include <minizinc/solver.hh>

namespace MiniZinc {

/// Drives a solver behind the FZnSO interface: a shared library named by the
/// solver configuration, loaded at run time and run directly on the flat model.
///
/// A solver declares its own options, constraints, decision-variable types and
/// statistics across that interface, so this factory needs none of them spelled
/// out in the configuration file: the command line flags are derived from the
/// declared options (\a finaliseSolverConfigs) and the MiniZinc library
/// features from the declared constraints and decision types
/// (\a configureFlattener).
///
/// The FZnSO headers are deliberately kept out of this interface, so that the
/// rest of libmzn does not depend on them.
class FZNSOSolverFactory : public SolverFactory {
protected:
  SolverInstanceBase* doCreateSI(Env& env, std::ostream& log,
                                 SolverInstanceBase::Options* opt) override;

public:
  FZNSOSolverFactory();
  SolverInstanceBase::Options* createOptions() override;
  std::string getDescription(SolverInstanceBase::Options* opt = nullptr) override;
  std::string getVersion(SolverInstanceBase::Options* opt = nullptr) override;
  std::string getId() override;
  bool processOption(SolverInstanceBase::Options* opt, int& i, std::vector<std::string>& argv,
                     const std::string& workingDir = std::string()) override;
  void printHelp(std::ostream& os) override;
  /// Load the library of every FZnSO solver configuration and fill in the
  /// standard and extra command line flags its declared options provide.
  void finaliseSolverConfigs(SolverConfigs& solverConfigs) override;

  /// Select the MiniZinc library features the loaded solver's declared
  /// capabilities call for: the globals it implements natively, and the set
  /// variable decomposition if it has no set variables.
  ///
  /// The FlatZinc builtin layer is not touched: a solver says which builtins it
  /// supports, and supplies a decomposition for the ones it does not, through
  /// the standard `redefinitions*.mzn` mechanism.
  static void configureFlattener(SolverInstanceBase::Options* opt, Flattener& flt);
};

}  // namespace MiniZinc
