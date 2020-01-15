/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_EVAL_PAR_HH__
#define __MINIZINC_EVAL_PAR_HH__

#include <minizinc/model.hh>
#include <minizinc/iter.hh>
#include <minizinc/astexception.hh>

#include <minizinc/prettyprinter.hh>

namespace MiniZinc {
  
  /// Evaluate par int expression \a e
  IntVal eval_int(EnvI& env, Expression* e);
  /// Evaluate par bool expression \a e
  bool eval_bool(EnvI& env, Expression* e);
  /// Evaluate par float expression \a e
  FloatVal eval_float(EnvI& env, Expression* e);
  /// Evaluate an array expression \a e into an array literal
  ArrayLit* eval_array_lit(EnvI& env, Expression* e);
  /// Evaluate an access to array \a with indices \a idx and return whether
  /// access succeeded in \a success
  Expression* eval_arrayaccess(EnvI& env, ArrayLit* a, const std::vector<IntVal>& idx, bool& success);
  /// Evaluate an array access \a e and return whether access succeeded in \a success
  Expression* eval_arrayaccess(EnvI& env, ArrayAccess* e, bool& success);
  /// Evaluate a par integer set \a e
  IntSetVal* eval_intset(EnvI& env, Expression* e);
  /// Evaluate a par bool set \a e
  IntSetVal* eval_boolset(EnvI& env, Expression* e);
  /// Evaluate a par float set \a e
  FloatSetVal* eval_floatset(EnvI& env, Expression* e);
  /// Evaluate a par string \a e
  std::string eval_string(EnvI& env, Expression* e);
  /// Evaluate a par expression \a e and return it wrapped in a literal
  Expression* eval_par(EnvI& env, Expression* e);
  /// Check if variable declaration \a vd satisfies the domain and index set constraints
  void checkParDeclaration(EnvI& env, VarDecl* vd);
  
  /// Representation for bounds of an integer expression
  struct IntBounds {
    /// Lower bound
    IntVal l;
    /// Upper bound
    IntVal u;
    /// Whether the bounds are valid
    bool valid;
    /// Constructor
    IntBounds(IntVal l0, IntVal u0, bool valid0)
      : l(l0), u(u0), valid(valid0) {}
  };
  
  /// Compute bounds of an integer expression
  IntBounds compute_int_bounds(EnvI& env, Expression* e);

  /// Representation for bounds of a float expression
  struct FloatBounds {
    /// Lower bound
    FloatVal l;
    /// Upper bound
    FloatVal u;
    /// Whether the bounds are valid
    bool valid;
    /// Constructor
    FloatBounds(FloatVal l0, FloatVal u0, bool valid0)
    : l(l0), u(u0), valid(valid0) {}
  };
  
  /// Compute bounds of an integer expression
  FloatBounds compute_float_bounds(EnvI& env, Expression* e);
  
  
  /**
   * \brief Compute bounds of a set of int expression
   *
   * Returns NULL if bounds cannot be determined
   */
  IntSetVal* compute_intset_bounds(EnvI& env, Expression* e);

  template<class Eval>
  void
  eval_comp_array(EnvI& env, Eval& eval, Comprehension* e, int gen, int id,
                  KeepAlive in, std::vector<typename Eval::ArrayVal>& a);

  template<class Eval>
  void
  eval_comp_set(EnvI& env, Eval& eval, Comprehension* e, int gen, int id,
                KeepAlive in, std::vector<typename Eval::ArrayVal>& a);

  template<class Eval>
  void
  eval_comp_set(EnvI& env, Eval& eval, Comprehension* e, int gen, int id,
                IntVal i, KeepAlive in, std::vector<typename Eval::ArrayVal>& a) {
    {
      GCLock lock;
      GC::mark();
      e->decl(gen,id)->trail();
      e->decl(gen,id)->e(IntLit::a(i));
    }
    CallStackItem csi(env, e->decl(gen,id)->id(), i);
    if (id == e->n_decls(gen)-1) {
      bool where = true;
      if (e->where(gen) != NULL) {
        GCLock lock;
        where = e->where(gen)->type().isvar() ? true : eval_bool(env, e->where(gen));
      }
      if (where) {
        if (gen == e->n_generators()-1) {
          a.push_back(eval.e(env,e->e()));
        } else {
          if (e->in(gen+1)==NULL) {
            eval_comp_array<Eval>(env, eval,e,gen+1,0,0,e->in(gen+1),a);
          } else {
            KeepAlive nextin;
            if (e->in(gen+1)->type().dim()==0) {
              GCLock lock;
              nextin = new SetLit(Location(),eval_intset(env, e->in(gen+1)));
            } else {
              GCLock lock;
              nextin = eval_array_lit(env, e->in(gen+1));
            }
            if (e->in(gen+1)->type().dim()==0) {
              eval_comp_set<Eval>(env, eval,e,gen+1,0,nextin,a);
            } else {
              eval_comp_array<Eval>(env, eval,e,gen+1,0,nextin,a);
            }
          }
        }
      }
    } else {
      eval_comp_set<Eval>(env, eval,e,gen,id+1,in,a);
    }
    GC::untrail();
    e->decl(gen,id)->flat(NULL);
  }

