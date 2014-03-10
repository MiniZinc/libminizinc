/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/eval_par.hh>
#include <minizinc/exception.hh>
#include <minizinc/iter.hh>
#include <minizinc/hash.hh>
#include <minizinc/copy.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/flatten.hh>

namespace MiniZinc {

  template<class E>
  typename E::Val eval_id(Expression* e) {
    Id* id = e->cast<Id>();
    if (id->decl() == NULL)
      throw EvalError(e->loc(), "undeclared identifier", id->v());
    if (id->decl()->e() == NULL)
      throw EvalError(e->loc(), "cannot evaluate expression", id->v());
    typename E::Val r = E::e(id->decl()->e());
    id->decl()->e(r);
    return r;
  }

  class EvalIntLit {
  public:
    typedef IntLit* Val;
    typedef Expression* ArrayVal;
    static IntLit* e(Expression* e) {
      return new IntLit(Location(),eval_int(e));
    }
  };
  class EvalIntVal {
  public:
    typedef IntVal Val;
    typedef IntVal ArrayVal;
    static IntVal e(Expression* e) {
      return eval_int(e);
    }
  };
  class EvalFloatLit {
  public:
    typedef FloatLit* Val;
    typedef Expression* ArrayVal;
    static FloatLit* e(Expression* e) {
      return new FloatLit(Location(),eval_float(e));
    }
  };
  class EvalString {
  public:
    typedef std::string Val;
    typedef std::string ArrayVal;
    static std::string e(Expression* e) {
      return eval_string(e);
    }
  };
  class EvalStringLit {
  public:
    typedef StringLit* Val;
    typedef Expression* ArrayVal;
    static StringLit* e(Expression* e) {
      return new StringLit(Location(),eval_string(e));
    }
  };
  class EvalBoolLit {
  public:
    typedef BoolLit* Val;
    typedef Expression* ArrayVal;
    static BoolLit* e(Expression* e) {
      return constants().boollit(eval_bool(e));
    }
  };
  class EvalArrayLit {
  public:
    typedef ArrayLit* Val;
    typedef Expression* ArrayVal;
    static ArrayLit* e(Expression* e) {
      return eval_array_lit(e);
    }
  };
  class EvalSetLit {
  public:
    typedef SetLit* Val;
    typedef Expression* ArrayVal;
    static SetLit* e(Expression* e) {
      return new SetLit(e->loc(),eval_intset(e));
    }
  };
  class EvalNone {
  public:
    typedef Expression* Val;
    typedef Expression* ArrayVal;
    static Expression* e(Expression* e) {
      return e;
    }
  };
  class EvalCopy {
  public:
    typedef Expression* Val;
    typedef Expression* ArrayVal;
    static Expression* e(Expression* e) {
      return copy(e,true);
    }
  };

  ArrayLit* eval_array_comp(Comprehension* e) {
    ArrayLit* ret;
    if (e->type() == Type::parint(1)) {
      std::vector<Expression*> a = eval_comp<EvalIntLit>(e);
      ret = new ArrayLit(e->loc(),a);
    } else if (e->type() == Type::parbool(1)) {
      std::vector<Expression*> a = eval_comp<EvalBoolLit>(e);
      ret = new ArrayLit(e->loc(),a);
    } else if (e->type() == Type::parfloat(1)) {
      std::vector<Expression*> a = eval_comp<EvalFloatLit>(e);
      ret = new ArrayLit(e->loc(),a);
    } else if (e->type() == Type::parsetint(1)) {
      std::vector<Expression*> a = eval_comp<EvalSetLit>(e);
      ret = new ArrayLit(e->loc(),a);
    } else if (e->type() == Type::parstring(1)) {
      std::vector<Expression*> a = eval_comp<EvalStringLit>(e);
      ret = new ArrayLit(e->loc(),a);
    } else {
      std::vector<Expression*> a = eval_comp<EvalCopy>(e);
      ret = new ArrayLit(e->loc(),a);
    }
    ret->type(e->type());
    return ret;
  }
  
