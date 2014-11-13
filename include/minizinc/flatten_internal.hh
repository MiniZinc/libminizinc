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
    Ctx(void) : b(C_ROOT), i(C_POS), neg(false) {}
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
    Model* orig;
    Model* output;
    VarOccurrences vo;
    VarOccurrences output_vo;
    CopyMap cmap;
    IdMap<KeepAlive> reverseMappers;
    struct WW {
      WeakRef r;
      WeakRef b;
      WW(WeakRef r0, WeakRef b0) : r(r0), b(b0) {}
    };
    typedef KeepAliveMap<WW> Map;
    bool ignorePartial;
    std::vector<const Expression*> callStack;
    std::vector<const Expression*> errorStack;
    std::vector<int> idStack;
    std::vector<std::string> warnings;
  protected:
    Map map;
    Model* _flat;
    unsigned int ids;
    ASTStringMap<ASTString>::t reifyMap;
  public:
    EnvI(Model* orig0);
    ~EnvI(void);
    long long int genId(void);
    void map_insert(Expression* e, const EE& ee);
    Map::iterator map_find(Expression* e);
    void map_remove(Expression* e);
    Map::iterator map_end(void);
    void dump(void);
    
    void flat_addItem(Item* i);
    void vo_add_exp(VarDecl* vd);
    Model* flat(void);
    ASTString reifyId(const ASTString& id);
    std::ostream& dumpStack(std::ostream& os, bool errStack);
    void addWarning(const std::string& msg);
  };

  Expression* follow_id(Expression* e);
  Expression* follow_id_to_decl(Expression* e);
  Expression* follow_id_to_value(Expression* e);

  EE flat_exp(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);

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
    static Val eval(Expression* e) { return eval_int(e); }
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
    static Bounds compute_bounds(Expression* e) { return compute_int_bounds(e); }
    typedef IntSetVal* Domain;
    static Domain eval_domain(Expression* e) { return eval_intset(e); }
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
    static bool domain_intersects(Domain dom, Val v0, Val v1) {
      return dom->min(0) <= v1 && v0 <= dom->max(dom->size()-1);
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
          Ranges::Bounded<IntSetRanges> b = Ranges::Bounded<IntSetRanges>::maxiter(dr,v);
          ndomain = IntSetVal::ai(b);
        }
          break;
        case BOT_GR:
          v += 1;
          // fall through
        case BOT_GQ:
        {
          Ranges::Bounded<IntSetRanges> b = Ranges::Bounded<IntSetRanges>::miniter(dr,v);
          ndomain = IntSetVal::ai(b);
        }
          break;
        case BOT_NQ:
        {
          Ranges::Const c(v,v);
          Ranges::Diff<IntSetRanges,Ranges::Const> d(dr,c);
          ndomain = IntSetVal::ai(d);
        }
          break;
        default: assert(false); return NULL;
      }
      return ndomain;
    }
    static Domain intersect_domain(Domain dom, Val v0, Val v1) {
      IntSetRanges dr(dom);
      Ranges::Const c(v0,v1);
      Ranges::Inter<IntSetRanges,Ranges::Const> inter(dr,c);
      return IntSetVal::ai(inter);
    }
    static Val floor_div(Val v0, Val v1) {
      return static_cast<long long int>(floor(static_cast<FloatVal>(v0.toInt()) / static_cast<FloatVal>(v1.toInt())));
    }
    static Val ceil_div(Val v0, Val v1) { return static_cast<long long int>(ceil(static_cast<FloatVal>(v0.toInt()) / v1.toInt())); }
  };
  template<>
  class LinearTraits<FloatLit> {
  public:
    typedef FloatVal Val;
    static Val eval(Expression* e) { return eval_float(e); }
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
    static bool finite(const FloatBounds& ib) { return true; }
    static Bounds compute_bounds(Expression* e) { return compute_float_bounds(e); }
    typedef BinOp* Domain;
    static Domain eval_domain(Expression* e) {
      BinOp* bo = e->cast<BinOp>();
      assert(bo->op() == BOT_DOTDOT);
      if (bo->lhs()->isa<FloatLit>() && bo->rhs()->isa<FloatLit>())
        return bo;
      BinOp* ret = new BinOp(bo->loc(),eval_par(bo->lhs()),BOT_DOTDOT,eval_par(bo->rhs()));
      ret->type(bo->type());
      return ret;
    }
    static Expression* new_domain(Val v) {
      BinOp* ret = new BinOp(Location().introduce(),new FloatLit(Location().introduce(),v),BOT_DOTDOT,new FloatLit(Location().introduce(),v));
      ret->type(Type::parsetfloat());
      return ret;
    }
    static Expression* new_domain(Val v0, Val v1) {
      BinOp* ret = new BinOp(Location().introduce(),new FloatLit(Location().introduce(),v0),BOT_DOTDOT,new FloatLit(Location().introduce(),v1));
      ret->type(Type::parsetfloat());
      return ret;
    }
    static Expression* new_domain(Domain d) { return d; }
    static bool domain_contains(Domain dom, Val v) {
      return dom==NULL || (dom->lhs()->cast<FloatLit>()->v() <= v && dom->rhs()->cast<FloatLit>()->v() >= v);
    }
    static bool domain_intersects(Domain dom, Val v0, Val v1) {
      return dom==NULL || (dom->lhs()->cast<FloatLit>()->v() <= v1 && dom->rhs()->cast<FloatLit>()->v() >= v0);
    }
    static bool domain_equals(Domain dom, Val v) {
      return dom != NULL && dom->lhs()->cast<FloatLit>()->v() == v && dom->rhs()->cast<FloatLit>()->v() == v;
    }
    static bool domain_equals(Domain dom1, Domain dom2) {
      if (dom1==dom2) return true;
      if (dom1==NULL || dom2==NULL) return false;
      return
      dom1->lhs()->cast<FloatLit>()->v() == dom2->lhs()->cast<FloatLit>()->v() &&
      dom1->rhs()->cast<FloatLit>()->v() == dom2->rhs()->cast<FloatLit>()->v();
    }
    static bool domain_empty(Domain dom) {
      return dom != NULL && dom->lhs()->cast<FloatLit>()->v() > dom->rhs()->cast<FloatLit>()->v();
    }
    static Domain intersect_domain(Domain dom, Val v0, Val v1) {
      if (dom) {
        Val lb = dom->lhs()->cast<FloatLit>()->v();
        Val ub = dom->rhs()->cast<FloatLit>()->v();
        lb = std::max(lb,v0);
        ub = std::min(ub,v1);
        Domain d = new BinOp(Location().introduce(), new FloatLit(Location().introduce(),lb), BOT_DOTDOT, new FloatLit(Location().introduce(),ub));
        d->type(Type::parsetfloat());
        return d;
      } else {
        Domain d = new BinOp(Location().introduce(), new FloatLit(Location().introduce(),v0), BOT_DOTDOT, new FloatLit(Location().introduce(),v1));
        d->type(Type::parsetfloat());
        return d;
      }
    }
    static Domain limit_domain(BinOpType bot, Domain dom, Val v) {
      if (dom) {
        Val lb = dom->lhs()->cast<FloatLit>()->v();
        Val ub = dom->rhs()->cast<FloatLit>()->v();
        Domain ndomain;
        switch (bot) {
          case BOT_LE:
            return NULL;
          case BOT_LQ:
            if (v < ub) {
              Domain d = new BinOp(dom->loc(),dom->lhs(),BOT_DOTDOT,new FloatLit(Location().introduce(),v));
              d->type(Type::parsetfloat());
              return d;
            } else {
              return dom;
            }
          case BOT_GR:
            return NULL;
          case BOT_GQ:
            if (v > lb) {
              Domain d = new BinOp(dom->loc(),new FloatLit(Location().introduce(),v),BOT_DOTDOT,dom->rhs());
              d->type(Type::parsetfloat());
              return d;
            } else {
              return dom;
            }
          case BOT_NQ:
            return NULL;
          default: assert(false); return NULL;
        }
        return ndomain;
      }
      return NULL;
    }
    static Val floor_div(Val v0, Val v1) { return v0 / v1; }
    static Val ceil_div(Val v0, Val v1) { return v0 / v1; }
  };

  template<class Lit>
  void simplify_lin(std::vector<typename LinearTraits<Lit>::Val>& c,
                    std::vector<KeepAlive>& x,
                    typename LinearTraits<Lit>::Val& d) {
    std::vector<int> idx(c.size());
    for (unsigned int i=idx.size(); i--;) {
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
