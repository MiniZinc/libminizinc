/*
 *  Main authors:
 *     Kevin Leo <kevin.leo@monash.edu>
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/flattener.hh>
#include <minizinc/solver.hh>
#include <minizinc/solvers/gecode/fzn_space.hh>

#include <unordered_map>

#if GECODE_VERSION_NUMBER < 600000
#error Gecode versions before 6.0 are not supported
#endif

#define MZ_IntConLevel Gecode::IntPropLevel
#define MZ_ICL_VAL Gecode::IPL_VAL
#define MZ_ICL_DOM Gecode::IPL_DOM
#define MZ_ICL_BND Gecode::IPL_BND
#define MZ_ICL_DEF Gecode::IPL_DEF
#define MZ_EPK_DEF Gecode::IPL_DEF

namespace MiniZinc {

/* class GecodeVariable {
 public:
   enum vartype {BOOL_TYPE,FLOAT_TYPE,INT_TYPE,SET_TYPE};
 protected:
   Gecode::VarImpBase* _var;
   vartype _t;
   /// the index in FznSpace::bv of the boolean variable that corresponds to the int var; if not
 exists then -1 int _boolAliasIndex; public: GecodeVariable(Gecode::IntVar& x) : _var(x.varimp()),
 _t(INT_TYPE), _boolAliasIndex(-1) {} GecodeVariable(Gecode::BoolVar& x) : _var(x.varimp()),
 _t(BOOL_TYPE), _boolAliasIndex(-1) {} GecodeVariable(Gecode::FloatVar& x) : _var(x.varimp()),
 _t(FLOAT_TYPE), _boolAliasIndex(-1) {} GecodeVariable(Gecode::SetVar& x) : _var(x.varimp()),
 _t(SET_TYPE), _boolAliasIndex(-1) {}

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
  enum vartype { BOOL_TYPE, FLOAT_TYPE, INT_TYPE, SET_TYPE };

protected:
  /// variable type
  vartype _t;
  /// the index in the iv/bv/fv/sv array in the space, depending the type _t
  unsigned int _index;
  /// the index in FznSpace::bv of the boolean variable that corresponds to the int var; if not
  /// exists then -1
  int _boolAliasIndex;

public:
  GecodeVariable(vartype t, unsigned int index) : _t(t), _index(index), _boolAliasIndex(-1) {}

  bool isint() const { return _t == INT_TYPE; }

  bool isbool() const { return _t == BOOL_TYPE; }

  bool isfloat() const { return _t == FLOAT_TYPE; }

  bool isset() const { return _t == SET_TYPE; }

  bool hasBoolAlias() const { return _boolAliasIndex >= 0; }

  /// set the index in FznSpace::bv of the Boolean variable that corresponds to the int variable
  void setBoolAliasIndex(unsigned int index) {
    assert(_t == INT_TYPE);
    _boolAliasIndex = static_cast<int>(index);
  }

  int boolAliasIndex() const { return _boolAliasIndex; }

  unsigned int index() const { return _index; }

  Gecode::IntVar& intVar(MiniZinc::FznSpace* space) const {
    assert(_t == INT_TYPE);
    assert(_index < space->iv.size());
    return space->iv[_index];
  }

  Gecode::BoolVar& boolVar(MiniZinc::FznSpace* space) const {
    assert(_t == BOOL_TYPE);
    assert(_index < space->bv.size());
    return space->bv[_index];
  }

#ifdef GECODE_HAS_FLOAT_VARS
  Gecode::FloatVar& floatVar(MiniZinc::FznSpace* space) const {
    assert(_t == FLOAT_TYPE);
    assert(_index < space->fv.size());
    return space->fv[_index];
  }
#endif

#ifdef GECODE_HAS_SET_VARS
  Gecode::SetVar& setVar(MiniZinc::FznSpace* space) const {
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

class GecodeOptions : public SolverInstanceBase::Options {
public:
  bool allowUnboundedVars = false;
  bool onlyRangeDomains = false;
  bool sac = false;
  bool shave = false;
  bool verbose = false;
  unsigned int prePasses = 0;
  bool statistics = false;
  bool allSolutions = false;
  int nSolutions = -1;
  unsigned int c_d = 8;  // NOLINT(readability-identifier-naming)
  unsigned int a_d = 2;  // NOLINT(readability-identifier-naming)
  int nodes = 0;
  int fails = 0;
  int time = 0;
  int seed = 1;
  double decay = 0.5;
};

class GecodeSolverInstance : public SolverInstanceImpl<GecodeSolver> {
private:
  bool _printStats;
  bool _onlyRangeDomains;
  bool _runSac;
  bool _runShave;
  unsigned int _prePasses;
  bool _allSolutions;
  int _nMaxSolutions;
  int _nFoundSolutions;
  bool _allowUnboundedVars;
  Model* _flat;

public:
  /// the Gecode space that will be/has been solved
  FznSpace* currentSpace;
  /// the solution (or NULL if does not exist or not yet computed)
  FznSpace* solution;
  /// the variable declarations with output annotations
  std::vector<VarDecl*> varsWithOutput;
  /// declaration map for processing and printing output
  // typedef std::pair<VarDecl*,Expression*> DE;
  // ASTStringMap<DE>::t _declmap;
  /// TODO: we can probably get rid of this
  std::unordered_map<VarDecl*, std::vector<Expression*>*> arrayMap;
  /// The solver engine
  GecodeEngine* engine;
  Gecode::Search::Options engineOptions;

  GecodeSolverInstance(Env& env, std::ostream& log, SolverInstanceBase::Options* opt);
  ~GecodeSolverInstance() override;

  Status next() override;
  void processFlatZinc() override;
  Status solve() override;
  void resetSolver() override;

  // Presolve the currently loaded model, updating variables with the same
  // names in the given Model* originalModel.
  bool presolve(Model* originalModel = nullptr);
  bool sac(bool toFixedPoint, bool shaving) const;
  void printStatistics() override;

  void processSolution(bool last_sol = false);
  Expression* getSolutionValue(Id* id) override;

  Gecode::Space* getGecodeModel();

  // helpers for getting correct int bounds
  static bool valueWithinBounds(double b);

  // helper functions for processing flatzinc constraints
  /// Convert \a arg (array of integers) to IntArgs
  static Gecode::IntArgs arg2intargs(Expression* arg, int offset = 0);
  /// Convert \a arg (array of Booleans) to IntArgs
  static Gecode::IntArgs arg2boolargs(Expression* arg, int offset = 0);
  /// Convert \a n to IntSet
  Gecode::IntSet arg2intset(EnvI& envi, Expression* arg);
  /// Convert \a n to IntSetArgs
  Gecode::IntSetArgs arg2intsetargs(EnvI& envi, Expression* arg, int offset = 0);
  /// Convert \a arg to IntVarArgs
  Gecode::IntVarArgs arg2intvarargs(Expression* arg, int offset = 0);
  /// Convert \a arg to BoolVarArgs
  Gecode::BoolVarArgs arg2boolvarargs(Expression* a, int offset = 0, int siv = -1);
  /// Convert \a n to BoolVar
  Gecode::BoolVar arg2boolvar(Expression* e);
  /// Convert \a n to IntVar
  Gecode::IntVar arg2intvar(Expression* e);
  /// Convert \a n to SetVar
  Gecode::SetVar arg2setvar(Expression* e);
  /// Convert \a arg to SetVarArgs
  Gecode::SetVarArgs arg2setvarargs(Expression* arg, int offset = 0, int doffset = 0,
                                    const Gecode::IntSet& od = Gecode::IntSet::empty);
  /// convert \a arg to an ArrayLit (throws InternalError if not possible)
  ArrayLit* arg2arraylit(Expression* arg);
  /// Check if \a b is array of Booleans (or has a single integer)
  bool isBoolArray(ArrayLit* a, int& singleInt);
#ifdef GECODE_HAS_FLOAT_VARS
  /// Convert \a n to FloatValArgs
  static Gecode::FloatValArgs arg2floatargs(Expression* arg, int offset = 0);
  /// Convert \a n to FloatVar
  Gecode::FloatVar arg2floatvar(Expression* e);
  /// Convert \a n to FloatVarArgs
  Gecode::FloatVarArgs arg2floatvarargs(Expression* arg, int offset = 0);
#endif
  /// Convert \a ann to IntConLevel

  static MZ_IntConLevel ann2icl(const Annotation& ann);

  /// convert the annotation \a s int variable selection to the respective Gecode var selection
  static Gecode::TieBreak<Gecode::IntVarBranch> ann2ivarsel(ASTString s, Gecode::Rnd& rnd,
                                                            double decay);
  /// convert the annotation \a s int value selection to the respective Gecode val selection
  static Gecode::IntValBranch ann2ivalsel(ASTString s, std::string& r0, std::string& r1,
                                          Gecode::Rnd& rnd);
  /// convert assign value selection
  static Gecode::IntAssign ann2asnivalsel(ASTString s, Gecode::Rnd& rnd);

  static Gecode::TieBreak<Gecode::BoolVarBranch> ann2bvarsel(ASTString s, Gecode::Rnd& rnd,
                                                             double decay);
  /// convert the annotation \a s int value selection to the respectbve Gecode val selection
  static Gecode::BoolValBranch ann2bvalsel(ASTString s, std::string& r0, std::string& r1,
                                           Gecode::Rnd& rnd);
  /// convert assign value selection
  static Gecode::BoolAssign ann2asnbvalsel(ASTString s, Gecode::Rnd& rnd);

#ifdef GECODE_HAS_SET_VARS
  static Gecode::SetVarBranch ann2svarsel(ASTString s, Gecode::Rnd& rnd, double decay);
  static Gecode::SetValBranch ann2svalsel(ASTString s, std::string& r0, std::string& r1,
                                          Gecode::Rnd& rnd);
#endif
#ifdef GECODE_HAS_FLOAT_VARS
  static Gecode::TieBreak<Gecode::FloatVarBranch> ann2fvarsel(ASTString s, Gecode::Rnd& rnd,
                                                              double decay);
  static Gecode::FloatValBranch ann2fvalsel(ASTString s, std::string& r0, std::string& r1);
#endif
  /// Returns the VarDecl of \a expr and throws an InternalError if not possible
  VarDecl* getVarDecl(Expression* expr);
  /// Returns the VarDecl of \a aa
  VarDecl* resolveArrayAccess(ArrayAccess* aa);
  /// Returns the VarDecl of \a array at index \a index
  VarDecl* resolveArrayAccess(VarDecl* vd, long long int index);

  /// Returns the GecodeVariable representing the Id, VarDecl or ArrayAccess
  GecodeSolver::Variable resolveVar(Expression* e);

  /// Inserts variable gv into _variableMap with key id
  void insertVar(Id* id, GecodeVariable gv);

protected:
  void registerConstraints();
  void registerConstraint(const std::string& name, poster p);

  /// creates the gecode branchers // TODO: what is decay, ignoreUnknown -> do we need all the args?
  void createBranchers(Annotation& ann, Expression* additionalAnn, int seed, double decay,
                       bool ignoreUnknown, std::ostream& err);
  void prepareEngine();
  void setSearchStrategyFromAnnotation(
      std::vector<Expression*> flatAnn, std::vector<bool>& iv_searched,
      std::vector<bool>& bv_searched,
#ifdef GECODE_HAS_SET_VARS
      std::vector<bool>& sv_searched,
#endif
#ifdef GECODE_HAS_FLOAT_VARS
      std::vector<bool>& fv_searched,
#endif
      Gecode::TieBreak<Gecode::IntVarBranch>& def_int_varsel, Gecode::IntValBranch& def_int_valsel,
      Gecode::TieBreak<Gecode::BoolVarBranch>& def_bool_varsel,
      Gecode::BoolValBranch& def_bool_valsel,

#ifdef GECODE_HAS_SET_VARS
      Gecode::SetVarBranch& def_set_varsel, Gecode::SetValBranch& def_set_valsel,
#endif
#ifdef GECODE_HAS_FLOAT_VARS
      Gecode::TieBreak<Gecode::FloatVarBranch>& def_float_varsel,
      Gecode::FloatValBranch& def_float_valsel,
#endif
      Gecode::Rnd& rnd, double decay, bool ignoreUnknown, std::ostream& err);
};

class GecodeSolverFactory : public SolverFactory {
public:
  GecodeSolverFactory();
  SolverInstanceBase::Options* createOptions() override;
  SolverInstanceBase* doCreateSI(Env& env, std::ostream& log,
                                 SolverInstanceBase::Options* opt) override;
  std::string getDescription(SolverInstanceBase::Options* opt = nullptr) override;
  std::string getVersion(SolverInstanceBase::Options* opt = nullptr) override;
  std::string getId() override { return "org.minizinc.gecode_presolver"; }
  bool processOption(SolverInstanceBase::Options* opt, int& i,
                     std::vector<std::string>& argv) override;
  void printHelp(std::ostream& os) override;
};

}  // namespace MiniZinc
