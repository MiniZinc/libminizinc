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

namespace MiniZinc {

  Expression* eval_arrayaccess(ASTContext& ctx, ArrayAccess* e);
  bool eval_bool(ASTContext& ctx, Expression* e);

  template<class E>
  typename E::Val eval_id(ASTContext& ctx, Expression* e) {
    Id* id = e->template cast<Id>();
    if (id->_decl == NULL)
      throw EvalError(e->_loc, "undeclared identifier");
    if (id->_decl->_e == NULL)
      return E::e(ctx,id->_decl);
    ctx.push_allocator(id->_decl->_allocator);
    typename E::Val r = E::e(ctx,id->_decl->_e);
    id->_decl->_e = r;
    ctx.pop_allocator();
    return r;
  }

  class EvalIntLit {
  public:
    typedef IntLit* Val;
    typedef Expression* ArrayVal;
    static IntLit* e(ASTContext& ctx, Expression* e) {
      return IntLit::a(ctx,Location(),eval_int(ctx,e));
    }
  };
  class EvalIntVal {
  public:
    typedef IntVal Val;
    typedef IntVal ArrayVal;
    static IntVal e(ASTContext& ctx, Expression* e) {
      return eval_int(ctx,e);
    }
  };
  class EvalBoolLit {
  public:
    typedef BoolLit* Val;
    typedef Expression* ArrayVal;
    static BoolLit* e(ASTContext& ctx, Expression* e) {
      return BoolLit::a(ctx,Location(),eval_bool(ctx,e));
    }
  };
  class EvalArrayLit {
  public:
    typedef ArrayLit* Val;
    typedef Expression* ArrayVal;
    static ArrayLit* e(ASTContext& ctx, Expression* e) {
      return eval_array_lit(ctx,e);
    }
  };
  class EvalSetLit {
  public:
    typedef SetLit* Val;
    typedef Expression* ArrayVal;
    static SetLit* e(ASTContext& ctx, Expression* e) {
      return SetLit::a(ctx,e->_loc,eval_intset(ctx,e));
    }
  };
  class EvalNone {
  public:
    typedef Expression* Val;
    typedef Expression* ArrayVal;
    static Expression* e(ASTContext& ctx, Expression* e) {
      return e;
    }
  };

  ArrayLit* eval_array_comp(ASTContext& ctx, Comprehension* e) {
    ArrayLit* ret;
    if (e->_type == Type::parint(1)) {
      std::vector<Expression*> a = eval_comp<EvalIntLit>(ctx, e);
      ret = ArrayLit::a(ctx,e->_loc,a);
    } else if (e->_type == Type::parbool(1)) {
      std::vector<Expression*> a = eval_comp<EvalBoolLit>(ctx, e);
      ret = ArrayLit::a(ctx,e->_loc,a);
    } else if (e->_type == Type::parsetint(1)) {
      std::vector<Expression*> a = eval_comp<EvalSetLit>(ctx, e);
      ret = ArrayLit::a(ctx,e->_loc,a);
    } else {
      std::vector<Expression*> a = eval_comp<EvalNone>(ctx, e);
      ret = ArrayLit::a(ctx,e->_loc,a);
    }
    ret->_type = e->_type;
    return ret;
  }
  
