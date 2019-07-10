/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_FLATTEN_INTERNAL_HH__
#define __MINIZINC_FLATTEN_INTERNAL_HH__

#include <cmath>

#include <minizinc/copy.hh>
#include <minizinc/flatten.hh>
#include <minizinc/optimize.hh>
#include <minizinc/eval_par.hh>

namespace MiniZinc {

  /// Result of evaluation
  class EE {
  public:
    /// The result value
    KeepAlive r;
    /// Boolean expression representing whether result is defined
    KeepAlive b;
    /// Constructor
    explicit EE(Expression* r0=NULL, Expression* b0=NULL) : r(r0), b(b0) {}
  };

  /// Boolean evaluation context
  enum BCtx { C_ROOT, C_POS, C_NEG, C_MIX };
  
  /// Evaluation context
  struct Ctx {
    /// Boolean context
    BCtx b;
    /// Integer context
    BCtx i;
    /// Boolen negation flag
    bool neg;
    /// Default constructor (root context)
    Ctx(void) : b(C_ROOT), i(C_MIX), neg(false) {}
    /// Copy constructor
    Ctx(const Ctx& ctx) : b(ctx.b), i(ctx.i), neg(ctx.neg) {}
    /// Assignment operator
    Ctx& operator =(const Ctx& ctx) {
      if (this!=&ctx) {
        b = ctx.b;
        i = ctx.i;
        neg = ctx.neg;
      }
      return *this;
    }
  };
  
  /// Turn \a c into positive context
  BCtx operator +(const BCtx& c);
  /// Negate context \a c
  BCtx operator -(const BCtx& c);
  
  class EnvI {
  public:
    Model* model;
    Model* orig_model;
    Model* output;
    VarOccurrences vo;
    VarOccurrences output_vo;
    
    std::ostream& outstream;
    std::ostream& errstream;

    // The current pass number (used for unifying and disabling path construction in final pass)
    unsigned int current_pass_no;
    // Used for disabling path construction in final pass
    unsigned int final_pass_no;
    // Used for disabling path construction past the maxPathDepth of previous passes
    unsigned int maxPathDepth;

    VarOccurrences output_vo_flat;
    CopyMap cmap;
    IdMap<KeepAlive> reverseMappers;
    struct WW {
      WeakRef r;
      WeakRef b;
      WW(WeakRef r0, WeakRef b0) : r(r0), b(b0) {}
    };
    typedef KeepAliveMap<WW> CSEMap;
    bool ignorePartial;
    bool ignoreUnknownIds;
    std::vector<Expression*> callStack;
    std::vector<std::pair<KeepAlive,bool> > errorStack;
    std::vector<int> idStack;
    unsigned int maxCallStack;
    std::vector<std::string> warnings;
    bool collect_vardecls;
    std::vector<int> modifiedVarDecls;
    int in_redundant_constraint;
    int in_maybe_partial;
    int n_reif_ct;
    int n_imp_ct;
    int n_imp_del;
    int n_lin_del;
    bool in_reverse_map_var;
    FlatteningOptions fopts;
    unsigned int pathUse;
    std::unordered_map<std::string, int> reverseEnum;

    struct PathVar {
      KeepAlive decl;
      unsigned int pass_no;
    };
    // Store mapping from path string to (VarDecl, pass_no) tuples
    typedef std::unordered_map<std::string, PathVar> PathMap;
    // Mapping from arbitrary Expressions to paths
    typedef KeepAliveMap<std::string> ReversePathMap;
    // Map from filename to integer (space saving optimisation)
    typedef std::unordered_map<std::string, int> FilenameMap;
    std::vector<KeepAlive> checkVars;
  protected:
    CSEMap cse_map;
    Model* _flat;
    bool _failed;
    unsigned int ids;
    ASTStringMap<ASTString>::t reifyMap;
    PathMap pathMap;
    ReversePathMap reversePathMap;
    FilenameMap filenameMap;
    typedef std::unordered_map<VarDeclI*,unsigned int> EnumMap;
    EnumMap enumMap;
    std::vector<VarDeclI*> enumVarDecls;
    typedef std::unordered_map<std::string,unsigned int> ArrayEnumMap;
    ArrayEnumMap arrayEnumMap;
    std::vector<std::vector<unsigned int> > arrayEnumDecls;
  public:
    EnvI(Model* orig0, std::ostream& outstream0=std::cout, std::ostream& errstream0=std::cerr);
    ~EnvI(void);
    long long int genId(void);
    /// Set minimum new temporary id to \a i+1
    void minId(unsigned int i) { ids = std::max(ids, i+1); }
    void cse_map_insert(Expression* e, const EE& ee);
    CSEMap::iterator cse_map_find(Expression* e);
    void cse_map_remove(Expression* e);
    CSEMap::iterator cse_map_end(void);
    void dump(void);
    
