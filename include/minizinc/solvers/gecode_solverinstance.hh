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

#ifdef GECODE_HAS_FLOAT_VARS
#include <gecode/float.hh>
#endif

#include <minizinc/flattener.hh>
#include <minizinc/solver.hh>
#include <minizinc/solvers/gecode/fzn_space.hh>

#if GECODE_VERSION_NUMBER >= 500000

#define HAS_GECODE_VERSION_5

#if GECODE_VERSION_NUMBER >= 500100
#define HAS_GECODE_VERSION_5_1
#endif

#endif

#ifdef HAS_GECODE_VERSION_5
#define MZ_IntConLevel Gecode::IntPropLevel
#define MZ_ICL_VAL Gecode::IPL_VAL
#define MZ_ICL_DOM Gecode::IPL_DOM 
#define MZ_ICL_BND Gecode::IPL_BND
#define MZ_ICL_DEF Gecode::IPL_DEF
#define MZ_EPK_DEF Gecode::IPL_DEF
#else
#define MZ_IntConLevel Gecode::IntConLevel
#define MZ_ICL_VAL Gecode::ICL_VAL
#define MZ_ICL_DOM Gecode::ICL_DOM 
#define MZ_ICL_BND Gecode::ICL_BND
#define MZ_ICL_DEF Gecode::ICL_DEF
#define MZ_EPK_DEF Gecode::EPK_DEF
#endif


namespace MiniZinc {
  
 /* class GecodeVariable {
  public:
    enum vartype {BOOL_TYPE,FLOAT_TYPE,INT_TYPE,SET_TYPE};
  protected:
    Gecode::VarImpBase* _var;
    vartype _t;
    /// the index in FznSpace::bv of the boolean variable that corresponds to the int var; if not exists then -1
    int _boolAliasIndex;
  public:
    GecodeVariable(Gecode::IntVar& x) : _var(x.varimp()), _t(INT_TYPE), _boolAliasIndex(-1) {}
    GecodeVariable(Gecode::BoolVar& x) : _var(x.varimp()), _t(BOOL_TYPE), _boolAliasIndex(-1) {}
    GecodeVariable(Gecode::FloatVar& x) : _var(x.varimp()), _t(FLOAT_TYPE), _boolAliasIndex(-1) {}
    GecodeVariable(Gecode::SetVar& x) : _var(x.varimp()), _t(SET_TYPE), _boolAliasIndex(-1) {}
    
    Gecode::IntVar intVar(void) {
      assert(_t == INT_TYPE);
      Gecode::Int::IntView iv(static_cast<Gecode::Int::IntVarImp*>(_var));
      return Gecode::IntVar(iv);
    }
    
    Gecode::BoolVar boolVar(void) {
      assert(_t == BOOL_TYPE);
      Gecode::Int::BoolView bv(static_cast<Gecode::Int::BoolVarImp*>(_var));
      return Gecode::BoolVar(bv);
    }
    
    Gecode::FloatVar floatVar(void) {
      assert(_t == FLOAT_TYPE);
      Gecode::Float::FloatView fv(static_cast<Gecode::Float::FloatVarImp*>(_var));
      return Gecode::FloatVar(fv);
    }
    
    Gecode::SetVar setVar(void) {
      assert(_t == FLOAT_TYPE);
      Gecode::Set::SetView sv(static_cast<Gecode::Set::SetVarImp*>(_var));
      return Gecode::SetVar(sv);
    }
    
    bool isint(void) const {
      return _t == INT_TYPE;
    }
    
    bool isbool(void) const {
      return _t == BOOL_TYPE;
    }
    
    bool isfloat(void) const {
      return _t == FLOAT_TYPE;
    }
    
    bool isset(void) const {
      return _t == SET_TYPE;
    }
    
    bool hasBoolAlias(void) {
      return _boolAliasIndex >= 0;
    }
    
    /// set the index in FznSpace::bv of the Boolean variable that corresponds to the int variable
    void setBoolAliasIndex(int index) {
      assert(_t == INT_TYPE);
      assert(index >= 0);    
      _boolAliasIndex = index;
    }
    
    int boolAliasIndex(void) {
      return  _boolAliasIndex;
    }
    
    vartype t(void) const {
      return _t;
    }
  }; */
  
  class GecodeVariable {
  public:
    enum vartype {BOOL_TYPE,FLOAT_TYPE,INT_TYPE,SET_TYPE};
  protected:
    /// variable type
    vartype _t;
    /// the index in the iv/bv/fv/sv array in the space, depending the type _t
    unsigned int _index;
    /// the index in FznSpace::bv of the boolean variable that corresponds to the int var; if not exists then -1
    int _boolAliasIndex;
  public:
    GecodeVariable(vartype t, unsigned int index) : 
        _t(t), _index(index), _boolAliasIndex(-1) {}
        
    bool isint(void) const {
      return _t == INT_TYPE;
    }
    
    bool isbool(void) const {
      return _t == BOOL_TYPE;
    }
    