  ArrayLit* eval_array_lit(ASTContext& ctx, Expression* e) {
    switch (e->_eid) {
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
      return eval_id<EvalArrayLit>(ctx,e);
    case Expression::E_ARRAYLIT:
      return e->template cast<ArrayLit>();
    case Expression::E_ARRAYACCESS:
      throw EvalError(e->_loc,"arrays of arrays not supported");
    case Expression::E_COMP:
      return eval_array_comp(ctx, e->template cast<Comprehension>());
    case Expression::E_ITE:
      {
        ITE* ite = e->template cast<ITE>();
        for (ITE::IfThen& it : *ite->_e_if) {
          if (eval_bool(ctx,it.first))
            return eval_array_lit(ctx,it.second);
        }
        return eval_array_lit(ctx,ite->_e_else);
      }
    case Expression::E_BINOP:
      {
        BinOp* bo = e->template cast<BinOp>();
        if (bo->_op==BOT_PLUSPLUS) {
          ArrayLit* al0 = eval_array_lit(ctx,bo->_e0);
          ArrayLit* al1 = eval_array_lit(ctx,bo->_e1);
          std::vector<Expression*> v(al0->_v->size()+al1->_v->size());
          for (unsigned int i=al0->_v->size(); i--;)
            v[i] = (*al0->_v)[i];
          for (unsigned int i=al1->_v->size(); i--;)
            v[al0->_v->size()+i] = (*al1->_v)[i];
          ArrayLit* ret = ArrayLit::a(ctx,e->_loc,v);
          ret->_type = e->_type;
          return ret;
        }
      }
      break;
    case Expression::E_UNOP:
      throw EvalError(e->_loc, "unary operator not supported");
    case Expression::E_CALL:
      {
        Call* ce = e->template cast<Call>();
        if (ce->_decl==NULL)
          throw EvalError(e->_loc, "undeclared function");
        
        if (ce->_decl->_builtins.e)
          return ce->_decl->_builtins.e(ctx,ce->_args)
            ->template cast<ArrayLit>();
        assert(false); /// TODO
        throw EvalError(e->_loc, "unforseen error");
      }
    case Expression::E_LET:
      {
        Let* l = e->template cast<Let>();
        ctx.mark();
        for (Expression* e : *l->_let)
          if (e->isa<VarDecl>())
            ctx.trail(e->cast<VarDecl>());
        ArrayLit* ret = eval_array_lit(ctx, l->_in);
        ctx.untrail();
        return ret;
      }
    }
    assert(false);
  }

  Expression* eval_arrayaccess(ASTContext& ctx, ArrayLit* al,
                               const std::vector<IntVal>& dims) {
    assert(al->_dims->size() == dims.size());
    int realidx = 0;
    int realdim = 1;
    for (unsigned int i=0; i<al->_dims->size(); i++)
      realdim *= (*al->_dims)[i].second-(*al->_dims)[i].first+1;
    for (unsigned int i=0; i<al->_dims->size(); i++) {
      int ix = dims[i];
      if (ix < (*al->_dims)[i].first || ix > (*al->_dims)[i].second)
        throw EvalError(al->_loc, "array index out of bounds");
      realdim /= (*al->_dims)[i].second-(*al->_dims)[i].first+1;
      realidx += (ix-(*al->_dims)[i].first)*realdim;
    }
    assert(realidx >= 0 && realidx <= al->_v->size());
    return (*al->_v)[realidx];
  }
  Expression* eval_arrayaccess(ASTContext& ctx, ArrayAccess* e) {
    ArrayLit* al = eval_array_lit(ctx, e->_v);
    std::vector<IntVal> dims(e->_idx->size());
    for (unsigned int i=e->_idx->size(); i--;) {
      dims[i] = eval_int(ctx, (*e->_idx)[i]);
    }
    return eval_arrayaccess(ctx,al,dims);
  }