    unsigned int registerEnum(VarDeclI* vdi);
    VarDeclI* getEnum(unsigned int i) const;
    unsigned int registerArrayEnum(const std::vector<unsigned int>& arrayEnum);
    const std::vector<unsigned int>& getArrayEnum(unsigned int i) const;
    /// Check if \a t1 is a subtype of \a t2 (including enumerated types if \a strictEnum is true)
    bool isSubtype(const Type& t1, const Type& t2, bool strictEnum);
    
    void flat_addItem(Item* i);
    void flat_removeItem(int i);
    void flat_removeItem(Item* i);
    void vo_add_exp(VarDecl* vd);
    void annotateFromCallStack(Expression* e);
    void fail(const std::string& msg = std::string());
    bool failed(void) const;
    Model* flat(void);
    void swap();
    void swap_output() { std::swap( model, output ); }
    ASTString reifyId(const ASTString& id);
    ASTString halfReifyId(const ASTString& id);
    std::ostream& dumpStack(std::ostream& os, bool errStack);
    bool dumpPath(std::ostream& os, bool force = false);
    void addWarning(const std::string& msg);
    void collectVarDecls(bool b);
    PathMap& getPathMap() { return pathMap; }
    ReversePathMap& getReversePathMap() { return reversePathMap; }
    FilenameMap& getFilenameMap() { return filenameMap; }

    void copyPathMapsAndState(EnvI& env);
    /// deprecated, use Solns2Out
    std::ostream& evalOutput(std::ostream& os);
    void createErrorStack(void);
    Call* surroundingCall(void) const;

    void cleanupExceptOutput();
  };
  
  void setComputedDomain(EnvI& envi, VarDecl* vd, Expression* domain, bool is_computed);
  EE flat_exp(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);
  EE flatten_id(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b, bool doNotFollowChains);

  class CmpExpIdx {
  public:
    std::vector<KeepAlive>& x;
    CmpExpIdx(std::vector<KeepAlive>& x0) : x(x0) {}
    bool operator ()(int i, int j) const {
      if (Expression::equal(x[i](),x[j]()))
        return false;
      if (x[i]()->isa<Id>() && x[j]()->isa<Id>() &&
          x[i]()->cast<Id>()->idn() != -1 &&
          x[j]()->cast<Id>()->idn() != -1)
        return x[i]()->cast<Id>()->idn() < x[j]()->cast<Id>()->idn();
      return x[i]()<x[j]();
    }
  };
  
