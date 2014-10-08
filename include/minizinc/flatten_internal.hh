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
    std::ostream& evalOutput(std::ostream& os);
  };

  Expression* follow_id(Expression* e);
  Expression* follow_id_to_decl(Expression* e);

  EE flat_exp(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);

}

#endif
