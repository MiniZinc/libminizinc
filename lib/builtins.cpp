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
#include <minizinc/exception.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/prettyprinter.hh>

#include <iomanip>

namespace MiniZinc {
  
  void rb(Model* m, const ASTString& id, const std::vector<Type>& t, 
          FunctionI::builtin_e b) {
    FunctionI* fi = m->matchFn(id,t);
    if (fi) {
      fi->_builtins.e = b;
    } else {
      assert(false); // TODO: is this an error?
    }
  }
  void rb(Model* m, const ASTString& id, const std::vector<Type>& t, 
          FunctionI::builtin_f b) {
    FunctionI* fi = m->matchFn(id,t);
    if (fi) {
      fi->_builtins.f = b;
    } else {
      assert(false); // TODO: is this an error?
    }
  }
  void rb(Model* m, const ASTString& id, const std::vector<Type>& t, 
          FunctionI::builtin_i b) {
    FunctionI* fi = m->matchFn(id,t);
    if (fi) {
      fi->_builtins.i = b;
    } else {
      assert(false); // TODO: is this an error?
    }
  }
  void rb(Model* m, const ASTString& id, const std::vector<Type>& t, 
          FunctionI::builtin_b b) {
    FunctionI* fi = m->matchFn(id,t);
    if (fi) {
      fi->_builtins.b = b;
    } else {
      assert(false); // TODO: is this an error?
    }
  }
  void rb(Model* m, const ASTString& id, const std::vector<Type>& t, 
          FunctionI::builtin_s b) {
    FunctionI* fi = m->matchFn(id,t);
    if (fi) {
      fi->_builtins.s = b;
    } else {
      assert(false); // TODO: is this an error?
    }
  }
  void rb(Model* m, const ASTString& id, const std::vector<Type>& t,
          FunctionI::builtin_str b) {
    FunctionI* fi = m->matchFn(id,t);
    if (fi) {
      fi->_builtins.str = b;
    } else {
      assert(false); // TODO: is this an error?
    }
  }

  IntVal b_min(ASTExprVec<Expression> args) {
    switch (args.size()) {
    case 1:
      if (args[0]->type().isset()) {
        throw EvalError(args[0]->loc(), "sets not supported");
      } else {
        ArrayLit* al = eval_array_lit(args[0]);
        if (al->v().size()==0)
          throw EvalError(al->loc(), "min on empty array undefined");
        IntVal m = eval_int(al->v()[0]);
        for (unsigned int i=1; i<al->v().size(); i++)
          m = std::min(m, eval_int(al->v()[i]));
        return m;
      }
    case 2:
      {
        return std::min(eval_int(args[0]),eval_int(args[1]));
      }
    default:
      throw EvalError(Location(), "dynamic type error");
    }
  }

  IntVal b_max(ASTExprVec<Expression> args) {
    switch (args.size()) {
    case 1:
      if (args[0]->type().isset()) {
        throw EvalError(args[0]->loc(), "sets not supported");
      } else {
        ArrayLit* al = eval_array_lit(args[0]);
        if (al->v().size()==0)
          throw EvalError(al->loc(), "max on empty array undefined");
        IntVal m = eval_int(al->v()[0]);
        for (unsigned int i=1; i<al->v().size(); i++)
          m = std::max(m, eval_int(al->v()[i]));
        return m;
      }
    case 2:
      {
        return std::max(eval_int(args[0]),eval_int(args[1]));
      }
    default:
      throw EvalError(Location(), "dynamic type error");
    }
  }
  
  bool b_has_bounds(ASTExprVec<Expression> args) {
    if (args.size() != 1)
      throw EvalError(Location(), "dynamic type error");
    return compute_int_bounds(args[0]).valid;
  }
  
  IntVal lb_varoptint(Expression* e) {
    IntBounds b = compute_int_bounds(e);
    if (b.valid)
      return b.l;
    else
      throw EvalError(e->loc(),"cannot determine bounds");
  }
  IntVal b_lb_varoptint(ASTExprVec<Expression> args) {
    if (args.size() != 1)
      throw EvalError(Location(), "dynamic type error");
    return lb_varoptint(args[0]);
  }