  template<class Lit>
  class LinearTraits {
  };
  template<>
  class LinearTraits<IntLit> {
  public:
    typedef IntVal Val;
    static Val eval(EnvI& env, Expression* e) { return eval_int(env,e); }
    static void constructLinBuiltin(BinOpType bot, ASTString& callid, int& coeff_sign, Val& d) {
      switch (bot) {
        case BOT_LE:
          callid = constants().ids.int_.lin_le;
          coeff_sign = 1;
          d += 1;
          break;
        case BOT_LQ:
          callid = constants().ids.int_.lin_le;
          coeff_sign = 1;
          break;
        case BOT_GR:
          callid = constants().ids.int_.lin_le;
          coeff_sign = -1;
          d = -d+1;
          break;
        case BOT_GQ:
          callid = constants().ids.int_.lin_le;
          coeff_sign = -1;
          d = -d;
          break;
        case BOT_EQ:
          callid = constants().ids.int_.lin_eq;
          coeff_sign = 1;
          break;
        case BOT_NQ:
          callid = constants().ids.int_.lin_ne;
          coeff_sign = 1;
          break;
        default: assert(false); break;
      }
    }
    static ASTString id_eq(void) { return constants().ids.int_.eq; }
    typedef IntBounds Bounds;
    static bool finite(const IntBounds& ib) { return ib.l.isFinite() && ib.u.isFinite(); }
    static bool finite(const IntVal& v) { return v.isFinite(); }
    static Bounds compute_bounds(EnvI& env, Expression* e) { return compute_int_bounds(env,e); }
    typedef IntSetVal* Domain;
    static Domain eval_domain(EnvI& env, Expression* e) { return eval_intset(env, e); }
    static Expression* new_domain(Val v) { return new SetLit(Location().introduce(),IntSetVal::a(v,v)); }
    static Expression* new_domain(Val v0, Val v1) { return new SetLit(Location().introduce(),IntSetVal::a(v0,v1)); }
    static Expression* new_domain(Domain d) { return new SetLit(Location().introduce(),d); }
    static bool domain_contains(Domain dom, Val v) { return dom->contains(v); }
    static bool domain_equals(Domain dom, Val v) { return dom->size()==1 && dom->min(0)==v && dom->max(0)==v; }
    static bool domain_equals(Domain dom1, Domain dom2) {
      IntSetRanges d1(dom1);
      IntSetRanges d2(dom2);
      return Ranges::equal(d1,d2);
    }
    static bool domain_tighter(Domain dom, Bounds b) {
      return !b.valid || dom->min() > b.l || dom->max() < b.u;
    }
    static bool domain_intersects(Domain dom, Val v0, Val v1) {
      return (v0 > v1) || (dom->size() > 0 && dom->min(0) <= v1 && v0 <= dom->max(dom->size()-1));
    }
    static bool domain_empty(Domain dom) { return dom->size()==0; }
    static Domain limit_domain(BinOpType bot, Domain dom, Val v) {
      IntSetRanges dr(dom);
      IntSetVal* ndomain;
      switch (bot) {
        case BOT_LE:
          v -= 1;
          // fall through
        case BOT_LQ:
        {
          Ranges::Bounded<IntVal,IntSetRanges> b = Ranges::Bounded<IntVal,IntSetRanges>::maxiter(dr,v);
          ndomain = IntSetVal::ai(b);
        }
          break;
        case BOT_GR:
          v += 1;
          // fall through
        case BOT_GQ:
        {
          Ranges::Bounded<IntVal,IntSetRanges> b = Ranges::Bounded<IntVal,IntSetRanges>::miniter(dr,v);
          ndomain = IntSetVal::ai(b);
        }
          break;
        case BOT_NQ:
        {
          Ranges::Const<IntVal> c(v,v);
          Ranges::Diff<IntVal,IntSetRanges,Ranges::Const<IntVal> > d(dr,c);
          ndomain = IntSetVal::ai(d);
        }
          break;
        default: assert(false); return NULL;
      }
      return ndomain;
    }
    static Domain intersect_domain(Domain dom, Val v0, Val v1) {
      IntSetRanges dr(dom);
      Ranges::Const<IntVal> c(v0,v1);
      Ranges::Inter<IntVal,IntSetRanges,Ranges::Const<IntVal> > inter(dr,c);
      return IntSetVal::ai(inter);
    }
    static Val floor_div(Val v0, Val v1) {
      return static_cast<long long int>(floor(static_cast<double>(v0.toInt()) / static_cast<double>(v1.toInt())));
    }
    static Val ceil_div(Val v0, Val v1) { return static_cast<long long int>(ceil(static_cast<double>(v0.toInt()) / v1.toInt())); }
    static IntLit* newLit(Val v) { return IntLit::a(v); }
  };
  template<>
  class LinearTraits<FloatLit> {
  public:
    typedef FloatVal Val;
    static Val eval(EnvI& env, Expression* e) { return eval_float(env,e); }
    static void constructLinBuiltin(BinOpType bot, ASTString& callid, int& coeff_sign, Val& d) {
      switch (bot) {
        case BOT_LE:
          callid = constants().ids.float_.lin_lt;
          coeff_sign = 1;
          break;
        case BOT_LQ:
          callid = constants().ids.float_.lin_le;
          coeff_sign = 1;
          break;
        case BOT_GR:
          callid = constants().ids.float_.lin_lt;
          coeff_sign = -1;
          d = -d;
          break;
        case BOT_GQ:
          callid = constants().ids.float_.lin_le;
          coeff_sign = -1;
          d = -d;
          break;
        case BOT_EQ:
          callid = constants().ids.float_.lin_eq;
          coeff_sign = 1;
          break;
        case BOT_NQ:
          callid = constants().ids.float_.lin_ne;
          coeff_sign = 1;
          break;
        default: assert(false); break;
      }
    }
    static ASTString id_eq(void) { return constants().ids.float_.eq; }
    typedef FloatBounds Bounds;
    static bool finite(const FloatBounds& ib) { return ib.l.isFinite() && ib.u.isFinite(); }
    static bool finite(const FloatVal& v) { return v.isFinite(); }
    static Bounds compute_bounds(EnvI& env, Expression* e) { return compute_float_bounds(env,e); }
    typedef FloatSetVal* Domain;
    static Domain eval_domain(EnvI& env, Expression* e) { return eval_floatset(env, e); }

