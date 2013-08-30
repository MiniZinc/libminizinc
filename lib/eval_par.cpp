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

namespace MiniZinc {

  Expression* eval_arrayaccess(ArrayAccess* e);
  bool eval_bool(Expression* e);

  template<class E>
  typename E::Val eval_id(Expression* e) {
    Id* id = e->cast<Id>();
    if (id->_decl == NULL)
      throw EvalError(e->_loc, "undeclared identifier");
    if (id->_decl->_e == NULL)
      return E::e(id->_decl);
    typename E::Val r = E::e(id->_decl->_e);
    id->_decl->_e = r;
    return r;
  }

  class EvalIntLit {
  public:
    typedef IntLit* Val;
    typedef Expression* ArrayVal;
    static IntLit* e(Expression* e) {
      return IntLit::a(Location(),eval_int(e));
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
  class EvalBoolLit {
  public:
    typedef BoolLit* Val;
    typedef Expression* ArrayVal;
    static BoolLit* e(Expression* e) {
      return BoolLit::a(Location(),eval_bool(e));
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
      return SetLit::a(e->_loc,eval_intset(e));
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

  ArrayLit* eval_array_comp(Comprehension* e) {
    ArrayLit* ret;
    if (e->_type == Type::parint(1)) {
      std::vector<Expression*> a = eval_comp<EvalIntLit>(e);
      ret = ArrayLit::a(e->_loc,a);
    } else if (e->_type == Type::parbool(1)) {
      std::vector<Expression*> a = eval_comp<EvalBoolLit>(e);
      ret = ArrayLit::a(e->_loc,a);
    } else if (e->_type == Type::parsetint(1)) {
      std::vector<Expression*> a = eval_comp<EvalSetLit>(e);
      ret = ArrayLit::a(e->_loc,a);
    } else {
      std::vector<Expression*> a = eval_comp<EvalNone>(e);
      ret = ArrayLit::a(e->_loc,a);
    }
    ret->_type = e->_type;
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
      throw EvalError(e->_loc, "not an array expression");
    case Expression::E_ID:
      return eval_id<EvalArrayLit>(e);
    case Expression::E_ARRAYLIT:
      return e->cast<ArrayLit>();
    case Expression::E_ARRAYACCESS:
      throw EvalError(e->_loc,"arrays of arrays not supported");
    case Expression::E_COMP:
      return eval_array_comp(e->cast<Comprehension>());
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        for (unsigned int i=0; i<ite->_e_if_then.size(); i+=2) {
          if (eval_bool(ite->_e_if_then[i]))
            return eval_array_lit(ite->_e_if_then[i+1]);
        }
        return eval_array_lit(ite->_e_else);
      }
    case Expression::E_BINOP:
      {
        BinOp* bo = e->cast<BinOp>();
        if (bo->op()==BOT_PLUSPLUS) {
          ArrayLit* al0 = eval_array_lit(bo->_e0);
          ArrayLit* al1 = eval_array_lit(bo->_e1);
          std::vector<Expression*> v(al0->_v.size()+al1->_v.size());
          for (unsigned int i=al0->_v.size(); i--;)
            v[i] = al0->_v[i];
          for (unsigned int i=al1->_v.size(); i--;)
            v[al0->_v.size()+i] = al1->_v[i];
          ArrayLit* ret = ArrayLit::a(e->_loc,v);
          ret->_type = e->_type;
          return ret;
        } else {
          throw EvalError(e->_loc, "not an array expression");
        }
      }
      break;
    case Expression::E_UNOP:
      throw EvalError(e->_loc, "unary operator not supported");
    case Expression::E_CALL:
      {
        Call* ce = e->cast<Call>();
        if (ce->_decl==NULL)
          throw EvalError(e->_loc, "undeclared function");
        
        if (ce->_decl->_builtins.e)
          return ce->_decl->_builtins.e(ce->_args)
            ->cast<ArrayLit>();
        for (unsigned int i=ce->_decl->_params.size(); i--;) {
          ce->_decl->_params[i]->_e = ce->_args[i];
        }
        ArrayLit* ret = eval_array_lit(ce->_decl->_e);
        for (unsigned int i=ce->_decl->_params.size(); i--;) {
          ce->_decl->_params[i]->_e = NULL;
        }
        return ret;
      }
    case Expression::E_LET:
      {
        Let* l = e->cast<Let>();
        l->pushbindings();
        ArrayLit* ret = eval_array_lit(l->_in);
        l->popbindings();
        return ret;
      }
    }
    assert(false);
  }

  Expression* eval_arrayaccess(ArrayLit* al,
                               const std::vector<IntVal>& dims) {
    assert(al->dims() == dims.size());
    int realidx = 0;
    int realdim = 1;
    for (unsigned int i=0; i<al->dims(); i++)
      realdim *= al->max(i)-al->min(i)+1;
    for (unsigned int i=0; i<al->dims(); i++) {
      int ix = dims[i];
      if (ix < al->min(i) || ix > al->max(i))
        throw EvalError(al->_loc, "array index out of bounds");
      realdim /= al->max(i)-al->min(i)+1;
      realidx += (ix-al->min(i))*realdim;
    }
    assert(realidx >= 0 && realidx <= al->_v.size());
    return al->_v[realidx];
  }
  Expression* eval_arrayaccess(ArrayAccess* e) {
    ArrayLit* al = eval_array_lit(e->_v);
    std::vector<IntVal> dims(e->_idx.size());
    for (unsigned int i=e->_idx.size(); i--;) {
      dims[i] = eval_int(e->_idx[i]);
    }
    return eval_arrayaccess(al,dims);
  }

  IntSetVal* eval_intset(Expression* e) {
    switch (e->eid()) {
    case Expression::E_SETLIT:
      {
        SetLit* sl = e->cast<SetLit>();
        if (sl->_isv)
          return sl->_isv;
        std::vector<IntVal> vals(sl->_v.size());
        for (unsigned int i=0; i<sl->_v.size(); i++)
          vals[i] = eval_int(sl->_v[i]);
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
      throw EvalError(e->_loc,"not a set of int expression");
      break;
    case Expression::E_ARRAYLIT:
      {
        ArrayLit* al = e->cast<ArrayLit>();
        std::vector<IntVal> vals(al->_v.size());
        for (unsigned int i=0; i<al->_v.size(); i++)
          vals[i] = eval_int(al->_v[i]);
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
      return eval_id<EvalSetLit>(e)->_isv;
      break;
    case Expression::E_ARRAYACCESS:
      return eval_intset(eval_arrayaccess(e->cast<ArrayAccess>()));
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        for (unsigned int i=0; i<ite->_e_if_then.size(); i+=2) {
          if (eval_bool(ite->_e_if_then[i]))
            return eval_intset(ite->_e_if_then[i+1]);
        }
        return eval_intset(ite->_e_else);
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* bo = e->cast<BinOp>();
        if (bo->_e0->_type.isintset() && bo->_e1->_type.isintset()) {
          IntSetVal* v0 = eval_intset(bo->_e0);
          IntSetVal* v1 = eval_intset(bo->_e1);
          IntSetRanges ir0(v0);
          IntSetRanges ir1(v1);
          switch (bo->op()) {
          case BOT_UNION:
            {
              Ranges::Union<IntSetRanges,IntSetRanges> u(ir0,ir1);
              return IntSetVal::ai(u);
            }
          case BOT_DIFF:
          case BOT_SYMDIFF:
            assert(false); /// TODO
          case BOT_INTERSECT:
            {
              Ranges::Inter<IntSetRanges,IntSetRanges> u(ir0,ir1);
              return IntSetVal::ai(u);
            }
          default: throw EvalError(e->_loc,"not a set of int expression");
          }
        } else if (bo->_e0->_type.isint() && bo->_e1->_type.isint()) {
          if (bo->op() != BOT_DOTDOT)
            throw EvalError(e->_loc, "not a set of int expression");
          return IntSetVal::a(eval_int(bo->_e0),
                              eval_int(bo->_e1));
        } else {
          throw EvalError(e->_loc, "not a set of int expression");
        }
      }
      break;
    case Expression::E_CALL:
      {
        Call* ce = e->cast<Call>();
        if (ce->_decl==NULL)
          throw EvalError(e->_loc, "undeclared function");
        
        if (ce->_decl->_builtins.s)
          return ce->_decl->_builtins.s(ce->_args);

        for (unsigned int i=ce->_decl->_params.size(); i--;) {
          ce->_decl->_params[i]->_e = ce->_args[i];
        }
        IntSetVal* ret = eval_intset(ce->_decl->_e);
        for (unsigned int i=ce->_decl->_params.size(); i--;) {
          ce->_decl->_params[i]->_e = NULL;
        }
        return ret;
      }
      break;
    case Expression::E_LET:
      {
        Let* l = e->cast<Let>();
        l->pushbindings();
        IntSetVal* ret = eval_intset(l->_in);
        l->popbindings();
        return ret;
      }
      break;
    }
  }

  FloatVal eval_float(Expression* e) {
    throw EvalError(e->_loc, "floats not supported yet");
  }

  bool eval_bool(Expression* e) {
    switch (e->eid()) {
    case Expression::E_BOOLLIT: return e->cast<BoolLit>()->_v;
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
      throw EvalError(e->_loc,"not a bool expression");
      break;
    case Expression::E_ID:
      return eval_id<EvalBoolLit>(e)->_v;
      break;
    case Expression::E_ARRAYACCESS:
      return eval_bool(eval_arrayaccess(e->cast<ArrayAccess>()));
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        for (unsigned int i=0; i<ite->_e_if_then.size(); i+=2) {
          if (eval_bool(ite->_e_if_then[i]))
            return eval_bool(ite->_e_if_then[i+1]);
        }
        return eval_bool(ite->_e_else);
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* bo = e->cast<BinOp>();
        if (bo->_e0->_type.isbool() && bo->_e1->_type.isbool()) {
          switch (bo->op()) {
          case BOT_LE: return eval_bool(bo->_e0)<eval_bool(bo->_e1);
          case BOT_LQ: return eval_bool(bo->_e0)<=eval_bool(bo->_e1);
          case BOT_GR: return eval_bool(bo->_e0)>eval_bool(bo->_e1);
          case BOT_GQ: return eval_bool(bo->_e0)>=eval_bool(bo->_e1);
          case BOT_EQ: return eval_bool(bo->_e0)==eval_bool(bo->_e1);
          case BOT_NQ: return eval_bool(bo->_e0)!=eval_bool(bo->_e1);
          case BOT_EQUIV: return eval_bool(bo->_e0)==eval_bool(bo->_e1);
          case BOT_IMPL: return (!eval_bool(bo->_e0))||eval_bool(bo->_e1);
          case BOT_RIMPL: return (!eval_bool(bo->_e1))||eval_bool(bo->_e0);
          case BOT_OR: return eval_bool(bo->_e0)||eval_bool(bo->_e1);
          case BOT_AND: return eval_bool(bo->_e0)&&eval_bool(bo->_e1);
          case BOT_XOR: return eval_bool(bo->_e0)^eval_bool(bo->_e1);
          default:
            assert(false);
            throw EvalError(e->_loc,"not a bool expression");
          }
        } else if (bo->_e0->_type.isint() && bo->_e1->_type.isint()) {
          IntVal v0 = eval_int(bo->_e0);
          IntVal v1 = eval_int(bo->_e1);
          switch (bo->op()) {
          case BOT_LE: return v0<v1;
          case BOT_LQ: return v0<=v1;
          case BOT_GR: return v0>v1;
          case BOT_GQ: return v0>=v1;
          case BOT_EQ: return v0==v1;
          case BOT_NQ: return v0!=v1;
          default:
            assert(false);
            throw EvalError(e->_loc,"not a bool expression");
          }
        } else if (bo->_e0->_type.isfloat() && bo->_e1->_type.isfloat()) {
          FloatVal v0 = eval_float(bo->_e0);
          FloatVal v1 = eval_float(bo->_e1);
          switch (bo->op()) {
          case BOT_LE: return v0<v1;
          case BOT_LQ: return v0<=v1;
          case BOT_GR: return v0>v1;
          case BOT_GQ: return v0>=v1;
          case BOT_EQ: return v0==v1;
          case BOT_NQ: return v0!=v1;
          default:
            assert(false);
            throw EvalError(e->_loc,"not a bool expression");
          }
        } else if (bo->_e0->_type.isint() && bo->_e1->_type.isintset()) {
          IntVal v0 = eval_int(bo->_e0);
          IntSetVal* v1 = eval_intset(bo->_e1);
          switch (bo->op()) {
          case BOT_IN: return v1->contains(v0);
          default:
            assert(false);
            throw EvalError(e->_loc,"not a bool expression");
          }
        } else if (bo->_e0->_type.isset() && bo->_e1->_type.isset()) {
          IntSetVal* v0 = eval_intset(bo->_e0);
          IntSetVal* v1 = eval_intset(bo->_e1);
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
            assert(false);
            throw EvalError(e->_loc,"not a bool expression");
          }
        } else {
          assert(false);
          throw EvalError(e->_loc, "not a bool expression");
        }
      }
      break;
    case Expression::E_UNOP:
      {
        UnOp* uo = e->cast<UnOp>();
        bool v0 = eval_bool(uo->_e0);
        switch (uo->op()) {
        case UOT_NOT: return !v0;
        default:
          assert(false);
          throw EvalError(e->_loc,"not a bool expression");
        }
      }
      break;
    case Expression::E_CALL:
      {
        Call* ce = e->cast<Call>();
        if (ce->_decl==NULL)
          throw EvalError(e->_loc, "undeclared function");
        
        if (ce->_decl->_builtins.b)
          return ce->_decl->_builtins.b(ce->_args);

        for (unsigned int i=ce->_decl->_params.size(); i--;) {
          ce->_decl->_params[i]->_e = ce->_args[i];
        }
        bool ret = eval_bool(ce->_decl->_e);
        for (unsigned int i=ce->_decl->_params.size(); i--;) {
          ce->_decl->_params[i]->_e = NULL;
        }
        return ret;
      }
      break;
    case Expression::E_LET:
      {
        Let* l = e->cast<Let>();
        l->pushbindings();
        bool ret = eval_bool(l->_in);
        l->popbindings();
        return ret;
      }
      break;
    }
  }

