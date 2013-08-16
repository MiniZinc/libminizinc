/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_ASTITERATOR_HH__
#define __MINIZINC_ASTITERATOR_HH__

#include <minizinc/ast.hh>

namespace MiniZinc {
  
  template<class T>
  class BottomUpIterator {
  protected:
    T& _t;
    
    struct C {
      Expression* _e;
      bool _done;
      C(Expression* e) : _e(e), _done(false) {}
    };
    
    template<class E>
    void pushVec(std::vector<C>& stack, ASTExprVec<E>& v) {
      for (unsigned int i=0; i<v.size(); i++)
        stack.push_back(C(v[i]));
    }
    
  public:
    BottomUpIterator(T& t) : _t(t) {}
    void run(Expression* e);
  };

  template<class T> void
  BottomUpIterator<T>::run(Expression* root) {
    std::vector<C> stack;
    stack.push_back(C(root));
    while (!stack.empty()) {
      C& c = stack.back();
      if (c._e==NULL) {
        stack.pop_back();
        continue;
      }
      if (c._done) {
        switch (c._e->eid()) {
        case Expression::E_INTLIT:
          _t.vIntLit(*c._e->template cast<IntLit>());
          break;
        case Expression::E_FLOATLIT:
          _t.vFloatLit(*c._e->template cast<FloatLit>());
          break;
        case Expression::E_SETLIT:
          _t.vSetLit(*c._e->template cast<SetLit>());
          break;
        case Expression::E_BOOLLIT:
          _t.vBoolLit(*c._e->template cast<BoolLit>());
          break;
        case Expression::E_STRINGLIT:
          _t.vStringLit(*c._e->template cast<StringLit>());
          break;
        case Expression::E_ID:
          _t.vId(*c._e->template cast<Id>());
          break;
        case Expression::E_ANON:
          _t.vAnonVar(*c._e->template cast<AnonVar>());
          break;
        case Expression::E_ARRAYLIT:
          _t.vArrayLit(*c._e->template cast<ArrayLit>());
          break;
        case Expression::E_ARRAYACCESS:
          _t.vArrayAccess(*c._e->template cast<ArrayAccess>());
          break;
        case Expression::E_COMP:
          _t.vComprehension(*c._e->template cast<Comprehension>());
          break;
        case Expression::E_ITE:
          _t.vITE(*c._e->template cast<ITE>());
          break;
        case Expression::E_BINOP:
          _t.vBinOp(*c._e->template cast<BinOp>());
          break;
        case Expression::E_UNOP:
          _t.vUnOp(*c._e->template cast<UnOp>());
          break;
        case Expression::E_CALL:
          _t.vCall(*c._e->template cast<Call>());
          break;
        case Expression::E_VARDECL:
          _t.vVarDecl(*c._e->template cast<VarDecl>());
          break;
        case Expression::E_LET:
          _t.vLet(*c._e->template cast<Let>());
          break;
        case Expression::E_ANN:
          _t.vAnnotation(*c._e->template cast<Annotation>());
          break;
        case Expression::E_TI:
          _t.vTypeInst(*c._e->template cast<TypeInst>());
          break;
        case Expression::E_TIID:
          _t.vTIId(*c._e->template cast<TIId>());
          break;
        }
        stack.pop_back();
      } else {
        c._done=true;
        Expression* ce = c._e;
        if (_t.visitAnnotation && ce->_ann) {
          stack.push_back(C(ce->_ann));
        }
        switch (ce->eid()) {
        case Expression::E_INTLIT:
        case Expression::E_FLOATLIT:
        case Expression::E_BOOLLIT:
        case Expression::E_STRINGLIT:
        case Expression::E_ANON:
        case Expression::E_ID:
        case Expression::E_TIID:
          break;
        case Expression::E_SETLIT:
          pushVec(stack, ce->template cast<SetLit>()->_v);
          break;
        case Expression::E_ARRAYLIT:
          pushVec(stack, ce->template cast<ArrayLit>()->_v);
          break;
        case Expression::E_ARRAYACCESS:
          pushVec(stack, ce->template cast<ArrayAccess>()->_idx);
          stack.push_back(C(ce->template cast<ArrayAccess>()->_v));
          break;
        case Expression::E_COMP:
          {
            Comprehension* comp = ce->template cast<Comprehension>();
            stack.push_back(C(comp->_where));
            for (unsigned int i=0; i<comp->_g.size(); i++) {
              stack.push_back(C(comp->_g[i]));
            }
            stack.push_back(C(comp->_e));
          }
          break;
        case Expression::E_ITE:
          {
            ITE* ite = ce->template cast<ITE>();
            stack.push_back(C(ite->_e_else));
            for (unsigned int i=0; i<ite->_e_if_then.size(); i++) {
              stack.push_back(C(ite->_e_if_then[i]));
            }
          }
          break;
        case Expression::E_BINOP:
          stack.push_back(C(ce->template cast<BinOp>()->_e1));
          stack.push_back(C(ce->template cast<BinOp>()->_e0));
          break;
        case Expression::E_UNOP:
          stack.push_back(C(ce->template cast<UnOp>()->_e0));
          break;
        case Expression::E_CALL:
          pushVec(stack, ce->template cast<Call>()->_args);
          break;
        case Expression::E_VARDECL:
          stack.push_back(C(ce->template cast<VarDecl>()->_e));
          stack.push_back(C(ce->template cast<VarDecl>()->_ti));
          break;
        case Expression::E_LET:
          stack.push_back(C(ce->template cast<Let>()->_in));
          pushVec(stack, ce->template cast<Let>()->_let);
          break;
        case Expression::E_ANN:
          stack.push_back(C(ce->template cast<Annotation>()->_a));
          stack.push_back(C(ce->template cast<Annotation>()->_e));
          break;
        case Expression::E_TI:
          stack.push_back(C(ce->template cast<TypeInst>()->_domain));
          pushVec(stack,ce->template cast<TypeInst>()->_ranges);
          break;
        }
      }
    }
  }
  
}

#endif
