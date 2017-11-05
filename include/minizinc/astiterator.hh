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
#include <minizinc/hash.hh>

namespace MiniZinc {
  
  /**
   * \brief Bottom-up iterator for expressions
   */
  template<class T>
  class BottomUpIterator {
  protected:
    /// The visitor to call back during iteration
    T& _t;
    
    /// Stack item
    struct C {
      /// Expression on the stack
      Expression* _e;
      /// Whether this expression has been visited before
      bool _done;
      /// If part of a generator expression, which one it is
      int _gen_i;
      /// Constructor
      C(Expression* e) : _e(e), _done(false), _gen_i(-1) {}
      /// Constructor for generator expression
      C(Expression* e, int gen_i) : _e(e), _done(true), _gen_i(gen_i) {}
    };
    
    /// Push all elements of \a v onto \a stack
    template<class E>
    void pushVec(std::vector<C>& stack, ASTExprVec<E> v) {
      for (unsigned int i=0; i<v.size(); i++)
        stack.push_back(C(v[i]));
    }
    
  public:
    /// Constructor
    BottomUpIterator(T& t) : _t(t) {}
    /// Run iterator on expression \a e
    void run(Expression* e);
  };

  template<class T>
  void bottomUp(T& t, Expression* e) {
    BottomUpIterator<T>(t).run(e);
  }

  /**
   * \brief Leaf iterator for expressions
   */
  template<class T>
  class TopDownIterator {
  protected:
    /// The visitor to call back during iteration
    T& _t;
    
    /// Push all elements of \a v onto \a stack
    template<class E>
    static void pushVec(std::vector<Expression*>& stack, ASTExprVec<E> v) {
      for (unsigned int i=0; i<v.size(); i++)
        stack.push_back(v[i]);
    }
    
  public:
    /// Constructor
    TopDownIterator(T& t) : _t(t) {}
    /// Run iterator on expression \a e
    void run(Expression* e);
  };

  template<class T>
  void topDown(T& t, Expression* e) {
    TopDownIterator<T>(t).run(e);
  }

  /* IMPLEMENTATION */

