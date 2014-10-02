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
  
  class FznSpace : public Gecode::Space {
  public:
    /// The integer variables
    std::vector<Gecode::IntVar> iv;
    /// The introduced integer variables
    Gecode::IntVarArray iv_aux;
    /// Indicates whether an integer variable is introduced by mzn2fzn
    std::vector<bool> iv_introduced;
    /// Indicates whether an integer variable aliases a Boolean variable
    std::vector<int> iv_boolalias;
    /// The Boolean variables
    std::vector<Gecode::BoolVar> bv;
    /// The introduced Boolean variables
    Gecode::BoolVarArray bv_aux;
    /// Indicates whether a Boolean variable is introduced by mzn2fzn
    std::vector<bool> bv_introduced;
#ifdef GECODE_HAS_SET_VARS
    /// The set variables
    std::vector<Gecode::SetVar> sv;
    /// The introduced set variables
    Gecode::SetVarArray sv_aux;
    /// Indicates whether a set variable is introduced by mzn2fzn
    std::vector<bool> sv_introduced;
#endif
#ifdef GECODE_HAS_FLOAT_VARS
    /// The float variables
    std::vector<Gecode::FloatVar> fv;
    /// The introduced float variables
    Gecode::FloatVarArray fv_aux;
    /// Indicates whether a float variable is introduced by mzn2fzn
    std::vector<bool> fv_introduced;
#endif
    
    /// copy constructor
    FznSpace(bool share, FznSpace&);
    /// standard constructor
    FznSpace(void) : intVarCount(-1), boolVarCount(-1), floatVarCount(-1),
            setVarCount(-1), needAuxVars(true) {} ; 
            
    /// Link integer variable \a iv to Boolean variable \a bv TODO: copied from old interface, do we still need this?
    void aliasBool2Int(int iv, int bv);
    /// Return linked Boolean variable for integer variable \a iv TODO: copied from old interface, do we still need this?
    int aliasBool2Int(int iv);
  
  protected:
    /// Implement optimization
    virtual void constrain(const Space& s);
    /// Copy function
    virtual Gecode::Space* copy(bool share);
    
    /// Number of integer variables
    int intVarCount;
    /// Number of Boolean variables
    int boolVarCount;
    /// Number of float variables
    int floatVarCount;
    /// Number of set variables
    int setVarCount;
    /// Whether the introduced variables still need to be copied
    bool needAuxVars;    
  };
  
  
  class GecodeSolverInstance : public SolverInstanceImpl<GecodeSolver> {   
  public:
    FznSpace* model; /// we could also call it 'solver', 'working_instance' etc
     
    GecodeSolverInstance(Env& env, const Options& options);
    virtual ~GecodeSolverInstance(void);
    
    virtual Status next(void);    
    virtual void processFlatZinc(void);    
    virtual void resetSolver(void);
    
    Gecode::Space* getGecodeModel(void);
    
    // helper functions for processing flatzinc constraints
    /// Convert \a arg (array of integers) to IntArgs
    Gecode::IntArgs arg2intargs(Expression* arg, int offset = 0);
    /// Convert \a arg (array of Booleans) to IntArgs
    Gecode::IntArgs arg2boolargs(Expression* arg, int offset = 0);
    /// Convert \a n to IntSet
    Gecode::IntSet arg2intset(Expression* sl);
    /// Convert \a arg to IntVarArgs
    Gecode::IntVarArgs arg2intvarargs(Expression* arg, int offset = 0);
    /// Convert \a arg to BoolVarArgs
    Gecode::BoolVarArgs arg2boolvarargs(Expression* a, int offset = 0, int siv=-1);
    /// Convert \a n to BoolVar
    Gecode::BoolVar arg2BoolVar(Expression* e);
    /// Convert \a n to IntVar
    Gecode::IntVar arg2IntVar(Expression* e);
    /// Check if \a b is array of Booleans (or has a single integer)
    bool isBoolArray(ArrayLit* a, int& singleInt);
#ifdef GECODE_HAS_FLOAT_VARS
    /// Convert \a n to FloatValArgs
    Gecode::FloatValArgs arg2floatargs(Expression* arg, int offset = 0);
    /// Convert \a n to FloatVar
    Gecode::FloatVar arg2FloatVar(Expression* n);
    /// Convert \a n to FloatVarArgs
    Gecode::FloatVarArgs arg2floatvarargs(Expression* arg, int offset = 0);
#endif
    /// Convert \a ann to IntConLevel
    Gecode::IntConLevel ann2icl(const Annotation& ann);  
    /// TODO: copied this function from SolverInterface -> should be moved somewhere else?
    ArrayLit* getArrayLit(Expression* arg);  
    /// TODO: copied from old SolverInterface -> needs to be adapted/changed (void pointer!)
    void* resolveVar(Expression* e);
    /// TODO: copied from old SolverInterface -> do we really need this?
    VarDecl* getVarDecl(Expression* expr);
    
  protected:
    void registerConstraints(void);
  };
}

#endif