/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/exception.hh>
#include <minizinc/flattener.hh>
#include <minizinc/solver_config.hh>
#include <minizinc/solver_instance_base.hh>

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace MiniZinc {

class SolverInitialiser {
public:
  SolverInitialiser();
};

class SolverFactory;

/// SolverRegistry is a storage for all SolverFactories in linked modules
class SolverRegistry {
public:
  void addSolverFactory(SolverFactory* sf);
  void removeSolverFactory(SolverFactory* sf);
  typedef std::vector<SolverFactory*> SFStorage;
  const SFStorage& getSolverFactories() const { return _sfstorage; }
  typedef std::vector<std::pair<std::string, SolverFactory*>> FactoryFlagStorage;
  void addFactoryFlag(const std::string& flag, SolverFactory* sf);
  void removeFactoryFlag(const std::string& flag, SolverFactory* sf);
  const FactoryFlagStorage& getFactoryFlags() const { return _factoryFlagStorage; }

private:
  SFStorage _sfstorage;
  FactoryFlagStorage _factoryFlagStorage;
};  // SolverRegistry

/// this function returns the global SolverRegistry object
SolverRegistry* get_global_solver_registry();

/// Representation of flags that can be passed to solvers
class MZNFZNSolverFlag {
public:
  /// The type of the solver flag
  enum FlagType { FT_ARG, FT_NOARG } t;
  /// The name of the solver flag
  std::string n;

protected:
  MZNFZNSolverFlag(const FlagType& t0, std::string n0) : t(t0), n(std::move(n0)) {}

public:
  /// Create flag that has an argument
  static MZNFZNSolverFlag arg(const std::string& n) { return MZNFZNSolverFlag(FT_ARG, n); }
  /// Create flag that has no argument
  static MZNFZNSolverFlag noarg(const std::string& n) { return MZNFZNSolverFlag(FT_NOARG, n); }
  /// Create solver flag from standard flag
  static MZNFZNSolverFlag std(const std::string& n);
  /// Create solver flag from extra flag
  static MZNFZNSolverFlag extra(const SolverConfig::ExtraFlag& ef);
};

/// SolverFactory's descendants create, store and destroy SolverInstances
/// A SolverFactory stores all Instances itself and upon module exit,
/// destroys them and de-registers itself from the global SolverRegistry
/// An instance of SolverFactory's descendant can be created directly
/// or by one of the specialized createF_...() functions
class SolverFactory {
protected:
  /// doCreateSI should be implemented to actually allocate a SolverInstance using new()
  virtual SolverInstanceBase* doCreateSI(Env&, std::ostream&, SolverInstanceBase::Options* opt) = 0;
  typedef std::vector<std::unique_ptr<SolverInstanceBase>> SIStorage;
  SIStorage _sistorage;

  SolverFactory() { get_global_solver_registry()->addSolverFactory(this); }

public:
  virtual ~SolverFactory() {
    try {
      get_global_solver_registry()->removeSolverFactory(this);
    } catch (std::exception&) {
      assert(false);  // Assert that the solver registry can be obtained and the solver factory
                      // safely removed
    }
  }

  /// Processes a previously registered factory flag.
  virtual bool processFactoryOption(int& i, std::vector<std::string>& argv,
                                    const std::string& workingDir = std::string()) {
    return false;
  };
  /// Called after any registered factory flags have been processed.
  virtual void factoryOptionsFinished(){};

  /// Create solver-specific options object
  virtual SolverInstanceBase::Options* createOptions() = 0;
  /// Function createSI also adds each SI to the local storage
  SolverInstanceBase* createSI(Env& env, std::ostream& log, SolverInstanceBase::Options* opt);
  /// also providing a manual destroy function.
  /// there is no need to call it upon overall finish - that is taken care of
  void destroySI(SolverInstanceBase* pSI);

  /// Process an item in the command line.
  /// Leaving this now like this because this seems simpler.
  /// We can also pass options internally between modules in this way
  /// and it only needs 1 format
  virtual bool processOption(SolverInstanceBase::Options* opt, int& i,
                             std::vector<std::string>& argv,
                             const std::string& workingDir = std::string()) {
    return false;
  }

  virtual std::string getDescription(SolverInstanceBase::Options* opt = nullptr) = 0;
  virtual std::string getVersion(SolverInstanceBase::Options* opt = nullptr) = 0;
  virtual std::string getId() = 0;
  virtual void printHelp(std::ostream& /*os*/) {}
};  // SolverFactory

// Class MznSolver coordinates flattening and solving.
class MznSolver {
private:
  SolverInitialiser _solverInit;
  enum OptionStatus { OPTION_OK, OPTION_ERROR, OPTION_FINISH };
  /// Solver configurations
  SolverConfigs _solverConfigs;
  Flattener _flt;
  SolverInstanceBase* _si = nullptr;
  SolverInstanceBase::Options* _siOpt = nullptr;
  SolverFactory* _sf = nullptr;
  bool _isMzn2fzn = false;

  std::string _executableName;
  std::ostream& _os;
  std::ostream& _log;

  // These have special handling here as the stdFlags they correspond to
  // depend on the method and whether the solver supports the flag
  bool _supportsA = false;
  bool _supportsI = false;
  bool _flagAllSatisfaction = false;
  bool _flagIntermediate = false;

public:
  Solns2Out s2out;

  /// global options
  bool flagVerbose = false;
  bool flagStatistics = false;
  bool flagCompilerVerbose = false;
  bool flagCompilerStatistics = false;
  bool flagIsSolns2out = false;
  int flagOverallTimeLimit = 0;

  MznSolver(std::ostream& os = std::cout, std::ostream& log = std::cerr);
  ~MznSolver();

  SolverInstance::Status run(const std::vector<std::string>& args,
                             const std::string& model = std::string(),
                             const std::string& exeName = std::string("minizinc"),
                             const std::string& modelName = std::string("stdin"));
  OptionStatus processOptions(std::vector<std::string>& argv);
  SolverFactory* getSF() {
    assert(_sf);
    return _sf;
  }
  SolverInstanceBase::Options* getSIOptions() {
    assert(_siOpt);
    return _siOpt;
  }
  bool getFlagVerbose() const { return flagVerbose; /*getFlt()->getFlagVerbose();*/ }
  void printUsage();

private:
  void printHelp(const std::string& selectedSolver = std::string());
  /// Flatten model
  void flatten(const std::string& modelString = std::string(),
               const std::string& modelName = std::string("stdin"));
  static size_t getNSolvers() { return get_global_solver_registry()->getSolverFactories().size(); }
  /// If building a flattening exe only.
  bool ifMzn2Fzn() const;
  bool ifSolns2out() const;
  void addSolverInterface();
  void addSolverInterface(SolverFactory* sf);
  SolverInstance::Status solve();

  SolverInstance::Status getFltStatus() const { return _flt.status; }
  SolverInstanceBase* getSI() {
    assert(_si);
    return _si;
  }
  bool getFlagStatistics() const { return flagStatistics; }
};

}  // namespace MiniZinc
