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
      switch (Expression::eid(c.e)) {
        case Expression::E_INTLIT:
          _t.vIntLit(Expression::cast<IntLit>(c.e));
          break;
        case Expression::E_FLOATLIT:
          _t.vFloatLit(Expression::cast<FloatLit>(c.e));
          break;
        case Expression::E_SETLIT:
          _t.vSetLit(Expression::cast<SetLit>(c.e));
          break;
        case Expression::E_BOOLLIT:
          _t.vBoolLit(Expression::cast<BoolLit>(c.e));
          break;
        case Expression::E_STRINGLIT:
          _t.vStringLit(Expression::cast<StringLit>(c.e));
          break;
        case Expression::E_ID:
          _t.vId(Expression::cast<Id>(c.e));
          break;
        case Expression::E_ANON:
          _t.vAnonVar(Expression::cast<AnonVar>(c.e));
          break;
        case Expression::E_ARRAYLIT:
          _t.vArrayLit(Expression::cast<ArrayLit>(c.e));
          break;
        case Expression::E_ARRAYACCESS:
          _t.vArrayAccess(Expression::cast<ArrayAccess>(c.e));
          break;
        case Expression::E_FIELDACCESS:
          _t.vFieldAccess(Expression::cast<FieldAccess>(c.e));
          break;
        case Expression::E_COMP:
          if (c.genNumber >= 0) {
            _t.vComprehensionGenerator(Expression::cast<Comprehension>(c.e), c.genNumber);
          } else {
            _t.vComprehension(Expression::cast<Comprehension>(c.e));
          }
          break;
        case Expression::E_ITE:
          _t.vITE(Expression::cast<ITE>(c.e));
          break;
        case Expression::E_BINOP:
          _t.vBinOp(Expression::cast<BinOp>(c.e));
          break;
        case Expression::E_UNOP:
          _t.vUnOp(Expression::cast<UnOp>(c.e));
          break;
        case Expression::E_CALL:
          _t.vCall(Expression::cast<Call>(c.e));
          break;
        case Expression::E_VARDECL:
          _t.vVarDecl(Expression::cast<VarDecl>(c.e));
          break;
        case Expression::E_LET:
          _t.vLet(Expression::cast<Let>(c.e));
          break;
        case Expression::E_TI:
          _t.vTypeInst(Expression::cast<TypeInst>(c.e));
          break;
        case Expression::E_TIID:
          _t.vTIId(Expression::cast<TIId>(c.e));
          break;
      }
      _t.exit(c.e);
      stack.pop_back();
    } else {
      c.done = true;
      Expression* ce = c.e;
      for (ExpressionSetIter it = Expression::ann(ce).begin(); it != Expression::ann(ce).end();
           ++it) {
        if (_t.enter(*it)) {
          stack.push_back(C(*it));
        }
      }
      if (_t.enter(ce)) {
        switch (Expression::eid(ce)) {
          case Expression::E_INTLIT:
          case Expression::E_FLOATLIT:
          case Expression::E_BOOLLIT:
          case Expression::E_STRINGLIT:
          case Expression::E_ANON:
          case Expression::E_ID:
          case Expression::E_TIID:
            break;
          case Expression::E_SETLIT:
            pushVec(stack, Expression::cast<SetLit>(ce)->v());
            break;
          case Expression::E_ARRAYLIT: {
            for (unsigned int i = 0; i < Expression::cast<ArrayLit>(ce)->size(); i++) {
              stack.push_back((*Expression::cast<ArrayLit>(ce))[i]);
            }
          } break;
          case Expression::E_ARRAYACCESS:
            pushVec(stack, Expression::cast<ArrayAccess>(ce)->idx());
            stack.push_back(C(Expression::cast<ArrayAccess>(ce)->v()));
            break;
          case Expression::E_FIELDACCESS:
            // TODO: is this ever required? Should be a literal
            // stack.push_back(C(ce->template cast<FieldAccess>()->field()));
            stack.push_back(C(Expression::cast<FieldAccess>(ce)->v()));
            break;
          case Expression::E_COMP: {
            auto* comp = Expression::cast<Comprehension>(ce);
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
            ITE* ite = Expression::cast<ITE>(ce);
            stack.push_back(C(ite->elseExpr()));
            for (int i = 0; i < ite->size(); i++) {
              stack.push_back(C(ite->ifExpr(i)));
              stack.push_back(C(ite->thenExpr(i)));
            }
          } break;
          case Expression::E_BINOP:
            stack.push_back(C(Expression::cast<BinOp>(ce)->rhs()));
            stack.push_back(C(Expression::cast<BinOp>(ce)->lhs()));
            break;
          case Expression::E_UNOP:
            stack.push_back(C(Expression::cast<UnOp>(ce)->e()));
            break;
          case Expression::E_CALL:
            for (unsigned int i = 0; i < Expression::cast<Call>(ce)->argCount(); i++) {
              stack.push_back(Expression::cast<Call>(ce)->arg(i));
            }
            break;
          case Expression::E_VARDECL:
            stack.push_back(C(Expression::cast<VarDecl>(ce)->e()));
            stack.push_back(C(Expression::cast<VarDecl>(ce)->ti()));
            break;
          case Expression::E_LET:
            stack.push_back(C(Expression::cast<Let>(ce)->in()));
            for (unsigned int i = Expression::cast<Let>(ce)->let().size(); (i--) != 0U;) {
              stack.push_back(Expression::cast<Let>(ce)->let()[i]);
            }
            break;
          case Expression::E_TI:
            stack.push_back(C(Expression::cast<TypeInst>(ce)->domain()));
            pushVec(stack, Expression::cast<TypeInst>(ce)->ranges());
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
    for (ExpressionSetIter it = Expression::ann(e).begin(); it != Expression::ann(e).end(); ++it) {
      stack.push_back(*it);
    }
    switch (Expression::eid(e)) {
      case Expression::E_INTLIT:
        _t.vIntLit(Expression::cast<IntLit>(e));
        break;
      case Expression::E_FLOATLIT:
        _t.vFloatLit(Expression::cast<FloatLit>(e));
        break;
      case Expression::E_SETLIT:
        _t.vSetLit(Expression::cast<SetLit>(e));
        pushVec(stack, Expression::cast<SetLit>(e)->v());
        break;
      case Expression::E_BOOLLIT:
        _t.vBoolLit(Expression::cast<BoolLit>(e));
        break;
      case Expression::E_STRINGLIT:
        _t.vStringLit(Expression::cast<StringLit>(e));
        break;
      case Expression::E_ID:
        _t.vId(Expression::cast<Id>(e));
        break;
      case Expression::E_ANON:
        _t.vAnonVar(Expression::cast<AnonVar>(e));
        break;
      case Expression::E_ARRAYLIT:
        _t.vArrayLit(Expression::cast<ArrayLit>(e));
        for (unsigned int i = 0; i < Expression::cast<ArrayLit>(e)->size(); i++) {
          stack.push_back((*Expression::cast<ArrayLit>(e))[i]);
        }
        break;
      case Expression::E_ARRAYACCESS:
        _t.vArrayAccess(Expression::cast<ArrayAccess>(e));
        pushVec(stack, Expression::cast<ArrayAccess>(e)->idx());
        stack.push_back(Expression::cast<ArrayAccess>(e)->v());
        break;
      case Expression::E_FIELDACCESS:
        _t.vFieldAccess(Expression::cast<FieldAccess>(e));
        // TODO: Is this ever required? Should be a literal
        // stack.push_back(e->template cast<FieldAccess>()->field());
        stack.push_back(Expression::cast<FieldAccess>(e)->v());
        break;
      case Expression::E_COMP:
        _t.vComprehension(Expression::cast<Comprehension>(e));
        {
          auto* comp = Expression::cast<Comprehension>(e);
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
        _t.vITE(Expression::cast<ITE>(e));
        {
          ITE* ite = Expression::cast<ITE>(e);
          stack.push_back(ite->elseExpr());
          for (int i = 0; i < ite->size(); i++) {
            stack.push_back(ite->ifExpr(i));
            stack.push_back(ite->thenExpr(i));
          }
        }
        break;
      case Expression::E_BINOP:
        _t.vBinOp(Expression::cast<BinOp>(e));
        stack.push_back(Expression::cast<BinOp>(e)->rhs());
        stack.push_back(Expression::cast<BinOp>(e)->lhs());
        break;
      case Expression::E_UNOP:
        _t.vUnOp(Expression::cast<UnOp>(e));
        stack.push_back(Expression::cast<UnOp>(e)->e());
        break;
      case Expression::E_CALL:
        _t.vCall(Expression::cast<Call>(e));
        for (unsigned int i = 0; i < Expression::cast<Call>(e)->argCount(); i++) {
          stack.push_back(Expression::cast<Call>(e)->arg(i));
        }
        break;
      case Expression::E_VARDECL:
        _t.vVarDecl(Expression::cast<VarDecl>(e));
        stack.push_back(Expression::cast<VarDecl>(e)->e());
        stack.push_back(Expression::cast<VarDecl>(e)->ti());
        break;
      case Expression::E_LET:
        _t.vLet(Expression::cast<Let>(e));
        stack.push_back(Expression::cast<Let>(e)->in());
        pushVec(stack, Expression::cast<Let>(e)->let());
        break;
      case Expression::E_TI:
        _t.vTypeInst(Expression::cast<TypeInst>(e));
        stack.push_back(Expression::cast<TypeInst>(e)->domain());
        pushVec(stack, Expression::cast<TypeInst>(e)->ranges());
        break;
      case Expression::E_TIID:
        _t.vTIId(Expression::cast<TIId>(e));
        break;
    }
  }
}

}  // namespace MiniZinc
