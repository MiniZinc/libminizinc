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

namespace MiniZinc {
  
  /// Evaluate par int expression \a e
  IntVal eval_int(Expression* e);
  /// Evaluate par bool expression \a e
  bool eval_bool(Expression* e);
  /// Evaluate par float expression \a e
  FloatVal eval_float(Expression* e);
  /// Evaluate an array expression \a e into an array literal
  ArrayLit* eval_array_lit(Expression* e);
  /// Evaluate an access to array \a with indices \a idx and return whether
  /// access succeeded in \a success
  Expression* eval_arrayaccess(ArrayLit* a, const std::vector<IntVal>& idx, bool& success);
  /// Evaluate an array access \a e and return whether access succeeded in \a success
  Expression* eval_arrayaccess(ArrayAccess* e, bool& success);
  /// Evaluate a par integer set \a e
  IntSetVal* eval_intset(Expression* e);
  /// Evaluate a par string \a e
  std::string eval_string(Expression* e);
  /// Evaluate a par expression \a e and return it wrapped in a literal
  Expression* eval_par(Expression* e);

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
  IntBounds compute_int_bounds(Expression* e);

  /**
   * \brief Compute bounds of a set of int expression
   *
   * Returns NULL if bounds cannot be determined
   */
  IntSetVal* compute_intset_bounds(Expression* e);

  template<class Eval>
  void
  eval_comp(Eval& eval, Comprehension* e, int gen, int id,
            KeepAlive in, std::vector<typename Eval::ArrayVal>& a);

  template<class Eval>
  void
  eval_comp(Eval& eval, Comprehension* e, int gen, int id,
            int i, KeepAlive in, std::vector<typename Eval::ArrayVal>& a) {
    e->decl(gen,id)->e()->cast<IntLit>()->v(i);
    if (id == e->n_decls(gen)-1) {
      if (gen == e->n_generators()-1) {
        bool where = true;
        if (e->where() != NULL) {
          GCLock lock;
          where = eval_bool(e->where());
        }
        if (where) {
          a.push_back(eval.e(e->e()));
        }
      } else {
        KeepAlive nextin;
        {
          GCLock lock;
          nextin = new SetLit(Location(),eval_intset(e->in(gen+1)));
        }
        eval_comp<Eval>(eval,e,gen+1,0,nextin,a);
      }
    } else {
      eval_comp<Eval>(eval,e,gen,id+1,in,a);
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
  eval_comp(Eval& eval, Comprehension* e, int gen, int id,
            KeepAlive in, std::vector<typename Eval::ArrayVal>& a) {
    IntSetRanges rsi(in()->cast<SetLit>()->isv());
    Ranges::ToValues<IntSetRanges> rsv(rsi);
    for (; rsv(); ++rsv) {
      eval_comp<Eval>(eval,e,gen,id,rsv.val(),in,a);
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
  eval_comp(Eval& eval, Comprehension* e) {
    std::vector<typename Eval::ArrayVal> a;
    KeepAlive in;
    {
      GCLock lock;
      in = new SetLit(Location(),eval_intset(e->in(0)));
    }
    eval_comp<Eval>(eval,e,0,0,in,a);
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
  eval_comp(Comprehension* e) {
    Eval eval;
    return eval_comp(eval,e);
  }  
}

#endif