  Expression* deref_id(Expression* e) {
    Expression* cur = e;
    for (;;) {
      Id* id = cur->dyn_cast<Id>();
      if (id) {
        if (id->decl()->e()==NULL)
          return id;
        else
          cur = id->decl()->e();
      } else {
        return cur;
      }
    }
  }
  
  IntVal b_array_lb_int(ASTExprVec<Expression> args) {
    assert(args.size()==1);
    Expression* e = deref_id(args[0]);
    if (Id* id = e->dyn_cast<Id>()) {
      if (id->decl()->ti()->domain()) {
        GCLock lock;
        IntSetVal* isv = eval_intset(id->decl()->ti()->domain());
        if (isv->size()==0) {
          throw EvalError(e->loc(),"cannot determine bounds");
        } else {
          return isv->min(0);
        }
      } else {
        throw EvalError(e->loc(),"cannot determine bounds");
      }
    } else {
      ArrayLit* al = eval_array_lit(args[0]);
      if (al->v().size()==0)
        throw EvalError(Location(), "lower bound of empty array undefined");
      IntVal min = lb_varoptint(al->v()[0]);
      for (unsigned int i=1; i<al->v().size(); i++)
        min = std::min(min, lb_varoptint(al->v()[i]));
      return min;
    }
  }

  IntVal ub_varoptint(Expression* e) {
    IntBounds b = compute_int_bounds(e);
    if (b.valid)
      return b.u;
    else
      throw EvalError(e->loc(),"cannot determine bounds");
  }
  IntVal b_ub_varoptint(ASTExprVec<Expression> args) {
    if (args.size() != 1)
      throw EvalError(Location(), "dynamic type error");
    return ub_varoptint(args[0]);
  }

  IntVal b_array_ub_int(ASTExprVec<Expression> args) {
    assert(args.size()==1);
    Expression* e = deref_id(args[0]);
    if (Id* id = e->dyn_cast<Id>()) {
      if (id->decl()->ti()->domain()) {
        GCLock lock;
        IntSetVal* isv = eval_intset(id->decl()->ti()->domain());
        if (isv->size()==0) {
          throw EvalError(e->loc(),"cannot determine bounds");
        } else {
          return isv->max(isv->size()-1);
        }
      } else {
        throw EvalError(e->loc(),"cannot determine bounds");
      }
    } else {
      ArrayLit* al = eval_array_lit(args[0]);
      if (al->v().size()==0)
        throw EvalError(Location(), "upper bound of empty array undefined");
      IntVal max = ub_varoptint(al->v()[0]);
      for (unsigned int i=1; i<al->v().size(); i++)
        max = std::max(max, ub_varoptint(al->v()[i]));
      return max;
    }
  }

  IntVal b_sum(ASTExprVec<Expression> args) {
    assert(args.size()==1);
    ArrayLit* al = eval_array_lit(args[0]);
    if (al->v().size()==0)
      return 0;
    IntVal m = 0;
    for (unsigned int i=0; i<al->v().size(); i++)
      m += eval_int(al->v()[i]);
    return m;
  }

  IntVal b_product(ASTExprVec<Expression> args) {
    assert(args.size()==1);
    ArrayLit* al = eval_array_lit(args[0]);
    if (al->v().size()==0)
      return 1;
    IntVal m = 1;
    for (unsigned int i=0; i<al->v().size(); i++)
      m *= eval_int(al->v()[i]);
    return m;
  }
  
  
  FloatVal b_sum_float(ASTExprVec<Expression> args) {
    assert(args.size()==1);
    ArrayLit* al = eval_array_lit(args[0]);
    if (al->v().size()==0)
      return 0;
    FloatVal m = 0;
    for (unsigned int i=0; i<al->v().size(); i++)
      m += eval_float(al->v()[i]);
    return m;
  }