  IntSetVal* eval_intset(ASTContext& ctx, Expression* e) {
    switch (e->_eid) {
    case Expression::E_SETLIT:
      {
        SetLit* sl = e->template cast<SetLit>();
        if (sl->_isv)
          return sl->_isv;
        std::vector<IntVal> vals(sl->_v->size());
        for (unsigned int i=0; i<sl->_v->size(); i++)
          vals[i] = eval_int(ctx, (*sl->_v)[i]);
        return IntSetVal::a(ctx, vals);
      }
    case Expression::E_BOOLLIT:
    case Expression::E_INTLIT: 
    case Expression::E_FLOATLIT:
    case Expression::E_STRINGLIT:
    case Expression::E_ANON:
    case Expression::E_TIID:
    case Expression::E_ARRAYLIT:
    case Expression::E_VARDECL:
    case Expression::E_ANN:
    case Expression::E_TI:
    case Expression::E_UNOP:
      throw EvalError(e->_loc,"not a set of int expression");
      break;
    case Expression::E_COMP:
      {
        Comprehension* c = e->template cast<Comprehension>();
        if (!c->_set)
          throw EvalError(e->_loc,"not a set of int expression");
        std::vector<IntVal> a = eval_comp<EvalIntVal>(ctx,c);
        return IntSetVal::a(ctx,a);
      }
    case Expression::E_ID:
      return eval_id<EvalSetLit>(ctx,e)->_isv;
      break;
    case Expression::E_ARRAYACCESS:
      return eval_intset(ctx,eval_arrayaccess(ctx,
        e->template cast<ArrayAccess>()));
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->template cast<ITE>();
        for (ITE::IfThen& it : *ite->_e_if) {
          if (eval_bool(ctx,it.first))
            return eval_intset(ctx,it.second);
        }
        return eval_intset(ctx,ite->_e_else);
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* bo = e->template cast<BinOp>();
        if (bo->_e0->_type.isintset() && bo->_e1->_type.isintset()) {
          IntSetVal* v0 = eval_intset(ctx,bo->_e0);
          IntSetVal* v1 = eval_intset(ctx,bo->_e1);
          IntSetRanges ir0(v0);
          IntSetRanges ir1(v1);
          switch (bo->_op) {
          case BOT_UNION:
            {
              Ranges::Union<IntSetRanges,IntSetRanges> u(ir0,ir1);
              return IntSetVal::ai(ctx,u);
            }
          case BOT_DIFF:
          case BOT_SYMDIFF:
            assert(false); /// TODO
          case BOT_INTERSECT:
            {
              Ranges::Inter<IntSetRanges,IntSetRanges> u(ir0,ir1);
              return IntSetVal::ai(ctx,u);
            }
          default: throw EvalError(e->_loc,"not a set of int expression");
          }
        } else if (bo->_e0->_type.isint() && bo->_e1->_type.isint()) {
          if (bo->_op != BOT_DOTDOT)
            throw EvalError(e->_loc, "not a set of int expression");
          return IntSetVal::a(ctx,eval_int(ctx,bo->_e0),
                                  eval_int(ctx,bo->_e1));
        } else {
          throw EvalError(e->_loc, "not a set of int expression");
        }
      }
      break;
    case Expression::E_CALL:
      {
        Call* ce = e->template cast<Call>();
        if (ce->_decl==NULL)
          throw EvalError(e->_loc, "undeclared function");
        
        if (ce->_decl->_builtins.s)
          return ce->_decl->_builtins.s(ctx,ce->_args);
        assert(false); /// TODO
        throw EvalError(e->_loc, "unforseen error");
      }
      break;
    case Expression::E_LET:
      {
        Let* l = e->template cast<Let>();
        ctx.mark();
        for (Expression* e : *l->_let)
          if (e->isa<VarDecl>())
            ctx.trail(e->cast<VarDecl>());
        IntSetVal* ret = eval_intset(ctx, l->_in);
        ctx.untrail();
        return ret;
      }
      break;
    }
  }

  FloatVal eval_float(ASTContext& ctx, Expression* e) {
    throw EvalError(e->_loc, "floats not supported yet");
  }

  bool eval_bool(ASTContext& ctx, Expression* e) {
    switch (e->_eid) {
    case Expression::E_BOOLLIT: return e->template cast<BoolLit>()->_v;
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
      return eval_id<EvalBoolLit>(ctx,e)->_v;
      break;
    case Expression::E_ARRAYACCESS:
      return eval_bool(ctx,eval_arrayaccess(ctx,
        e->template cast<ArrayAccess>()));
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->template cast<ITE>();
        for (ITE::IfThen& it : *ite->_e_if) {
          if (eval_bool(ctx,it.first))
            return eval_bool(ctx,it.second);
        }
        return eval_bool(ctx,ite->_e_else);
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* bo = e->template cast<BinOp>();
        if (bo->_e0->_type.isbool() && bo->_e1->_type.isbool()) {
          bool v0 = eval_bool(ctx,bo->_e0);
          bool v1 = eval_bool(ctx,bo->_e1);
          switch (bo->_op) {
          case BOT_LE: return v0<v1;
          case BOT_LQ: return v0<=v1;
          case BOT_GR: return v0>v1;
          case BOT_GQ: return v0>=v1;
          case BOT_EQ: return v0==v1;
          case BOT_NQ: return v0!=v1;
          case BOT_EQUIV: return v0==v1;
          case BOT_IMPL: return (!v0)||v1;
          case BOT_RIMPL: return (!v1)||v0;
          case BOT_OR: return v0||v1;
          case BOT_AND: return v0&&v1;
          case BOT_XOR: return v0^v1;
          default:
            assert(false);
            throw EvalError(e->_loc,"not a bool expression");
          }
        } else if (bo->_e0->_type.isint() && bo->_e1->_type.isint()) {
          IntVal v0 = eval_int(ctx,bo->_e0);
          IntVal v1 = eval_int(ctx,bo->_e1);
          switch (bo->_op) {
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
          FloatVal v0 = eval_float(ctx,bo->_e0);
          FloatVal v1 = eval_float(ctx,bo->_e1);
          switch (bo->_op) {
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
          IntVal v0 = eval_int(ctx,bo->_e0);
          IntSetVal* v1 = eval_intset(ctx,bo->_e1);
          switch (bo->_op) {
          case BOT_IN: return v1->contains(v0);
          default:
            assert(false);
            throw EvalError(e->_loc,"not a bool expression");
          }
        } else if (bo->_e0->_type.isset() && bo->_e1->_type.isset()) {
          IntSetVal* v0 = eval_intset(ctx,bo->_e0);
          IntSetVal* v1 = eval_intset(ctx,bo->_e1);
          IntSetRanges ir0(v0);
          IntSetRanges ir1(v1);
          switch (bo->_op) {
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
        UnOp* uo = e->template cast<UnOp>();
        bool v0 = eval_bool(ctx,uo->_e0);
        switch (uo->_op) {
        case UOT_NOT: return !v0;
        default:
          assert(false);
          throw EvalError(e->_loc,"not a bool expression");
        }
      }
      break;
    case Expression::E_CALL:
      {
        Call* ce = e->template cast<Call>();
        if (ce->_decl==NULL)
          throw EvalError(e->_loc, "undeclared function");
        
        if (ce->_decl->_builtins.b)
          return ce->_decl->_builtins.b(ctx,ce->_args);
        assert(false); /// TODO
        throw EvalError(e->_loc, "unforseen error");
      }
      break;
    case Expression::E_LET:
      {
        Let* l = e->template cast<Let>();
        ctx.mark();
        for (Expression* e : *l->_let)
          if (e->isa<VarDecl>())
            ctx.trail(e->cast<VarDecl>());
        bool ret = eval_bool(ctx, l->_in);
        ctx.untrail();
        return ret;
      }
      break;
    }
  }

  IntVal eval_int(ASTContext& ctx, Expression* e) {
    switch (e->_eid) {
    case Expression::E_INTLIT: return e->template cast<IntLit>()->_v;
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
      return eval_id<EvalIntLit>(ctx,e)->_v;
      break;
    case Expression::E_ARRAYACCESS:
      return eval_int(ctx,eval_arrayaccess(ctx,
        e->template cast<ArrayAccess>()));
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->template cast<ITE>();
        for (ITE::IfThen& it : *ite->_e_if) {
          if (eval_bool(ctx,it.first))
            return eval_int(ctx,it.second);
        }
        return eval_int(ctx,ite->_e_else);
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* bo = e->template cast<BinOp>();
        IntVal v0 = eval_int(ctx,bo->_e0);
        IntVal v1 = eval_int(ctx,bo->_e1);
        switch (bo->_op) {
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
        UnOp* uo = e->template cast<UnOp>();
        IntVal v0 = eval_int(ctx,uo->_e0);
        switch (uo->_op) {
        case UOT_PLUS: return v0;
        case UOT_MINUS: return -v0;
        default: throw EvalError(e->_loc,"not an integer expression");
        }
      }
      break;
    case Expression::E_CALL:
      {
        Call* ce = e->template cast<Call>();
        if (ce->_decl==NULL)
          throw EvalError(e->_loc, "undeclared function");
        if (ce->_decl->_builtins.i)
          return ce->_decl->_builtins.i(ctx,ce->_args);
        assert(false); /// TODO
        throw EvalError(e->_loc, "unforseen error");
      }
      break;
    case Expression::E_LET:
      {
        Let* l = e->template cast<Let>();
        ctx.mark();
        for (Expression* e : *l->_let)
          if (e->isa<VarDecl>())
            ctx.trail(e->cast<VarDecl>());
        IntVal ret = eval_int(ctx, l->_in);
        ctx.untrail();
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
    ASTContext& ctx;
    ExpressionMap<VarDecl*> em;
  public:
    EvalVisitor(ASTContext& ctx0) : ctx(ctx0) {}
    void vVarDeclI(VarDeclI* i) {
      if (i->_e->_e != NULL) {
        if (i->_e->_e->_type.isint())
          std::cerr << i->_e->_id.c_str() << " = " << eval_int(ctx,i->_e->_e) << "\n";
        if (i->_e->_e->_type.isbool())
          std::cerr << i->_e->_id.c_str() << " = " << eval_bool(ctx,i->_e->_e) << "\n";
        if (i->_e->_e->_type.isintset()) {
          SetLit* sl = EvalSetLit::e(ctx,i->_e->_e);
          auto it = em.find(sl);
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

  void eval_int(ASTContext& ctx, Model* m) {
    AssignVisitor av;
    ItemIter<AssignVisitor>(av).run(m);
    EvalVisitor ev(ctx);
    ItemIter<EvalVisitor>(ev).run(m);
  }

  Expression* eval_par(ASTContext& ctx, Expression* e) {
    if (e==NULL) return NULL;
    switch (e->_eid) {
    case Expression::E_ANON:
    case Expression::E_TIID:
      throw EvalError(e->_loc,"not a par expression");
    case Expression::E_COMP:
    case Expression::E_ARRAYLIT:
      {
        ArrayLit* al = eval_array_lit(ctx,e);
        for (unsigned int i=al->_v->size(); i--;)
          (*al->_v)[i] = eval_par(ctx, (*al->_v)[i]);
        return al;
      }
    case Expression::E_VARDECL:
      {
        VarDecl* vd = e->cast<VarDecl>();
        if (vd->_e==NULL)
          throw EvalError(vd->_loc,"not a par expression");
        return eval_par(ctx,vd->_e);
      }
    case Expression::E_ANN:
      {
        Annotation* a = e->cast<Annotation>();
        Annotation* r = Annotation::a(ctx,Location(),
          eval_par(ctx,a->_e),
          static_cast<Annotation*>(eval_par(ctx,a->_a)));
        return r;
      }
    case Expression::E_TI:
      {
        TypeInst* t = e->cast<TypeInst>();
        CtxVec<TypeInst*>* r = NULL;
        if (t->_ranges) {
          std::vector<TypeInst*> rv(t->_ranges->size());
          for (unsigned int i=t->_ranges->size(); i--;)
            rv[i] = static_cast<TypeInst*>(eval_par(ctx,(*t->_ranges)[i]));
          r = CtxVec<TypeInst*>::a(ctx,rv);
        }
        return 
          TypeInst::a(ctx,Location(),t->_type,eval_par(ctx,t->_domain),r);
      }
    case Expression::E_ID:
      {
        Id* id = e->cast<Id>();
        if (id->_decl==NULL)
          throw EvalError(e->_loc,"undefined identifier");
        if (id->_decl->_e==NULL)
          throw EvalError(e->_loc,"not a par expression");
        return eval_par(ctx,id->_decl->_e);
      }
    case Expression::E_ITE:
    case Expression::E_CALL:
    case Expression::E_LET:
    case Expression::E_BINOP:
    case Expression::E_SETLIT:
      {
        if (e->_type._dim != 0) {
          ArrayLit* al = eval_array_lit(ctx,e);
          for (unsigned int i=al->_v->size(); i--;)
            (*al->_v)[i] = eval_par(ctx, (*al->_v)[i]);
          return al;
        }
        if (e->_type._st == Type::ST_SET) {
          if (e->_type.isintset()) {
            return EvalSetLit::e(ctx,e);
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
        case Type::BT_BOOL: return EvalBoolLit::e(ctx,e);
        case Type::BT_INT: return EvalIntLit::e(ctx,e);
        case Type::BT_FLOAT: throw InternalError("not yet implemented");
        case Type::BT_STRING: throw InternalError("not yet implemented");
        case Type::BT_ANN: case Type::BT_BOT: case Type::BT_UNKNOWN:
          throw EvalError(e->_loc,"not a par expression");
        }
      }
    }
  }

}
