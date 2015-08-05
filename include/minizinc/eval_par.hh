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

#include <minizinc/prettyprinter.hh>

namespace MiniZinc {
  
  /// Evaluate par int expression \a e, \a om is true if \a e is part of the output model
  IntVal eval_int(EnvI& env, Expression* e, bool om = false);
  /// Evaluate par bool expression \a e, \a om is true if if \a e is part of the output model
  bool eval_bool(EnvI& env, Expression* e, bool om = false);
  /// Evaluate par float expression \a e, \a om is true if \a e is part of the output model
  FloatVal eval_float(EnvI& env, Expression* e, bool om = false);
  /// Evaluate an array expression \a e into an array literal, \a om is true if if \a e is part of the output model
  ArrayLit* eval_array_lit(EnvI& env, Expression* e, bool om = false);
  /// Evaluate an access to array \a with indices \a idx and return whether
  /// access succeeded in \a success
  Expression* eval_arrayaccess(EnvI& env, ArrayLit* a, const std::vector<IntVal>& idx, bool& success, bool om);
  /// Evaluate an array access \a e and return whether access succeeded in \a success
  Expression* eval_arrayaccess(EnvI& env, ArrayAccess* e, bool& success, bool om);
  /// Evaluate a par integer set \a e
  IntSetVal* eval_intset(EnvI& env, Expression* e, bool om = false);
  /// Evaluate a par bool set \a e
  IntSetVal* eval_boolset(EnvI& env, Expression* e, bool om = false);
  /// Evaluate a par string \a e
  std::string eval_string(EnvI& env, Expression* e, bool om = false);
  /// Evaluate a par expression \a e and return it wrapped in a literal; \a om is true if if \a e is part of the output model
  Expression* eval_par(EnvI& env, Expression* e, bool om = false);
  
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
                  KeepAlive in, std::vector<typename Eval::ArrayVal>& a, bool om = false);

  template<class Eval>
  void
  eval_comp_set(EnvI& env, Eval& eval, Comprehension* e, int gen, int id,
                KeepAlive in, std::vector<typename Eval::ArrayVal>& a, bool om = false);

  template<class Eval>
  void
  eval_comp_set(EnvI& env, Eval& eval, Comprehension* e, int gen, int id,
                IntVal i, KeepAlive in, std::vector<typename Eval::ArrayVal>& a, bool om) {
    e->decl(gen,id)->e()->cast<IntLit>()->v(i);
    CallStackItem csi(env, e->decl(gen,id)->id(), i);
    if (id == e->n_decls(gen)-1) {
      if (gen == e->n_generators()-1) {
        bool where = true;
        if (e->where() != NULL && !e->where()->type().isvar()) {
          GCLock lock;
          where = eval_bool(env, e->where(),om); 
        }
        if (where) {
          a.push_back(eval.e(env,e->e()));
        }
      } else {
        KeepAlive nextin;
        {
          if (e->in(gen+1)->type().dim()==0) {
            GCLock lock;
            nextin = new SetLit(Location(),eval_intset(env, e->in(gen+1),om)); 
          } else {
            GCLock lock;
            nextin = eval_array_lit(env, e->in(gen+1),om); 
          }
        }
        if (e->in(gen+1)->type().dim()==0) {
          eval_comp_set<Eval>(env, eval,e,gen+1,0,nextin,a,om);
        } else {
          eval_comp_array<Eval>(env, eval,e,gen+1,0,nextin,a,om); 
        }
      }
    } else {
      eval_comp_set<Eval>(env, eval,e,gen,id+1,in,a,om);
    }
  }

  template<class Eval>
  void
  eval_comp_array(EnvI& env, Eval& eval, Comprehension* e, int gen, int id,
                  IntVal i, KeepAlive in, std::vector<typename Eval::ArrayVal>& a,
                  bool om) {
    ArrayLit* al = in()->cast<ArrayLit>();
    CallStackItem csi(env, e->decl(gen,id)->id(), i);
    e->decl(gen,id)->e(al->v()[i.toInt()]);
    e->rehash();
    if (id == e->n_decls(gen)-1) {
      if (gen == e->n_generators()-1) {
        bool where = true;
        if (e->where() != NULL) {
          GCLock lock;
          where = eval_bool(env, e->where(),om);
        }
        if (where) {
          a.push_back(eval.e(env,e->e()));
        }
      } else {
        KeepAlive nextin;
        {
          if (e->in(gen+1)->type().dim()==0) {
            GCLock lock;
            nextin = new SetLit(Location(),eval_intset(env,e->in(gen+1),om));
          } else {
            GCLock lock;
            nextin = eval_array_lit(env, e->in(gen+1),om);
          }
        }
        if (e->in(gen+1)->type().dim()==0) {
          eval_comp_set<Eval>(env, eval,e,gen+1,0,nextin,a,om); 
        } else {
          eval_comp_array<Eval>(env, eval,e,gen+1,0,nextin,a,om);
        }
      }
    } else {
      eval_comp_array<Eval>(env, eval,e,gen,id+1,in,a,om);
    }
    e->decl(gen,id)->e(NULL);
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
                KeepAlive in, std::vector<typename Eval::ArrayVal>& a, bool om) {
    IntSetVal* isv = eval_intset(env, in(),om);
    IntSetRanges rsi(isv);
    Ranges::ToValues<IntSetRanges> rsv(rsi);
    for (; rsv(); ++rsv) {
      eval_comp_set<Eval>(env, eval,e,gen,id,rsv.val(),in,a,om);
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
                  KeepAlive in, std::vector<typename Eval::ArrayVal>& a, bool om) {
    ArrayLit* al = in()->cast<ArrayLit>();
    for (unsigned int i=0; i<al->v().size(); i++) {
      eval_comp_array<Eval>(env, eval,e,gen,id,i,in,a,om);
    }
  }

  /**
   * \brief Evaluate comprehension expression
   * 
   * Calls \a eval.e for every element of the comprehension \a e and
   * returns a vector with all the evaluated results. Flag \a eval_om
   * is true if we are evaluating an expression in an output model
   */
  template<class Eval>
  std::vector<typename Eval::ArrayVal>
  eval_comp(EnvI& env, Eval& eval, Comprehension* e, bool om = false) {
    std::vector<typename Eval::ArrayVal> a;
    KeepAlive in;
    {
      GCLock lock;
      if (e->in(0)->type().dim()==0) {
        if (e->in(0)->type().isvar()) {
          in = new SetLit(Location(),compute_intset_bounds(env, e->in(0)));
        } else {
          in = new SetLit(Location(),eval_intset(env, e->in(0),om));
        }
      } else {
        in = eval_array_lit(env, e->in(0), om);
      }
    }
    if (e->in(0)->type().dim()==0) {
      eval_comp_set<Eval>(env, eval,e,0,0,in,a,om);
    } else {
      eval_comp_array<Eval>(env, eval,e,0,0,in,a,om); 
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
  eval_comp(EnvI& env, Comprehension* e, bool om) {
    Eval eval;
    return eval_comp(env, eval,e,om);
  }  
  
}

#endif
