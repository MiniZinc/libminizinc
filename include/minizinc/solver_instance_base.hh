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

namespace MiniZinc {

  class SolverInstanceBase {
  protected:
    Env& _env;
    Options& _options;
    Statistics _statistics;

    typedef void (*poster) (SolverInstanceBase&, const Call* call);

    class Registry {
    protected:
      ASTStringMap<poster> _registry;
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
    
    SolverInstanceBase(Env& env) : _env(env), _constraintRegistry(*this) {}
    
    enum Status { OPT, SAT, UNSAT, UNKNOWN, ERROR };
    
    virtual ~SolverInstanceBase(void);
    
    virtual Status next(void) = 0;

    virtual void processFlatZinc(void) = 0;

    virtual Status solve(void);
    
    void reset(void);
    
    virtual void resetSolver(void) = 0;
    
    virtual void resetWithConstraints(Model::iterator begin, Model::iterator end);
    
    virtual void processPermanentConstraints(Model::iterator begin, Model::iterator end);

    void setOptions(Options& o) { _options = o; }
    Options& getOptions() { return _options; }
    
    Statistics& getStatistics() { return _statistics; }
    
  private:
    SolverInstanceBase(const SolverInstanceBase&);
    SolverInstanceBase& operator= (const SolverInstanceBase&);
  };
  
  template<class Solver>
  class SolverInstanceImpl : public SolverInstanceBase {
  protected:
    IdMap<typename Solver::Variable> _variableMap;
  public:
  };
  
}

#endif
