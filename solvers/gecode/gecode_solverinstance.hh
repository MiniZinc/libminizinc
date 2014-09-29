/*
 *  Main authors:
 *     Kevin Leo <kevin.leo@monash.edu>
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __MINIZINC_GECODE_SOLVER_INSTANCE_HH__
#define __MINIZINC_GECODE_SOLVER_INSTANCE_HH__

#include <gecode/kernel.hh>
#include <gecode/int.hh>
#include <gecode/driver.hh>

#ifdef GECODE_HAS_SET_VARS
#include <gecode/set.hh>
#endif

#define GECODE_HAS_FLOAT_VARS
#ifdef GECODE_HAS_FLOAT_VARS
#include <gecode/float.hh>
#endif

#include <minizinc/solver_instance_base.hh>

namespace MiniZinc {
 
  
  class GecodeSolver {
  public:
    typedef Gecode::Var Variable;
    typedef MiniZinc::Statistics Statistics;
  };
  
  
  class GecodeSolverInstance : public SolverInstanceImpl<GecodeSolver> {
  protected:
    Gecode::Space* model; /// we could also call it 'solver'
  public:
    GecodeSolverInstance(Env& env, const Options& options);
    virtual ~GecodeSolverInstance(void);
    
    virtual Status next(void);    
    virtual void processFlatZinc(void);    
    virtual void resetSolver(void);
    
    Gecode::Space* getGecodeModel(void);
    
  protected:
    void registerConstraints(void);
    
    IntVarArgs arg2intvarargs(Expression* e, int offset);
      
    
  };
}

#endif