  IntVal eval_int(Expression* e) {
    switch (e->eid()) {
    case Expression::E_INTLIT: return e->cast<IntLit>()->_v;
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
      throw EvalError(e->_loc,"not an integer expression");
      break;
    case Expression::E_ID:
      return eval_id<EvalIntLit>(e)->_v;
      break;
    case Expression::E_ARRAYACCESS:
      return eval_int(eval_arrayaccess(e->cast<ArrayAccess>()));
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        for (unsigned int i=0; i<ite->_e_if_then.size(); i+=2) {
          if (eval_bool(ite->_e_if_then[i]))
            return eval_int(ite->_e_if_then[i+1]);
        }
        return eval_int(ite->_e_else);
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* bo = e->cast<BinOp>();
        IntVal v0 = eval_int(bo->_e0);
        IntVal v1 = eval_int(bo->_e1);
        switch (bo->op()) {
        case BOT_PLUS: return v0+v1;
        case BOT_MINUS: return v0-v1;
        case BOT_MULT: return v0*v1;
        case BOT_IDIV: return v0 / v1;
        case BOT_MOD: return v0 % v1;
        default: throw EvalError(e->_loc,"not an integer expression");
        }
      }
      break;
    case Expression::E_UNOP:
      {
        UnOp* uo = e->cast<UnOp>();
        IntVal v0 = eval_int(uo->_e0);
        switch (uo->op()) {
        case UOT_PLUS: return v0;
        case UOT_MINUS: return -v0;
        default: throw EvalError(e->_loc,"not an integer expression");
        }
      }
      break;
    case Expression::E_CALL:
      {
        Call* ce = e->cast<Call>();
        if (ce->_decl==NULL)
          throw EvalError(e->_loc, "undeclared function");
        if (ce->_decl->_builtins.i)
          return ce->_decl->_builtins.i(ce->_args);

        for (unsigned int i=ce->_decl->_params.size(); i--;) {
          ce->_decl->_params[i]->_e = ce->_args[i];
        }
        IntVal ret = eval_int(ce->_decl->_e);
        for (unsigned int i=ce->_decl->_params.size(); i--;) {
          ce->_decl->_params[i]->_e = NULL;
        }
        return ret;
      }
      break;
    case Expression::E_LET:
      {
        Let* l = e->cast<Let>();
        l->pushbindings();
        IntVal ret = eval_int(l->_in);
        l->popbindings();
        return ret;
      }
      break;
    }
  }