  ArrayLit* eval_array_lit(Expression* e) {
    switch (e->eid()) {
    case Expression::E_INTLIT:
    case Expression::E_FLOATLIT:
    case Expression::E_BOOLLIT:
    case Expression::E_STRINGLIT:
    case Expression::E_SETLIT:
    case Expression::E_ANON:
    case Expression::E_ANN:
    case Expression::E_TI:
    case Expression::E_TIID:
    case Expression::E_VARDECL:
      throw EvalError(e->loc(), "not an array expression");
    case Expression::E_ID:
      return eval_id<EvalArrayLit>(e);
    case Expression::E_ARRAYLIT:
      return e->cast<ArrayLit>();
    case Expression::E_ARRAYACCESS:
      throw EvalError(e->loc(),"arrays of arrays not supported");
    case Expression::E_COMP:
      return eval_array_comp(e->cast<Comprehension>());
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        for (unsigned int i=0; i<ite->size(); i++) {
          if (eval_bool(ite->e_if(i)))
            return eval_array_lit(ite->e_then(i));
        }
        return eval_array_lit(ite->e_else());
      }
    case Expression::E_BINOP:
      {
        BinOp* bo = e->cast<BinOp>();
        if (bo->op()==BOT_PLUSPLUS) {
          ArrayLit* al0 = eval_array_lit(bo->lhs());
          ArrayLit* al1 = eval_array_lit(bo->rhs());
          std::vector<Expression*> v(al0->v().size()+al1->v().size());
          for (unsigned int i=al0->v().size(); i--;)
            v[i] = al0->v()[i];
          for (unsigned int i=al1->v().size(); i--;)
            v[al0->v().size()+i] = al1->v()[i];
          ArrayLit* ret = new ArrayLit(e->loc(),v);
          ret->type(e->type());
          return ret;
        } else {
          throw EvalError(e->loc(), "not an array expression", bo->opToString());
        }
      }
      break;
    case Expression::E_UNOP:
      throw EvalError(e->loc(), "unary operator not supported");
    case Expression::E_CALL:
      {
        Call* ce = e->cast<Call>();
        if (ce->decl()==NULL)
          throw EvalError(e->loc(), "undeclared function", ce->id());
        
        if (ce->decl()->_builtins.e)
          return ce->decl()->_builtins.e(ce->args())
            ->cast<ArrayLit>();

        if (ce->decl()->e()==NULL)
          throw EvalError(ce->loc(), "internal error: missing builtin '"+ce->id().str()+"'");

        /// TODO: fix for recursion
        for (unsigned int i=ce->decl()->params().size(); i--;) {
          ce->decl()->params()[i]->e(ce->args()[i]);
        }
        ArrayLit* ret = eval_array_lit(ce->decl()->e());
        for (unsigned int i=ce->decl()->params().size(); i--;) {
          ce->decl()->params()[i]->e(NULL);
        }
        return ret;
      }
    case Expression::E_LET:
      {
        Let* l = e->cast<Let>();
        l->pushbindings();
        ArrayLit* ret = eval_array_lit(l->in());
        l->popbindings();
        return ret;
      }
    }
    assert(false);
  }

  Expression* eval_arrayaccess(ArrayLit* al, const std::vector<IntVal>& dims,
                               bool& success) {
    success = true;
    assert(al->dims() == dims.size());
    int realidx = 0;
    int realdim = 1;
    for (unsigned int i=0; i<al->dims(); i++)
      realdim *= al->max(i)-al->min(i)+1;
    for (unsigned int i=0; i<al->dims(); i++) {
      int ix = dims[i];
      if (ix < al->min(i) || ix > al->max(i)) {
        success = false;
        Type t = al->type();
        t._dim = 0;
        if (t.isint())
          return new IntLit(Location(),0);
        if (t.isbool())
          return constants().lit_false;
        if (t.isfloat())
          return new FloatLit(Location(),0.0);
        if (t.isintset())
          return new SetLit(Location(),IntSetVal::a());
        if (t.isstring())
          return new StringLit(Location(),"");
        assert(false);
        return NULL;
      }
      realdim /= al->max(i)-al->min(i)+1;
      realidx += (ix-al->min(i))*realdim;
    }
    assert(realidx >= 0 && realidx <= al->v().size());
    return al->v()[realidx];
  }
  Expression* eval_arrayaccess(ArrayAccess* e, bool& success) {
    ArrayLit* al = eval_array_lit(e->v());
    std::vector<IntVal> dims(e->idx().size());
    for (unsigned int i=e->idx().size(); i--;) {
      dims[i] = eval_int(e->idx()[i]);
    }
    return eval_arrayaccess(al,dims,success);
  }
  Expression* eval_arrayaccess(ArrayAccess* e) {
    bool success;
    Expression* ret = eval_arrayaccess(e,success);
    if (success)
      return ret;
    else
      throw EvalError(e->loc(), "array access out of bounds");
  }

  IntSetVal* eval_intset(Expression* e) {
    switch (e->eid()) {
    case Expression::E_SETLIT:
      {
        SetLit* sl = e->cast<SetLit>();
        if (sl->isv())
          return sl->isv();
        std::vector<IntVal> vals(sl->v().size());
        for (unsigned int i=0; i<sl->v().size(); i++)
          vals[i] = eval_int(sl->v()[i]);
        return IntSetVal::a(vals);
      }
    case Expression::E_BOOLLIT:
    case Expression::E_INTLIT: 
    case Expression::E_FLOATLIT:
    case Expression::E_STRINGLIT:
    case Expression::E_ANON:
    case Expression::E_TIID:
    case Expression::E_VARDECL:
    case Expression::E_ANN:
    case Expression::E_TI:
    case Expression::E_UNOP:
      throw EvalError(e->loc(),"not a set of int expression");
      break;
    case Expression::E_ARRAYLIT:
      {
        ArrayLit* al = e->cast<ArrayLit>();
        std::vector<IntVal> vals(al->v().size());
        for (unsigned int i=0; i<al->v().size(); i++)
          vals[i] = eval_int(al->v()[i]);
        return IntSetVal::a(vals);
      }
      break;
    case Expression::E_COMP:
      {
        Comprehension* c = e->cast<Comprehension>();
        std::vector<IntVal> a = eval_comp<EvalIntVal>(c);
        return IntSetVal::a(a);
      }
    case Expression::E_ID:
      return eval_id<EvalSetLit>(e)->isv();
      break;
    case Expression::E_ARRAYACCESS:
      return eval_intset(eval_arrayaccess(e->cast<ArrayAccess>()));
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        for (unsigned int i=0; i<ite->size(); i++) {
          if (eval_bool(ite->e_if(i)))
            return eval_intset(ite->e_then(i));
        }
        return eval_intset(ite->e_else());
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* bo = e->cast<BinOp>();
        if (bo->lhs()->type().isintset() && bo->rhs()->type().isintset()) {
          IntSetVal* v0 = eval_intset(bo->lhs());
          IntSetVal* v1 = eval_intset(bo->rhs());
          IntSetRanges ir0(v0);
          IntSetRanges ir1(v1);
          switch (bo->op()) {
          case BOT_UNION:
            {
              Ranges::Union<IntSetRanges,IntSetRanges> u(ir0,ir1);
              return IntSetVal::ai(u);
            }
          case BOT_DIFF:
            {
              Ranges::Diff<IntSetRanges,IntSetRanges> u(ir0,ir1);
              return IntSetVal::ai(u);
            }
          case BOT_SYMDIFF:
            {
              Ranges::Union<IntSetRanges,IntSetRanges> u(ir0,ir1);
              Ranges::Inter<IntSetRanges,IntSetRanges> i(ir0,ir1);
              Ranges::Diff<Ranges::Union<IntSetRanges,IntSetRanges>,
                           Ranges::Inter<IntSetRanges,IntSetRanges>> sd(u,i);
              return IntSetVal::ai(sd);
            }
          case BOT_INTERSECT:
            {
              Ranges::Inter<IntSetRanges,IntSetRanges> u(ir0,ir1);
              return IntSetVal::ai(u);
            }
          default: throw EvalError(e->loc(),"not a set of int expression", bo->opToString());
          }
        } else if (bo->lhs()->type().isint() && bo->rhs()->type().isint()) {
          if (bo->op() != BOT_DOTDOT)
            throw EvalError(e->loc(), "not a set of int expression", bo->opToString());
          return IntSetVal::a(eval_int(bo->lhs()),
                              eval_int(bo->rhs()));
        } else {
          throw EvalError(e->loc(), "not a set of int expression", bo->opToString());
        }
      }
      break;
    case Expression::E_CALL:
      {
        Call* ce = e->cast<Call>();
        if (ce->decl()==NULL)
          throw EvalError(e->loc(), "undeclared function", ce->id());
        
        if (ce->decl()->_builtins.s)
          return ce->decl()->_builtins.s(ce->args());

        if (ce->decl()->_builtins.e)
          return eval_intset(ce->decl()->_builtins.e(ce->args()));

        if (ce->decl()->e()==NULL)
          throw EvalError(ce->loc(), "internal error: missing builtin '"+ce->id().str()+"'");
        
        for (unsigned int i=ce->decl()->params().size(); i--;) {
          ce->decl()->params()[i]->e(ce->args()[i]);
        }
        IntSetVal* ret = eval_intset(ce->decl()->e());
        for (unsigned int i=ce->decl()->params().size(); i--;) {
          ce->decl()->params()[i]->e(NULL);
        }
        return ret;
      }
      break;
    case Expression::E_LET:
      {
        Let* l = e->cast<Let>();
        l->pushbindings();
        IntSetVal* ret = eval_intset(l->in());
        l->popbindings();
        return ret;
      }
      break;
    }
  }

  bool eval_bool(Expression* e) {
    switch (e->eid()) {
    case Expression::E_BOOLLIT: return e->cast<BoolLit>()->v();
    case Expression::E_INTLIT: 
    case Expression::E_FLOATLIT:
    case Expression::E_STRINGLIT:
    case Expression::E_ANON:
    case Expression::E_TIID:
    case Expression::E_SETLIT:
    case Expression::E_ARRAYLIT:
    case Expression::E_COMP:
    case Expression::E_VARDECL:
    case Expression::E_ANN:
    case Expression::E_TI:
      assert(false);
      throw EvalError(e->loc(),"not a bool expression");
      break;
    case Expression::E_ID:
      return eval_id<EvalBoolLit>(e)->v();
      break;
    case Expression::E_ARRAYACCESS:
      return eval_bool(eval_arrayaccess(e->cast<ArrayAccess>()));
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        for (unsigned int i=0; i<ite->size(); i++) {
          if (eval_bool(ite->e_if(i)))
            return eval_bool(ite->e_then(i));
        }
        return eval_bool(ite->e_else());
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* bo = e->cast<BinOp>();
        if (bo->lhs()->type().isbool() && bo->rhs()->type().isbool()) {
          switch (bo->op()) {
          case BOT_LE: return eval_bool(bo->lhs())<eval_bool(bo->rhs());
          case BOT_LQ: return eval_bool(bo->lhs())<=eval_bool(bo->rhs());
          case BOT_GR: return eval_bool(bo->lhs())>eval_bool(bo->rhs());
          case BOT_GQ: return eval_bool(bo->lhs())>=eval_bool(bo->rhs());
          case BOT_EQ: return eval_bool(bo->lhs())==eval_bool(bo->rhs());
          case BOT_NQ: return eval_bool(bo->lhs())!=eval_bool(bo->rhs());
          case BOT_EQUIV: return eval_bool(bo->lhs())==eval_bool(bo->rhs());
          case BOT_IMPL: return (!eval_bool(bo->lhs()))||eval_bool(bo->rhs());
          case BOT_RIMPL: return (!eval_bool(bo->rhs()))||eval_bool(bo->lhs());
          case BOT_OR: return eval_bool(bo->lhs())||eval_bool(bo->rhs());
          case BOT_AND: return eval_bool(bo->lhs())&&eval_bool(bo->rhs());
          case BOT_XOR: return eval_bool(bo->lhs())^eval_bool(bo->rhs());
          default:
            assert(false);
            throw EvalError(e->loc(),"not a bool expression", bo->opToString());
          }
        } else if (bo->lhs()->type().isint() && bo->rhs()->type().isint()) {
          IntVal v0 = eval_int(bo->lhs());
          IntVal v1 = eval_int(bo->rhs());
          switch (bo->op()) {
          case BOT_LE: return v0<v1;
          case BOT_LQ: return v0<=v1;
          case BOT_GR: return v0>v1;
          case BOT_GQ: return v0>=v1;
          case BOT_EQ: return v0==v1;
          case BOT_NQ: return v0!=v1;
          default:
            assert(false);
            throw EvalError(e->loc(),"not a bool expression", bo->opToString());
          }
        } else if (bo->lhs()->type().isfloat() && bo->rhs()->type().isfloat()) {
          FloatVal v0 = eval_float(bo->lhs());
          FloatVal v1 = eval_float(bo->rhs());
          switch (bo->op()) {
          case BOT_LE: return v0<v1;
          case BOT_LQ: return v0<=v1;
          case BOT_GR: return v0>v1;
          case BOT_GQ: return v0>=v1;
          case BOT_EQ: return v0==v1;
          case BOT_NQ: return v0!=v1;
          default:
            assert(false);
            throw EvalError(e->loc(),"not a bool expression", bo->opToString());
          }
        } else if (bo->lhs()->type().isint() && bo->rhs()->type().isintset()) {
          IntVal v0 = eval_int(bo->lhs());
          GCLock lock;
          IntSetVal* v1 = eval_intset(bo->rhs());
          switch (bo->op()) {
          case BOT_IN: return v1->contains(v0);
          default:
            assert(false);
            throw EvalError(e->loc(),"not a bool expression", bo->opToString());
          }
        } else if (bo->lhs()->type().isset() && bo->rhs()->type().isset()) {
          GCLock lock;
          IntSetVal* v0 = eval_intset(bo->lhs());
          IntSetVal* v1 = eval_intset(bo->rhs());
          IntSetRanges ir0(v0);
          IntSetRanges ir1(v1);
          switch (bo->op()) {
          case BOT_LE: assert(false); /// TODO
          case BOT_LQ: assert(false); /// TODO
          case BOT_GR: assert(false); /// TODO
          case BOT_GQ: assert(false); /// TODO
          case BOT_EQ: return Ranges::equal(ir0,ir1);
          case BOT_NQ: return !Ranges::equal(ir0,ir1);
          case BOT_SUBSET: return Ranges::subset(ir0,ir1);
          case BOT_SUPERSET: return Ranges::subset(ir1,ir0);
          default:
            throw EvalError(e->loc(),"not a bool expression", bo->opToString());
          }
        } else if (bo->lhs()->type().isstring() && bo->rhs()->type().isstring()) {
          GCLock lock;
          std::string s0 = eval_string(bo->lhs());
          std::string s1 = eval_string(bo->rhs());
          switch (bo->op()) {
            case BOT_EQ: return s0==s1;
            case BOT_NQ: return s0!=s1;
            case BOT_LE: return s0<s1;
            case BOT_LQ: return s0<=s1;
            case BOT_GR: return s0>s1;
            case BOT_GQ: return s0>=s1;
            default:
              throw EvalError(e->loc(),"not a bool expression", bo->opToString());
          }
        } else {
          throw EvalError(e->loc(), "not a bool expression", bo->opToString());
        }
      }
      break;
    case Expression::E_UNOP:
      {
        UnOp* uo = e->cast<UnOp>();
        bool v0 = eval_bool(uo->e());
        switch (uo->op()) {
        case UOT_NOT: return !v0;
        default:
          assert(false);
          throw EvalError(e->loc(),"not a bool expression", uo->opToString());
        }
      }
      break;
    case Expression::E_CALL:
      {
        Call* ce = e->cast<Call>();
        if (ce->decl()==NULL)
          throw EvalError(e->loc(), "undeclared function", ce->id());
        
        if (ce->decl()->_builtins.b)
          return ce->decl()->_builtins.b(ce->args());

        if (ce->decl()->_builtins.e)
          return eval_bool(ce->decl()->_builtins.e(ce->args()));

        if (ce->decl()->e()==NULL)
          throw EvalError(ce->loc(), "internal error: missing builtin '"+ce->id().str()+"'");
        
        for (unsigned int i=ce->decl()->params().size(); i--;) {
          ce->decl()->params()[i]->e(ce->args()[i]);
        }
        bool ret = eval_bool(ce->decl()->e());
        for (unsigned int i=ce->decl()->params().size(); i--;) {
          ce->decl()->params()[i]->e(NULL);
        }
        return ret;
      }
      break;
    case Expression::E_LET:
      {
        Let* l = e->cast<Let>();
        l->pushbindings();
        bool ret = eval_bool(l->in());
        l->popbindings();
        return ret;
      }
      break;
    }
  }

  IntVal eval_int(Expression* e) {
    switch (e->eid()) {
    case Expression::E_INTLIT: return e->cast<IntLit>()->v();
    case Expression::E_FLOATLIT:
    case Expression::E_BOOLLIT:
    case Expression::E_STRINGLIT:
    case Expression::E_ANON:
    case Expression::E_TIID:
    case Expression::E_SETLIT:
    case Expression::E_ARRAYLIT:
    case Expression::E_COMP:
    case Expression::E_VARDECL:
    case Expression::E_ANN:
    case Expression::E_TI:
      throw EvalError(e->loc(),"not an integer expression");
      break;
    case Expression::E_ID:
      {
        GCLock lock;
        return eval_id<EvalIntLit>(e)->v();
      }
      break;
    case Expression::E_ARRAYACCESS:
      return eval_int(eval_arrayaccess(e->cast<ArrayAccess>()));
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        for (unsigned int i=0; i<ite->size(); i++) {
          if (eval_bool(ite->e_if(i)))
            return eval_int(ite->e_then(i));
        }
        return eval_int(ite->e_else());
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* bo = e->cast<BinOp>();
        IntVal v0 = eval_int(bo->lhs());
        IntVal v1 = eval_int(bo->rhs());
        switch (bo->op()) {
        case BOT_PLUS: return v0+v1;
        case BOT_MINUS: return v0-v1;
        case BOT_MULT: return v0*v1;
        case BOT_IDIV: return v0 / v1;
        case BOT_MOD: return v0 % v1;
        default: throw EvalError(e->loc(),"not an integer expression", bo->opToString());
        }
      }
      break;
    case Expression::E_UNOP:
      {
        UnOp* uo = e->cast<UnOp>();
        IntVal v0 = eval_int(uo->e());
        switch (uo->op()) {
        case UOT_PLUS: return v0;
        case UOT_MINUS: return -v0;
        default: throw EvalError(e->loc(),"not an integer expression", uo->opToString());
        }
      }
      break;
    case Expression::E_CALL:
      {
        Call* ce = e->cast<Call>();
        if (ce->decl()==NULL)
          throw EvalError(e->loc(), "undeclared function", ce->id());
        if (ce->decl()->_builtins.i)
          return ce->decl()->_builtins.i(ce->args());

        if (ce->decl()->_builtins.e)
          return eval_int(ce->decl()->_builtins.e(ce->args()));

        if (ce->decl()->e()==NULL)
          throw EvalError(ce->loc(), "internal error: missing builtin '"+ce->id().str()+"'");
        
        for (unsigned int i=ce->decl()->params().size(); i--;) {
          ce->decl()->params()[i]->e(ce->args()[i]);
        }
        IntVal ret = eval_int(ce->decl()->e());
        for (unsigned int i=ce->decl()->params().size(); i--;) {
          ce->decl()->params()[i]->e(NULL);
        }
        return ret;
      }
      break;
    case Expression::E_LET:
      {
        Let* l = e->cast<Let>();
        l->pushbindings();
        IntVal ret = eval_int(l->in());
        l->popbindings();
        return ret;
      }
      break;
    }
  }

  FloatVal eval_float(Expression* e) {
    switch (e->eid()) {
      case Expression::E_FLOATLIT: return e->cast<FloatLit>()->v();
      case Expression::E_INTLIT:
      case Expression::E_BOOLLIT:
      case Expression::E_STRINGLIT:
      case Expression::E_ANON:
      case Expression::E_TIID:
      case Expression::E_SETLIT:
      case Expression::E_ARRAYLIT:
      case Expression::E_COMP:
      case Expression::E_VARDECL:
      case Expression::E_ANN:
      case Expression::E_TI:
        throw EvalError(e->loc(),"not a float expression");
        break;
      case Expression::E_ID:
      {
        GCLock lock;
        return eval_id<EvalFloatLit>(e)->v();
      }
        break;
      case Expression::E_ARRAYACCESS:
        return eval_float(eval_arrayaccess(e->cast<ArrayAccess>()));
        break;
      case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        for (unsigned int i=0; i<ite->size(); i++) {
          if (eval_bool(ite->e_if(i)))
            return eval_float(ite->e_then(i));
        }
        return eval_float(ite->e_else());
      }
        break;
      case Expression::E_BINOP:
      {
        BinOp* bo = e->cast<BinOp>();
        FloatVal v0 = eval_float(bo->lhs());
        FloatVal v1 = eval_float(bo->rhs());
        switch (bo->op()) {
          case BOT_PLUS: return v0+v1;
          case BOT_MINUS: return v0-v1;
          case BOT_MULT: return v0*v1;
          case BOT_DIV: return v0 / v1;
          default: throw EvalError(e->loc(),"not a float expression", bo->opToString());
        }
      }
        break;
      case Expression::E_UNOP:
      {
        UnOp* uo = e->cast<UnOp>();
        FloatVal v0 = eval_float(uo->e());
        switch (uo->op()) {
          case UOT_PLUS: return v0;
          case UOT_MINUS: return -v0;
          default: throw EvalError(e->loc(),"not a float expression", uo->opToString());
        }
      }
        break;
      case Expression::E_CALL:
      {
        Call* ce = e->cast<Call>();
        if (ce->decl()==NULL)
          throw EvalError(e->loc(), "undeclared function", ce->id());
        if (ce->decl()->_builtins.f)
          return ce->decl()->_builtins.f(ce->args());
        
        if (ce->decl()->_builtins.e)
          return eval_float(ce->decl()->_builtins.e(ce->args()));

        if (ce->decl()->e()==NULL)
          throw EvalError(ce->loc(), "internal error: missing builtin '"+ce->id().str()+"'");

        for (unsigned int i=ce->decl()->params().size(); i--;) {
          ce->decl()->params()[i]->e(ce->args()[i]);
        }
        FloatVal ret = eval_float(ce->decl()->e());
        for (unsigned int i=ce->decl()->params().size(); i--;) {
          ce->decl()->params()[i]->e(NULL);
        }
        return ret;
      }
        break;
      case Expression::E_LET:
      {
        Let* l = e->cast<Let>();
        l->pushbindings();
        FloatVal ret = eval_float(l->in());
        l->popbindings();
        return ret;
      }
        break;
    }
  }

  std::string eval_string(Expression* e) {
    switch (e->eid()) {
      case Expression::E_STRINGLIT:
        return e->cast<StringLit>()->v().str();
      case Expression::E_FLOATLIT:
      case Expression::E_INTLIT:
      case Expression::E_BOOLLIT:
      case Expression::E_ANON:
      case Expression::E_TIID:
      case Expression::E_SETLIT:
      case Expression::E_ARRAYLIT:
      case Expression::E_COMP:
      case Expression::E_VARDECL:
      case Expression::E_ANN:
      case Expression::E_TI:
        throw EvalError(e->loc(),"not a string expression");
        break;
      case Expression::E_ID:
      {
        GCLock lock;
        return eval_id<EvalStringLit>(e)->v().str();
      }
        break;
      case Expression::E_ARRAYACCESS:
        return eval_string(eval_arrayaccess(e->cast<ArrayAccess>()));
        break;
      case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        for (unsigned int i=0; i<ite->size(); i++) {
          if (eval_bool(ite->e_if(i)))
            return eval_string(ite->e_then(i));
        }
        return eval_string(ite->e_else());
      }
        break;
      case Expression::E_BINOP:
      {
        BinOp* bo = e->cast<BinOp>();
        std::string v0 = eval_string(bo->lhs());
        std::string v1 = eval_string(bo->rhs());
        switch (bo->op()) {
          case BOT_PLUSPLUS: return v0+v1;
          default: throw EvalError(e->loc(),"not a string expression", bo->opToString());
        }
      }
        break;
      case Expression::E_UNOP:
        throw EvalError(e->loc(),"not a string expression");
        break;
      case Expression::E_CALL:
      {
        Call* ce = e->cast<Call>();
        if (ce->decl()==NULL)
          throw EvalError(e->loc(), "undeclared function", ce->id());

        if (ce->decl()->_builtins.str)
          return ce->decl()->_builtins.str(ce->args());
        if (ce->decl()->_builtins.e)
          return eval_string(ce->decl()->_builtins.e(ce->args()));
        
        if (ce->decl()->e()==NULL)
          throw EvalError(ce->loc(), "internal error: missing builtin '"+ce->id().str()+"'");
        
        for (unsigned int i=ce->decl()->params().size(); i--;) {
          ce->decl()->params()[i]->e(ce->args()[i]);
        }
        std::string ret = eval_string(ce->decl()->e());
        for (unsigned int i=ce->decl()->params().size(); i--;) {
          ce->decl()->params()[i]->e(NULL);
        }
        return ret;
      }
        break;
      case Expression::E_LET:
      {
        Let* l = e->cast<Let>();
        l->pushbindings();
        std::string ret = eval_string(l->in());
        l->popbindings();
        return ret;
      }
        break;
    }
  }

  class AssignVisitor : public ItemVisitor {
  public:
    void vAssignI(AssignI* i) {
      if (i->decl() == NULL)
        throw EvalError(i->loc(), "undeclared identifier", i->id());
      if (i->decl()->e() != NULL)
        throw EvalError(i->loc(), "multiple assignments to same identifier", i->id());
      i->decl()->e(i->e());
    }
  };

  Expression* eval_par(Expression* e) {
    if (e==NULL) return NULL;
    switch (e->eid()) {
    case Expression::E_ANON:
    case Expression::E_TIID:
      {
        TIId* tiid = e->cast<TIId>();
        throw EvalError(e->loc(),"not a par expression", tiid->v());
      }
    case Expression::E_COMP:
      if (e->cast<Comprehension>()->set())
        return EvalSetLit::e(e);
      // fall through
    case Expression::E_ARRAYLIT:
      {
        ArrayLit* al = eval_array_lit(e);
        for (unsigned int i=al->v().size(); i--;)
          al->v()[i] = eval_par(al->v()[i]);
        return al;
      }
    case Expression::E_VARDECL:
      {
        VarDecl* vd = e->cast<VarDecl>();
        if (vd->e()==NULL)
          throw EvalError(vd->loc(),"not a par expression", vd->id()->v());
        return eval_par(vd->e());
      }
    case Expression::E_ANN:
      {
        Annotation* a = e->cast<Annotation>();
        Annotation* r = new Annotation(Location(),eval_par(a->e()),
                                       static_cast<Annotation*>(eval_par(a->next())));
        return r;
      }
    case Expression::E_TI:
      {
        TypeInst* t = e->cast<TypeInst>();
        ASTExprVec<TypeInst> r;
        if (t->ranges().size() > 0) {
          std::vector<TypeInst*> rv(t->ranges().size());
          for (unsigned int i=t->ranges().size(); i--;)
            rv[i] = static_cast<TypeInst*>(eval_par(t->ranges()[i]));
          r = ASTExprVec<TypeInst>(rv);
        }
        return 
          new TypeInst(Location(),t->type(),r,eval_par(t->domain()));
      }
    case Expression::E_ID:
      {
        Id* id = e->cast<Id>();
        if (id->decl()==NULL)
          throw EvalError(e->loc(),"undefined identifier", id->v());
        if (id->decl()->e()==NULL)
          throw EvalError(e->loc(),"not a par expression", id->v());
        return eval_par(id->decl()->e());
      }
    case Expression::E_ITE:
    case Expression::E_CALL:
    case Expression::E_LET:
    case Expression::E_BINOP:
    case Expression::E_SETLIT:
      {
        if (e->type()._dim != 0) {
          ArrayLit* al = eval_array_lit(e);
          for (unsigned int i=al->v().size(); i--;)
            al->v()[i] = eval_par(al->v()[i]);
          return al;
        }
        if (e->type()._st == Type::ST_SET) {
          if (e->type().isintset()) {
            return EvalSetLit::e(e);
          } else {
            /// TODO
            throw InternalError("not yet implemented");
          }
        }
      }
      // fall through!
    case Expression::E_BOOLLIT:
    case Expression::E_INTLIT: 
    case Expression::E_FLOATLIT:
    case Expression::E_UNOP:
    case Expression::E_ARRAYACCESS:
      {
        switch (e->type()._bt) {
        case Type::BT_BOOL: return EvalBoolLit::e(e);
        case Type::BT_INT:
          if (e->type()._st == Type::ST_PLAIN)
            return EvalIntLit::e(e);
          else
            return EvalSetLit::e(e);
        case Type::BT_FLOAT:
            if (e->type()._st == Type::ST_PLAIN)
              return EvalFloatLit::e(e);
            else
              throw InternalError("set of float not yet implemented");
        case Type::BT_STRING:
            if (e->type()._st == Type::ST_PLAIN)
              return EvalStringLit::e(e);
            else
              throw InternalError("set of string not yet implemented");
        case Type::BT_ANN:
        case Type::BT_BOT:
        case Type::BT_TOP:
        case Type::BT_UNKNOWN:
          throw EvalError(e->loc(),"not a par expression");
        }
      }
    case Expression::E_STRINGLIT:
      return e;
    }
  }

  class ComputeIntBounds : public EVisitor {
  protected:
    typedef std::pair<IntVal,IntVal> Bounds;
  public:
    std::vector<Bounds> _bounds;
    bool valid;
    ComputeIntBounds(void) : valid(true) {}
    bool enter(Expression* e) {
      if (e->type()._dim > 0)
        return false;
      if (e->type().ispar()) {
        if (e->type().isint()) {
          IntVal v = eval_int(e);
          _bounds.push_back(Bounds(v,v));
        } else {
          valid = false;
        }
        return false;
      } else {
        return e->type().isint();
      }
    }
    /// Visit integer literal
    void vIntLit(const IntLit& i) {
      _bounds.push_back(Bounds(i.v(),i.v()));
    }
    /// Visit floating point literal
    void vFloatLit(const FloatLit&) {
      throw EvalError(Location(), "not yet supported");
    }
    /// Visit Boolean literal
    void vBoolLit(const BoolLit&) {
      throw EvalError(Location(), "not yet supported");
    }
    /// Visit set literal
    void vSetLit(const SetLit&) {
      throw EvalError(Location(), "not yet supported");
    }
    /// Visit string literal
    void vStringLit(const StringLit&) {
      throw EvalError(Location(), "not yet supported");
    }
    /// Visit identifier
    void vId(const Id& id) {
      if (id.decl()->ti()->domain()) {
        GCLock lock;
        IntSetVal* isv = eval_intset(id.decl()->ti()->domain());
        if (isv->size()==0) {
          valid = false;
          _bounds.push_back(Bounds(0,0));
        } else {
          _bounds.push_back(Bounds(isv->min(0),isv->max(isv->size()-1)));
        }
      } else {
        if (id.decl()->e()) {
          BottomUpIterator<ComputeIntBounds> cbi(*this);
          cbi.run(id.decl()->e());
        } else {
          valid = false;
          _bounds.push_back(Bounds(0,0));
        }
      }
    }
    /// Visit anonymous variable
    void vAnonVar(const AnonVar& v) {
      valid = false;
      _bounds.push_back(Bounds(0,0));
    }
    /// Visit array literal
    void vArrayLit(const ArrayLit& al) {
    }
    /// Visit array access
    void vArrayAccess(const ArrayAccess& aa) {
      if (Id* id = aa.v()->dyn_cast<Id>()) {
        vId(*id);
      } else {
        throw EvalError(aa.loc(), "not yet supported");
      }
    }
    /// Visit array comprehension
    void vComprehension(const Comprehension& c) {
      throw EvalError(c.loc(), "not yet supported");
    }
    /// Visit if-then-else
    void vITE(const ITE& ite) {
      throw EvalError(ite.loc(), "not yet supported");
    }
    /// Visit binary operator
    void vBinOp(const BinOp& bo) {
      Bounds b1 = _bounds.back(); _bounds.pop_back();
      Bounds b0 = _bounds.back(); _bounds.pop_back();
      switch (bo.op()) {
      case BOT_PLUS:
        _bounds.push_back(Bounds(b0.first+b1.first,b0.second+b1.second));
        break;
      case BOT_MINUS:
        _bounds.push_back(Bounds(b0.first-b1.second,b0.second-b1.first));
        break;
      case BOT_MULT:
        {
          IntVal x0 = b0.first*b1.first;
          IntVal x1 = b0.first*b1.second;
          IntVal x2 = b0.second*b1.first;
          IntVal x3 = b0.second*b1.second;
          IntVal m = std::min(x0,std::min(x1,std::min(x2,x3)));
          IntVal n = std::max(x0,std::max(x1,std::max(x2,x3)));
          _bounds.push_back(Bounds(m,n));
        }
        break;
      case BOT_DIV:
      case BOT_IDIV:
      case BOT_MOD:
      case BOT_LE:
      case BOT_LQ:
      case BOT_GR:
      case BOT_GQ:
      case BOT_EQ:
      case BOT_NQ:
      case BOT_IN:
      case BOT_SUBSET:
      case BOT_SUPERSET:
      case BOT_UNION:
      case BOT_DIFF:
      case BOT_SYMDIFF:
      case BOT_INTERSECT:
      case BOT_PLUSPLUS:
      case BOT_EQUIV:
      case BOT_IMPL:
      case BOT_RIMPL:
      case BOT_OR:
      case BOT_AND:
      case BOT_XOR:
      case BOT_DOTDOT:
        throw EvalError(bo.loc(), "not yet supported");
      }
    }
    /// Visit unary operator
    void vUnOp(const UnOp& uo) {
      switch (uo.op()) {
      case UOT_PLUS:
        break;
      case UOT_MINUS:
        _bounds.back().first = -_bounds.back().first;
        _bounds.back().second = -_bounds.back().second;
        break;
      case UOT_NOT:
        throw EvalError(uo.loc(), "not yet supported");
      }
    }
    /// Visit call
    void vCall(Call& c) {
      if (c.id() == constants().ids.lin_exp &&
          c.args()[0]->isa<ArrayLit>() &&
          c.args()[1]->isa<ArrayLit>() &&
          c.args()[2]->isa<IntLit>()) {
        ArrayLit* coeff = c.args()[0]->cast<ArrayLit>();
        ArrayLit* al = c.args()[1]->cast<ArrayLit>();
        IntVal d = c.args()[2]->cast<IntLit>()->v();
        int stacktop = _bounds.size();
        for (unsigned int i=al->v().size(); i--;) {
          BottomUpIterator<ComputeIntBounds> cbi(*this);
          cbi.run(al->v()[i]);
          if (!valid)
            return;
        }
        assert(stacktop+al->v().size()==_bounds.size());
        IntVal lb = d;
        IntVal ub = d;
        for (unsigned int i=0; i<al->v().size(); i++) {
          Bounds b = _bounds.back(); _bounds.pop_back();
          IntVal cv = eval_int(coeff->v()[i]);
          if (cv > 0) {
            lb += cv*b.first;
            ub += cv*b.second;
          } else {
            lb += cv*b.second;
            ub += cv*b.first;
          }
        }
        _bounds.push_back(Bounds(lb,ub));
      } else if (c.id() == "card") {
        if (IntSetVal* isv = compute_intset_bounds(c.args()[0])) {
          IntSetRanges isr(isv);
          _bounds.push_back(Bounds(0,Ranges::size(isr)));
        } else {
          valid = false;
          _bounds.push_back(Bounds(0,0));
        }
      } else if (c.id() == constants().ids.bool2int) {
          _bounds.push_back(Bounds(0,1));
      } else {
        valid = false;
        _bounds.push_back(Bounds(0,0));
      }
    }
    /// Visit let
    void vLet(const Let& l) {
      throw EvalError(l.loc(), "not yet supported");
    }
    /// Visit variable declaration
    void vVarDecl(const VarDecl& vd) {
      throw EvalError(vd.loc(), "not yet supported");
    }
    /// Visit annotation
    void vAnnotation(const Annotation& e) {
      throw EvalError(e.loc(), "not yet supported");
    }
    /// Visit type inst
    void vTypeInst(const TypeInst& e) {
      throw EvalError(e.loc(), "not yet supported");
    }
    /// Visit TIId
    void vTIId(const TIId& e) {
      throw EvalError(e.loc(), "not yet supported");
    }
  };

  IntBounds compute_int_bounds(Expression* e) {
    ComputeIntBounds cb;
    BottomUpIterator<ComputeIntBounds> cbi(cb);
    cbi.run(e);
    if (cb.valid) {
      assert(cb._bounds.size() > 0);
      return IntBounds(cb._bounds.back().first,cb._bounds.back().second,true);
    } else {
      return IntBounds(0,0,false);
    }
  }

  class ComputeIntSetBounds : public EVisitor {
  public:
    std::vector<IntSetVal*> _bounds;
    bool valid;
    ComputeIntSetBounds(void) : valid(true) {}
    bool enter(Expression* e) {
      if (e->type()._dim > 0)
        return false;
      if (!e->type().isintset())
        return false;
      if (e->type().ispar()) {
        _bounds.push_back(eval_intset(e));
        return false;
      } else {
        return true;
      }
    }
    /// Visit set literal
    void vSetLit(const SetLit&) {
      throw EvalError(Location(), "not yet supported");
    }
    /// Visit identifier
    void vId(const Id& id) {
      if (id.decl()->ti()->domain()) {
        _bounds.push_back(eval_intset(id.decl()->ti()->domain()));
      } else {
        if (id.decl()->e()) {
          BottomUpIterator<ComputeIntSetBounds> cbi(*this);
          cbi.run(id.decl()->e());
        } else {
          valid = false;
          _bounds.push_back(NULL);
        }
      }
    }
    /// Visit anonymous variable
    void vAnonVar(const AnonVar& v) {
      valid = false;
      _bounds.push_back(NULL);
    }
    /// Visit array access
    void vArrayAccess(const ArrayAccess& aa) {
      throw EvalError(aa.loc(), "not yet supported");
    }
    /// Visit array comprehension
    void vComprehension(const Comprehension& c) {
      throw EvalError(c.loc(), "not yet supported");
    }
    /// Visit if-then-else
    void vITE(const ITE& ite) {
      throw EvalError(ite.loc(), "not yet supported");
    }
    /// Visit binary operator
    void vBinOp(const BinOp& bo) {
      IntSetVal* b1 = _bounds.back(); _bounds.pop_back();
      IntSetVal* b0 = _bounds.back(); _bounds.pop_back();
      switch (bo.op()) {
      case BOT_UNION:
        {
          IntSetRanges b0r(b0);
          IntSetRanges b1r(b1);
          Ranges::Union<IntSetRanges,IntSetRanges> u(b0r,b1r);
          _bounds.push_back(IntSetVal::ai(u));
        }
        break;
      case BOT_DIFF:
        {
          IntSetRanges b0r(b0);
          IntSetRanges b1r(b1);
          Ranges::Diff<IntSetRanges,IntSetRanges> u(b0r,b1r);
          _bounds.push_back(IntSetVal::ai(u));
        }
        break;
      case BOT_SYMDIFF:
        throw EvalError(bo.loc(), "not yet supported");
        break;
      case BOT_INTERSECT:
        {
          IntSetRanges b0r(b0);
          IntSetRanges b1r(b1);
          Ranges::Inter<IntSetRanges,IntSetRanges> u(b0r,b1r);
          _bounds.push_back(IntSetVal::ai(u));
        }
        break;
      case BOT_PLUS:
      case BOT_MINUS:
      case BOT_MULT:
      case BOT_DIV:
      case BOT_IDIV:
      case BOT_MOD:
      case BOT_LE:
      case BOT_LQ:
      case BOT_GR:
      case BOT_GQ:
      case BOT_EQ:
      case BOT_NQ:
      case BOT_IN:
      case BOT_SUBSET:
      case BOT_SUPERSET:
      case BOT_PLUSPLUS:
      case BOT_EQUIV:
      case BOT_IMPL:
      case BOT_RIMPL:
      case BOT_OR:
      case BOT_AND:
      case BOT_XOR:
      case BOT_DOTDOT:
        throw EvalError(bo.loc(), "not yet supported");
      }
    }
    /// Visit unary operator
    void vUnOp(const UnOp& uo) {
      throw EvalError(uo.loc(), "not yet supported");
    }
    /// Visit call
    void vCall(Call& c) {
      if (c.id() == "set_intersect") {
        IntSetVal* b0 = _bounds.back(); _bounds.pop_back();
        IntSetVal* b1 = _bounds.back(); _bounds.pop_back();
        IntSetRanges b0r(b0);
        IntSetRanges b1r(b1);
        Ranges::Inter<IntSetRanges,IntSetRanges> u(b0r,b1r);
        _bounds.push_back(IntSetVal::ai(u));
      } else if (c.id() == "set_union") {
        IntSetVal* b0 = _bounds.back(); _bounds.pop_back();
        IntSetVal* b1 = _bounds.back(); _bounds.pop_back();
        IntSetRanges b0r(b0);
        IntSetRanges b1r(b1);
        Ranges::Union<IntSetRanges,IntSetRanges> u(b0r,b1r);
        _bounds.push_back(IntSetVal::ai(u));
      } else if (c.id() == "set_diff") {
        IntSetVal* b1 = _bounds.back(); _bounds.pop_back();
        IntSetVal* b0 = _bounds.back(); _bounds.pop_back();
        IntSetRanges b0r(b0);
        IntSetRanges b1r(b1);
        Ranges::Diff<IntSetRanges,IntSetRanges> u(b0r,b1r);
        _bounds.push_back(IntSetVal::ai(u));
      } else {
        valid = false;
        _bounds.push_back(NULL);
      }
    }
    /// Visit let
    void vLet(const Let& l) {
      throw EvalError(l.loc(), "not yet supported");
    }
    /// Visit variable declaration
    void vVarDecl(const VarDecl& vd) {
      throw EvalError(vd.loc(), "not yet supported");
    }
    /// Visit annotation
    void vAnnotation(const Annotation& e) {
      throw EvalError(e.loc(), "not yet supported");
    }
    /// Visit type inst
    void vTypeInst(const TypeInst& e) {
      throw EvalError(e.loc(), "not yet supported");
    }
    /// Visit TIId
    void vTIId(const TIId& e) {
      throw EvalError(e.loc(), "not yet supported");
    }
  };

  IntSetVal* compute_intset_bounds(Expression* e) {
    ComputeIntSetBounds cb;
    BottomUpIterator<ComputeIntSetBounds> cbi(cb);
    cbi.run(e);
    if (cb.valid)
      return cb._bounds.back();
    else
      return NULL;  
  }

}
