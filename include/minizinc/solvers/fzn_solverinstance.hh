/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_FZN_SOLVER_INSTANCE_HH__
#define __MINIZINC_FZN_SOLVER_INSTANCE_HH__

#include <minizinc/solver_instance_base.hh>

namespace MiniZinc {
  
  class FZNSolver {
  public:
    typedef Expression* Variable;
    typedef MiniZinc::Statistics Statistics;
  };
  
  class FZNSolverInstance : public NISolverInstanceImpl<FZNSolver> {
  protected:
    Model* _fzn;
    Model* _ozn;
    IdMap<Expression*> _solution;
  public:
    FZNSolverInstance(Env& env, const Options& options);
    
    virtual ~FZNSolverInstance(void);
    
    virtual SolverInstanceBase* copy(void);
    
    virtual Status solve(void);  
    
    virtual void processFlatZinc(void);    
    
  protected:
    void setSolution(Id* id, Expression* e);
    
    virtual Expression* getSolutionValue(Id* id);
    
    virtual SolverInstance::Status nextSolution(void);
  };
  
}

#endif
