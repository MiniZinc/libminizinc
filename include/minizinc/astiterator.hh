/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/ast.hh>
#include <minizinc/hash.hh>

namespace MiniZinc {

/**
 * \brief Bottom-up iterator for expressions
 */
template <class T>
class BottomUpIterator {
protected:
  /// The visitor to call back during iteration
  T& _t;

  /// Stack item
  struct C {
    /// Expression on the stack
    Expression* e;
    /// Whether this expression has been visited before
    bool done;
    /// If part of a generator expression, which one it is
    int genNumber;
    /// Constructor
    C(Expression* e0) : e(e0), done(false), genNumber(-1) {}
    /// Constructor for generator expression
    C(Expression* e0, int genNumber0) : e(e0), done(true), genNumber(genNumber0) {}
  };

  /// Push all elements of \a v onto \a stack
  template <class E>
  void pushVec(std::vector<C>& stack, ASTExprVec<E> v) {
    for (unsigned int i = 0; i < v.size(); i++) {
      stack.push_back(C(v[i]));
    }
  }

public:
  /// Constructor
  BottomUpIterator(T& t) : _t(t) {}
  /// Run iterator on expression \a e
  void run(Expression* root);
};

template <class T>
void bottom_up(T& t, Expression* e) {
  BottomUpIterator<T>(t).run(e);
}

/**
 * \brief Leaf iterator for expressions
 */
template <class T>
class TopDownIterator {
protected:
  /// The visitor to call back during iteration
  T& _t;

  /// Push all elements of \a v onto \a stack
  template <class E>
  static void pushVec(std::vector<Expression*>& stack, ASTExprVec<E> v) {
    for (unsigned int i = 0; i < v.size(); i++) {
      stack.push_back(v[i]);
    }
  }

public:
  /// Constructor
  TopDownIterator(T& t) : _t(t) {}
  /// Run iterator on expression \a e
  void run(Expression* root);
};

template <class T>
void top_down(T& t, Expression* root) {
  TopDownIterator<T>(t).run(root);
}

/* IMPLEMENTATION */

template <class T>
void BottomUpIterator<T>::run(Expression* root) {
  std::vector<C> stack;
  if (_t.enter(root)) {
    stack.push_back(C(root));
  }
  while (!stack.empty()) {
    C& c = stack.back();
    if (c.e == nullptr) {
      stack.pop_back();
      continue;
    }
    if (c.done) {
      switch (c.e->eid()) {
        case Expression::E_INTLIT:
          _t.vIntLit(*c.e->template cast<IntLit>());
          break;
        case Expression::E_FLOATLIT:
          _t.vFloatLit(*c.e->template cast<FloatLit>());
          break;
        case Expression::E_SETLIT:
          _t.vSetLit(*c.e->template cast<SetLit>());
          break;
        case Expression::E_BOOLLIT:
          _t.vBoolLit(*c.e->template cast<BoolLit>());
          break;
        case Expression::E_STRINGLIT:
          _t.vStringLit(*c.e->template cast<StringLit>());
          break;
        case Expression::E_ID:
          _t.vId(*c.e->template cast<Id>());
          break;
        case Expression::E_ANON:
          _t.vAnonVar(*c.e->template cast<AnonVar>());
          break;
        case Expression::E_ARRAYLIT:
          _t.vArrayLit(*c.e->template cast<ArrayLit>());
          break;
        case Expression::E_ARRAYACCESS:
          _t.vArrayAccess(*c.e->template cast<ArrayAccess>());
          break;
        case Expression::E_COMP:
          if (c.genNumber >= 0) {
            _t.vComprehensionGenerator(*c.e->template cast<Comprehension>(), c.genNumber);
          } else {
            _t.vComprehension(*c.e->template cast<Comprehension>());
          }
          break;
        case Expression::E_ITE:
          _t.vITE(*c.e->template cast<ITE>());
          break;
        case Expression::E_BINOP:
          _t.vBinOp(*c.e->template cast<BinOp>());
          break;
        case Expression::E_UNOP:
          _t.vUnOp(*c.e->template cast<UnOp>());
          break;
        case Expression::E_CALL:
          _t.vCall(*c.e->template cast<Call>());
          break;
        case Expression::E_VARDECL:
          _t.vVarDecl(*c.e->template cast<VarDecl>());
          break;
        case Expression::E_LET:
          _t.vLet(*c.e->template cast<Let>());
          break;
        case Expression::E_TI:
          _t.vTypeInst(*c.e->template cast<TypeInst>());
          break;
        case Expression::E_TIID:
          _t.vTIId(*c.e->template cast<TIId>());
          break;
      }
      _t.exit(c.e);
      stack.pop_back();
    } else {
      c.done = true;
      Expression* ce = c.e;
      for (ExpressionSetIter it = ce->ann().begin(); it != ce->ann().end(); ++it) {
        if (_t.enter(*it)) {
          stack.push_back(C(*it));
        }
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
          case Expression::E_ARRAYLIT: {
            for (unsigned int i = 0; i < ce->cast<ArrayLit>()->size(); i++) {
              stack.push_back((*ce->cast<ArrayLit>())[i]);
            }
          } break;
          case Expression::E_ARRAYACCESS:
            pushVec(stack, ce->template cast<ArrayAccess>()->idx());
            stack.push_back(C(ce->template cast<ArrayAccess>()->v()));
            break;
          case Expression::E_COMP: {
            auto* comp = ce->template cast<Comprehension>();
            stack.push_back(C(comp->e()));
            for (unsigned int i = comp->numberOfGenerators(); (i--) != 0U;) {
              for (unsigned int j = comp->numberOfDecls(i); (j--) != 0U;) {
                stack.push_back(C(comp->decl(i, j)));
              }
              if (comp->in(i) != nullptr) {
                stack.push_back(C(comp->where(i)));
                stack.push_back(C(comp, i));
                stack.push_back(C(comp->in(i)));
              } else {
                stack.push_back(C(comp, i));
                stack.push_back(C(comp->where(i)));
              }
            }
          } break;
          case Expression::E_ITE: {
            ITE* ite = ce->template cast<ITE>();
            stack.push_back(C(ite->elseExpr()));
            for (int i = 0; i < ite->size(); i++) {
              stack.push_back(C(ite->ifExpr(i)));
              stack.push_back(C(ite->thenExpr(i)));
            }
          } break;
          case Expression::E_BINOP:
            stack.push_back(C(ce->template cast<BinOp>()->rhs()));
            stack.push_back(C(ce->template cast<BinOp>()->lhs()));
            break;
          case Expression::E_UNOP:
            stack.push_back(C(ce->template cast<UnOp>()->e()));
            break;
          case Expression::E_CALL:
            for (unsigned int i = 0; i < ce->template cast<Call>()->argCount(); i++) {
              stack.push_back(ce->template cast<Call>()->arg(i));
            }
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
            pushVec(stack, ce->template cast<TypeInst>()->ranges());
            break;
        }
      } else {
        c.e = nullptr;
      }
    }
  }
}

