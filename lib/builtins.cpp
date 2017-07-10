/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/builtins.hh>
#include <minizinc/ast.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/astexception.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/file_utils.hh>

#include <iomanip>
#include <climits>
#include <cmath>
#include <random>

namespace MiniZinc {
  
  void rb(EnvI& env, Model* m, const ASTString& id, const std::vector<Type>& t,
          FunctionI::builtin_e b) {
    FunctionI* fi = m->matchFn(env,id,t,false);
    if (fi) {
      fi->_builtins.e = b;
    } else {
      throw InternalError("no definition found for builtin "+id.str());
    }
  }
  void rb(EnvI& env, Model* m, const ASTString& id, const std::vector<Type>& t,
          FunctionI::builtin_f b) {
    FunctionI* fi = m->matchFn(env,id,t,false);
    if (fi) {
      fi->_builtins.f = b;
    } else {
      throw InternalError("no definition found for builtin "+id.str());
    }
  }
  void rb(EnvI& env, Model* m, const ASTString& id, const std::vector<Type>& t,
          FunctionI::builtin_i b) {
    FunctionI* fi = m->matchFn(env,id,t,false);
    if (fi) {
      fi->_builtins.i = b;
    } else {
      throw InternalError("no definition found for builtin "+id.str());
    }
  }
  void rb(EnvI& env, Model* m, const ASTString& id, const std::vector<Type>& t,
          FunctionI::builtin_b b) {
    FunctionI* fi = m->matchFn(env,id,t,false);
    if (fi) {
      fi->_builtins.b = b;
    } else {
      throw InternalError("no definition found for builtin "+id.str());
    }
  }
  void rb(EnvI& env, Model* m, const ASTString& id, const std::vector<Type>& t,
          FunctionI::builtin_s b) {
    FunctionI* fi = m->matchFn(env,id,t,false);
    if (fi) {
      fi->_builtins.s = b;
    } else {
      throw InternalError("no definition found for builtin "+id.str());
    }
  }
  void rb(EnvI& env, Model* m, const ASTString& id, const std::vector<Type>& t,
          FunctionI::builtin_str b) {
    FunctionI* fi = m->matchFn(env,id,t,false);
    if (fi) {
      fi->_builtins.str = b;
    } else {
      throw InternalError("no definition found for builtin "+id.str());
    }
  }

  IntVal b_int_min(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    switch (args.size()) {
    case 1:
      if (args[0]->type().is_set()) {
        throw EvalError(env, args[0]->loc(), "sets not supported");
      } else {
        GCLock lock;
        ArrayLit* al = eval_array_lit(env,args[0]);
        if (al->v().size()==0)
          throw ResultUndefinedError(env, al->loc(), "minimum of empty array is undefined");
        IntVal m = eval_int(env,al->v()[0]);
        for (unsigned int i=1; i<al->v().size(); i++)
          m = std::min(m, eval_int(env,al->v()[i]));
        return m;
      }
    case 2:
      {
        return std::min(eval_int(env,args[0]),eval_int(env,args[1]));
      }
    default:
      throw EvalError(env, Location(), "dynamic type error");
    }
  }

  IntVal b_int_max(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    switch (args.size()) {
    case 1:
      if (args[0]->type().is_set()) {
        throw EvalError(env, args[0]->loc(), "sets not supported");
      } else {
        GCLock lock;
        ArrayLit* al = eval_array_lit(env,args[0]);
        if (al->v().size()==0)
          throw ResultUndefinedError(env, al->loc(), "maximum of empty array is undefined");
        IntVal m = eval_int(env,al->v()[0]);
        for (unsigned int i=1; i<al->v().size(); i++)
          m = std::max(m, eval_int(env,al->v()[i]));
        return m;
      }
    case 2:
      {
        return std::max(eval_int(env,args[0]),eval_int(env,args[1]));
      }
    default:
      throw EvalError(env, Location(), "dynamic type error");
    }
  }
  