  class AssignVisitor : public ItemVisitor {
  public:
    void vAssignI(AssignI* i) {
      if (i->_decl == NULL)
        throw EvalError(i->_loc, "undeclared identifier");
      if (i->_decl->_e != NULL)
        throw EvalError(i->_loc, "multiple assignments to same identifier");
      i->_decl->_e = i->_e;
    }
  };

  class EvalVisitor : public ItemVisitor {
  protected:
    ExpressionMap<VarDecl*> em;
  public:
    EvalVisitor(void) {}
    void vVarDeclI(VarDeclI* i) {
      if (i->_e->_e != NULL) {
        if (i->_e->_e->_type.isint())
          std::cerr << i->_e->_id.c_str() << " = " << eval_int(i->_e->_e) << "\n";
        if (i->_e->_e->_type.isbool())
          std::cerr << i->_e->_id.c_str() << " = " << eval_bool(i->_e->_e) << "\n";
        if (i->_e->_e->_type.isintset()) {
          SetLit* sl = EvalSetLit::e(i->_e->_e);
          ExpressionMap<VarDecl*>::iterator it = em.find(sl);
          std::cerr << i->_e->_id.c_str() << " = ";
          if (it == em.end()) {
            std::cerr << "{";
            for (IntSetRanges ir(sl->_isv); ir(); ++ir)
              std::cerr << ir.min() << ".." << ir.max() << ", ";
            std::cerr << "}\n";
            em.insert(sl, i->_e);
          } else {
            std::cerr << it->second->_id.c_str() << "\n";
          }
                    
        }
        
      }
    }
  };