template <class T>
void TopDownIterator<T>::run(Expression* root) {
  std::vector<Expression*> stack;
  if (_t.enter(root)) {
    stack.push_back(root);
  }
  while (!stack.empty()) {
    Expression* e = stack.back();
    stack.pop_back();
    if (e == nullptr) {
      continue;
    }
    if (!_t.enter(e)) {
      continue;
    }
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
        for (unsigned int i = 0; i < e->cast<ArrayLit>()->size(); i++) {
          stack.push_back((*e->cast<ArrayLit>())[i]);
        }
        break;
      case Expression::E_ARRAYACCESS:
        _t.vArrayAccess(*e->template cast<ArrayAccess>());
        pushVec(stack, e->template cast<ArrayAccess>()->idx());
        stack.push_back(e->template cast<ArrayAccess>()->v());
        break;
      case Expression::E_COMP:
        _t.vComprehension(*e->template cast<Comprehension>());
        {
          auto* comp = e->template cast<Comprehension>();
          for (unsigned int i = comp->numberOfGenerators(); (i--) != 0U;) {
            stack.push_back(comp->where(i));
            stack.push_back(comp->in(i));
            for (unsigned int j = comp->numberOfDecls(i); (j--) != 0U;) {
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
          stack.push_back(ite->elseExpr());
          for (int i = 0; i < ite->size(); i++) {
            stack.push_back(ite->ifExpr(i));
            stack.push_back(ite->thenExpr(i));
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
        for (unsigned int i = 0; i < e->template cast<Call>()->argCount(); i++) {
          stack.push_back(e->template cast<Call>()->arg(i));
        }
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
        pushVec(stack, e->template cast<TypeInst>()->ranges());
        break;
      case Expression::E_TIID:
        _t.vTIId(*e->template cast<TIId>());
        break;
    }
  }
}

}  // namespace MiniZinc