  template<class Eval>
  void
  eval_comp_array(EnvI& env, Eval& eval, Comprehension* e, int gen, int id,
                  IntVal i, KeepAlive in, std::vector<typename Eval::ArrayVal>& a) {
    GC::mark();
    e->decl(gen,id)->trail();
    CallStackItem csi(env, e->decl(gen,id)->id(), i);
    if (in()==NULL) {
      // this is an assignment generator
      Expression* asn = e->where(gen)->type().ispar() ? eval_par(env, e->where(gen)) : eval.flatten(env,e->where(gen));
      e->decl(gen,id)->e(asn);
      e->rehash();
    } else {
      ArrayLit* al = in()->cast<ArrayLit>();
      e->decl(gen,id)->e((*al)[static_cast<int>(i.toInt())]);
      e->rehash();
    }
    if (id == e->n_decls(gen)-1) {
      bool where = true;
      if (e->in(gen) != NULL && e->where(gen) != NULL) {
        GCLock lock;
        where = e->where(gen)->type().isvar() ? true : eval_bool(env, e->where(gen));
      }
      if (where) {
        if (gen == e->n_generators()-1) {
          a.push_back(eval.e(env,e->e()));
        } else {
          if (e->in(gen+1)==NULL) {
            eval_comp_array<Eval>(env, eval,e,gen+1,0,0,e->in(gen+1),a);
          } else {
            KeepAlive nextin;
            if (e->in(gen+1)->type().dim()==0) {
              GCLock lock;
              nextin = new SetLit(Location(),eval_intset(env,e->in(gen+1)));
            } else {
              GCLock lock;
              nextin = eval_array_lit(env, e->in(gen+1));
            }
            if (e->in(gen+1)->type().dim()==0) {
              eval_comp_set<Eval>(env, eval,e,gen+1,0,nextin,a);
            } else {
              eval_comp_array<Eval>(env, eval,e,gen+1,0,nextin,a);
            }
          }
        }
      }
    } else {
      eval_comp_array<Eval>(env, eval,e,gen,id+1,in,a);
    }
    GC::untrail();
    e->decl(gen,id)->flat(NULL);
  }

  /**
   * \brief Evaluate comprehension expression
   * 
   * Calls \a eval.e for every element of the comprehension \a e,
   * where \a gen is the current generator, \a id is the current identifier
   * in that generator, \a in is the expression of that generator, and
   * \a a is the array in which to place the result.
   */
  template<class Eval>
  void
  eval_comp_set(EnvI& env, Eval& eval, Comprehension* e, int gen, int id,
                KeepAlive in, std::vector<typename Eval::ArrayVal>& a) {
    IntSetVal* isv = eval_intset(env, in());
    if (isv->card().isPlusInfinity()) {
      throw EvalError(env,in()->loc(),"comprehension iterates over an infinite set");
    }
    IntSetRanges rsi(isv);
    Ranges::ToValues<IntSetRanges> rsv(rsi);
    for (; rsv(); ++rsv) {
      eval_comp_set<Eval>(env, eval,e,gen,id,rsv.val(),in,a);
    }
  }

  /**
   * \brief Evaluate comprehension expression
   *
   * Calls \a eval.e for every element of the comprehension \a e,
   * where \a gen is the current generator, \a id is the current identifier
   * in that generator, \a in is the expression of that generator, and
   * \a a is the array in which to place the result.
   */
  template<class Eval>
  void
  eval_comp_array(EnvI& env, Eval& eval, Comprehension* e, int gen, int id,
                  KeepAlive in, std::vector<typename Eval::ArrayVal>& a) {
    ArrayLit* al = in()->cast<ArrayLit>();
    for (unsigned int i=0; i<al->size(); i++) {
      eval_comp_array<Eval>(env, eval,e,gen,id,i,in,a);
    }
  }

  /**
   * \brief Evaluate comprehension expression
   * 
   * Calls \a eval.e for every element of the comprehension \a e and
   * returns a vector with all the evaluated results.
   */
  template<class Eval>
  std::vector<typename Eval::ArrayVal>
  eval_comp(EnvI& env, Eval& eval, Comprehension* e) {
    std::vector<typename Eval::ArrayVal> a;
    if (e->in(0)==NULL) {
      eval_comp_array<Eval>(env, eval,e,0,0,0,e->in(0),a);
    } else {
      KeepAlive in;
      {
        GCLock lock;
        if (e->in(0)->type().dim()==0) {
          if (e->in(0)->type().isvar()) {
            in = new SetLit(Location(),compute_intset_bounds(env, e->in(0)));
          } else {
            in = new SetLit(Location(),eval_intset(env, e->in(0)));
          }
        } else {
          in = eval_array_lit(env, e->in(0));
        }
      }
      if (e->in(0)->type().dim()==0) {
        eval_comp_set<Eval>(env, eval,e,0,0,in,a);
      } else {
        eval_comp_array<Eval>(env, eval,e,0,0,in,a);
      }
    }
    return a;
  }  

  /**
   * \brief Evaluate comprehension expression
   * 
   * Calls \a Eval::e for every element of the comprehension \a e and
   * returns a vector with all the evaluated results.
   */
  template<class Eval>
  std::vector<typename Eval::ArrayVal>
  eval_comp(EnvI& env, Comprehension* e) {
    Eval eval;
    return eval_comp(env, eval,e);
  }  
  
  Expression* follow_id(Expression* e);
  Expression* follow_id_to_decl(Expression* e);
  Expression* follow_id_to_value(Expression* e);
  
}

#endif