  void eval_int(Model* m) {
    AssignVisitor av;
    ItemIter<AssignVisitor>(av).run(m);
    EvalVisitor ev;
    ItemIter<EvalVisitor>(ev).run(m);
  }

  Expression* eval_par(Expression* e) {
    if (e==NULL) return NULL;
    switch (e->eid()) {
    case Expression::E_ANON:
    case Expression::E_TIID:
      throw EvalError(e->_loc,"not a par expression");
    case Expression::E_COMP:
      if (e->cast<Comprehension>()->set())
        return EvalSetLit::e(e);
      // fall through
    case Expression::E_ARRAYLIT:
      {
        ArrayLit* al = eval_array_lit(e);
        for (unsigned int i=al->_v.size(); i--;)
          al->_v[i] = eval_par(al->_v[i]);
        return al;
      }
    case Expression::E_VARDECL:
      {
        VarDecl* vd = e->cast<VarDecl>();
        if (vd->_e==NULL)
          throw EvalError(vd->_loc,"not a par expression");
        return eval_par(vd->_e);
      }
    case Expression::E_ANN:
      {
        Annotation* a = e->cast<Annotation>();
        Annotation* r = Annotation::a(Location(),
          eval_par(a->_e),
          static_cast<Annotation*>(eval_par(a->_a)));
        return r;
      }
    case Expression::E_TI:
      {
        TypeInst* t = e->cast<TypeInst>();
        ASTExprVec<TypeInst> r;
        if (t->_ranges.size() > 0) {
          std::vector<TypeInst*> rv(t->_ranges.size());
          for (unsigned int i=t->_ranges.size(); i--;)
            rv[i] = static_cast<TypeInst*>(eval_par(t->_ranges[i]));
          r = ASTExprVec<TypeInst>(rv);
        }
        return 
          TypeInst::a(Location(),t->_type,r,eval_par(t->_domain));
      }
    case Expression::E_ID:
      {
        Id* id = e->cast<Id>();
        if (id->_decl==NULL)
          throw EvalError(e->_loc,"undefined identifier");
        if (id->_decl->_e==NULL)
          throw EvalError(e->_loc,"not a par expression");
        return eval_par(id->_decl->_e);
      }
    case Expression::E_ITE:
    case Expression::E_CALL:
    case Expression::E_LET:
    case Expression::E_BINOP:
    case Expression::E_SETLIT:
      {
        if (e->_type._dim != 0) {
          ArrayLit* al = eval_array_lit(e);
          for (unsigned int i=al->_v.size(); i--;)
            al->_v[i] = eval_par(al->_v[i]);
          return al;
        }
        if (e->_type._st == Type::ST_SET) {
          if (e->_type.isintset()) {
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
    case Expression::E_STRINGLIT:
    case Expression::E_UNOP:
    case Expression::E_ARRAYACCESS:
      {
        switch (e->_type._bt) {
        case Type::BT_BOOL: return EvalBoolLit::e(e);
        case Type::BT_INT:
          if (e->_type._st == Type::ST_PLAIN)
            return EvalIntLit::e(e);
          else
            return EvalSetLit::e(e);
        case Type::BT_FLOAT: throw InternalError("not yet implemented");
        case Type::BT_STRING: throw InternalError("not yet implemented");
        case Type::BT_ANN:
        case Type::BT_BOT:
        case Type::BT_TOP:
        case Type::BT_UNKNOWN:
          throw EvalError(e->_loc,"not a par expression");
        }
      }
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
      if (e->_type._dim > 0)
        return false;
      if (e->_type.ispar()) {
        if (e->_type.isint()) {
          IntVal v = eval_int(e);
          _bounds.push_back(Bounds(v,v));
        } else {
          throw EvalError(e->_loc, "not yet supported");
        }
        return false;
      } else {
        return true;
      }
    }
    /// Visit integer literal
    void vIntLit(const IntLit& i) {
      _bounds.push_back(Bounds(i._v,i._v));
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
      if (id._decl->_ti->_domain) {
        IntSetVal* isv = eval_intset(id._decl->_ti->_domain);
        if (isv->size()==0) {
          valid = false;
          _bounds.push_back(Bounds(0,0));
        } else {
          _bounds.push_back(Bounds(isv->min(0),isv->max(isv->size()-1)));
        }
      } else {
        if (id._decl->_e) {
          BottomUpIterator<ComputeIntBounds> cbi(*this);
          cbi.run(id._decl->_e);
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
      throw EvalError(aa._loc, "not yet supported");
    }
    /// Visit array comprehension
    void vComprehension(const Comprehension& c) {
      throw EvalError(c._loc, "not yet supported");
    }
    /// Visit if-then-else
    void vITE(const ITE& ite) {
      throw EvalError(ite._loc, "not yet supported");
    }
    /// Visit binary operator
    void vBinOp(const BinOp& bo) {
      Bounds b0 = _bounds.back(); _bounds.pop_back();
      Bounds b1 = _bounds.back(); _bounds.pop_back();
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
        throw EvalError(bo._loc, "not yet supported");
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
        throw EvalError(uo._loc, "not yet supported");
      }
    }
    /// Visit call
    void vCall(Call& c) {
      if (c._id == "lin_exp" &&
          c._args[0]->isa<ArrayLit>() &&
          c._args[1]->isa<ArrayLit>() &&
          c._args[2]->isa<IntLit>()) {
        ArrayLit* coeff = c._args[0]->cast<ArrayLit>();
        ArrayLit* al = c._args[1]->cast<ArrayLit>();
        IntVal d = c._args[2]->cast<IntLit>()->_v;
        int stacktop = _bounds.size();
        for (unsigned int i=al->_v.size(); i--;) {
          BottomUpIterator<ComputeIntBounds> cbi(*this);
          cbi.run(al->_v[i]);
          if (!valid)
            return;
        }
        assert(stacktop+al->_v.size()==_bounds.size());
        IntVal lb = d;
        IntVal ub = d;
        for (unsigned int i=al->_v.size(); i--;) {
          Bounds b = _bounds.back(); _bounds.pop_back();
          lb += eval_int(coeff->_v[i])*b.first;
          ub += eval_int(coeff->_v[i])*b.second;
        }
        _bounds.push_back(Bounds(lb,ub));
      } else {
        valid = false;
      }
    }
    /// Visit let
    void vLet(const Let& l) {
      throw EvalError(l._loc, "not yet supported");
    }
    /// Visit variable declaration
    void vVarDecl(const VarDecl& vd) {
      throw EvalError(vd._loc, "not yet supported");
    }
    /// Visit annotation
    void vAnnotation(const Annotation& e) {
      throw EvalError(e._loc, "not yet supported");
    }
    /// Visit type inst
    void vTypeInst(const TypeInst& e) {
      throw EvalError(e._loc, "not yet supported");
    }
    /// Visit TIId
    void vTIId(const TIId& e) {
      throw EvalError(e._loc, "not yet supported");
    }
  };

  IntBounds compute_int_bounds(Expression* e) {
    ComputeIntBounds cb;
    BottomUpIterator<ComputeIntBounds> cbi(cb);
    cbi.run(e);
    if (cb.valid)
      return IntBounds(cb._bounds.back().first,cb._bounds.back().second,true);
    else
      return IntBounds(0,0,false);
  }


}