    bool isfloat(void) const {
      return _t == FLOAT_TYPE;
    }
    
    bool isset(void) const {
      return _t == SET_TYPE;
    }        
    
    bool hasBoolAlias(void) {
      return _boolAliasIndex >= 0;
    }
    
    /// set the index in FznSpace::bv of the Boolean variable that corresponds to the int variable
    void setBoolAliasIndex(int index) {
      assert(_t == INT_TYPE);
      assert(index >= 0);    
      _boolAliasIndex = index;
    }
    
    int boolAliasIndex(void) {
      return  _boolAliasIndex;
    }

    int index(void) {
      return _index;
    }
    
    Gecode::IntVar& intVar(MiniZinc::FznSpace* space) {
      assert(_t == INT_TYPE);
      assert(_index < space->iv.size());
      return space->iv[_index];
    }
    
    Gecode::BoolVar& boolVar(MiniZinc::FznSpace* space) {
      assert(_t == BOOL_TYPE);
      assert(_index < space->bv.size());
      return space->bv[_index];
    }

#ifdef GECODE_HAS_FLOAT_VARS
    Gecode::FloatVar& floatVar(MiniZinc::FznSpace* space) {
      assert(_t == FLOAT_TYPE);
      assert(_index < space->fv.size());
      return space->fv[_index];
    }
#endif

#ifdef GECODE_HAS_SET_VARS
    Gecode::SetVar& setVar(MiniZinc::FznSpace* space) {
      assert(_t == SET_TYPE);
      assert(_index < space->sv.size());
      return space->sv[_index];
    }
#endif


    
  };
  
  
  class GecodeSolver {
  public:
    typedef GecodeVariable Variable;
    typedef MiniZinc::Statistics Statistics;
  };
  
  class GecodeEngine;
  
  class GecodeSolverInstance : public SolverInstanceImpl<GecodeSolver> {   
  private:
    bool _print_stats;
    bool _only_range_domains;
    bool _run_sac;
    bool _run_shave;
    unsigned int _pre_passes;
    bool _all_solutions;
    unsigned int _n_max_solutions;
    unsigned int _n_found_solutions;
    Model* _flat;
  public:
    /// the Gecode space that will be/has been solved
    FznSpace* _current_space; 
    /// the solution (or NULL if does not exist or not yet computed)
    FznSpace* _solution;
    /// the variable declarations with output annotations
    std::vector<VarDecl*> _varsWithOutput;
    /// declaration map for processing and printing output
    //typedef std::pair<VarDecl*,Expression*> DE;
    //ASTStringMap<DE>::t _declmap;
    /// TODO: we can probably get rid of this
    UNORDERED_NAMESPACE::unordered_map<VarDecl*, std::vector<Expression*>* > arrayMap;
    /// The solver engine
    GecodeEngine* engine;
    Gecode::Search::Options engine_options;

    GecodeSolverInstance(Env& env, const Options& options);
    virtual ~GecodeSolverInstance(void);

    virtual Status next(void);    
    virtual void processFlatZinc(void);    
    virtual Status solve(void);
    virtual void resetSolver(void);

    // Presolve the currently loaded model, updating variables with the same
    // names in the given Model* m.
    bool presolve(Model* m = NULL);
    bool sac(bool toFixedPoint, bool shaving);
    void print_stats();

    void processSolution(bool last_sol = false);
    virtual Expression* getSolutionValue(Id* id);

    Gecode::Space* getGecodeModel(void);

    // helpers for getting correct int bounds
    bool valueWithinBounds(double b);

    // helper functions for processing flatzinc constraints
    /// Convert \a arg (array of integers) to IntArgs
    Gecode::IntArgs arg2intargs(Expression* arg, int offset = 0);
    /// Convert \a arg (array of Booleans) to IntArgs
    Gecode::IntArgs arg2boolargs(Expression* arg, int offset = 0);
    /// Convert \a n to IntSet
    Gecode::IntSet arg2intset(EnvI& envi, Expression* sl);
    /// Convert \a arg to IntVarArgs
    Gecode::IntVarArgs arg2intvarargs(Expression* arg, int offset = 0);
    /// Convert \a arg to BoolVarArgs
    Gecode::BoolVarArgs arg2boolvarargs(Expression* a, int offset = 0, int siv=-1);
    /// Convert \a n to BoolVar
    Gecode::BoolVar arg2boolvar(Expression* e);
    /// Convert \a n to IntVar
    Gecode::IntVar arg2intvar(Expression* e);
     /// convert \a arg to an ArrayLit (throws InternalError if not possible)
    ArrayLit* arg2arraylit(Expression* arg);  
    /// Check if \a b is array of Booleans (or has a single integer)
    bool isBoolArray(ArrayLit* a, int& singleInt);
#ifdef GECODE_HAS_FLOAT_VARS
    /// Convert \a n to FloatValArgs
    Gecode::FloatValArgs arg2floatargs(Expression* arg, int offset = 0);
    /// Convert \a n to FloatVar
    Gecode::FloatVar arg2floatvar(Expression* n);
    /// Convert \a n to FloatVarArgs
    Gecode::FloatVarArgs arg2floatvarargs(Expression* arg, int offset = 0);
#endif
    /// Convert \a ann to IntConLevel


