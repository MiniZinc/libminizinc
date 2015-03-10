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

#include <minizinc/model.hh>
#include <minizinc/flatten.hh>
#include <minizinc/hash.hh>
#include <minizinc/options.hh>
#include <minizinc/statistics.hh>
#include <minizinc/solver_instance.hh>

namespace MiniZinc {

  /// abstract class for incremental solver interfaces (incremental := solvers that allow adding of variables/constraints during search)
  class SolverInstanceBase {
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
    };
    
    Registry _constraintRegistry;
    
    virtual Expression* getSolutionValue(Id* id) = 0;

    void assignSolutionToOutput(void); 
    
  public:
    typedef SolverInstance::Status Status;
    SolverInstanceBase(Env& env, const Options& options) : _env(env), _options(options), _constraintRegistry(*this) {}
    
    virtual ~SolverInstanceBase(void) {}
    /// find the next solution (when overwriting this method, make sure that assignSolutionToOutput is called after a new solution is found)
    virtual Status next(void) = 0;
    /// generate the solver-instance-representation from the flatzinc model
    virtual void processFlatZinc(void) = 0;
    /// solve the problem instance (according to the solve specification in the flatzinc model)
    virtual Status solve(void);
    /// update the bounds of the given variables to the new integer bounds during search (after next() has been called)
    virtual bool updateIntBounds(VarDecl* vd, int lb, int ub) = 0;
    /// post constraints during search (after next() has been called)
    virtual bool postConstraints(std::vector<Call*> cts) { return false; }    
    /// add variables during search (after next() has been called)
    virtual bool addVariables(std::vector<VarDecl*> vars) { return false; }    
    /// update the bounds of the given variables to the new integer bounds during search (after next() has been called)
    //bool updateFloatBounds(VarDecl* vd, float lb, float ub) { return false; }
    void setOptions(Options& o) { _options = o; }
    Options& getOptions() { return _options; }
    Env& env(void) { return _env; }
    
  private:
    SolverInstanceBase(const SolverInstanceBase&);
    SolverInstanceBase& operator= (const SolverInstanceBase&);
  };
  
  template<class Solver>
  class SolverInstanceImpl : public SolverInstanceBase {
  protected:
    typename Solver::Statistics _statistics;
    /// maps each identifier to the respective GecodeVariable in the _current_space
    IdMap<typename Solver::Variable> _variableMap;
  public:
    SolverInstanceImpl(Env& env, const Options& options) : SolverInstanceBase(env,options) {}
    Statistics& getStatistics() { return _statistics; }
  };
  
  
  
  
  /// class to inherit from, if the solver is not incremental (i.e. constraints cannot be added on the fly after calling next-solution)
  class SolverInstanceBaseNonIncremental : public SolverInstanceBase {
  public:
    bool postConstraints(std::vector<Call*> cts) { return false; }
  };
  
  /// abstract solver interface for non-incremental solvers (solvers that do NOT allow adding variables/constraints during search)
  class NISolverInstanceBase : public SolverInstanceBase {
  private:
    /// if true, there is a new solution whose nogoods need to be posted, false otherwise
    bool _new_solution; 
  protected:    
    // overwrite this function in your solver: build the solver representation of the flat model and find the first solution
    virtual Status nextSolution(void) = 0;
    /// posts the nogoods representing the previous solution 
    void postSolutionNoGoods(void);
    // derives the nogoods from the last solution (through the output model) and returns them
    std::vector<Call*> deriveNoGoodsFromSolution(void);
  public:
    /// add constraints 
    virtual bool postConstraints(std::vector<Call*> cts);
   /// add variables during search (after next() has been called)
    virtual bool addVariables(std::vector<VarDecl*> vars);   
    /// retrieve the next solution
    virtual Status next(void);
  };
}

#endif