  IntSetVal* b_index_set(ASTExprVec<Expression> args, int i) {
    if (args.size() != 1)
      throw EvalError(Location(), "index_set needs exactly one argument");
    if (args[0]->eid() != Expression::E_ID)
      throw EvalError(Location(), "index_set only supported for identifiers");
    Id* id = args[0]->cast<Id>();
    if (id->decl() == NULL)
      throw EvalError(id->loc(), "undefined identifier");
    if (id->decl()->ti()->ranges().size() < i)
      throw EvalError(id->loc(), "index_set: wrong dimension");
    if (id->decl()->ti()->ranges()[i-1]->domain() == NULL) {
      ArrayLit* al = eval_array_lit(id);
      if (al->dims() < i)
        throw EvalError(id->loc(), "index_set: wrong dimension");
      return IntSetVal::a(al->min(i-1),al->max(i-1));
    }
    return eval_intset(id->decl()->ti()->ranges()[i-1]->domain());
  }
  IntSetVal* b_index_set1(ASTExprVec<Expression> args) {
    return b_index_set(args,1);
  }
  IntSetVal* b_index_set2(ASTExprVec<Expression> args) {
    return b_index_set(args,2);
  }
  IntSetVal* b_index_set3(ASTExprVec<Expression> args) {
    return b_index_set(args,3);
  }
  IntSetVal* b_index_set4(ASTExprVec<Expression> args) {
    return b_index_set(args,4);
  }
  IntSetVal* b_index_set5(ASTExprVec<Expression> args) {
    return b_index_set(args,5);
  }
  IntSetVal* b_index_set6(ASTExprVec<Expression> args) {
    return b_index_set(args,6);
  }

  IntVal b_min_parsetint(ASTExprVec<Expression> args) {
    assert(args.size() == 1);
    IntSetVal* isv = eval_intset(args[0]);
    return isv->min(0);
  }
  IntVal b_max_parsetint(ASTExprVec<Expression> args) {
    assert(args.size() == 1);
    IntSetVal* isv = eval_intset(args[0]);
    return isv->max(isv->size()-1);
  }
  IntSetVal* b_ub_set(Expression* e) {
    for (;;) {
      switch (e->eid()) {
      case Expression::E_SETLIT: return eval_intset(e);
      case Expression::E_ID:
        {
          Id* id = e->cast<Id>();
          if (id->decl()==NULL)
            throw EvalError(id->loc(),"undefined identifier");
          if (id->decl()->e()==NULL)
            return eval_intset(id->decl()->ti()->domain());
          else
            e = id->decl()->e();
        }
        break;
      default:
        throw EvalError(e->loc(),"invalid argument to ub");
      }
    }
  }
  IntSetVal* b_ub_set(ASTExprVec<Expression> args) {
    assert(args.size() == 1);
    return b_ub_set(args[0]);
  }

  IntSetVal* b_array_ub_set(ASTExprVec<Expression> args) {
    assert(args.size()==1);
    ArrayLit* al = eval_array_lit(args[0]);
    if (al->v().size()==0)
      throw EvalError(Location(), "upper bound of empty array undefined");
    IntSetVal* ub = b_ub_set(al->v()[0]);
    for (unsigned int i=1; i<al->v().size(); i++) {
      IntSetRanges isr(ub);
      IntSetRanges r(b_ub_set(al->v()[i]));
      Ranges::Union<IntSetRanges,IntSetRanges> u(isr,r);
      ub = IntSetVal::ai(u);
    }
    return ub;
  }

  IntSetVal* b_dom_varint(Expression* e) {
    Id* lastid = NULL;
    for (;;) {
      if (e==NULL) {
        if (lastid==NULL) {
          EvalError(e->loc(),"invalid argument to dom");
        } else {
          if (lastid->decl()->ti()->domain()==NULL) {
            EvalError(e->loc(),"invalid argument to dom");
          }
          return eval_intset(lastid->decl()->ti()->domain());
        }
      }
      switch (e->eid()) {
      case Expression::E_INTLIT:
        {
          IntVal v = e->cast<IntLit>()->v();
          return IntSetVal::a(v,v);
        }
      case Expression::E_ID:
        {
          lastid = e->cast<Id>();
          if (lastid->decl()==NULL)
            throw EvalError(lastid->loc(),"undefined identifier");
          e = lastid->decl()->e();
        }
        break;
      default:
        e = NULL;
        break;
      }
    }
  }
  IntSetVal* b_dom_varint(ASTExprVec<Expression> args) {
    assert(args.size() == 1);
    return b_dom_varint(args[0]);
  }

