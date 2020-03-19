/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <minizinc/flattener.hh>
#include <minizinc/solver_config.hh>
#include <minizinc/solver_instance_base.hh>
#include <minizinc/exception.hh>

namespace MiniZinc {
  
  class SolverInitialiser {
  public:
    SolverInitialiser(void);
  };
  
  class SolverFactory;
  
  /// SolverRegistry is a storage for all SolverFactories in linked modules
  class SolverRegistry {
  public:
    void addSolverFactory(SolverFactory * );
    void removeSolverFactory(SolverFactory * );
    typedef std::vector<SolverFactory*> SFStorage;
    const SFStorage& getSolverFactories() const { return sfstorage; }
  private:
    SFStorage sfstorage;
  };  // SolverRegistry

  /// this function returns the global SolverRegistry object
  SolverRegistry* getGlobalSolverRegistry();

  /// Representation of flags that can be passed to solvers
  class MZNFZNSolverFlag {
  public:
    /// The type of the solver flag
    enum FlagType { FT_ARG, FT_NOARG } t;
    /// The name of the solver flag
    std::string n;
  protected:
    MZNFZNSolverFlag(const FlagType& t0, const std::string& n0) : t(t0), n(n0) {}
  public:
    /// Create flag that has an argument
    static MZNFZNSolverFlag arg(const std::string& n) {
      return MZNFZNSolverFlag(FT_ARG,n);
    }
    /// Create flag that has no argument
    static MZNFZNSolverFlag noarg(const std::string& n) {
      return MZNFZNSolverFlag(FT_NOARG,n);
    }
    /// Create solver flag from standard flag
    static MZNFZNSolverFlag std(const std::string& n);
    /// Create solver flag from extra flag with name \a n and type \a t
    static MZNFZNSolverFlag extra(const std::string& n, const std::string& t);
  };

  
  /// SolverFactory's descendants create, store and destroy SolverInstances
  /// A SolverFactory stores all Instances itself and upon module exit,
  /// destroys them and de-registers itself from the global SolverRegistry
  /// An instance of SolverFactory's descendant can be created directly
  /// or by one of the specialized createF_...() functions
  class SolverFactory {
  protected:
    /// doCreateSI should be implemented to actually allocate a SolverInstance using new()
    virtual SolverInstanceBase * doCreateSI(Env&, std::ostream&, SolverInstanceBase::Options* opt) = 0;
    typedef std::vector<std::unique_ptr<SolverInstanceBase> > SIStorage;
    SIStorage sistorage;
  protected:
    SolverFactory()            { getGlobalSolverRegistry()->addSolverFactory(this); }
  public:
    virtual ~SolverFactory()   { getGlobalSolverRegistry()->removeSolverFactory(this); }
    
  public:
    /// Create solver-specific options object
    virtual SolverInstanceBase::Options* createOptions(void) = 0;
    /// Function createSI also adds each SI to the local storage
    SolverInstanceBase * createSI(Env& env, std::ostream& log, SolverInstanceBase::Options* opt);
    /// also providing a manual destroy function.
    /// there is no need to call it upon overall finish - that is taken care of
    void destroySI(SolverInstanceBase * pSI);
    
  public:
    /// Process an item in the command line.
    /// Leaving this now like this because this seems simpler.
    /// We can also pass options internally between modules in this way
    /// and it only needs 1 format
    virtual bool processOption(SolverInstanceBase::Options* opt, int& i, std::vector<std::string>& argv) { return false; }

    virtual std::string getDescription(SolverInstanceBase::Options* opt=NULL) = 0;
    virtual std::string getVersion(SolverInstanceBase::Options* opt=NULL) = 0;
    virtual std::string getId(void) = 0;
    virtual void printHelp(std::ostream& ) { }
  };  // SolverFactory
  
  // Class MznSolver coordinates flattening and solving.
  class MznSolver {
  private:
    SolverInitialiser _solver_init;
    enum OptionStatus { OPTION_OK, OPTION_ERROR, OPTION_FINISH };
    /// Solver configurations
    SolverConfigs solver_configs;
    Flattener flt;
    SolverInstanceBase* si=0;
    SolverInstanceBase::Options* si_opt=0;
    SolverFactory* sf=0;
    bool is_mzn2fzn=0;

    std::string executable_name;
    std::ostream& os;
    std::ostream& log;

  public:
    Solns2Out s2out;
    
    /// global options
    bool flag_verbose=false;
    bool flag_statistics=false;
    bool flag_compiler_verbose=false;
    bool flag_compiler_statistics=false;
    bool flag_is_solns2out=false;
    int flag_overall_time_limit=0;

  public:
    MznSolver(std::ostream& os = std::cout, std::ostream& log = std::cerr);
    ~MznSolver();
    
    SolverInstance::Status run(const std::vector<std::string>& args, const std::string& model = std::string(),
                               const std::string& exeName = std::string("minizinc"),
                               const std::string& modelName = std::string("stdin"));
    OptionStatus processOptions(std::vector<std::string>& argv);
    SolverFactory* getSF() { assert(sf); return sf; }
    SolverInstanceBase::Options* getSI_OPT() { assert(si_opt); return si_opt; }
    bool get_flag_verbose() { return flag_verbose; /*getFlt()->get_flag_verbose();*/ }
    void printUsage();
    
  private:
    void printHelp(const std::string& selectedSolver=std::string());
    /// Flatten model
    void flatten(const std::string& modelString = std::string(), const std::string& modelName = std::string("stdin"));
    size_t getNSolvers() { return getGlobalSolverRegistry()->getSolverFactories().size(); }
    /// If building a flattening exe only.
    bool ifMzn2Fzn();
    bool ifSolns2out();
    void addSolverInterface();
    void addSolverInterface(SolverFactory* sf);
    SolverInstance::Status solve();
    
    SolverInstance::Status getFltStatus() { return flt.status; }
    SolverInstanceBase* getSI() { assert(si); return si; }
    bool get_flag_statistics() { return flag_statistics; }
    
  };

}