  IntVal b_arg_min_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[0]);
    if (al->v().size()==0)
      throw ResultUndefinedError(env, al->loc(), "argmin of empty array is undefined");
    IntVal m = eval_int(env,al->v()[0]);
    int m_idx = 0;
    for (unsigned int i=1; i<al->v().size(); i++) {
      IntVal mi = eval_int(env,al->v()[i]);
      if (mi < m) {
        m = mi;
        m_idx = i;
      }
    }
    return m_idx+1;
  }
  IntVal b_arg_max_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[0]);
    if (al->v().size()==0)
      throw ResultUndefinedError(env, al->loc(), "argmax of empty array is undefined");
    IntVal m = eval_int(env,al->v()[0]);
    int m_idx = 0;
    for (unsigned int i=1; i<al->v().size(); i++) {
      IntVal mi = eval_int(env,al->v()[i]);
      if (mi > m) {
        m = mi;
        m_idx = i;
      }
    }
    return m_idx+1;
  }
  IntVal b_arg_min_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[0]);
    if (al->v().size()==0)
      throw ResultUndefinedError(env, al->loc(), "argmin of empty array is undefined");
    FloatVal m = eval_float(env,al->v()[0]);
    int m_idx = 0;
    for (unsigned int i=1; i<al->v().size(); i++) {
      FloatVal mi = eval_float(env,al->v()[i]);
      if (mi < m) {
        m = mi;
        m_idx = i;
      }
    }
    return m_idx+1;
  }
  IntVal b_arg_max_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[0]);
    if (al->v().size()==0)
      throw ResultUndefinedError(env, al->loc(), "argmax of empty array is undefined");
    FloatVal m = eval_float(env,al->v()[0]);
    int m_idx = 0;
    for (unsigned int i=1; i<al->v().size(); i++) {
      FloatVal mi = eval_float(env,al->v()[i]);
      if (mi > m) {
        m = mi;
        m_idx = i;
      }
    }
    return m_idx+1;
  }
  
  
  IntVal b_abs_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    return std::abs(eval_int(env,args[0]));
  }

  FloatVal b_abs_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    return std::abs(eval_float(env,args[0]));
  }
  
  bool b_has_bounds_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size() != 1)
      throw EvalError(env, Location(), "dynamic type error");
    IntBounds ib = compute_int_bounds(env,args[0]);
    return ib.valid && ib.l.isFinite() && ib.u.isFinite();
  }
  bool b_has_bounds_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size() != 1)
      throw EvalError(env, Location(), "dynamic type error");
    FloatBounds fb = compute_float_bounds(env,args[0]);
    return fb.valid;
  }
  
  IntVal lb_varoptint(EnvI& env, Expression* e) {
    IntBounds b = compute_int_bounds(env,e);
    if (b.valid)
      return b.l;
    else
      return -IntVal::infinity();
  }
  IntVal b_lb_varoptint(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size() != 1)
      throw EvalError(env, Location(), "dynamic type error");
    return lb_varoptint(env,args[0]);
  }

  bool b_occurs(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    GCLock lock;
    return eval_par(env,args[0]) != constants().absent;
  }
  
  IntVal b_deopt_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    GCLock lock;
    Expression* e = eval_par(env,args[0]);
    if (e==constants().absent)
      throw EvalError(env, e->loc(), "cannot evaluate deopt on absent value");
    return eval_int(env,e);
  }

  bool b_deopt_bool(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    GCLock lock;
    Expression* e = eval_par(env,args[0]);
    if (e==constants().absent)
      throw EvalError(env, e->loc(), "cannot evaluate deopt on absent value");
    return eval_bool(env,e);
  }
  
  IntVal b_array_lb_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    Expression* e = follow_id_to_decl(args[0]);
    
    bool foundMin = false;
    IntVal array_lb = -IntVal::infinity();
    
    if (VarDecl* vd = e->dyn_cast<VarDecl>()) {
      if (vd->ti()->domain()) {
        GCLock lock;
        IntSetVal* isv = eval_intset(env,vd->ti()->domain());
        if (isv->size()!=0) {
          array_lb = isv->min();
          foundMin = true;
        }
      }
      e = vd->e();
    }
    
    if (e != NULL) {
      GCLock lock;
      ArrayLit* al = eval_array_lit(env,e);
      if (al->v().size()==0)
        throw EvalError(env, Location(), "lower bound of empty array undefined");
      IntVal min = IntVal::infinity();
      for (unsigned int i=0; i<al->v().size(); i++) {
        IntBounds ib = compute_int_bounds(env,al->v()[i]);
        if (!ib.valid)
          goto b_array_lb_int_done;
        min = std::min(min, ib.l);
      }
      if (foundMin)
        array_lb = std::max(array_lb, min);
      else
        array_lb = min;
      foundMin = true;
    }
  b_array_lb_int_done:
    if (foundMin) {
      return array_lb;
    } else {
      return -IntVal::infinity();
    }
  }

  IntVal ub_varoptint(EnvI& env, Expression* e) {
    IntBounds b = compute_int_bounds(env,e);
    if (b.valid)
      return b.u;
    else
      return IntVal::infinity();
  }
  IntVal b_ub_varoptint(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size() != 1)
      throw EvalError(env, Location(), "dynamic type error");
    return ub_varoptint(env,args[0]);
  }

  IntVal b_array_ub_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    Expression* e = follow_id_to_decl(args[0]);
    
    bool foundMax = false;
    IntVal array_ub = IntVal::infinity();
    
    if (VarDecl* vd = e->dyn_cast<VarDecl>()) {
      if (vd->ti()->domain()) {
        GCLock lock;
        IntSetVal* isv = eval_intset(env,vd->ti()->domain());
        if (isv->size()!=0) {
          array_ub = isv->max();
          foundMax = true;
        }
      }
      e = vd->e();
    }
    
    if (e != NULL) {
      GCLock lock;
      ArrayLit* al = eval_array_lit(env,e);
      if (al->v().size()==0)
        throw EvalError(env, Location(), "upper bound of empty array undefined");
      IntVal max = -IntVal::infinity();
      for (unsigned int i=0; i<al->v().size(); i++) {
        IntBounds ib = compute_int_bounds(env,al->v()[i]);
        if (!ib.valid)
          goto b_array_ub_int_done;
        max = std::max(max, ib.u);
      }
      if (foundMax)
        array_ub = std::min(array_ub, max);
      else
        array_ub = max;
      foundMax = true;
    }
  b_array_ub_int_done:
    if (foundMax) {
      return array_ub;
    } else {
      return IntVal::infinity();
    }
  }

  IntVal b_sum_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[0]);
    if (al->v().size()==0)
      return 0;
    IntVal m = 0;
    for (unsigned int i=0; i<al->v().size(); i++)
      m += eval_int(env,al->v()[i]);
    return m;
  }

  IntVal b_product_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[0]);
    if (al->v().size()==0)
      return 1;
    IntVal m = 1;
    for (unsigned int i=0; i<al->v().size(); i++)
      m *= eval_int(env,al->v()[i]);
    return m;
  }

  FloatVal b_product_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[0]);
    if (al->v().size()==0)
      return 1;
    FloatVal m = 1.0;
    for (unsigned int i=0; i<al->v().size(); i++)
      m *= eval_float(env,al->v()[i]);
    return m;
  }

  FloatVal lb_varoptfloat(EnvI& env, Expression* e) {
    FloatBounds b = compute_float_bounds(env,e);
    if (b.valid)
      return b.l;
    else
      throw EvalError(env, e->loc(),"cannot determine bounds");
  }
  FloatVal ub_varoptfloat(EnvI& env, Expression* e) {
    FloatBounds b = compute_float_bounds(env,e);
    if (b.valid)
      return b.u;
    else
      throw EvalError(env, e->loc(),"cannot determine bounds");
  }

  FloatVal b_lb_varoptfloat(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size() != 1)
      throw EvalError(env, Location(), "dynamic type error");
    return lb_varoptfloat(env,args[0]);
  }
  FloatVal b_ub_varoptfloat(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size() != 1)
      throw EvalError(env, Location(), "dynamic type error");
    return ub_varoptfloat(env,args[0]);
  }

  FloatVal b_array_lb_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    Expression* e = follow_id_to_decl(args[0]);
    
    bool foundMin = false;
    FloatVal array_lb = 0.0;
    
    if (VarDecl* vd = e->dyn_cast<VarDecl>()) {
      if (vd->ti()->domain()) {
        FloatSetVal* fsv = eval_floatset(env, vd->ti()->domain());
        array_lb = fsv->min();
        foundMin = true;
      }
      e = vd->e();
    }
    
    if (e != NULL) {
      GCLock lock;
      ArrayLit* al = eval_array_lit(env,e);
      if (al->v().size()==0)
        throw EvalError(env, Location(), "lower bound of empty array undefined");
      bool min_valid = false;
      FloatVal min = 0.0;
      for (unsigned int i=0; i<al->v().size(); i++) {
        FloatBounds fb = compute_float_bounds(env,al->v()[i]);
        if (!fb.valid)
          goto b_array_lb_float_done;
        if (min_valid) {
          min = std::min(min, fb.l);
        } else {
          min_valid = true;
          min = fb.l;
        }
      }
      assert(min_valid);
      if (foundMin)
        array_lb = std::max(array_lb, min);
      else
        array_lb = min;
      foundMin = true;
    }
  b_array_lb_float_done:
    if (foundMin) {
      return array_lb;
    } else {
      throw EvalError(env, e->loc(),"cannot determine lower bound");
    }
  }
  
  FloatVal b_array_ub_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    Expression* e = follow_id_to_decl(args[0]);
    
    bool foundMax = false;
    FloatVal array_ub = 0.0;
    
    if (VarDecl* vd = e->dyn_cast<VarDecl>()) {
      if (vd->ti()->domain()) {
        FloatSetVal* fsv = eval_floatset(env, vd->ti()->domain());
        array_ub = fsv->max();
        foundMax = true;
      }
      e = vd->e();
    }
    
    if (e != NULL) {
      GCLock lock;
      ArrayLit* al = eval_array_lit(env,e);
      if (al->v().size()==0)
        throw EvalError(env, Location(), "upper bound of empty array undefined");
      bool max_valid = false;
      FloatVal max = 0.0;
      for (unsigned int i=0; i<al->v().size(); i++) {
        FloatBounds fb = compute_float_bounds(env,al->v()[i]);
        if (!fb.valid)
          goto b_array_ub_float_done;
        if (max_valid) {
          max = std::max(max, fb.u);
        } else {
          max_valid = true;
          max = fb.u;
        }
      }
      assert(max_valid);
      if (foundMax)
        array_ub = std::min(array_ub, max);
      else
        array_ub = max;
      foundMax = true;
    }
  b_array_ub_float_done:
    if (foundMax) {
      return array_ub;
    } else {
      throw EvalError(env, e->loc(),"cannot determine upper bound");
    }
  }
  
  FloatVal b_sum_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[0]);
    if (al->v().size()==0)
      return 0;
    FloatVal m = 0;
    for (unsigned int i=0; i<al->v().size(); i++)
      m += eval_float(env,al->v()[i]);
    return m;
  }

  FloatVal b_float_min(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    switch (args.size()) {
      case 1:
        if (args[0]->type().is_set()) {
          throw EvalError(env, args[0]->loc(), "sets not supported");
        } else {
          GCLock lock;
          ArrayLit* al = eval_array_lit(env,args[0]);
          if (al->v().size()==0)
            throw EvalError(env, al->loc(), "min on empty array undefined");
          FloatVal m = eval_float(env,al->v()[0]);
          for (unsigned int i=1; i<al->v().size(); i++)
            m = std::min(m, eval_float(env,al->v()[i]));
          return m;
        }
      case 2:
      {
        return std::min(eval_float(env,args[0]),eval_float(env,args[1]));
      }
      default:
        throw EvalError(env, Location(), "dynamic type error");
    }
  }
  
  FloatVal b_float_max(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    switch (args.size()) {
      case 1:
        if (args[0]->type().is_set()) {
          throw EvalError(env, args[0]->loc(), "sets not supported");
        } else {
          GCLock lock;
          ArrayLit* al = eval_array_lit(env,args[0]);
          if (al->v().size()==0)
            throw EvalError(env, al->loc(), "max on empty array undefined");
          FloatVal m = eval_float(env,al->v()[0]);
          for (unsigned int i=1; i<al->v().size(); i++)
            m = std::max(m, eval_float(env,al->v()[i]));
          return m;
        }
      case 2:
      {
        return std::max(eval_float(env,args[0]),eval_float(env,args[1]));
      }
      default:
        throw EvalError(env, Location(), "dynamic type error");
    }
  }
  
  IntSetVal* b_index_set(EnvI& env, Expression* e, int i) {
    if (e->eid() != Expression::E_ID) {
      GCLock lock;
      ArrayLit* al = eval_array_lit(env,e);
      if (al->dims() < i)
        throw EvalError(env, e->loc(), "index_set: wrong dimension");
      return IntSetVal::a(al->min(i-1),al->max(i-1));
    }
    Id* id = e->cast<Id>();
    if (id->decl() == NULL)
      throw EvalError(env, id->loc(), "undefined identifier");
    if ( ( id->decl()->ti()->ranges().size()==1 &&
           id->decl()->ti()->ranges()[0]->domain() != NULL &&
           id->decl()->ti()->ranges()[0]->domain()->isa<TIId>() ) ||
         ( static_cast<int>(id->decl()->ti()->ranges().size()) >= i &&
           ( id->decl()->ti()->ranges()[i-1]->domain() == NULL ||
             id->decl()->ti()->ranges()[i-1]->domain()->isa<TIId>()) )) {
      GCLock lock;
      ArrayLit* al = eval_array_lit(env,id);
      if (al->dims() < i)
        throw EvalError(env, id->loc(), "index_set: wrong dimension");
      return IntSetVal::a(al->min(i-1),al->max(i-1));
    }
    if (static_cast<int>(id->decl()->ti()->ranges().size()) < i)
      throw EvalError(env, id->loc(), "index_set: wrong dimension");
    return eval_intset(env,id->decl()->ti()->ranges()[i-1]->domain());
  }
  bool b_index_sets_agree(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size() != 2)
      throw EvalError(env, Location(), "index_sets_agree needs exactly two arguments");
    GCLock lock;
    ArrayLit* al0 = eval_array_lit(env,args[0]);
    ArrayLit* al1 = eval_array_lit(env,args[1]);
    if (al0->type().dim() != al1->type().dim())
      return false;
    for (int i=1; i<=al0->type().dim(); i++) {
      IntSetVal* index0 = b_index_set(env, al0, i);
      IntSetVal* index1 = b_index_set(env, al1, i);
      if (!index0->equal(index1))
        return false;
    }
    return true;
  }
  IntSetVal* b_index_set1(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size() != 1)
      throw EvalError(env, Location(), "index_set needs exactly one argument");
    return b_index_set(env,args[0],1);
  }
  IntSetVal* b_index_set2(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size() != 1)
      throw EvalError(env, Location(), "index_set needs exactly one argument");
    return b_index_set(env,args[0],2);
  }
  IntSetVal* b_index_set3(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size() != 1)
      throw EvalError(env, Location(), "index_set needs exactly one argument");
    return b_index_set(env,args[0],3);
  }
  IntSetVal* b_index_set4(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size() != 1)
      throw EvalError(env, Location(), "index_set needs exactly one argument");
    return b_index_set(env,args[0],4);
  }
  IntSetVal* b_index_set5(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size() != 1)
      throw EvalError(env, Location(), "index_set needs exactly one argument");
    return b_index_set(env,args[0],5);
  }
  IntSetVal* b_index_set6(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size() != 1)
      throw EvalError(env, Location(), "index_set needs exactly one argument");
    return b_index_set(env,args[0],6);
  }

  IntVal b_min_parsetint(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 1);
    IntSetVal* isv = eval_intset(env,args[0]);
    return isv->min();
  }
  IntVal b_max_parsetint(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 1);
    IntSetVal* isv = eval_intset(env,args[0]);
    return isv->max();
  }
  IntSetVal* b_lb_set(EnvI& env, Call* e) {
    Expression* ee = eval_par(env, e->args()[0]);
    if (ee->type().ispar()) {
      return eval_intset(env, ee);
    }
    return IntSetVal::a();
  }
  IntSetVal* b_ub_set(EnvI& env, Expression* e) {
    IntSetVal* isv = compute_intset_bounds(env,e);
    if (isv)
      return isv;
    throw EvalError(env, e->loc(), "cannot determine bounds of set expression");
  }
  IntSetVal* b_ub_set(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 1);
    return b_ub_set(env,args[0]);
  }
  bool b_has_ub_set(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    Expression* e = args[0];
    for (;;) {
      switch (e->eid()) {
        case Expression::E_SETLIT: return true;
        case Expression::E_ID:
        {
          Id* id = e->cast<Id>();
          if (id->decl()==NULL)
            throw EvalError(env, id->loc(),"undefined identifier");
          if (id->decl()->e()==NULL)
            return id->decl()->ti()->domain() != NULL;
          else
            e = id->decl()->e();
        }
          break;
        default:
          throw EvalError(env, e->loc(),"invalid argument to has_ub_set");
      }
    }
  }
  
  IntSetVal* b_array_ub_set(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[0]);
    if (al->v().size()==0)
      throw EvalError(env, Location(), "upper bound of empty array undefined");
    IntSetVal* ub = b_ub_set(env,al->v()[0]);
    for (unsigned int i=1; i<al->v().size(); i++) {
      IntSetRanges isr(ub);
      IntSetRanges r(b_ub_set(env,al->v()[i]));
      Ranges::Union<IntVal,IntSetRanges,IntSetRanges> u(isr,r);
      ub = IntSetVal::ai(u);
    }
    return ub;
  }

  IntSetVal* b_dom_varint(EnvI& env, Expression* e) {
    Id* lastid = NULL;
    Expression* cur = e;
    for (;;) {
      if (cur==NULL) {
        if (lastid==NULL || lastid->decl()->ti()->domain()==NULL) {
          IntBounds b = compute_int_bounds(env,e);
          if (b.valid)
            return IntSetVal::a(b.l,b.u);
          else
            return IntSetVal::a(-IntVal::infinity(),IntVal::infinity());
        } else {
          return eval_intset(env,lastid->decl()->ti()->domain());
        }
      }
      switch (cur->eid()) {
      case Expression::E_INTLIT:
        {
          IntVal v = cur->cast<IntLit>()->v();
          return IntSetVal::a(v,v);
        }
      case Expression::E_ID:
        {
          lastid = cur->cast<Id>();
          if (lastid->decl()==NULL)
            throw EvalError(env, lastid->loc(),"undefined identifier");
          cur = lastid->decl()->e();
        }
        break;
      case Expression::E_ARRAYACCESS:
        {
          bool success;
          cur = eval_arrayaccess(env,cur->cast<ArrayAccess>(), success);
          if (!success) {
            cur = NULL;
          }
        }
        break;
      default:
        cur = NULL;
        break;
      }
    }
  }
  IntSetVal* b_dom_varint(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 1);
    return b_dom_varint(env,args[0]);
  }

  IntSetVal* b_dom_bounds_array(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    Expression* arg_e = args[0];
    Expression* e = follow_id_to_decl(arg_e);
    
    bool foundBounds = false;
    IntVal array_lb = -IntVal::infinity();
    IntVal array_ub = IntVal::infinity();
    
    if (VarDecl* vd = e->dyn_cast<VarDecl>()) {
      if (vd->ti()->domain()) {
        GCLock lock;
        IntSetVal* isv = eval_intset(env,vd->ti()->domain());
        if (isv->size()!=0) {
          array_lb = isv->min();
          array_ub = isv->max();
          foundBounds = true;
        }
      }
      e = vd->e();
    }

    if (foundBounds) {
      return IntSetVal::a(array_lb,array_ub);
    }
    
    if (e != NULL) {
      GCLock lock;
      ArrayLit* al = eval_array_lit(env,e);
      if (al->v().size()==0)
        throw EvalError(env, Location(), "lower bound of empty array undefined");
      IntVal min = IntVal::infinity();
      IntVal max = -IntVal::infinity();
      for (unsigned int i=0; i<al->v().size(); i++) {
        IntBounds ib = compute_int_bounds(env,al->v()[i]);
        if (!ib.valid)
          goto b_array_lb_int_done;
        min = std::min(min, ib.l);
        max = std::max(max, ib.u);
      }
      array_lb = std::max(array_lb, min);
      array_ub = std::min(array_ub, max);
      foundBounds = true;
    }
  b_array_lb_int_done:
    if (foundBounds) {
      return IntSetVal::a(array_lb,array_ub);
    } else {
      throw EvalError(env, e->loc(),"cannot determine lower bound");
    }
  }
  
  IntSetVal* b_dom_array(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 1);
    Expression* ae = args[0];
    ArrayLit* al = NULL;
    while (al==NULL) {
      switch (ae->eid()) {
      case Expression::E_ARRAYLIT:
        al = ae->cast<ArrayLit>();
        break;
      case Expression::E_ID:
        {
          Id* id = ae->cast<Id>();
          if (id->decl()==NULL)
            throw EvalError(env, id->loc(),"undefined identifier");
          if (id->decl()->e()==NULL)
            throw EvalError(env, id->loc(),"array without initialiser");
          else
            ae = id->decl()->e();
        }
        break;
      default:
        throw EvalError(env, ae->loc(),"invalid argument to dom");
      }
    }
    if (al->v().size()==0)
      return IntSetVal::a();
    IntSetVal* isv = b_dom_varint(env,al->v()[0]);
    for (unsigned int i=1; i<al->v().size(); i++) {
      IntSetRanges isr(isv);
      IntSetRanges r(b_dom_varint(env,al->v()[i]));
      Ranges::Union<IntVal,IntSetRanges,IntSetRanges> u(isr,r);
      isv = IntSetVal::ai(u);
    }
    return isv;
  }
  IntSetVal* b_compute_div_bounds(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==2);
    IntBounds bx = compute_int_bounds(env,args[0]);
    if (!bx.valid)
      throw EvalError(env, args[0]->loc(),"cannot determine bounds");
    /// TODO: better bounds if only some input bounds are infinite
    if (!bx.l.isFinite() || !bx.u.isFinite())
      return constants().infinity->isv();
    IntBounds by = compute_int_bounds(env,args[1]);
    if (!by.valid)
      throw EvalError(env, args[1]->loc(),"cannot determine bounds");
    if (!by.l.isFinite() || !by.u.isFinite())
      return constants().infinity->isv();
    Ranges::Const<IntVal> byr(by.l,by.u);
    Ranges::Const<IntVal> by0(0,0);
    Ranges::Diff<IntVal,Ranges::Const<IntVal>, Ranges::Const<IntVal> > byr0(byr,by0);


    IntVal min=IntVal::maxint();
    IntVal max=IntVal::minint();
    if (byr0()) {
      min = std::min(min, bx.l / byr0.min());
      min = std::min(min, bx.l / byr0.max());
      min = std::min(min, bx.u / byr0.min());
      min = std::min(min, bx.u / byr0.max());
      max = std::max(max, bx.l / byr0.min());
      max = std::max(max, bx.l / byr0.max());
      max = std::max(max, bx.u / byr0.min());
      max = std::max(max, bx.u / byr0.max());
      ++byr0;
      if (byr0()) {
        min = std::min(min, bx.l / byr0.min());
        min = std::min(min, bx.l / byr0.max());
        min = std::min(min, bx.u / byr0.min());
        min = std::min(min, bx.u / byr0.max());
        max = std::max(max, bx.l / byr0.min());
        max = std::max(max, bx.l / byr0.max());
        max = std::max(max, bx.u / byr0.min());
        max = std::max(max, bx.u / byr0.max());
      }
    }
    return IntSetVal::a(min,max);
  }

  ArrayLit* b_arrayXd(EnvI& env, ASTExprVec<Expression> args, int d) {
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[d]);
    std::vector<std::pair<int,int> > dims(d);
    unsigned int dim1d = 1;
    for (int i=0; i<d; i++) {
      IntSetVal* di = eval_intset(env,args[i]);
      if (di->size()==0) {
        dims[i] = std::pair<int,int>(1,0);
        dim1d = 0;
      } else if (di->size() != 1) {
        throw EvalError(env, args[i]->loc(), "arrayXd only defined for ranges");
      } else {
        dims[i] = std::pair<int,int>(static_cast<int>(di->min(0).toInt()),
                                     static_cast<int>(di->max(0).toInt()));
        dim1d *= dims[i].second-dims[i].first+1;
      }
    }
    if (dim1d != al->v().size())
      throw EvalError(env, al->loc(), "mismatch in array dimensions");
    ArrayLit* ret = new ArrayLit(al->loc(), al->v(), dims);
    Type t = al->type();
    t.dim(d);
    ret->type(t);
    ret->flat(al->flat());
    return ret;
  }
  Expression* b_array1d_list(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[0]);
    if (al->dims()==1 && al->min(0)==1) {
      return args[0]->isa<Id>() ? args[0] : al;
    }
    ArrayLit* ret = new ArrayLit(al->loc(), al->v());
    Type t = al->type();
    t.dim(1);
    ret->type(t);
    ret->flat(al->flat());
    return ret;
  }
  Expression* b_array1d(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return b_arrayXd(env,args,1);
  }
  Expression* b_array2d(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return b_arrayXd(env,args,2);
  }
  Expression* b_array3d(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return b_arrayXd(env,args,3);
  }
  Expression* b_array4d(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return b_arrayXd(env,args,4);
  }
  Expression* b_array5d(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return b_arrayXd(env,args,5);
  }
  Expression* b_array6d(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return b_arrayXd(env,args,6);
  }

  Expression* b_arrayXd(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    GCLock lock;
    ArrayLit* al0 = eval_array_lit(env,args[0]);
    ArrayLit* al1 = eval_array_lit(env,args[1]);
    if (al0->dims()==al1->dims()) {
      bool sameDims = true;
      for (unsigned int i=al0->dims(); i--;) {
        if (al0->min(i)!=al1->min(i) || al0->max(i)!=al1->max(i)) {
          sameDims = false;
          break;
        }
      }
      if (sameDims)
        return args[1]->isa<Id>() ? args[1] : al1;
    }
    std::vector<std::pair<int,int> > dims(al0->dims());
    for (unsigned int i=al0->dims(); i--;) {
      dims[i] = std::make_pair(al0->min(i), al0->max(i));
    }
    ArrayLit* ret = new ArrayLit(al1->loc(), al1->v(), dims);
    Type t = al1->type();
    t.dim(dims.size());
    ret->type(t);
    ret->flat(al1->flat());
    return ret;
  }
  
  IntVal b_length(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[0]);
    return al->v().size();
  }
  
  IntVal b_bool2int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return eval_bool(env,args[0]) ? 1 : 0;
  }

  bool b_forall_par(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size()!=1)
      throw EvalError(env, Location(), "forall needs exactly one argument");
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[0]);
    for (unsigned int i=al->v().size(); i--;)
      if (!eval_bool(env,al->v()[i]))
        return false;
    return true;
  }
  bool b_exists_par(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size()!=1)
      throw EvalError(env, Location(), "exists needs exactly one argument");
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[0]);
    for (unsigned int i=al->v().size(); i--;)
      if (eval_bool(env,al->v()[i]))
        return true;
    return false;
  }
  bool b_clause_par(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size()!=2)
      throw EvalError(env, Location(), "clause needs exactly two arguments");
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[0]);
    for (unsigned int i=al->v().size(); i--;)
      if (eval_bool(env,al->v()[i]))
        return true;
    al = eval_array_lit(env,args[1]);
    for (unsigned int i=al->v().size(); i--;)
      if (!eval_bool(env,al->v()[i]))
        return true;
    return false;
  }
  bool b_xorall_par(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size()!=1)
      throw EvalError(env, Location(), "xorall needs exactly one argument");
    GCLock lock;
    int count = 0;
    ArrayLit* al = eval_array_lit(env,args[0]);
    for (unsigned int i=al->v().size(); i--;)
      count += eval_bool(env,al->v()[i]);
    return count % 2 == 1;
  }
  bool b_iffall_par(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size()!=1)
      throw EvalError(env, Location(), "xorall needs exactly one argument");
    GCLock lock;
    int count = 0;
    ArrayLit* al = eval_array_lit(env,args[0]);
    for (unsigned int i=al->v().size(); i--;)
      count += eval_bool(env,al->v()[i]);
    return count % 2 == 0;
  }
  
  IntVal b_card(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    if (args.size()!=1)
      throw EvalError(env, Location(), "card needs exactly one argument");
    IntSetVal* isv = eval_intset(env,args[0]);
    IntSetRanges isr(isv);
    return Ranges::cardinality(isr);
  }
  
  Expression* exp_is_fixed(EnvI& env, Expression* e) {
    GCLock lock;
    Expression* cur = eval_par(env,e);
    for (;;) {
      if (cur==NULL)
        return NULL;
      if (cur->type().ispar())
        return cur;
      switch (cur->eid()) {
        case Expression::E_ID:
          cur = cur->cast<Id>()->decl();
          break;
        case Expression::E_VARDECL:
          if (cur->type().st() != Type::ST_SET) {
            Expression* dom = cur->cast<VarDecl>()->ti()->domain();
            if (dom && (dom->isa<IntLit>() || dom->isa<BoolLit>() || dom->isa<FloatLit>()))
              return dom;
          }
          cur = cur->cast<VarDecl>()->e();
          break;
        default:
          return NULL;
      }
    }
  }
  
  bool b_is_fixed(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    return exp_is_fixed(env,args[0]) != NULL;
  }

  bool b_is_fixed_array(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[0]);
    if (al->v().size()==0)
      return true;
    for (unsigned int i=0; i<al->v().size(); i++) {
      if (exp_is_fixed(env,al->v()[i])==NULL)
        return false;
    }
    return true;
  }

  Expression* b_fix(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    Expression* ret = exp_is_fixed(env,args[0]);
    if (ret==NULL)
      throw EvalError(env, args[0]->loc(), "expression is not fixed");
    return ret;
  }

  IntVal b_fix_int(EnvI& env, Call* call) {
    return eval_int(env,b_fix(env,call));
  }
  bool b_fix_bool(EnvI& env, Call* call) {
    return eval_bool(env,b_fix(env,call));
  }
  FloatVal b_fix_float(EnvI& env, Call* call) {
    return eval_float(env,b_fix(env,call));
  }
  IntSetVal* b_fix_set(EnvI& env, Call* call) {
    return eval_intset(env,b_fix(env,call));
  }

  Expression* b_fix_array(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[0]);
    std::vector<Expression*> fixed(al->v().size());
    for (unsigned int i=0; i<fixed.size(); i++) {
      fixed[i] = exp_is_fixed(env,al->v()[i]);
      if (fixed[i]==NULL)
        throw EvalError(env, al->v()[i]->loc(), "expression is not fixed");
    }
    ArrayLit* ret = new ArrayLit(Location(), fixed);
    Type tt = al->type();
    tt.ti(Type::TI_PAR);
    ret->type(tt);
    return ret;
  }

  FloatVal b_int2float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return eval_int(env,args[0]);
  }
  IntVal b_ceil(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return static_cast<IntVal>(std::ceil(eval_float(env,args[0])));
  }
  IntVal b_floor(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return static_cast<IntVal>(std::floor(eval_float(env,args[0])));
  }
  IntVal b_round(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return static_cast<IntVal>(eval_float(env,args[0])+0.5);
  }
  FloatVal b_log10(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return std::log10(eval_float(env,args[0]).toDouble());
  }
  FloatVal b_log2(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return std::log(eval_float(env,args[0]).toDouble()) / std::log(2.0);
  }
  FloatVal b_ln(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return std::log(eval_float(env,args[0]).toDouble());
  }
  FloatVal b_log(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return std::log(eval_float(env,args[1]).toDouble()) / std::log(eval_float(env,args[0]).toDouble());
  }
  FloatVal b_exp(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return std::exp(eval_float(env,args[0]).toDouble());
  }
  FloatVal b_pow(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return std::pow(eval_float(env,args[0]).toDouble(),eval_float(env,args[1]).toDouble());
  }
  IntVal b_pow_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    IntVal p = eval_int(env,args[0]);
    IntVal r = 1;
    long long int e = eval_int(env,args[1]).toInt();
    if (e < 0)
      throw EvalError(env, args[1]->loc(), "Cannot raise integer to a negative power");
    for (long long int i=e; i--;)
      r = r*p;
    return r;
  }
  FloatVal b_sqrt(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return std::sqrt(eval_float(env,args[0]).toDouble());
  }
  
  bool b_assert_bool(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==2);
    GCLock lock;
    if (eval_bool(env,args[0]))
      return true;
    StringLit* err = eval_par(env,args[1])->cast<StringLit>();
    throw EvalError(env, args[0]->loc(),"Assertion failed: "+err->v().str());
  }

  Expression* b_assert(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==3);
    GCLock lock;
    if (eval_bool(env,args[0]))
      return args[2];
    StringLit* err = eval_par(env,args[1])->cast<StringLit>();
    throw EvalError(env, args[0]->loc(),"Assertion failed: "+err->v().str());
  }

  bool b_abort(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    StringLit* err = eval_par(env,args[0])->cast<StringLit>();
    throw EvalError(env, args[0]->loc(),"Abort: "+err->v().str());
  }
  
  Expression* b_trace(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    GCLock lock;
    StringLit* msg = eval_par(env,args[0])->cast<StringLit>();
    std::cerr << msg->v();
    return args.size()==1 ? constants().lit_true : args[1];
  }

  Expression* b_trace_stdout(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    GCLock lock;
    StringLit* msg = eval_par(env,args[0])->cast<StringLit>();
    std::cout << msg->v();
    return args.size()==1 ? constants().lit_true : args[1];
  }
  
  bool b_in_redundant_constraint(EnvI& env, Call*) {
    return env.in_redundant_constraint > 0;
  }
  
  Expression* b_set2array(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    GCLock lock;
    IntSetVal* isv = eval_intset(env,args[0]);
    std::vector<Expression*> elems;
    IntSetRanges isr(isv);
    for (Ranges::ToValues<IntSetRanges> isr_v(isr); isr_v(); ++isr_v)
      elems.push_back(IntLit::a(isr_v.val()));
    ArrayLit* al = new ArrayLit(args[0]->loc(),elems);
    al->type(Type::parint(1));
    return al;
  }

  IntVal b_string_length(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    GCLock lock;
    std::string s = eval_string(env,args[0]);
    return s.size();
  }
  
  std::string show(EnvI& env, Expression* exp) {
    std::ostringstream oss;
    GCLock lock;
    Printer p(oss,0,false);
    Expression* e = eval_par(env,exp);
    if (e->type().isvar()) {
      p.print(e);
    } else {
      e = eval_par(env,e);
      if (ArrayLit* al = e->dyn_cast<ArrayLit>()) {
        oss << "[";
        for (unsigned int i=0; i<al->v().size(); i++) {
          p.print(al->v()[i]);
          if (i<al->v().size()-1)
            oss << ", ";
        }
        oss << "]";
      } else {
        p.print(e);
      }
    }
    return oss.str();
  }
  std::string b_show(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    return show(env,args[0]);
  }
  
  std::string b_show_json_basic(EnvI& env, Expression* e) {
    std::ostringstream oss;
    Printer p(oss,0,false);
    if (SetLit* sl = e->dyn_cast<SetLit>()) {
      oss << "{ \"set\" : [";
      if (IntSetVal* isv = sl->isv()) {
        bool first=true;
        for (IntSetRanges isr(isv); isr(); ++isr) {
          if (first) {
            first=false;
          } else {
            oss << ",";
          }
          if (isr.min()==isr.max()) {
            oss << isr.min();
          } else {
            oss << "[" << isr.min() << "," << isr.max() << "]";
          }
        }
      } else if (FloatSetVal* fsv = sl->fsv()) {
        bool first=true;
        for (FloatSetRanges fsr(fsv); fsr(); ++fsr) {
          if (first) {
            first=false;
          } else {
            oss << ",";
          }
          if (fsr.min()==fsr.max()) {
            ppFloatVal(oss, fsr.min());
          } else {
            oss << "[";
            ppFloatVal(oss, fsr.min());
            oss << ",";
            ppFloatVal(oss, fsr.max());
            oss << "]";
          }
        }
      } else {
        for (unsigned int i=0; i<sl->v().size(); i++) {
          p.print(sl->v()[i]);
          if (i<sl->v().size()-1)
            oss << ",";
        }
      }
      oss << "]}";
    } else {
      p.print(e);
    }
    return oss.str();
  }
  
  std::string b_show_json(EnvI& env, Call* call) {
    Expression* exp = call->args()[0];
    GCLock lock;
    Expression* e = eval_par(env,exp);
    if (e->type().isvar()) {
      std::ostringstream oss;
      Printer p(oss,0,false);
      p.print(e);
      return oss.str();
    } else {
      if (ArrayLit* al = e->dyn_cast<ArrayLit>()) {
        
        std::vector<unsigned int> dims(al->dims()-1);
        if (dims.size() != 0) {
          dims[0] = al->max(al->dims()-1)-al->min(al->dims()-1)+1;
        }
        
        for (unsigned int i=1; i<al->dims()-1; i++) {
          dims[i] = dims[i-1] * (al->max(al->dims()-1-i)-al->min(al->dims()-1-i)+1);
        }

        std::ostringstream oss;
        oss << "[";
        for (unsigned int i=0; i<al->v().size(); i++) {
          for (unsigned int j=0; j<dims.size(); j++) {
            if (i % dims[j] == 0) {
              oss << "[";
            }
          }
          oss << b_show_json_basic(env, al->v()[i]);
          for (unsigned int j=0; j<dims.size(); j++) {
            if (i % dims[j] == dims[j]-1) {
              oss << "]";
            }
          }
          
          if (i<al->v().size()-1)
            oss << ", ";
        }
        oss << "]";
        
        return oss.str();
      } else {
        return b_show_json_basic(env, e);
      }
    }
  }
  
  std::string b_format(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    int width = 0;
    int prec = -1;
    GCLock lock;
    Expression* e;
    if (args.size()>1) {
      width = eval_int(env,args[0]).toInt();
      if (args.size()==2) {
        e = eval_par(env,args[1]);
      } else {
        assert(args.size()==3);
        prec = eval_int(env,args[1]).toInt();
        if (prec < 0)
          throw EvalError(env, args[1]->loc(),"output precision cannot be negative");
        e = eval_par(env,args[2]);
      }
    } else {
      e = eval_par(env,args[0]);
    }
    if (e->type() == Type::parint()) {
      long long int i = eval_int(env,e).toInt();
      std::ostringstream formatted;
      if (width > 0) {
        formatted.width(width);
      } else if (width < 0) {
        formatted.width(-width);
        formatted.flags(std::ios::left);
      }
      if (prec != -1)
        formatted.precision(prec);
      formatted << i;
      return formatted.str();
    } else if (e->type() == Type::parfloat()) {
      FloatVal i = eval_float(env,e);
      std::ostringstream formatted;
      if (width > 0) {
        formatted.width(width);
      } else if (width < 0) {
        formatted.width(-width);
        formatted.flags(std::ios::left);
      }
      formatted.setf(std::ios::fixed);
      formatted.precision(std::numeric_limits<double>::digits10+2);
      if (prec != -1)
        formatted.precision(prec);
      formatted << i;
      return formatted.str();
    } else {
      std::string s = show(env,e);
      if (prec >= 0 && prec < s.size())
        s = s.substr(0,prec);
      std::ostringstream oss;
      if (s.size() < std::abs(width)) {
        int addLeft = width < 0 ? 0 : (width - s.size());
        if (addLeft < 0) addLeft = 0;
        int addRight = width < 0 ? (-width-s.size()) : 0;
        if (addRight < 0) addRight = 0;
        for (int i=addLeft; i--;)
          oss << " ";
        oss << s;
        for (int i=addRight; i--;)
          oss << " ";
        return oss.str();
      } else {
        return s;
      }
    }
  }
  
  std::string b_show_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==2);
    GCLock lock;
    Expression* e = eval_par(env,args[1]);
    std::ostringstream oss;
    if (IntLit* iv = e->dyn_cast<IntLit>()) {
      int justify = static_cast<int>(eval_int(env,args[0]).toInt());
      std::ostringstream oss_length;
      oss_length << iv->v();
      int iv_length = static_cast<int>(oss_length.str().size());
      int addLeft = justify < 0 ? 0 : (justify - iv_length);
      if (addLeft < 0) addLeft = 0;
      int addRight = justify < 0 ? (-justify-iv_length) : 0;
      if (addRight < 0) addRight = 0;
      for (int i=addLeft; i--;)
        oss << " ";
      oss << iv->v();
      for (int i=addRight; i--;)
        oss << " ";
    } else {
      Printer p(oss,0,false);
      p.print(e);
    }
    return oss.str();
  }

  std::string b_show_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==3);
    GCLock lock;
    Expression* e = eval_par(env,args[2]);
    std::ostringstream oss;
    if (FloatLit* fv = e->dyn_cast<FloatLit>()) {
      int justify = static_cast<int>(eval_int(env,args[0]).toInt());
      int prec = static_cast<int>(eval_int(env,args[1]).toInt());
      if (prec < 0)
        throw EvalError(env, args[1]->loc(), "number of digits in show_float cannot be negative");
      std::ostringstream oss_length;
      oss_length << std::setprecision(prec) << std::fixed << fv->v();
      int fv_length = static_cast<int>(oss_length.str().size());
      int addLeft = justify < 0 ? 0 : (justify - fv_length);
      if (addLeft < 0) addLeft = 0;
      int addRight = justify < 0 ? (-justify-fv_length) : 0;
      if (addRight < 0) addRight = 0;
      for (int i=addLeft; i--;)
        oss << " ";
      oss << std::setprecision(prec) << std::fixed << fv->v();
      for (int i=addRight; i--;)
        oss << " ";
    } else {
      Printer p(oss,0,false);
      p.print(e);
    }
    return oss.str();
  }

  std::string b_file_path(EnvI&, Call* call) {
    return FileUtils::file_path(call->loc().filename().str());
  }
  
  std::string b_concat(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[0]);
    std::ostringstream oss;
    for (unsigned int i=0; i<al->v().size(); i++) {
      oss << eval_string(env,al->v()[i]);
    }
    return oss.str();
  }

  std::string b_join(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==2);
    std::string sep = eval_string(env,args[0]);
    GCLock lock;
    ArrayLit* al = eval_array_lit(env,args[1]);
    std::ostringstream oss;
    for (unsigned int i=0; i<al->v().size(); i++) {
      oss << eval_string(env,al->v()[i]);
      if (i<al->v().size()-1)
        oss << sep;
    }
    return oss.str();
  }

  IntSetVal* b_array_union(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    ArrayLit* al = eval_array_lit(env,args[0]);
    if (al->v().size()==0)
      return IntSetVal::a();
    IntSetVal* isv = eval_intset(env,al->v()[0]);
    for (unsigned int i=0; i<al->v().size(); i++) {
      IntSetRanges i0(isv);
      IntSetRanges i1(eval_intset(env,al->v()[i]));
      Ranges::Union<IntVal,IntSetRanges, IntSetRanges> u(i0,i1);
      isv = IntSetVal::ai(u);
    }
    return isv;
  }
  
  IntSetVal* b_array_intersect(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    ArrayLit* al = eval_array_lit(env,args[0]);
    std::vector<IntSetVal::Range> ranges;
    if (al->v().size() > 0) {
      IntSetVal* i0 = eval_intset(env,al->v()[0]);
      if (i0->size() > 0) {
        IntSetRanges i0r(i0);
        IntVal min = i0r.min();
        while (i0r()) {
          // Initialize with last interval
          IntVal max = i0r.max();
          // Intersect with all other intervals
        restart:
          for (int j=al->v().size(); j--;) {
            IntSetRanges ij(eval_intset(env,al->v()[j]));
            // Skip intervals that are too small
            while (ij() && (ij.max() < min))
              ++ij;
            if (!ij())
              goto done;
            if (ij.min() > max) {
              min=ij.min();
              max=ij.max();
              goto restart;
            }
            // Now the intervals overlap
            if (min < ij.min())
              min = ij.min();
            if (max > ij.max())
              max = ij.max();
          }
          ranges.push_back(IntSetVal::Range(min,max));
          // The next interval must be at least two elements away
          min = max + 2;
        }
      done:
        return IntSetVal::a(ranges);
      } else {
        return IntSetVal::a();
      }
    } else {
      return IntSetVal::a();
    }
  }
  
  Expression* b_sort_by_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==2);
    ArrayLit* al = eval_array_lit(env,args[0]);
    ArrayLit* order_e = eval_array_lit(env,args[1]);
    std::vector<IntVal> order(order_e->v().size());
    std::vector<int> a(order_e->v().size());
    for (unsigned int i=0; i<order.size(); i++) {
      a[i] = i;
      order[i] = eval_int(env,order_e->v()[i]);
    }
    struct Ord {
      std::vector<IntVal>& order;
      Ord(std::vector<IntVal>& order0) : order(order0) {}
      bool operator()(int i, int j) {
        return order[i] < order[j];
      }
    } _ord(order);
    std::stable_sort(a.begin(), a.end(), _ord);
    std::vector<Expression*> sorted(a.size());
    for (unsigned int i=sorted.size(); i--;)
      sorted[i] = al->v()[a[i]];
    ArrayLit* al_sorted = new ArrayLit(al->loc(), sorted);
    al_sorted->type(al->type());
    return al_sorted;
  }

  Expression* b_sort_by_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==2);
    ArrayLit* al = eval_array_lit(env,args[0]);
    ArrayLit* order_e = eval_array_lit(env,args[1]);
    std::vector<FloatVal> order(order_e->v().size());
    std::vector<int> a(order_e->v().size());
    for (unsigned int i=0; i<order.size(); i++) {
      a[i] = i;
      order[i] = eval_float(env,order_e->v()[i]);
    }
    struct Ord {
      std::vector<FloatVal>& order;
      Ord(std::vector<FloatVal>& order0) : order(order0) {}
      bool operator()(int i, int j) {
        return order[i] < order[j];
      }
    } _ord(order);
    std::stable_sort(a.begin(), a.end(), _ord);
    std::vector<Expression*> sorted(a.size());
    for (unsigned int i=sorted.size(); i--;)
      sorted[i] = al->v()[a[i]];
    ArrayLit* al_sorted = new ArrayLit(al->loc(), sorted);
    al_sorted->type(al->type());
    return al_sorted;
  }

  Expression* b_sort(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    ArrayLit* al = eval_array_lit(env,args[0]);
    std::vector<Expression*> sorted(al->v().size());
    for (unsigned int i=sorted.size(); i--;)
      sorted[i] = al->v()[i];
    struct Ord {
      EnvI& env;
      Ord(EnvI& env0) : env(env0) {}
      bool operator()(Expression* e0, Expression* e1) {
        switch (e0->type().bt()) {
          case Type::BT_INT: return eval_int(env,e0) < eval_int(env,e1);
          case Type::BT_BOOL: return eval_bool(env,e0) < eval_bool(env,e1);
          case Type::BT_FLOAT: return eval_float(env,e0) < eval_float(env,e1);
          default: throw EvalError(env, e0->loc(), "unsupported type for sorting");
        }
      }
    } _ord(env);
    std::sort(sorted.begin(),sorted.end(),_ord);
    ArrayLit* al_sorted = new ArrayLit(al->loc(), sorted);
    al_sorted->type(al->type());
    return al_sorted;
  }
  
  std::default_random_engine& rnd_generator(void) {
    // TODO: initiate with seed if given as annotation/in command line
    static std::default_random_engine g;
    return g;
  }

  FloatVal b_normal_float_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() ==2);
    const double mean = eval_float(env,args[0]).toDouble();
    const double stdv = eval_float(env,args[1]).toDouble();
    std::normal_distribution<double> distribution(mean,stdv);
    // return a sample from the distribution
    return distribution(rnd_generator());
  }
  
  FloatVal b_normal_int_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() ==2);
    const double mean = double(eval_int(env,args[0]).toInt());
    const double stdv = eval_float(env,args[1]).toDouble();
    std::normal_distribution<double> distribution(mean,stdv);
    // return a sample from the distribution
    return distribution(rnd_generator());
  }
  
  FloatVal b_uniform_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 2);
    const double lb = eval_float(env,args[0]).toDouble();
    const double ub = eval_float(env,args[1]).toDouble();
    if(lb > ub) {
      std::stringstream ssm; ssm << "lowerbound of uniform distribution \"" 
      << lb << "\" is higher than its upperbound: " << ub;
      throw EvalError(env, args[0]->loc(),ssm.str());
    }
    std::uniform_real_distribution<double> distribution(lb,ub);
    // return a sample from the distribution
    return distribution(rnd_generator());   
  }
  
  IntVal b_uniform_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 2);
    const long long int lb = eval_int(env,args[0]).toInt();
    const long long int ub = eval_int(env,args[1]).toInt();
    if(lb > ub) {
      std::stringstream ssm; ssm << "lowerbound of uniform distribution \"" 
      << lb << "\" is higher than its upperbound: " << ub;
      throw EvalError(env, args[0]->loc(),ssm.str());
    }
    std::uniform_int_distribution<long long int> distribution(lb,ub);
    // return a sample from the distribution
    return IntVal(distribution(rnd_generator()));
  }
  
  IntVal b_poisson_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 1);
    long long int mean = eval_int(env,args[0]).toInt();
    std::poisson_distribution<long long int> distribution(mean);
    // return a sample from the distribution
    return IntVal(distribution(rnd_generator()));  
  }
  
  IntVal b_poisson_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 1);
    double mean = eval_float(env,args[0]).toDouble();
    std::poisson_distribution<long long int> distribution(mean);
    // return a sample from the distribution
    return IntVal(distribution(rnd_generator())); 
  }

  FloatVal b_gamma_float_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 2);
    const double alpha = eval_float(env,args[0]).toDouble();
    const double beta = eval_float(env,args[1]).toDouble();
    std::gamma_distribution<double> distribution(alpha,beta);
    // return a sample from the distribution
    return distribution(rnd_generator());     
  }
  
  FloatVal b_gamma_int_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 2);
    const double alpha = eval_float(env,args[0]).toDouble();
    const double beta = eval_float(env,args[1]).toDouble();
    std::gamma_distribution<double> distribution(alpha,beta);
    // return a sample from the distribution
    return distribution(rnd_generator());   
  }
  
  FloatVal b_weibull_int_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 2);
    const double shape = double(eval_int(env,args[0]).toInt());
    if(shape < 0) {
      std::stringstream ssm; 
      ssm << "The shape factor for the weibull distribution \"" 
          << shape << "\" has to be greater than zero.";
      throw EvalError(env, args[0]->loc(),ssm.str());
    }
    const double scale = eval_float(env,args[1]).toDouble();
    if(scale < 0) {
      std::stringstream ssm; 
      ssm << "The scale factor for the weibull distribution \"" 
          << scale << "\" has to be greater than zero.";
      throw EvalError(env, args[1]->loc(),ssm.str());
    }
    std::weibull_distribution<double> distribution(shape, scale);
    // return a sample from the distribution
    return distribution(rnd_generator());  
  }
  
  FloatVal b_weibull_float_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 2);
    const double shape = eval_float(env,args[0]).toDouble();
    if(shape < 0) {
      std::stringstream ssm; 
      ssm << "The shape factor for the weibull distribution \"" 
          << shape << "\" has to be greater than zero.";
      throw EvalError(env, args[0]->loc(),ssm.str());
    }
    const double scale = eval_float(env,args[1]).toDouble();
    if(scale < 0) {
      std::stringstream ssm; 
      ssm << "The scale factor for the weibull distribution \"" 
          << scale << "\" has to be greater than zero.";
      throw EvalError(env, args[1]->loc(),ssm.str());
    }
    std::weibull_distribution<double> distribution(shape, scale);
    // return a sample from the distribution
    return distribution(rnd_generator());
  }
  
  FloatVal b_exponential_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 1);
    const double lambda = eval_float(env,args[0]).toDouble();
    if(lambda < 0) {
      std::stringstream ssm; 
      ssm << "The lambda-parameter for the exponential distribution function \"" 
          << lambda << "\" has to be greater than zero.";
      throw EvalError(env, args[0]->loc(),ssm.str());
    }
    std::exponential_distribution<double> distribution(lambda);
    // return a sample from the distribution
    return distribution(rnd_generator());     
  }
  
  FloatVal b_exponential_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 1);
    const double lambda = double(eval_int(env,args[0]).toInt());
    if(lambda < 0) {
      std::stringstream ssm; 
      ssm << "The lambda-parameter for the exponential distribution function \"" 
          << lambda << "\" has to be greater than zero.";
      throw EvalError(env, args[0]->loc(),ssm.str());
    }      
    std::exponential_distribution<double> distribution(lambda);
    // return a sample from the distribution
    return distribution(rnd_generator());
  }
  
  FloatVal b_lognormal_float_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() ==2);
    const double mean = eval_float(env,args[0]).toDouble();
    const double stdv = eval_float(env,args[1]).toDouble();
    std::lognormal_distribution<double> distribution(mean,stdv);
    // return a sample from the distribution
    return distribution(rnd_generator()); 
  }
  
  FloatVal b_lognormal_int_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() ==2);
    const double mean = double(eval_int(env,args[0]).toInt());
    const double stdv = eval_float(env,args[1]).toDouble();
    std::lognormal_distribution<double> distribution(mean,stdv);
    // return a sample from the distribution
    return distribution(rnd_generator());
  }
  
  FloatVal b_chisquared_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 1);
    const double lambda = eval_float(env,args[0]).toDouble();
    std::exponential_distribution<double> distribution(lambda);
    // return a sample from the distribution
    return distribution(rnd_generator());
  }
  
  FloatVal b_chisquared_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 1);
    const double lambda = double(eval_int(env,args[0]).toInt());
    std::exponential_distribution<double> distribution(lambda);
    // return a sample from the distribution
    return distribution(rnd_generator());
  }
  
  FloatVal b_cauchy_float_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() ==2);
    const double mean = eval_float(env,args[0]).toDouble();
    const double scale = eval_float(env,args[1]).toDouble();
    std::cauchy_distribution<double> distribution(mean,scale);
    // return a sample from the distribution
    return distribution(rnd_generator());   
  }
  
  FloatVal b_cauchy_int_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() ==2);
    const double mean = double(eval_int(env,args[0]).toInt());
    const double scale = eval_float(env,args[1]).toDouble();
    std::cauchy_distribution<double> distribution(mean,scale);
    // return a sample from the distribution
    return distribution(rnd_generator());
  }
  
  FloatVal b_fdistribution_float_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() ==2);
    const double d1 = eval_float(env,args[0]).toDouble();
    const double d2 = eval_float(env,args[1]).toDouble();
    std::fisher_f_distribution<double> distribution(d1,d2);
    // return a sample from the distribution
    return distribution(rnd_generator());    
  }  
  
  FloatVal b_fdistribution_int_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() ==2);
    const double d1 = double(eval_int(env,args[0]).toInt());
    const double d2 = double(eval_int(env,args[1]).toInt());
    std::fisher_f_distribution<double> distribution(d1,d2);
    // return a sample from the distribution
    return distribution(rnd_generator());   
  }  
  
  FloatVal b_tdistribution_float(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 1);
    const double sampleSize = eval_float(env,args[0]).toDouble();
    std::student_t_distribution<double> distribution(sampleSize);
    // return a sample from the distribution
    return distribution(rnd_generator());
  }
  
  FloatVal b_tdistribution_int(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 1);
    const double sampleSize = double(eval_int(env,args[0]).toInt());
    std::student_t_distribution<double> distribution(sampleSize);
    // return a sample from the distribution
    return distribution(rnd_generator());   
  }
  
  IntVal b_discrete_distribution(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 1);
    GCLock lock;    
    ArrayLit* al = eval_array_lit(env,args[0]);
    if(al->dims() != 1) {
      std::stringstream ssm; 
      ssm << "expecting 1-dimensional array of weights for discrete distribution instead of: " 
          << *al << std::endl;
      throw EvalError(env, al->loc(), ssm.str());
    }
    std::vector<long long int> weights(al->v().size());
    for(unsigned int i = 0; i < al->v().size(); i++) {
      weights[i] = eval_int(env,al->v()[i]).toInt();
    }