  template<class T> void
  BottomUpIterator<T>::run(Expression* root) {
    std::vector<C> stack;
    if (_t.enter(root))
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
          if (c._gen_i >= 0) {
            _t.vComprehensionGenerator(*c._e->template cast<Comprehension>(), c._gen_i);
          } else {
            _t.vComprehension(*c._e->template cast<Comprehension>());
          }
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
        case Expression::E_TI:
          _t.vTypeInst(*c._e->template cast<TypeInst>());
          break;
        case Expression::E_TIID:
          _t.vTIId(*c._e->template cast<TIId>());
          break;
        }
        _t.exit(c._e);
        stack.pop_back();
      } else {
        c._done=true;
        Expression* ce = c._e;
        for (ExpressionSetIter it = ce->ann().begin(); it != ce->ann().end(); ++it) {
          if (_t.enter(*it))
            stack.push_back(C(*it));
        }
        if (_t.enter(ce)) {
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
            pushVec(stack, ce->template cast<SetLit>()->v());
            break;
          case Expression::E_ARRAYLIT:
            pushVec(stack, ce->template cast<ArrayLit>()->v());
            break;
          case Expression::E_ARRAYACCESS:
            pushVec(stack, ce->template cast<ArrayAccess>()->idx());
            stack.push_back(C(ce->template cast<ArrayAccess>()->v()));
            break;
          case Expression::E_COMP:
            {
              Comprehension* comp = ce->template cast<Comprehension>();
              stack.push_back(C(comp->e()));
              for (unsigned int i=comp->n_generators(); i--; ) {
                for (unsigned int j=comp->n_decls(i); j--; ) {
                  stack.push_back(C(comp->decl(i, j)));
                }
                stack.push_back(C(comp,i));
                stack.push_back(C(comp->in(i)));
                stack.push_back(C(comp->where(i)));
              }
            }
            break;
          case Expression::E_ITE:
            {
              ITE* ite = ce->template cast<ITE>();
              stack.push_back(C(ite->e_else()));
              for (int i=0; i<ite->size(); i++) {
                stack.push_back(C(ite->e_if(i)));
                stack.push_back(C(ite->e_then(i)));
              }
            }
            break;
          case Expression::E_BINOP:
            stack.push_back(C(ce->template cast<BinOp>()->rhs()));
            stack.push_back(C(ce->template cast<BinOp>()->lhs()));
            break;
          case Expression::E_UNOP:
            stack.push_back(C(ce->template cast<UnOp>()->e()));
            break;
          case Expression::E_CALL:
            pushVec(stack, ce->template cast<Call>()->args());
            break;
          case Expression::E_VARDECL:
            stack.push_back(C(ce->template cast<VarDecl>()->e()));
            stack.push_back(C(ce->template cast<VarDecl>()->ti()));
            break;
          case Expression::E_LET:
            stack.push_back(C(ce->template cast<Let>()->in()));
            pushVec(stack, ce->template cast<Let>()->let());
            break;
          case Expression::E_TI:
            stack.push_back(C(ce->template cast<TypeInst>()->domain()));
            pushVec(stack,ce->template cast<TypeInst>()->ranges());
            break;
          }
        } else {
          c._e = NULL;
        }
      }
    }
  }

  template<class T> void
  TopDownIterator<T>::run(Expression* root) {
    std::vector<Expression*> stack;
    if (_t.enter(root))
      stack.push_back(root);
    while (!stack.empty()) {
      Expression* e = stack.back();
      stack.pop_back();
      if (e==NULL) {
        continue;
      }
      if (!_t.enter(e))
        continue;
      for (ExpressionSetIter it = e->ann().begin(); it != e->ann().end(); ++it) {
        stack.push_back(*it);
      }
      switch (e->eid()) {
        case Expression::E_INTLIT:
        _t.vIntLit(*e->template cast<IntLit>());
        break;
        case Expression::E_FLOATLIT:
        _t.vFloatLit(*e->template cast<FloatLit>());
        break;
        case Expression::E_SETLIT:
        _t.vSetLit(*e->template cast<SetLit>());
        pushVec(stack, e->template cast<SetLit>()->v());
        break;
        case Expression::E_BOOLLIT:
        _t.vBoolLit(*e->template cast<BoolLit>());
        break;
        case Expression::E_STRINGLIT:
        _t.vStringLit(*e->template cast<StringLit>());
        break;
        case Expression::E_ID:
        _t.vId(*e->template cast<Id>());
        break;
        case Expression::E_ANON:
        _t.vAnonVar(*e->template cast<AnonVar>());
        break;
        case Expression::E_ARRAYLIT:
        _t.vArrayLit(*e->template cast<ArrayLit>());
        pushVec(stack, e->template cast<ArrayLit>()->v());
        break;
        case Expression::E_ARRAYACCESS:
        _t.vArrayAccess(*e->template cast<ArrayAccess>());
        pushVec(stack, e->template cast<ArrayAccess>()->idx());
        stack.push_back(e->template cast<ArrayAccess>()->v());
        break;
        case Expression::E_COMP:
        _t.vComprehension(*e->template cast<Comprehension>());
        {
          Comprehension* comp = e->template cast<Comprehension>();
          for (unsigned int i=comp->n_generators(); i--; ) {
            stack.push_back(comp->where(i));
            stack.push_back(comp->in(i));
            for (unsigned int j=comp->n_decls(i); j--; ) {
              stack.push_back(comp->decl(i, j));
            }
          }
          stack.push_back(comp->e());
        }
        break;
        case Expression::E_ITE:
        _t.vITE(*e->template cast<ITE>());
        {
          ITE* ite = e->template cast<ITE>();
          stack.push_back(ite->e_else());
          for (int i=0; i<ite->size(); i++) {
            stack.push_back(ite->e_if(i));
            stack.push_back(ite->e_then(i));
          }
        }
        break;
        case Expression::E_BINOP:
        _t.vBinOp(*e->template cast<BinOp>());
        stack.push_back(e->template cast<BinOp>()->rhs());
        stack.push_back(e->template cast<BinOp>()->lhs());
        break;
        case Expression::E_UNOP:
        _t.vUnOp(*e->template cast<UnOp>());
        stack.push_back(e->template cast<UnOp>()->e());
        break;
        case Expression::E_CALL:
        _t.vCall(*e->template cast<Call>());
        pushVec(stack, e->template cast<Call>()->args());
        break;
        case Expression::E_VARDECL:
        _t.vVarDecl(*e->template cast<VarDecl>());
        stack.push_back(e->template cast<VarDecl>()->e());
        stack.push_back(e->template cast<VarDecl>()->ti());
        break;
        case Expression::E_LET:
        _t.vLet(*e->template cast<Let>());
        stack.push_back(e->template cast<Let>()->in());
        pushVec(stack, e->template cast<Let>()->let());
        break;
        case Expression::E_TI:
        _t.vTypeInst(*e->template cast<TypeInst>());
        stack.push_back(e->template cast<TypeInst>()->domain());
        pushVec(stack,e->template cast<TypeInst>()->ranges());
        break;
        case Expression::E_TIID:
        _t.vTIId(*e->template cast<TIId>());
        break;
      }
    }
  }
  
}

#endif
