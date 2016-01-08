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

using namespace std;

#include <minizinc/flattener.h>
#include <minizinc/solver_instance_base.hh>
#include <minizinc/exception.hh>

namespace MiniZinc {
  
  class SolverFactory;
  
  /// SolverRegistry is a storage for all SolverFactories in linked modules
  /// NOTE THAT the .cpp containing its SolverFactory should be linked directly [in g++ 4.9.2]
  class SolverRegistry {
  public:
    void addSolverFactory(SolverFactory * );
    void removeSolverFactory(SolverFactory * );
    typedef vector<SolverFactory*> SFStorage;
    const SFStorage& getSolverFactories() const { return sfstorage; }
  private:
    SFStorage sfstorage;
  };  // SolverRegistry

  /// this function returns the global SolverRegistry object
  SolverRegistry* getGlobalSolverRegistry();
  
  /// SolverFactory's descendants create, store and destroy SolverInstances
  /// A SolverFactory stores all Instances itself and upon module exit,
  /// destroys them and de-registers itself from the global SolverRegistry
  class SolverFactory {
  protected:
    /// doCreateSI should be implemented to actually allocate a SolverInstance using new()
    virtual SolverInstanceBase * doCreateSI(Env&) = 0;
    
    typedef vector<unique_ptr<SolverInstanceBase> > SIStorage;
    SIStorage sistorage;
//   public:
    SolverFactory()            { getGlobalSolverRegistry()->addSolverFactory(this); }
    virtual ~SolverFactory()   { getGlobalSolverRegistry()->removeSolverFactory(this); }
    
  public:
    /// Function createSI also adds each SI to the local storage
    SolverInstanceBase * createSI(Env& env);
    /// also providing a manual destroy function.
    /// there is no need to call it upon overall finish - that is taken care of
    void destroySI(SolverInstanceBase * pSI);
    
  public:
    /// Process an item in the command line.
    /// Leaving this now like this because this seems simpler.
    /// We can also pass options internally between modules in this way
    /// and it only needs 1 format
    virtual bool processOption(int& i, int argc, const char** argv) { return false; }

    virtual string getVersion( ) { return "Abstract solver v0.-1"; }
    virtual void printHelp(std::ostream& ) { }
  };  // SolverFactory
  
  // Class MznSolver coordinates flattening and solving.
  class MznSolver {
  private:
    Flattener* flt=0;
    SolverInstanceBase* si=0;
    
  public:
    /// global options
    bool flag_verbose=0;
    
    /// solver options, not used    TODO
    Options options_solver;          // currently can create solver object only after flattening
                                     // so unflexible with solver cmdline options  TODO
  public:
    virtual ~MznSolver();
    virtual void addFlattener();
    virtual bool processOptions(int argc, const char** argv);
    virtual void printHelp();
    virtual void flatten();
    virtual void addSolverInterface();
    virtual void solve();
    virtual void printStatistics();
    
    virtual Flattener* getFlt() { assert(flt); return flt; }
    virtual SolverInstanceBase* getSI() { assert(si); return si; }
    virtual bool get_flag_verbose() { return flag_verbose; /*getFlt()->get_flag_verbose();*/ }
    
  private:
  };

}

#endif