#ifdef _MSC_VER
    std::size_t i(0);
    std::discrete_distribution<long long int> distribution(weights.size(), 0.0,1.0,
                                                 [&weights,&i](double){ return weights[i++]; });
#else
    std::discrete_distribution<long long int> distribution(weights.begin(), weights.end());
#endif
    // return a sample from the distribution
    IntVal iv = IntVal(distribution(rnd_generator()));
    return iv;         
  }

  bool b_bernoulli(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 1);
    const double p = eval_float(env,args[0]).toDouble();
    std::bernoulli_distribution distribution(p);
    // return a sample from the distribution
    return distribution(rnd_generator());         
  }
  
  IntVal b_binomial(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size() == 2);
    double t = double(eval_int(env,args[0]).toInt());
    double p = eval_float(env,args[1]).toDouble();
    std::binomial_distribution<long long int> distribution(t,p);
    // return a sample from the distribution
    return IntVal(distribution(rnd_generator()));    
  }  
  
  FloatVal b_atan(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    GCLock lock;
    FloatVal f = eval_float(env,args[0]);
    return std::atan(f.toDouble());
  }
  
  FloatVal b_cos(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    GCLock lock;
    FloatVal f = eval_float(env,args[0]);
    return std::cos(f.toDouble());
  }
  
  FloatVal b_sin(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    GCLock lock;
    FloatVal f = eval_float(env,args[0]);
    return std::sin(f.toDouble());
  }
  
  FloatVal b_asin(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    GCLock lock;
    FloatVal f = eval_float(env,args[0]);
    return std::asin(f.toDouble());
  }
  
  FloatVal b_acos(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    GCLock lock;
    FloatVal f = eval_float(env,args[0]);
    return std::acos(f.toDouble());
  }
  
  FloatVal b_tan(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==1);
    GCLock lock;
    FloatVal f = eval_float(env,args[0]);
    return std::tan(f.toDouble());
  }
  
  IntVal b_to_enum(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    assert(args.size()==2);
    IntSetVal* isv = eval_intset(env, args[0]);
    IntVal v = eval_int(env, args[1]);
    if (!isv->contains(v))
      throw ResultUndefinedError(env, call->loc(), "value outside of enum range");
    return v;
  }
  
  IntVal b_enum_next(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    IntSetVal* isv = eval_intset(env, args[0]);
    IntVal v = eval_int(env, args[1]);
    if (!isv->contains(v+1))
      throw ResultUndefinedError(env, call->loc(), "value outside of enum range");
    return v+1;
  }

  IntVal b_enum_prev(EnvI& env, Call* call) {
    ASTExprVec<Expression> args = call->args();
    IntSetVal* isv = eval_intset(env, args[0]);
    IntVal v = eval_int(env, args[1]);
    if (!isv->contains(v-1))
      throw ResultUndefinedError(env, call->loc(), "value outside of enum range");
    return v-1;
  }

  IntVal b_mzn_compiler_version(EnvI&, Call*) {
    return atoi(MZN_VERSION_MAJOR)*10000+atoi(MZN_VERSION_MINOR)*1000+atoi(MZN_VERSION_PATCH);
  }
  
  void registerBuiltins(Env& e, Model* m) {
    EnvI& env = e.envi();
    
    std::vector<Type> t_intint(2);
    t_intint[0] = Type::parint();
    t_intint[1] = Type::parint();

    std::vector<Type> t_intarray(1);
    t_intarray[0] = Type::parint(-1);
    
    GCLock lock;
    rb(env, m, ASTString("min"), t_intint, b_int_min);
    rb(env, m, ASTString("min"), t_intarray, b_int_min);
    rb(env, m, ASTString("max"), t_intint, b_int_max);
    rb(env, m, ASTString("max"), t_intarray, b_int_max);
    rb(env, m, constants().ids.sum, t_intarray, b_sum_int);
    rb(env, m, ASTString("product"), t_intarray, b_product_int);
    rb(env, m, ASTString("pow"), t_intint, b_pow_int);

    {
      std::vector<Type> t(2);
      t[0] = Type::top(-1);
      t[1] = Type::top(-1);
      rb(env, m, ASTString("index_sets_agree"), t, b_index_sets_agree);
    }
    {
      std::vector<Type> t_anyarray1(1);
      t_anyarray1[0] = Type::optvartop(1);
      rb(env, m, ASTString("index_set"), t_anyarray1, b_index_set1);
    }
    {
      std::vector<Type> t_anyarray2(1);
      t_anyarray2[0] = Type::optvartop(2);
      rb(env, m, ASTString("index_set_1of2"), t_anyarray2, b_index_set1);
      rb(env, m, ASTString("index_set_2of2"), t_anyarray2, b_index_set2);
    }
    {
      std::vector<Type> t_anyarray3(1);
      t_anyarray3[0] = Type::optvartop(3);
      rb(env, m, ASTString("index_set_1of3"), t_anyarray3, b_index_set1);
      rb(env, m, ASTString("index_set_2of3"), t_anyarray3, b_index_set2);
      rb(env, m, ASTString("index_set_3of3"), t_anyarray3, b_index_set3);
    }
    {
      std::vector<Type> t_anyarray4(1);
      t_anyarray4[0] = Type::optvartop(4);
      rb(env, m, ASTString("index_set_1of4"), t_anyarray4, b_index_set1);
      rb(env, m, ASTString("index_set_2of4"), t_anyarray4, b_index_set2);
      rb(env, m, ASTString("index_set_3of4"), t_anyarray4, b_index_set3);
      rb(env, m, ASTString("index_set_4of4"), t_anyarray4, b_index_set4);
    }
    {
      std::vector<Type> t_anyarray5(1);
      t_anyarray5[0] = Type::optvartop(5);
      rb(env, m, ASTString("index_set_1of5"), t_anyarray5, b_index_set1);
      rb(env, m, ASTString("index_set_2of5"), t_anyarray5, b_index_set2);
      rb(env, m, ASTString("index_set_3of5"), t_anyarray5, b_index_set3);
      rb(env, m, ASTString("index_set_4of5"), t_anyarray5, b_index_set4);
      rb(env, m, ASTString("index_set_5of5"), t_anyarray5, b_index_set5);
    }
    {
      std::vector<Type> t_anyarray6(1);
      t_anyarray6[0] = Type::optvartop(6);
      rb(env, m, ASTString("index_set_1of6"), t_anyarray6, b_index_set1);
      rb(env, m, ASTString("index_set_2of6"), t_anyarray6, b_index_set2);
      rb(env, m, ASTString("index_set_3of6"), t_anyarray6, b_index_set3);
      rb(env, m, ASTString("index_set_4of6"), t_anyarray6, b_index_set4);
      rb(env, m, ASTString("index_set_5of6"), t_anyarray6, b_index_set5);
      rb(env, m, ASTString("index_set_6of6"), t_anyarray6, b_index_set6);
    }
    {
      std::vector<Type> t_arrayXd(1);
      t_arrayXd[0] = Type::top(-1);
      rb(env, m, ASTString("array1d"), t_arrayXd, b_array1d_list);
      t_arrayXd[0] = Type::vartop(-1);
      rb(env, m, ASTString("array1d"), t_arrayXd, b_array1d_list);
      t_arrayXd[0] = Type::optvartop(-1);
      rb(env, m, ASTString("array1d"), t_arrayXd, b_array1d_list);
    }
    {
      std::vector<Type> t_arrayXd(2);
      t_arrayXd[0] = Type::parsetint();
      t_arrayXd[1] = Type::top(-1);
      rb(env, m, ASTString("array1d"), t_arrayXd, b_array1d);
      t_arrayXd[1] = Type::vartop(-1);
      rb(env, m, ASTString("array1d"), t_arrayXd, b_array1d);
      t_arrayXd[1] = Type::optvartop(-1);
      rb(env, m, ASTString("array1d"), t_arrayXd, b_array1d);
    }
    {
      std::vector<Type> t_arrayXd(2);
      t_arrayXd[0] = Type::top(-1);
      t_arrayXd[1] = Type::top(-1);
      rb(env, m, ASTString("arrayXd"), t_arrayXd, b_arrayXd);
      t_arrayXd[1] = Type::vartop(-1);
      rb(env, m, ASTString("arrayXd"), t_arrayXd, b_arrayXd);
      t_arrayXd[1] = Type::optvartop(-1);
      rb(env, m, ASTString("arrayXd"), t_arrayXd, b_arrayXd);
    }
    {
      std::vector<Type> t_arrayXd(3);
      t_arrayXd[0] = Type::parsetint();
      t_arrayXd[1] = Type::parsetint();
      t_arrayXd[2] = Type::top(-1);
      rb(env, m, ASTString("array2d"), t_arrayXd, b_array2d);
      t_arrayXd[2] = Type::vartop(-1);
      rb(env, m, ASTString("array2d"), t_arrayXd, b_array2d);
      t_arrayXd[2] = Type::optvartop(-1);
      rb(env, m, ASTString("array2d"), t_arrayXd, b_array2d);
    }
    {
      std::vector<Type> t_arrayXd(4);
      t_arrayXd[0] = Type::parsetint();
      t_arrayXd[1] = Type::parsetint();
      t_arrayXd[2] = Type::parsetint();
      t_arrayXd[3] = Type::top(-1);
      rb(env, m, ASTString("array3d"), t_arrayXd, b_array3d);
      t_arrayXd[3] = Type::vartop(-1);
      rb(env, m, ASTString("array3d"), t_arrayXd, b_array3d);
      t_arrayXd[3] = Type::optvartop(-1);
      rb(env, m, ASTString("array3d"), t_arrayXd, b_array3d);
    }
    {
      std::vector<Type> t_arrayXd(5);
      t_arrayXd[0] = Type::parsetint();
      t_arrayXd[1] = Type::parsetint();
      t_arrayXd[2] = Type::parsetint();
      t_arrayXd[3] = Type::parsetint();
      t_arrayXd[4] = Type::top(-1);
      rb(env, m, ASTString("array4d"), t_arrayXd, b_array4d);
      t_arrayXd[4] = Type::vartop(-1);
      rb(env, m, ASTString("array4d"), t_arrayXd, b_array4d);
      t_arrayXd[4] = Type::optvartop(-1);
      rb(env, m, ASTString("array4d"), t_arrayXd, b_array4d);
    }
    {
      std::vector<Type> t_arrayXd(6);
      t_arrayXd[0] = Type::parsetint();
      t_arrayXd[1] = Type::parsetint();
      t_arrayXd[2] = Type::parsetint();
      t_arrayXd[3] = Type::parsetint();
      t_arrayXd[4] = Type::parsetint();
      t_arrayXd[5] = Type::top(-1);
      rb(env, m, ASTString("array5d"), t_arrayXd, b_array5d);
      t_arrayXd[5] = Type::vartop(-1);
      rb(env, m, ASTString("array5d"), t_arrayXd, b_array5d);
      t_arrayXd[5] = Type::optvartop(-1);
      rb(env, m, ASTString("array5d"), t_arrayXd, b_array5d);
    }
    {
      std::vector<Type> t_arrayXd(7);
      t_arrayXd[0] = Type::parsetint();
      t_arrayXd[1] = Type::parsetint();
      t_arrayXd[2] = Type::parsetint();
      t_arrayXd[3] = Type::parsetint();
      t_arrayXd[4] = Type::parsetint();
      t_arrayXd[5] = Type::parsetint();
      t_arrayXd[6] = Type::top(-1);
      rb(env, m, ASTString("array6d"), t_arrayXd, b_array6d);
      t_arrayXd[6] = Type::vartop(-1);
      rb(env, m, ASTString("array6d"), t_arrayXd, b_array6d);
      t_arrayXd[6] = Type::optvartop(-1);
      rb(env, m, ASTString("array6d"), t_arrayXd, b_array6d);
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::parbool();
      t[1] = Type::parstring();
      rb(env, m, constants().ids.assert, t, b_assert_bool);
    }
    {
      std::vector<Type> t(3);
      t[0] = Type::parbool();
      t[1] = Type::parstring();
      t[2] = Type::top();
      rb(env, m, constants().ids.assert, t, b_assert);
      t[2] = Type::vartop();
      rb(env, m, constants().ids.assert, t, b_assert);
      t[2] = Type::optvartop();
      rb(env, m, constants().ids.assert, t, b_assert);
      t[2] = Type::top(-1);
      rb(env, m, constants().ids.assert, t, b_assert);
      t[2] = Type::vartop(-1);
      rb(env, m, constants().ids.assert, t, b_assert);
      t[2] = Type::optvartop(-1);
      rb(env, m, constants().ids.assert, t, b_assert);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parstring();
      rb(env, m, ASTString("abort"), t, b_abort);
      rb(env, m, constants().ids.trace, t, b_trace);
      rb(env, m, ASTString("trace_stdout"), t, b_trace_stdout);
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::parstring();
      t[1] = Type::top();
      rb(env, m, constants().ids.trace, t, b_trace);
      rb(env, m, ASTString("trace_stdout"), t, b_trace_stdout);
      t[1] = Type::vartop();
      rb(env, m, constants().ids.trace, t, b_trace);
      rb(env, m, ASTString("trace_stdout"), t, b_trace_stdout);
      t[1] = Type::optvartop();
      rb(env, m, constants().ids.trace, t, b_trace);
      rb(env, m, ASTString("trace_stdout"), t, b_trace_stdout);
    }
    {
      rb(env, m, ASTString("mzn_in_redundant_constraint"), std::vector<Type>(), b_in_redundant_constraint);
    }
    {
      std::vector<Type> t_length(1);
      t_length[0] = Type::optvartop(-1);
      rb(env, m, ASTString("length"), t_length, b_length);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parbool();
      rb(env, m, constants().ids.bool2int, t, b_bool2int);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parbool(-1);
      rb(env, m, constants().ids.forall, t, b_forall_par);
      rb(env, m, constants().ids.exists, t, b_exists_par);
      rb(env, m, ASTString("xorall"), t, b_xorall_par);
      rb(env, m, ASTString("iffall"), t, b_iffall_par);
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::parbool(-1);
      t[1] = Type::parbool(-1);
      rb(env, m, constants().ids.clause, t, b_clause_par);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varsetint();
      rb(env, m, ASTString("ub"), t, b_ub_set);
      rb(env, m, ASTString("lb"), t, b_lb_set);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varsetint(1);
      rb(env, m, ASTString("ub_array"), t, b_array_ub_set);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint();
      rb(env, m, ASTString("dom"), t, b_dom_varint);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint(-1);
      rb(env, m, ASTString("dom_array"), t, b_dom_array);
      rb(env, m, ASTString("dom_bounds_array"), t, b_dom_bounds_array);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parsetint();
      rb(env, m, ASTString("min"), t, b_min_parsetint);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parsetint();
      rb(env, m, ASTString("max"), t, b_max_parsetint);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint();
      t[0].ot(Type::OT_OPTIONAL);
      rb(env, m, ASTString("lb"), t, b_lb_varoptint);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint();
      t[0].ot(Type::OT_OPTIONAL);
      rb(env, m, ASTString("ub"), t, b_ub_varoptint);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint();
      rb(env, m, ASTString("lb"), t, b_lb_varoptint);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint();
      rb(env, m, ASTString("ub"), t, b_ub_varoptint);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint(-1);
      t[0].ot(Type::OT_OPTIONAL);
      rb(env, m, ASTString("lb_array"), t, b_array_lb_int);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint(-1);
      t[0].ot(Type::OT_OPTIONAL);
      rb(env, m, ASTString("ub_array"), t, b_array_ub_int);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varfloat();
      rb(env, m, ASTString("lb"), t, b_lb_varoptfloat);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varfloat();
      rb(env, m, ASTString("ub"), t, b_ub_varoptfloat);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varfloat(-1);
      rb(env, m, ASTString("lb_array"), t, b_array_lb_float);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varfloat(-1);
      rb(env, m, ASTString("ub_array"), t, b_array_ub_float);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parsetint();
      rb(env, m, ASTString("card"), t, b_card);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parint();
      rb(env, m, ASTString("abs"), t, b_abs_int);
      t[0] = Type::parfloat();
      rb(env, m, ASTString("abs"), t, b_abs_float);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint();
      rb(env, m, ASTString("has_bounds"), t, b_has_bounds_int);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varfloat();
      rb(env, m, ASTString("has_bounds"), t, b_has_bounds_float);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varsetint();
      rb(env, m, ASTString("has_ub_set"), t, b_has_ub_set);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::optvartop();
      rb(env, m, ASTString("is_fixed"), t, b_is_fixed);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::optvartop(-1);
      rb(env, m, ASTString("is_fixed"), t, b_is_fixed_array);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::optvartop();
      rb(env, m, ASTString("fix"), t, b_fix_bool);
      rb(env, m, ASTString("fix"), t, b_fix_int);
      rb(env, m, ASTString("fix"), t, b_fix_set);
      rb(env, m, ASTString("fix"), t, b_fix_float);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::optvartop(1);
      rb(env, m, ASTString("fix"), t, b_fix_array);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parint();
      rb(env, m, ASTString("int2float"), t, b_int2float);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parfloat();
      rb(env, m, ASTString("ceil"), t, b_ceil);
      rb(env, m, ASTString("floor"), t, b_floor);
      rb(env, m, ASTString("round"), t, b_round);
      rb(env, m, ASTString("log10"), t, b_log10);
      rb(env, m, ASTString("log2"), t, b_log2);
      rb(env, m, ASTString("ln"), t, b_ln);
      rb(env, m, ASTString("exp"), t, b_exp);
      rb(env, m, ASTString("sqrt"), t, b_sqrt);
      t.push_back(Type::parfloat());
      rb(env, m, ASTString("log"), t, b_log);
      rb(env, m, ASTString("pow"), t, b_pow);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parfloat(1);
      rb(env, m, constants().ids.sum, t, b_sum_float);      
      rb(env, m, ASTString("product"), t, b_product_float);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parfloat(1);
      rb(env, m, ASTString("min"), t, b_float_min);
      rb(env, m, ASTString("max"), t, b_float_max);

      t[0] = Type::parfloat();
      t.push_back(Type::parfloat());
      rb(env, m, ASTString("min"), t, b_float_min);
      rb(env, m, ASTString("max"), t, b_float_max);      
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parsetint();
      rb(env, m, ASTString("set2array"), t, b_set2array);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parstring();
      rb(env, m, ASTString("string_length"), t, b_string_length);
    }
    {
      rb(env, m, ASTString("file_path"), std::vector<Type>(), b_file_path);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::vartop();
      rb(env, m, ASTString("show"), t, b_show);
      rb(env, m, ASTString("showJSON"), t, b_show_json);
      t[0] = Type::vartop();
      t[0].st(Type::ST_SET);
      t[0].ot(Type::OT_OPTIONAL);
      rb(env, m, ASTString("show"), t, b_show);
      rb(env, m, ASTString("showJSON"), t, b_show_json);
      t[0] = Type::vartop(-1);
      rb(env, m, ASTString("show"), t, b_show);
      rb(env, m, ASTString("showJSON"), t, b_show_json);
    }
    {
      std::vector<Type> t(3);
      t[0] = t[1] = Type::parint();
      t[2] = Type::vartop();
      rb(env, m, ASTString("format"), t, b_format);
      t[2] = Type::vartop();
      t[2].st(Type::ST_SET);
      t[2].ot(Type::OT_OPTIONAL);
      rb(env, m, ASTString("format"), t, b_format);
      t[2] = Type::vartop(-1);
      rb(env, m, ASTString("format"), t, b_format);
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::parint();
      t[1] = Type::vartop();
      rb(env, m, ASTString("format"), t, b_format);
      t[1] = Type::vartop();
      t[1].st(Type::ST_SET);
      t[1].ot(Type::OT_OPTIONAL);
      rb(env, m, ASTString("format"), t, b_format);
      t[1] = Type::vartop(-1);
      rb(env, m, ASTString("format"), t, b_format);
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::parint();
      t[1] = Type::varint();
      rb(env, m, ASTString("show_int"), t, b_show_int);
    }
    {
      std::vector<Type> t(3);
      t[0] = Type::parint();
      t[1] = Type::parint();
      t[2] = Type::varfloat();
      rb(env, m, ASTString("show_float"), t, b_show_float);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parstring(1);
      rb(env, m, ASTString("concat"), t, b_concat);
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::parstring();
      t[1] = Type::parstring(1);
      rb(env, m, ASTString("join"), t, b_join);
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::varint();
      t[1] = Type::varint();
      rb(env, m, ASTString("compute_div_bounds"), t, b_compute_div_bounds);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parsetint(1);
      rb(env, m, ASTString("array_intersect"), t, b_array_intersect);
      rb(env, m, ASTString("array_union"), t, b_array_union);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parint();
      t[0].ot(Type::OT_OPTIONAL);
      rb(env, m, ASTString("occurs"), t, b_occurs);
      rb(env, m, ASTString("deopt"), t, b_deopt_int);
      t[0].bt(Type::BT_BOOL);
      rb(env, m, ASTString("occurs"), t, b_occurs);
      rb(env, m, ASTString("deopt"), t, b_deopt_bool);
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::varbot(1);
      t[1] = Type::parint(1);
      rb(env, m, ASTString("sort_by"), t, b_sort_by_int);
      t[0] = Type::bot(1);
      rb(env, m, ASTString("sort_by"), t, b_sort_by_int);
      t[0].ot(Type::OT_OPTIONAL);
      rb(env, m, ASTString("sort_by"), t, b_sort_by_int);
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::varbot(1);
      t[1] = Type::parfloat(1);
      rb(env, m, ASTString("sort_by"), t, b_sort_by_float);
      t[0] = Type::bot(1);
      rb(env, m, ASTString("sort_by"), t, b_sort_by_float);
      t[0].ot(Type::OT_OPTIONAL);
      rb(env, m, ASTString("sort_by"), t, b_sort_by_float);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parint(1);
      rb(env, m, ASTString("sort"), t, b_sort);
      rb(env, m, ASTString("arg_min"), t, b_arg_min_int);
      rb(env, m, ASTString("arg_max"), t, b_arg_max_int);
      t[0] = Type::parbool(1);
      rb(env, m, ASTString("sort"), t, b_sort);
      t[0] = Type::parfloat(1);
      rb(env, m, ASTString("sort"), t, b_sort);
      rb(env, m, ASTString("arg_min"), t, b_arg_min_float);
      rb(env, m, ASTString("arg_max"), t, b_arg_max_float);
    }
    {
     std::vector<Type> t(1);
     t[0] = Type::parfloat();
     rb(env, m, ASTString("atan"), t, b_atan);
    }
    {
     std::vector<Type> t(1);
     t[0] = Type::parfloat();
     rb(env, m, ASTString("cos"), t, b_cos);
    }
    {
     std::vector<Type> t(1);
     t[0] = Type::parfloat();
     rb(env, m, ASTString("sin"), t, b_sin);
    }
    {
     std::vector<Type> t(1);
     t[0] = Type::parfloat();
     rb(env, m, ASTString("asin"), t, b_asin);
    }
    {
     std::vector<Type> t(1);
     t[0] = Type::parfloat();
     rb(env, m, ASTString("acos"), t, b_acos);
    }
    {
     std::vector<Type> t(1);
     t[0] = Type::parfloat();
     rb(env, m, ASTString("tan"), t, b_tan);
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::parfloat();
      t[1] = Type::parfloat();     
      rb(env, m, ASTString("normal"),t,b_normal_float_float); 
      t[0] = Type::parint();     
      rb(env, m, ASTString("normal"),t,b_normal_int_float); 
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::parfloat();
      t[1] = Type::parfloat();     
      rb(env, m, ASTString("uniform"),t,b_uniform_float); 
      t[0] = Type::parint();
      t[1] = Type::parint();     
      rb(env, m, ASTString("uniform"),t,b_uniform_int);  
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parfloat();     
      rb(env, m, ASTString("poisson"),t,b_poisson_float); 
      t[0] = Type::parint();    
      rb(env, m, ASTString("poisson"),t,b_poisson_int);  
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::parfloat();
      t[1] = Type::parfloat();     
      rb(env, m, ASTString("gamma"),t,b_gamma_float_float);
      t[0] = Type::parint();
      rb(env, m, ASTString("gamma"),t,b_gamma_int_float);
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::parfloat();
      t[1] = Type::parfloat();     
      rb(env, m, ASTString("weibull"),t,b_weibull_float_float);
      t[0] = Type::parint();
      rb(env, m, ASTString("weibull"),t,b_weibull_int_float);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parfloat();     
      rb(env, m, ASTString("exponential"),t,b_exponential_float); 
      t[0] = Type::parint();    
      rb(env, m, ASTString("exponential"),t,b_exponential_int);  
    }  
    {
      std::vector<Type> t(2);
      t[0] = Type::parfloat();
      t[1] = Type::parfloat();     
      rb(env, m, ASTString("lognormal"),t,b_lognormal_float_float);
      t[0] = Type::parint();
      rb(env, m, ASTString("lognormal"),t,b_lognormal_int_float);
    } 
    {
      std::vector<Type> t(1);
      t[0] = Type::parfloat();     
      rb(env, m, ASTString("chisquared"),t,b_chisquared_float); 
      t[0] = Type::parint();    
      rb(env, m, ASTString("chisquared"),t,b_chisquared_int);  
    }  
    {
      std::vector<Type> t(2);
      t[0] = Type::parfloat(); 
      t[1] = Type::parfloat(); 
      rb(env, m, ASTString("cauchy"),t,b_cauchy_float_float); 
      t[0] = Type::parint();    
      rb(env, m, ASTString("cauchy"),t,b_cauchy_int_float);  
    }   
    {
      std::vector<Type> t(2);
      t[0] = Type::parfloat(); 
      t[1] = Type::parfloat(); 
      rb(env, m, ASTString("fdistribution"),t,b_fdistribution_float_float);  
      t[0] = Type::parint(); 
      t[1] = Type::parint(); 
      rb(env, m, ASTString("fdistribution"),t,b_fdistribution_int_int);  
    } 
    {
      std::vector<Type> t(1);
      t[0] = Type::parfloat();     
      rb(env, m, ASTString("tdistribution"),t,b_tdistribution_float); 
      t[0] = Type::parint();    
      rb(env, m, ASTString("tdistribution"),t,b_tdistribution_int);  
    }  
    {
      std::vector<Type> t(1);
      t[0] = Type::parint(1); 
      rb(env, m, ASTString("discrete_distribution"),t,b_discrete_distribution); 
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parint(); 
      rb(env, m, ASTString("bernoulli"),t,b_bernoulli); 
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::parint(); 
      t[1] = Type::parfloat(); 
      rb(env, m, ASTString("binomial"),t,b_binomial);  
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::parsetint();
      t[1] = Type::parint();
      rb(env, m, ASTString("to_enum"),t,b_to_enum);
      rb(env, m, ASTString("enum_next"),t,b_enum_next);
      rb(env, m, ASTString("enum_prev"),t,b_enum_prev);
    }
    {
      rb(env, m, ASTString("mzn_compiler_version"), std::vector<Type>(), b_mzn_compiler_version);
    }
  }
  
}


