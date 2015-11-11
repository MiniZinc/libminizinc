/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_SOLVER_INSTANCE_BASE_HH__
#define __MINIZINC_SOLVER_INSTANCE_BASE_HH__

#include <iostream>

#include <minizinc/model.hh>
#include <minizinc/flatten.hh>
#include <minizinc/hash.hh>
#include <minizinc/options.hh>
#include <minizinc/statistics.hh>
#include <minizinc/solver_instance.hh>

namespace MiniZinc {

  class SolverInstanceBase {
//     Env* pEnv = 0;
  protected:
    Env& _env;
    Options _options;
    
    typedef void (*poster) (SolverInstanceBase&, const Call* call);

    class Registry {
    protected:
      ASTStringMap<poster>::t _registry;
      SolverInstanceBase& _base;
    public:
      Registry(SolverInstanceBase& base) : _base(base) {}
      void add(const ASTString& name, poster p);
      void post(Call* c);      
      void cleanup() { _registry.clear(); }
    };
    
    Registry _constraintRegistry;
    
    virtual Expression* getSolutionValue(Id* id) = 0;

    /// Assign output for all vars:
    virtual void assignSolutionToOutput(void);
    
  public:
    typedef SolverInstance::Status Status;
    typedef SolverInstance::StatusReason StatusReason;
    Status _status;
    StatusReason _status_reason;
    SolverInstanceBase(Env& env, const Options& options) : _env(env), _options(options), _constraintRegistry(*this), _status(SolverInstance::UNKNOWN), _status_reason(SolverInstance::SR_OK) {}
    
    /// Probably should not be overridden above if using cleanup() again?
    /// Dangerous in C++. TODO
    virtual ~SolverInstanceBase(void) { /*cleanup();*/ }
    
    /// Set/get the environment:
    virtual Env* getEnv() const { assert(&_env); return &_env; }
    virtual Env& env(void) const { return *getEnv(); }
    
    virtual void setOptions(Options& o) { _options = o; }
    virtual Options& getOptions() { return _options; }

    virtual void printSolution(std::ostream& ) { }
    
    virtual void printStatistics(std::ostream&, bool fLegend=0) { }
    virtual void printStatisticsLine(std::ostream&, bool fLegend=0) { }

    /// find the next solution
    virtual Status next(void) = 0;
    /// generate the solver-instance-representation from the flatzinc model
    virtual void processFlatZinc(void) = 0;
    /// solve the problem instance (according to the solve specification in the flatzinc model)
    virtual Status solve(void);
    /// return reason for status given by solve
    virtual StatusReason reason(void) {return _status_reason;}
    virtual Status status(void) {return _status;}
    

    /// reset the model to its core (removing temporary cts) and the solver to the root node of the search 
    void reset(void);
    /// reset the solver to the root node of the search TODO: difference between reset() and resetSolver()?
    virtual void resetSolver(void) = 0;
    /// reset the solver and add temporary constraints given by the iterator
    virtual void resetWithConstraints(Model::iterator begin, Model::iterator end);
    /// add permanent constraints given by the iterator to the solver instance
    virtual void processPermanentConstraints(Model::iterator begin, Model::iterator end);
    /// clean up solver completely, e.g., before taking another problem environment
    /// derived classes should call their ancestor's cleanup after theirs
    /// Not implementing for now - only checking that env is set at most once. TODO
//     virtual void cleanup() { cleanupBase(); }
//     virtual void cleanupBase() {
//       _constraintRegistry.cleanup();
//       pEnv = 0;
//     }
  protected:
    /// flatten the search annotations, pushing them into the vector \a out
    void flattenSearchAnnotations(const Annotation& ann, std::vector<Expression*>& out);    

  private:
    SolverInstanceBase(const SolverInstanceBase&);
    SolverInstanceBase& operator= (const SolverInstanceBase&);
  };

  
  template<class Solver>
  class SolverInstanceImpl : public SolverInstanceBase {
  public:
    typedef typename Solver::Variable VarId;

  protected:
    typename Solver::Statistics _statistics;
    
    IdMap<VarId> _variableMap;

  public:
    SolverInstanceImpl(Env& env, const Options& options=Options()) : SolverInstanceBase(env, options) {}
    
//     void cleanup() { _statistics.cleanup(); _variableMap.clear(); SolverInstanceBase::cleanup(); }
    
    Statistics& getStatistics() { return _statistics; }
    // Could implement the ancestor's print functions:                 TODO?
//     virtual void printStatistics(std::ostream&, bool fLegend=0) { }
//     virtual void printStatisticsLine(std::ostream&, bool fLegend=0) { }

  };

}

#endif