    static Expression* new_domain(Val v) { return new SetLit(Location().introduce(),FloatSetVal::a(v,v)); }
    static Expression* new_domain(Val v0, Val v1) { return new SetLit(Location().introduce(),FloatSetVal::a(v0,v1)); }
    static Expression* new_domain(Domain d) { return new SetLit(Location().introduce(),d); }
    static bool domain_contains(Domain dom, Val v) { return dom->contains(v); }
    static bool domain_equals(Domain dom, Val v) { return dom->size()==1 && dom->min(0)==v && dom->max(0)==v; }
    
    static bool domain_tighter(Domain dom, Bounds b) {
      return !b.valid || dom->min() > b.l || dom->max() < b.u;
    }
    static bool domain_intersects(Domain dom, Val v0, Val v1) {
      return (v0 > v1) || (dom->size() > 0 && dom->min(0) <= v1 && v0 <= dom->max(dom->size()-1));
    }
    static bool domain_empty(Domain dom) { return dom->size()==0; }

    static bool domain_equals(Domain dom1, Domain dom2) {
      FloatSetRanges d1(dom1);
      FloatSetRanges d2(dom2);
      return Ranges::equal(d1,d2);
    }

    static Domain intersect_domain(Domain dom, Val v0, Val v1) {
      if (dom) {
        FloatSetRanges dr(dom);
        Ranges::Const<FloatVal> c(v0,v1);
        Ranges::Inter<FloatVal,FloatSetRanges,Ranges::Const<FloatVal> > inter(dr,c);
        return FloatSetVal::ai(inter);
      } else {
        Domain d = FloatSetVal::a(v0,v1);
        return d;
      }
    }
    static Domain limit_domain(BinOpType bot, Domain dom, Val v) {
      FloatSetRanges dr(dom);
      FloatSetVal* ndomain;
      switch (bot) {
        case BOT_LE:
          return NULL;
        case BOT_LQ:
        {
          Ranges::Bounded<FloatVal,FloatSetRanges> b = Ranges::Bounded<FloatVal,FloatSetRanges>::maxiter(dr,v);
          ndomain = FloatSetVal::ai(b);
        }
          break;
        case BOT_GR:
          return NULL;
        case BOT_GQ:
        {
          Ranges::Bounded<FloatVal,FloatSetRanges> b = Ranges::Bounded<FloatVal,FloatSetRanges>::miniter(dr,v);
          ndomain = FloatSetVal::ai(b);
        }
          break;
        case BOT_NQ:
        {
          Ranges::Const<FloatVal> c(v,v);
          Ranges::Diff<FloatVal,FloatSetRanges,Ranges::Const<FloatVal> > d(dr,c);
          ndomain = FloatSetVal::ai(d);
        }
          break;
        default: assert(false); return NULL;
      }
      return ndomain;
    }
    static Val floor_div(Val v0, Val v1) { return v0 / v1; }
    static Val ceil_div(Val v0, Val v1) { return v0 / v1; }
    static FloatLit* newLit(Val v) { return FloatLit::a(v); }
  };

  template<class Lit>
  void simplify_lin(std::vector<typename LinearTraits<Lit>::Val>& c,
                    std::vector<KeepAlive>& x,
                    typename LinearTraits<Lit>::Val& d) {
    std::vector<int> idx(c.size());
    for (unsigned int i=static_cast<unsigned int>(idx.size()); i--;) {
      idx[i]=i;
      Expression* e = follow_id_to_decl(x[i]());
      if (VarDecl* vd = e->dyn_cast<VarDecl>()) {
        if (vd->e() && vd->e()->isa<Lit>()) {
          x[i] = vd->e();
        } else {
          x[i] = e->cast<VarDecl>()->id();
        }
      } else {
        x[i] = e;
      }
    }
    std::sort(idx.begin(),idx.end(),CmpExpIdx(x));
    unsigned int ci = 0;
    for (; ci<x.size(); ci++) {
      if (Lit* il = x[idx[ci]]()->dyn_cast<Lit>()) {
        d += c[idx[ci]]*il->v();
        c[idx[ci]] = 0;
      } else {
        break;
      }
    }
    for (unsigned int i=ci+1; i<x.size(); i++) {
      if (Expression::equal(x[idx[i]](),x[idx[ci]]())) {
        c[idx[ci]] += c[idx[i]];
        c[idx[i]] = 0;
      } else if (Lit* il = x[idx[i]]()->dyn_cast<Lit>()) {
        d += c[idx[i]]*il->v();
        c[idx[i]] = 0;
      } else {
        ci=i;
      }
    }
    ci = 0;
    for (unsigned int i=0; i<c.size(); i++) {
      if (c[i] != 0) {
        c[ci] = c[i];
        x[ci] = x[i];
        ci++;
      }
    }
    c.resize(ci);
    x.resize(ci);
  }

}

#endif
