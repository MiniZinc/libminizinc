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
  
  IntVal eval_int(Expression* e);

  bool eval_bool(Expression* e);
  
  void eval_int(Model* m);
  ArrayLit* eval_array_lit(Expression* e);
  Expression* eval_arrayaccess(ArrayLit* a, const std::vector<IntVal>& dims);
  Expression* eval_arrayaccess(ArrayAccess* e);

  IntSetVal* eval_intset(Expression* e);

  Expression* eval_par(Expression* e);

  template<class Eval>
  void
  eval_comp(Eval& eval, Comprehension* e, int gen, int id,
            IntSetVal* in, std::vector<typename Eval::ArrayVal>& a);

  template<class Eval>
  void
  eval_comp(Eval& eval, Comprehension* e, int gen, int id,
            int i, IntSetVal* in, std::vector<typename Eval::ArrayVal>& a) {
    e->_g[e->_g_idx[gen]+id+1]->cast<VarDecl>()->_e->cast<IntLit>()->_v = i;
    if (e->_g_idx[gen]+id+1 == e->_g_idx[gen+1]-1) {
      if (gen == e->_g_idx.size()-2) {
        bool where = true;
        if (e->_where != NULL) {
          where = eval_bool(e->_where);
        }
        if (where) {
          a.push_back(eval.e(e->_e));
        }
      } else {
        IntSetVal* nextin = eval_intset(e->_g[e->_g_idx[gen+1]]);
        eval_comp<Eval>(eval,e,gen+1,0,nextin,a);
      }
    } else {
      eval_comp<Eval>(eval,e,gen,id+1,in,a);
    }
  }

  template<class Eval>
  void
  eval_comp(Eval& eval, Comprehension* e, int gen, int id,
            IntSetVal* in, std::vector<typename Eval::ArrayVal>& a) {
    IntSetRanges rsi(in);
    Ranges::ToValues<IntSetRanges> rsv(rsi);
    for (; rsv(); ++rsv) {
      eval_comp<Eval>(eval,e,gen,id,rsv.val(),in,a);
    }
  }

  template<class Eval>
  std::vector<typename Eval::ArrayVal>
  eval_comp(Eval& eval, Comprehension* e) {
    std::vector<typename Eval::ArrayVal> a;
    IntSetVal* in  = eval_intset(e->_g[e->_g_idx[0]]);
    eval_comp<Eval>(eval,e,0,0,in,a);
    return a;
  }  

  template<class Eval>
  std::vector<typename Eval::ArrayVal>
  eval_comp(Comprehension* e) {
    Eval eval;
    return eval_comp(eval,e);
  }  
}

#endif
