/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_SOLVER_HH__
#define __MINIZINC_SOLVER_HH__

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <minizinc/flattener.hh>
#include <minizinc/solver_instance_base.hh>
#include <minizinc/exception.hh>

namespace MiniZinc {
  
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
  
  /// SolverFactory's descendants create, store and destroy SolverInstances
  /// A SolverFactory stores all Instances itself and upon module exit,
  /// destroys them and de-registers itself from the global SolverRegistry
  /// An instance of SolverFactory's descendant can be created directly
  /// or by one of the specialized createF_...() functions
  class SolverFactory {
  public:
    /// Specialized factory creation funcs, defined in the
    /// solver instance modules
    static SolverFactory* createF_FZN();
    static SolverFactory* createF_GECODE();
    static SolverFactory* createF_MIP();
    static SolverFactory* createF_CHUFFED();
  protected:
    /// doCreateSI should be implemented to actually allocate a SolverInstance using new()
    virtual SolverInstanceBase * doCreateSI(Env&, std::ostream&) = 0;
    
    typedef std::vector<std::unique_ptr<SolverInstanceBase> > SIStorage;
    SIStorage sistorage;
  protected:
    SolverFactory()            { getGlobalSolverRegistry()->addSolverFactory(this); }
  public:
    virtual ~SolverFactory()   { getGlobalSolverRegistry()->removeSolverFactory(this); }
    
  public:
    /// Function createSI also adds each SI to the local storage
    SolverInstanceBase * createSI(Env& env, std::ostream& log);
    /// also providing a manual destroy function.
    /// there is no need to call it upon overall finish - that is taken care of
    void destroySI(SolverInstanceBase * pSI);
    
  public:
    /// Process an item in the command line.
    /// Leaving this now like this because this seems simpler.
    /// We can also pass options internally between modules in this way
    /// and it only needs 1 format
    virtual bool processOption(int& i, int argc, const char** argv) { return false; }

    virtual std::string getVersion( ) { return "Abstract solver v0.-1"; }
    virtual void printHelp(std::ostream& ) { }
  };  // SolverFactory
  
  // Class MznSolver coordinates flattening and solving.
  class MznSolver {
  private:
    Flattener flt;
    SolverInstanceBase* si=0;
    bool is_mzn2fzn;
    std::string executable_name;
    std::ostream& os;
    std::ostream& log;
  public:
    Solns2Out s2out;
    
    /// global options
    bool flag_verbose=0;
    bool flag_statistics=0;
    
    /// solver options, not used    TODO
    Options options_solver;          // currently can create solver object only after flattening
                                     // so unflexible with solver cmdline options  TODO
  public:
    MznSolver(std::ostream& os = std::cout, std::ostream& log = std::cerr, bool ism2f = false);
    ~MznSolver();
    bool processOptions(int argc, const char** argv);
    void printHelp();
    /// Flatten model
    void flatten(const std::string& modelString = std::string());
    size_t getNSolvers() { return getGlobalSolverRegistry()->getSolverFactories().size(); }
    /// If building a flattening exe only.
    bool ifMzn2Fzn();
    void addSolverInterface();
    void solve();
    void printStatistics();
    
    SolverInstance::Status getFltStatus() { return flt.status; }
    SolverInstanceBase* getSI() { assert(si); return si; }
    bool get_flag_verbose() { return flag_verbose; /*getFlt()->get_flag_verbose();*/ }
    bool get_flag_statistics() { return flag_statistics; }
    
  };

}

#endif