  IntSetVal* b_dom_array(ASTExprVec<Expression> args) {
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
            throw EvalError(id->loc(),"undefined identifier");
          if (id->decl()->e()==NULL)
            throw EvalError(id->loc(),"array without initialiser");
          else
            ae = id->decl()->e();
        }
        break;
      default:
        throw EvalError(ae->loc(),"invalid argument to ub");
      }
    }
    if (al->v().size()==0)
      return IntSetVal::a();
    IntSetVal* isv = b_dom_varint(al->v()[0]);
    for (unsigned int i=1; i<al->v().size(); i++) {
      IntSetRanges isr(isv);
      IntSetRanges r(b_dom_varint(al->v()[i]));
      Ranges::Union<IntSetRanges,IntSetRanges> u(isr,r);
      isv = IntSetVal::ai(u);
    }
    return isv;
  }

  ArrayLit* b_arrayXd(ASTExprVec<Expression> args, int d) {
    ArrayLit* al = eval_array_lit(args[d]);
    std::vector<std::pair<int,int> > dims(d);
    unsigned int dim1d = 1;
    for (int i=0; i<d; i++) {
      IntSetVal* di = eval_intset(args[i]);
      if (di->size()==0) {
        dims[i] = std::pair<int,int>(1,0);
        dim1d = 0;
      } else if (di->size() != 1) {
        throw EvalError(args[i]->loc(), "arrayXd only defined for ranges");
      } else {
        dims[i] = std::pair<int,int>(di->min(0),di->max(0));
        dim1d *= dims[i].second-dims[i].first+1;
      }
    }
    if (dim1d != al->v().size())
      throw EvalError(al->loc(), "mismatch in array dimensions");
    ArrayLit* ret = new ArrayLit(al->loc(), al->v(), dims);
    Type t = al->type();
    t._dim = d;
    ret->type(t);
    return ret;
  }
  Expression* b_array1d(ASTExprVec<Expression> args) {
    return b_arrayXd(args,1);
  }
  Expression* b_array2d(ASTExprVec<Expression> args) {
    return b_arrayXd(args,2);
  }
  Expression* b_array3d(ASTExprVec<Expression> args) {
    return b_arrayXd(args,3);
  }
  Expression* b_array4d(ASTExprVec<Expression> args) {
    return b_arrayXd(args,4);
  }
  Expression* b_array5d(ASTExprVec<Expression> args) {
    return b_arrayXd(args,5);
  }
  Expression* b_array6d(ASTExprVec<Expression> args) {
    return b_arrayXd(args,6);
  }

  IntVal b_length(ASTExprVec<Expression> args) {
    ArrayLit* al = eval_array_lit(args[0]);
    return al->v().size();
  }
  
  IntVal b_bool2int(ASTExprVec<Expression> args) {
    return eval_bool(args[0]) ? 1 : 0;
  }

  bool b_forall_par(ASTExprVec<Expression> args) {
    if (args.size()!=1)
      throw EvalError(Location(), "forall needs exactly one argument");
    ArrayLit* al = eval_array_lit(args[0]);
    for (unsigned int i=al->v().size(); i--;)
      if (!eval_bool(al->v()[i]))
        return false;
    return true;
  }
  bool b_exists_par(ASTExprVec<Expression> args) {
    if (args.size()!=1)
      throw EvalError(Location(), "exists needs exactly one argument");
    ArrayLit* al = eval_array_lit(args[0]);
    for (unsigned int i=al->v().size(); i--;)
      if (eval_bool(al->v()[i]))
        return true;
    return false;
  }
  bool b_clause_par(ASTExprVec<Expression> args) {
    if (args.size()!=2)
      throw EvalError(Location(), "clause needs exactly two arguments");
    ArrayLit* al = eval_array_lit(args[0]);
    for (unsigned int i=al->v().size(); i--;)
      if (eval_bool(al->v()[i]))
        return true;
    al = eval_array_lit(args[1]);
    for (unsigned int i=al->v().size(); i--;)
      if (!eval_bool(al->v()[i]))
        return true;
    return false;
  }

  IntVal b_card(ASTExprVec<Expression> args) {
    if (args.size()!=1)
      throw EvalError(Location(), "card needs exactly one argument");
    IntSetVal* isv = eval_intset(args[0]);
    IntSetRanges isr(isv);
    return Ranges::size(isr);
  }
  
  Expression* exp_is_fixed(Expression* e) {
    Expression* cur = eval_par(e);
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
          if (cur->type()._st != Type::ST_SET) {
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
  
  bool b_is_fixed(ASTExprVec<Expression> args) {
    assert(args.size()==1);
    return exp_is_fixed(args[0]) != NULL;
  }

  bool b_is_fixed_array(ASTExprVec<Expression> args) {
    assert(args.size()==1);
    ArrayLit* al = eval_array_lit(args[0]);
    if (al->v().size()==0)
      return true;
    for (unsigned int i=0; i<al->v().size(); i++) {
      if (exp_is_fixed(al->v()[i])==NULL)
        return false;
    }
    return true;
  }

  Expression* b_fix(ASTExprVec<Expression> args) {
    assert(args.size()==1);
    Expression* ret = exp_is_fixed(args[0]);
    if (ret==NULL)
      throw EvalError(args[0]->loc(), "expression is not fixed");
    return ret;
  }

  IntVal b_fix_int(ASTExprVec<Expression> args) {
    return eval_int(b_fix(args));
  }
  bool b_fix_bool(ASTExprVec<Expression> args) {
    return eval_bool(b_fix(args));
  }

  Expression* b_fix_array(ASTExprVec<Expression> args) {
    assert(args.size()==1);
    ArrayLit* al = eval_array_lit(args[0]);
    std::vector<Expression*> fixed(al->v().size());
    for (unsigned int i=0; i<fixed.size(); i++) {
      fixed[i] = exp_is_fixed(al->v()[i]);
      if (fixed[i]==NULL)
        throw EvalError(al->v()[i]->loc(), "expression is not fixed");
    }
    ArrayLit* ret = new ArrayLit(Location(), fixed);
    return ret;
  }

  FloatVal b_int2float(ASTExprVec<Expression> args) {
    return eval_int(args[0]);
  }
  IntVal b_ceil(ASTExprVec<Expression> args) {
    return std::ceil(eval_float(args[0]));
  }
  IntVal b_floor(ASTExprVec<Expression> args) {
    return std::floor(eval_float(args[0]));
  }
  IntVal b_round(ASTExprVec<Expression> args) {
    return std::round(eval_float(args[0]));
  }
  FloatVal b_log10(ASTExprVec<Expression> args) {
    return std::log10(eval_float(args[0]));
  }
  
  bool b_assert_bool(ASTExprVec<Expression> args) {
    assert(args.size()==2);
    if (eval_bool(args[0]))
      return true;
    StringLit* err = eval_par(args[1])->cast<StringLit>();
    throw EvalError(args[0]->loc(),"Assertion failed: "+err->v().str());
  }

  Expression* b_assert(ASTExprVec<Expression> args) {
    assert(args.size()==3);
    if (eval_bool(args[0]))
      return args[2];
    StringLit* err = eval_par(args[1])->cast<StringLit>();
    throw EvalError(args[0]->loc(),"Assertion failed: "+err->v().str());
  }

  Expression* b_trace(ASTExprVec<Expression> args) {
    assert(args.size()==2);
    StringLit* msg = eval_par(args[0])->cast<StringLit>();
    std::cerr << msg->v();
    return args[1];
  }
  
  Expression* b_set2array(ASTExprVec<Expression> args) {
    assert(args.size()==1);
    IntSetVal* isv = eval_intset(args[0]);
    std::vector<Expression*> elems;
    IntSetRanges isr(isv);
    for (Ranges::ToValues<IntSetRanges> isr_v(isr); isr_v(); ++isr_v)
      elems.push_back(new IntLit(Location(),isr_v.val()));
    ArrayLit* al = new ArrayLit(args[0]->loc(),elems);
    al->type(Type::parint(1));
    return al;
  }
  
  std::string b_show(ASTExprVec<Expression> args) {
    assert(args.size()==1);
    std::ostringstream oss;
    Expression* e = eval_par(args[0]);
    Printer p(oss,0,false);
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
    return oss.str();
  }

  std::string b_show_int(ASTExprVec<Expression> args) {
    assert(args.size()==2);
    Expression* e = eval_par(args[1]);
    std::ostringstream oss;
    if (IntLit* iv = e->dyn_cast<IntLit>()) {
      IntVal justify = eval_int(args[0]);
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

  std::string b_show_float(ASTExprVec<Expression> args) {
    assert(args.size()==3);
    Expression* e = eval_par(args[2]);
    std::ostringstream oss;
    if (FloatLit* fv = e->dyn_cast<FloatLit>()) {
      IntVal justify = eval_int(args[0]);
      IntVal prec = eval_int(args[1]);
      if (prec < 0)
        throw EvalError(args[1]->loc(), "number of digits in show_float cannot be negative");
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

  std::string b_concat(ASTExprVec<Expression> args) {
    assert(args.size()==1);
    ArrayLit* al = eval_array_lit(args[0]);
    std::ostringstream oss;
    for (unsigned int i=0; i<al->v().size(); i++) {
      oss << eval_string(al->v()[i]);
    }
    return oss.str();
  }

  std::string b_join(ASTExprVec<Expression> args) {
    assert(args.size()==2);
    std::string sep = eval_string(args[0]);
    ArrayLit* al = eval_array_lit(args[1]);
    std::ostringstream oss;
    for (unsigned int i=0; i<al->v().size(); i++) {
      oss << eval_string(al->v()[i]);
      if (i<al->v().size()-1)
        oss << sep;
    }
    return oss.str();
  }

  void registerBuiltins(Model* m) {
    
    std::vector<Type> t_intint(2);
    t_intint[0] = Type::parint();
    t_intint[1] = Type::parint();

    std::vector<Type> t_intarray(1);
    t_intarray[0] = Type::parint(-1);
    
    GCLock lock;
    rb(m, ASTString("min"), t_intint, b_min);
    rb(m, ASTString("min"), t_intarray, b_min);
    rb(m, ASTString("max"), t_intint, b_max);
    rb(m, ASTString("max"), t_intarray, b_max);
    rb(m, constants().ids.sum, t_intarray, b_sum);
    rb(m, ASTString("product"), t_intarray, b_product);

    {
      std::vector<Type> t_anyarray1(1);
      t_anyarray1[0] = Type::optvartop(1);
      rb(m, ASTString("index_set"), t_anyarray1, b_index_set1);
    }
    {
      std::vector<Type> t_anyarray2(1);
      t_anyarray2[0] = Type::optvartop(2);
      rb(m, ASTString("index_set_1of2"), t_anyarray2, b_index_set1);
      rb(m, ASTString("index_set_2of2"), t_anyarray2, b_index_set2);
    }
    {
      std::vector<Type> t_anyarray3(1);
      t_anyarray3[0] = Type::optvartop(3);
      rb(m, ASTString("index_set_1of3"), t_anyarray3, b_index_set1);
      rb(m, ASTString("index_set_2of3"), t_anyarray3, b_index_set2);
      rb(m, ASTString("index_set_3of3"), t_anyarray3, b_index_set3);
    }
    {
      std::vector<Type> t_anyarray4(1);
      t_anyarray4[0] = Type::optvartop(4);
      rb(m, ASTString("index_set_1of4"), t_anyarray4, b_index_set1);
      rb(m, ASTString("index_set_2of4"), t_anyarray4, b_index_set2);
      rb(m, ASTString("index_set_3of4"), t_anyarray4, b_index_set3);
      rb(m, ASTString("index_set_4of4"), t_anyarray4, b_index_set4);
    }
    {
      std::vector<Type> t_anyarray5(1);
      t_anyarray5[0] = Type::optvartop(5);
      rb(m, ASTString("index_set_1of5"), t_anyarray5, b_index_set1);
      rb(m, ASTString("index_set_2of5"), t_anyarray5, b_index_set2);
      rb(m, ASTString("index_set_3of5"), t_anyarray5, b_index_set3);
      rb(m, ASTString("index_set_4of5"), t_anyarray5, b_index_set4);
      rb(m, ASTString("index_set_5of5"), t_anyarray5, b_index_set5);
    }
    {
      std::vector<Type> t_anyarray6(1);
      t_anyarray6[0] = Type::optvartop(6);
      rb(m, ASTString("index_set_1of6"), t_anyarray6, b_index_set1);
      rb(m, ASTString("index_set_2of6"), t_anyarray6, b_index_set2);
      rb(m, ASTString("index_set_3of6"), t_anyarray6, b_index_set3);
      rb(m, ASTString("index_set_4of6"), t_anyarray6, b_index_set4);
      rb(m, ASTString("index_set_5of6"), t_anyarray6, b_index_set5);
      rb(m, ASTString("index_set_6of6"), t_anyarray6, b_index_set6);
    }
    {
      std::vector<Type> t_arrayXd(2);
      t_arrayXd[0] = Type::parsetint();
      t_arrayXd[1] = Type::top(-1);
      rb(m, ASTString("array1d"), t_arrayXd, b_array1d);
      t_arrayXd[1] = Type::vartop(-1);
      rb(m, ASTString("array1d"), t_arrayXd, b_array1d);
      t_arrayXd[1] = Type::optvartop(-1);
      rb(m, ASTString("array1d"), t_arrayXd, b_array1d);
    }
    {
      std::vector<Type> t_arrayXd(3);
      t_arrayXd[0] = Type::parsetint();
      t_arrayXd[1] = Type::parsetint();
      t_arrayXd[2] = Type::top(-1);
      rb(m, ASTString("array2d"), t_arrayXd, b_array2d);
      t_arrayXd[2] = Type::vartop(-1);
      rb(m, ASTString("array2d"), t_arrayXd, b_array2d);
      t_arrayXd[2] = Type::optvartop(-1);
      rb(m, ASTString("array2d"), t_arrayXd, b_array2d);
    }
    {
      std::vector<Type> t_arrayXd(4);
      t_arrayXd[0] = Type::parsetint();
      t_arrayXd[1] = Type::parsetint();
      t_arrayXd[2] = Type::parsetint();
      t_arrayXd[3] = Type::top(-1);
      rb(m, ASTString("array3d"), t_arrayXd, b_array3d);
      t_arrayXd[3] = Type::vartop(-1);
      rb(m, ASTString("array3d"), t_arrayXd, b_array3d);
      t_arrayXd[3] = Type::optvartop(-1);
      rb(m, ASTString("array3d"), t_arrayXd, b_array3d);
    }
    {
      std::vector<Type> t_arrayXd(5);
      t_arrayXd[0] = Type::parsetint();
      t_arrayXd[1] = Type::parsetint();
      t_arrayXd[2] = Type::parsetint();
      t_arrayXd[3] = Type::parsetint();
      t_arrayXd[4] = Type::top(-1);
      rb(m, ASTString("array4d"), t_arrayXd, b_array4d);
      t_arrayXd[4] = Type::vartop(-1);
      rb(m, ASTString("array4d"), t_arrayXd, b_array4d);
      t_arrayXd[4] = Type::optvartop(-1);
      rb(m, ASTString("array4d"), t_arrayXd, b_array4d);
    }
    {
      std::vector<Type> t_arrayXd(6);
      t_arrayXd[0] = Type::parsetint();
      t_arrayXd[1] = Type::parsetint();
      t_arrayXd[2] = Type::parsetint();
      t_arrayXd[3] = Type::parsetint();
      t_arrayXd[4] = Type::parsetint();
      t_arrayXd[5] = Type::top(-1);
      rb(m, ASTString("array5d"), t_arrayXd, b_array5d);
      t_arrayXd[5] = Type::vartop(-1);
      rb(m, ASTString("array5d"), t_arrayXd, b_array5d);
      t_arrayXd[5] = Type::optvartop(-1);
      rb(m, ASTString("array5d"), t_arrayXd, b_array5d);
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
      rb(m, ASTString("array6d"), t_arrayXd, b_array6d);
      t_arrayXd[6] = Type::vartop(-1);
      rb(m, ASTString("array6d"), t_arrayXd, b_array6d);
      t_arrayXd[6] = Type::optvartop(-1);
      rb(m, ASTString("array6d"), t_arrayXd, b_array6d);
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::parbool();
      t[1] = Type::parstring();
      rb(m, constants().ids.assert, t, b_assert_bool);
    }
    {
      std::vector<Type> t(3);
      t[0] = Type::parbool();
      t[1] = Type::parstring();
      t[2] = Type::top();
      rb(m, constants().ids.assert, t, b_assert);
      t[2] = Type::vartop();
      rb(m, constants().ids.assert, t, b_assert);
      t[2] = Type::optvartop();
      rb(m, constants().ids.assert, t, b_assert);
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::parstring();
      t[1] = Type::top();
      rb(m, constants().ids.trace, t, b_trace);
      t[1] = Type::vartop();
      rb(m, constants().ids.trace, t, b_trace);
      t[1] = Type::optvartop();
      rb(m, constants().ids.trace, t, b_trace);
    }
    {
      std::vector<Type> t_length(1);
      t_length[0] = Type::optvartop(-1);
      rb(m, ASTString("length"), t_length, b_length);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parbool();
      rb(m, constants().ids.bool2int, t, b_bool2int);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parbool(-1);
      rb(m, constants().ids.forall, t, b_forall_par);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parbool(-1);
      rb(m, constants().ids.exists, t, b_exists_par);
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::parbool(-1);
      t[1] = Type::parbool(-1);
      rb(m, constants().ids.clause, t, b_clause_par);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varsetint();
      rb(m, ASTString("ub"), t, b_ub_set);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varsetint(1);
      rb(m, ASTString("ub_array"), t, b_array_ub_set);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint();
      rb(m, ASTString("dom"), t, b_dom_varint);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint(-1);
      rb(m, ASTString("dom_array"), t, b_dom_array);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parsetint();
      rb(m, ASTString("min"), t, b_min_parsetint);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parsetint();
      rb(m, ASTString("max"), t, b_max_parsetint);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint();
      t[0]._ot = Type::OT_OPTIONAL;
      rb(m, ASTString("lb"), t, b_lb_varoptint);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint();
      t[0]._ot = Type::OT_OPTIONAL;
      rb(m, ASTString("ub"), t, b_ub_varoptint);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint();
      rb(m, ASTString("lb"), t, b_lb_varoptint);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint();
      rb(m, ASTString("ub"), t, b_ub_varoptint);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint(-1);
      t[0]._ot = Type::OT_OPTIONAL;
      rb(m, ASTString("lb_array"), t, b_array_lb_int);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint(-1);
      t[0]._ot = Type::OT_OPTIONAL;
      rb(m, ASTString("ub_array"), t, b_array_ub_int);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parsetint();
      rb(m, ASTString("card"), t, b_card);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint();
      rb(m, ASTString("has_bounds"), t, b_has_bounds);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::optvartop();
      rb(m, ASTString("is_fixed"), t, b_is_fixed);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::optvartop(-1);
      rb(m, ASTString("is_fixed"), t, b_is_fixed_array);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::optvartop();
      rb(m, ASTString("fix"), t, b_fix_bool);
      rb(m, ASTString("fix"), t, b_fix_int);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::optvartop(1);
      rb(m, ASTString("fix"), t, b_fix_array);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parint();
      rb(m, ASTString("int2float"), t, b_int2float);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parfloat();
      rb(m, ASTString("ceil"), t, b_ceil);
      rb(m, ASTString("floor"), t, b_floor);
      rb(m, ASTString("round"), t, b_round);
      rb(m, ASTString("log10"), t, b_log10);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parfloat(1);
      rb(m, constants().ids.sum, t, b_sum_float);      
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parsetint();
      rb(m, ASTString("set2array"), t, b_set2array);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::vartop();
      rb(m, ASTString("show"), t, b_show);
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::parint();
      t[1] = Type::varint();
      rb(m, ASTString("show_int"), t, b_show_int);
    }
    {
      std::vector<Type> t(3);
      t[0] = Type::parint();
      t[1] = Type::parint();
      t[2] = Type::varfloat();
      rb(m, ASTString("show_float"), t, b_show_float);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::vartop(-1);
      rb(m, ASTString("show"), t, b_show);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parstring(1);
      rb(m, ASTString("concat"), t, b_concat);
    }
    {
      std::vector<Type> t(2);
      t[0] = Type::parstring();
      t[1] = Type::parstring(1);
      rb(m, ASTString("join"), t, b_join);
    }
  }
  
}