    MZ_IntConLevel ann2icl(const Annotation& ann);

     /// convert the annotation \a s int variable selection to the respective Gecode var selection
    Gecode::TieBreak<Gecode::IntVarBranch> ann2ivarsel(std::string s, Gecode::Rnd& rnd, double decay);
    /// convert the annotation \a s int value selection to the respective Gecode val selection
    Gecode::IntValBranch ann2ivalsel(std::string s, std::string& r0, std::string& r1, Gecode::Rnd& rnd);
    /// convert assign value selection
    Gecode::IntAssign ann2asnivalsel(std::string s, Gecode::Rnd& rnd);

#ifdef HAS_GECODE_VERSION_5_1
    Gecode::TieBreak<Gecode::BoolVarBranch> ann2bvarsel(std::string s, Gecode::Rnd& rnd, double decay);
    /// convert the annotation \a s int value selection to the respectbve Gecode val selection
    Gecode::BoolValBranch ann2bvalsel(std::string s, std::string& r0, std::string& r1, Gecode::Rnd& rnd);
    /// convert assign value selection
    Gecode::BoolAssign ann2asnbvalsel(std::string s, Gecode::Rnd& rnd);
#endif


#ifdef GECODE_HAS_SET_VARS
    Gecode::SetVarBranch ann2svarsel(std::string s, Gecode::Rnd& rnd, double decay);
    Gecode::SetValBranch ann2svalsel(std::string s, std::string r0, std::string r1, Gecode::Rnd& rnd);
#endif
#ifdef GECODE_HAS_FLOAT_VARS
    Gecode::TieBreak<Gecode::FloatVarBranch> ann2fvarsel(std::string s, Gecode::Rnd& rnd, double decay);
    Gecode::FloatValBranch ann2fvalsel(std::string s, std::string r0, std::string r1);
#endif    
    /// Returns the VarDecl of \a expr and throws an InternalError if not possible
    VarDecl* getVarDecl(Expression* expr);
    /// Returns the VarDecl of \a aa 
    VarDecl* resolveArrayAccess(ArrayAccess* aa);
    /// Returns the VarDecl of \a array at index \a index
    VarDecl* resolveArrayAccess(VarDecl* array, int index);

    /// Returns the GecodeVariable representing the Id, VarDecl or ArrayAccess
    GecodeSolver::Variable resolveVar(Expression* e);

    /// Inserts variable gv into _variableMap with key id
    void insertVar(Id* id, GecodeVariable gv);

  protected:
    /// Flatzinc options // TODO: do we need specific Gecode options? Use MiniZinc::Options instead?
    // FlatZincOptions* opts;
    void registerConstraints(void);
    void registerConstraint(std::string name, poster p);

    /// creates the gecode branchers // TODO: what is decay, ignoreUnknown -> do we need all the args?
    void createBranchers(Annotation& ann, Expression* additionalAnn, int seed, double decay,
            bool ignoreUnknown, std::ostream& err);
    void prepareEngine(void);
    void setSearchStrategyFromAnnotation(std::vector<Expression*> flatAnn, 
                                                        std::vector<bool>& iv_searched, 
                                                        std::vector<bool>& bv_searched,
                                                #ifdef GECODE_HAS_SET_VARS
                                                        std::vector<bool>& sv_searched,
                                                #endif
                                                #ifdef GECODE_HAS_FLOAT_VARS
                                                        std::vector<bool>& fv_searched,
                                                #endif
                                                        Gecode::TieBreak<Gecode::IntVarBranch>& def_int_varsel,
                                                        Gecode::IntValBranch& def_int_valsel,
                                                #ifdef HAS_GECODE_VERSION_5_1
                                                        Gecode::TieBreak<Gecode::BoolVarBranch>& def_bool_varsel,
                                                        Gecode::BoolValBranch& def_bool_valsel,
                                                #else
                                                        Gecode::TieBreak<Gecode::IntVarBranch>& def_bool_varsel,
                                                        Gecode::IntValBranch& def_bool_valsel,
                                                #endif

                                                #ifdef GECODE_HAS_SET_VARS
                                                        Gecode::SetVarBranch& def_set_varsel,
                                                        Gecode::SetValBranch& def_set_valsel,
                                                #endif
                                                #ifdef GECODE_HAS_FLOAT_VARS
                                                        Gecode::TieBreak<Gecode::FloatVarBranch>& def_float_varsel,
                                                        Gecode::FloatValBranch& def_float_valsel,
                                                #endif
                                                        Gecode::Rnd& rnd,
                                                        double decay,
                                                        bool ignoreUnknown,
                                                        std::ostream& err
                                                       );
  };
}

#endif
