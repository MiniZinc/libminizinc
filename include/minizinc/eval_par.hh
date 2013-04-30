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
  
  IntVal eval_int(ASTContext& ctx, Expression* e);

  bool eval_bool(ASTContext& ctx, Expression* e);
  
  void eval_int(ASTContext& ctx, Model* m);
  ArrayLit* eval_array_lit(ASTContext& ctx, Expression* e);
  Expression* eval_arrayaccess(ASTContext& ctx, ArrayLit* a,
                               const std::vector<IntVal>& dims);

  IntSetVal* eval_intset(ASTContext& ctx, Expression* e);

  Expression* eval_par(ASTContext& ctx, Expression* e);

  template<class Eval>
  void
  eval_comp(ASTContext& ctx, Eval& eval, Comprehension* e, int gen, int id,
            IntSetVal* in, std::vector<typename Eval::ArrayVal>& a);

  template<class Eval>
  void
  eval_comp(ASTContext& ctx, Eval& eval, Comprehension* e, int gen, int id,
            int i, IntSetVal* in, std::vector<typename Eval::ArrayVal>& a) {
    (*(*e->_g)[gen]->_v)[id]->_e->cast<IntLit>()->_v = i;
    if (id == (*e->_g)[gen]->_v->size()-1) {
      if (gen == e->_g->size()-1) {
        bool where = true;
        if (e->_where != NULL) {
          where = eval_bool(ctx, e->_where);
        }
        if (where) {
          a.push_back(eval.e(ctx,e->_e));
        }
      } else {
        IntSetVal* nextin = eval_intset(ctx, (*e->_g)[gen+1]->_in);
        eval_comp<Eval>(ctx,eval,e,gen+1,0,nextin,a);
      }
    } else {
      eval_comp<Eval>(ctx,eval,e,gen,id+1,in,a);
    }
  }

  template<class Eval>
  void
  eval_comp(ASTContext& ctx, Eval& eval, Comprehension* e, int gen, int id,
            IntSetVal* in, std::vector<typename Eval::ArrayVal>& a) {
    IntSetRanges rsi(in);
    Ranges::ToValues<IntSetRanges> rsv(rsi);
    for (; rsv(); ++rsv) {
      eval_comp<Eval>(ctx,eval,e,gen,id,rsv.val(),in,a);
    }
  }

  template<class Eval>
  std::vector<typename Eval::ArrayVal>
  eval_comp(ASTContext& ctx, Eval& eval, Comprehension* e) {
    std::vector<typename Eval::ArrayVal> a;
    IntSetVal* in  = eval_intset(ctx, (*e->_g)[0]->_in);
    eval_comp<Eval>(ctx,eval,e,0,0,in,a);
    return a;
  }  

  template<class Eval>
  std::vector<typename Eval::ArrayVal>
  eval_comp(ASTContext& ctx, Comprehension* e) {
    Eval eval;
    return eval_comp(ctx,eval,e);
  }  
}

#endif
