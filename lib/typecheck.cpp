/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/typecheck.hh>

#include <minizinc/astiterator.hh>
#include <minizinc/astexception.hh>
#include <minizinc/hash.hh>

#include <string>
#include <sstream>

namespace MiniZinc {
  
  struct VarDeclCmp {
    UNORDERED_NAMESPACE::unordered_map<VarDecl*,int>& _pos;
    VarDeclCmp(UNORDERED_NAMESPACE::unordered_map<VarDecl*,int>& pos) : _pos(pos) {}
    bool operator()(Expression* e0, Expression* e1) {
      if (VarDecl* vd0 = e0->dyn_cast<VarDecl>()) {
        if (VarDecl* vd1 = e1->dyn_cast<VarDecl>()) {
          return _pos[vd0] < _pos[vd1];
        } else {
          return true;
        }
      } else {
        return false;
      }
    }
  };
  struct ItemCmp {
    UNORDERED_NAMESPACE::unordered_map<VarDecl*,int>& _pos;
    ItemCmp(UNORDERED_NAMESPACE::unordered_map<VarDecl*,int>& pos) : _pos(pos) {}
    bool operator()(Item* i0, Item* i1) {
      if (VarDeclI* vd0 = i0->cast<VarDeclI>()) {
        if (VarDeclI* vd1 = i1->cast<VarDeclI>()) {
          return _pos[vd0->e()] < _pos[vd1->e()];
        } else {
          return true;
        }
      } else {
        return false;
      }
    }
  };
  
  void
  TopoSorter::add(VarDecl* vd, bool unique) {
    DeclMap::iterator vdi = env.find(vd->id());
    if (vdi == env.end()) {
      Decls nd; nd.push_back(vd);
      env.insert(vd->id(),nd);
    } else {
      if (unique) {
        GCLock lock;
        throw TypeError(vd->loc(),"identifier `"+vd->id()->str().str()+
                        "' already defined");
      }
      vdi->second.push_back(vd);
    }
  }
  void
  TopoSorter::remove(VarDecl* vd) {
    DeclMap::iterator vdi = env.find(vd->id());
    assert(vdi != env.end());
    vdi->second.pop_back();
    if (vdi->second.empty())
      env.remove(vd->id());
  }
  
  VarDecl*
  TopoSorter::checkId(Id* id, const Location& loc) {
    DeclMap::iterator decl = env.find(id);
    if (decl==env.end()) {
      GCLock lock;
      throw TypeError(loc,"undefined identifier `"+id->str().str()+"'");
    }
    PosMap::iterator pi = pos.find(decl->second.back());
    if (pi==pos.end()) {
      // new id
      run(decl->second.back());
    } else {
      // previously seen, check if circular
      if (pi->second==-1) {
        GCLock lock;
        throw TypeError(loc,"circular definition of `"+id->str().str()+"'");
      }
    }
    return decl->second.back();
  }

  VarDecl*
  TopoSorter::checkId(const ASTString& id_v, const Location& loc) {
    GCLock lock;
    Id* id = new Id(loc,id_v,NULL);
    return checkId(id, loc);
  }

  void
  TopoSorter::run(Expression* e) {
    if (e==NULL)
      return;
    for (ExpressionSetIter it = e->ann().begin(); it != e->ann().end(); ++it)
      run(*it);
    switch (e->eid()) {
    case Expression::E_INTLIT:
    case Expression::E_FLOATLIT:
    case Expression::E_BOOLLIT:
    case Expression::E_STRINGLIT:
    case Expression::E_ANON:
      break;
    case Expression::E_SETLIT:
      {
        SetLit* sl = e->cast<SetLit>();
        if(sl->isv()==NULL)
          for (unsigned int i=0; i<sl->v().size(); i++)
            run(sl->v()[i]);
      }
      break;
    case Expression::E_ID:
      {
        if (e != constants().absent)
          e->cast<Id>()->decl(checkId(e->cast<Id>(),e->loc()));
      }
      break;
    case Expression::E_ARRAYLIT:
      {
        ArrayLit* al = e->cast<ArrayLit>();
        for (unsigned int i=0; i<al->v().size(); i++)
          run(al->v()[i]);
      }
      break;
    case Expression::E_ARRAYACCESS:
      {
        ArrayAccess* ae = e->cast<ArrayAccess>();
        run(ae->v());
        for (unsigned int i=0; i<ae->idx().size(); i++)
          run(ae->idx()[i]);
      }
      break;
    case Expression::E_COMP:
      {
        Comprehension* ce = e->cast<Comprehension>();
        for (int i=0; i<ce->n_generators(); i++) {
          run(ce->in(i));
          for (int j=0; j<ce->n_decls(i); j++) {
            add(ce->decl(i,j), false);
          }
        }
        if (ce->where())
          run(ce->where());
        run(ce->e());
        for (int i=0; i<ce->n_generators(); i++) {
          for (int j=0; j<ce->n_decls(i); j++) {
            remove(ce->decl(i,j));
          }
        }
      }
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        for (int i=0; i<ite->size(); i++) {
          run(ite->e_if(i));
          run(ite->e_then(i));
        }
        run(ite->e_else());
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* be = e->cast<BinOp>();
        run(be->lhs());
        run(be->rhs());
      }
      break;
    case Expression::E_UNOP:
      {
        UnOp* ue = e->cast<UnOp>();
        run(ue->e());
      }
      break;
    case Expression::E_CALL:
      {
        Call* ce = e->cast<Call>();
        for (unsigned int i=0; i<ce->args().size(); i++)
          run(ce->args()[i]);
      }
      break;
    case Expression::E_VARDECL:
      {
        VarDecl* ve = e->cast<VarDecl>();
        PosMap::iterator pi = pos.find(ve);
        if (pi==pos.end()) {
          pos.insert(std::pair<VarDecl*,int>(ve,-1));
          run(ve->ti());
          run(ve->e());
          ve->payload(decls.size());
          decls.push_back(ve);
          pi = pos.find(ve);
          pi->second = decls.size()-1;
        } else {
          assert(pi->second != -1);
        }
      }
      break;
    case Expression::E_TI:
      {
        TypeInst* ti = e->cast<TypeInst>();
        for (unsigned int i=0; i<ti->ranges().size(); i++)
          run(ti->ranges()[i]);
        run(ti->domain());
      }
      break;
    case Expression::E_TIID:
      break;
    case Expression::E_LET:
      {
        Let* let = e->cast<Let>();
        for (unsigned int i=0; i<let->let().size(); i++) {
          run(let->let()[i]);
          if (VarDecl* vd = let->let()[i]->dyn_cast<VarDecl>()) {
            add(vd,false);
          }
        }
        run(let->in());
        VarDeclCmp poscmp(pos);
        std::stable_sort(let->let().begin(), let->let().end(), poscmp);
        for (unsigned int i=0; i<let->let().size(); i++) {
          if (VarDecl* vd = let->let()[i]->dyn_cast<VarDecl>()) {
            remove(vd);
          }
        }
      }
      break;
    }
  }
  
  KeepAlive addCoercion(Model* m, Expression* e, const Type& funarg_t) {
    if (e->type().dim()==funarg_t.dim() && (funarg_t.bt()==Type::BT_BOT || funarg_t.bt()==Type::BT_TOP || e->type().bt()==funarg_t.bt() || e->type().bt()==Type::BT_BOT))
      return e;
    std::vector<Expression*> args(1);
    args[0] = e;
    GCLock lock;
    Call* c = NULL;
    if (e->type().dim()==0 && funarg_t.dim()!=0) {
      std::vector<Expression*> set2a_args(1);
      set2a_args[0] = e;
      Call* set2a = new Call(e->loc(), ASTString("set2array"), set2a_args);
      FunctionI* fi = m->matchFn(set2a);
      assert(fi);
      set2a->type(fi->rtype(args));
      set2a->decl(fi);
      e = set2a;
    }
    if (funarg_t.bt()==Type::BT_TOP || e->type().bt()==funarg_t.bt() || e->type().bt()==Type::BT_BOT) {
      KeepAlive ka(e);
      return ka;
    }
    if (e->type().bt()==Type::BT_BOOL) {
      if (funarg_t.bt()==Type::BT_INT) {
        c = new Call(e->loc(), constants().ids.bool2int, args);
      } else if (funarg_t.bt()==Type::BT_FLOAT) {
        c = new Call(e->loc(), constants().ids.bool2float, args);
      }
    } else if (e->type().bt()==Type::BT_INT) {
      if (funarg_t.bt()==Type::BT_FLOAT) {
        c = new Call(e->loc(), constants().ids.int2float, args);
      }
    }
    if (c) {
      FunctionI* fi = m->matchFn(c);
      assert(fi);
      c->type(fi->rtype(args));
      c->decl(fi);
      KeepAlive ka(c);
      return ka;
    }
    throw TypeError(e->loc(),"cannot determine coercion from type "+e->type().toString()+" to type "+funarg_t.toString());
  }
  KeepAlive addCoercion(Model* m, Expression* e, Expression* funarg) {
    return addCoercion(m, e, funarg->type());
  }
  
  template<bool ignoreVarDecl>
  class Typer {
  public:
    Model* _model;
    std::vector<TypeError>& _typeErrors;
    Typer(Model* model, std::vector<TypeError>& typeErrors) : _model(model), _typeErrors(typeErrors) {}
    /// Check annotations when expression is finished
    void exit(Expression* e) {
      for (ExpressionSetIter it = e->ann().begin(); it != e->ann().end(); ++it)
        if (!(*it)->type().isann())
          throw TypeError((*it)->loc(),"expected annotation, got `"+(*it)->type().toString()+"'");
    }
    bool enter(Expression*) { return true; }
    /// Visit integer literal
    void vIntLit(const IntLit&) {}
    /// Visit floating point literal
    void vFloatLit(const FloatLit&) {}
    /// Visit Boolean literal
    void vBoolLit(const BoolLit&) {}
    /// Visit set literal
    void vSetLit(SetLit& sl) {
      Type ty; ty.st(Type::ST_SET);
      for (unsigned int i=0; i<sl.v().size(); i++) {
        if (sl.v()[i]->type().isvar())
          ty.ti(Type::TI_VAR);
        if (sl.v()[i]->type().cv())
          ty.cv(true);
        /// TODO: add coercion if types don't match
        if (!Type::bt_subtype(sl.v()[i]->type().bt(), ty.bt())) {
          if (ty.bt() == Type::BT_UNKNOWN || Type::bt_subtype(ty.bt(), sl.v()[i]->type().bt()))
            ty.bt(sl.v()[i]->type().bt());
          else
            throw TypeError(sl.loc(),"non-uniform set literal");
        }
      }
      if (ty.bt() == Type::BT_UNKNOWN) {
        ty.bt(Type::BT_BOT);
      } else {
        for (unsigned int i=0; i<sl.v().size(); i++) {
          sl.v()[i] = addCoercion(_model, sl.v()[i], ty)();
        }
      }
      sl.type(ty);
    }
    /// Visit string literal
    void vStringLit(const StringLit&) {}
    /// Visit identifier
    void vId(Id& id) {
      if (&id != constants().absent) {
        assert(!id.decl()->type().isunknown());
        id.type(id.decl()->type());
      }
    }
    /// Visit anonymous variable
    void vAnonVar(const AnonVar&) {}
    /// Visit array literal
    void vArrayLit(ArrayLit& al) {
      Type ty; ty.dim(al.dims());
      std::vector<AnonVar*> anons;
      bool haveInferredType = false;
      for (unsigned int i=0; i<al.v().size(); i++) {
        Expression* vi = al.v()[i];
        AnonVar* av = vi->dyn_cast<AnonVar>();
        if (av) {
          ty.ti(Type::TI_VAR);
          anons.push_back(av);
        } else if (vi->type().isvar()) {
          ty.ti(Type::TI_VAR);
        }
        if (vi->type().cv())
          ty.cv(true);
        if (vi->type().isopt()) {
          ty.ot(Type::OT_OPTIONAL);
        }
        
        if (ty.bt()==Type::BT_UNKNOWN) {
          if (av == NULL) {
            if (haveInferredType) {
              if (ty.st() != vi->type().st()) {
                throw TypeError(al.loc(),"non-uniform array literal");
              }
            } else {
              haveInferredType = true;
              ty.st(vi->type().st());
            }
            if (vi->type().bt() != Type::BT_BOT) {
              ty.bt(vi->type().bt());
            }
          }
        } else {
          if (av == NULL) {
            if (vi->type().bt() == Type::BT_BOT) {
              if (vi->type().st() != ty.st()) {
                throw TypeError(al.loc(),"non-uniform array literal");
              }
            } else {
              if (Type::bt_subtype(ty.bt(), vi->type().bt())) {
                ty.bt(vi->type().bt());
              }
              if (!Type::bt_subtype(vi->type().bt(),ty.bt()) || ty.st() != vi->type().st()) {
                throw TypeError(al.loc(),"non-uniform array literal");
              }
            }
          }
        }
      }
      if (ty.bt() == Type::BT_UNKNOWN) {
        ty.bt(Type::BT_BOT);
        if (!anons.empty())
          throw TypeError(al.loc(),"array literal must contain at least one non-anonymous variable");
      } else {
        Type at = ty;
        at.dim(0);
        for (unsigned int i=0; i<anons.size(); i++) {
          anons[i]->type(at);
        }
        for (unsigned int i=0; i<al.v().size(); i++) {
          al.v()[i] = addCoercion(_model, al.v()[i], at)();
        }
      }
      al.type(ty);
    }
    /// Visit array access
    void vArrayAccess(ArrayAccess& aa) {
      if (aa.v()->type().dim()==0) {
        if (aa.v()->type().st() == Type::ST_SET) {
          Type tv = aa.v()->type();
          tv.st(Type::ST_PLAIN);
          tv.dim(1);
          aa.v(addCoercion(_model, aa.v(), tv)());
        } else {
          throw TypeError(aa.v()->loc(),"not an array in array access");
        }
      }
      if (aa.v()->type().dim() != aa.idx().size())
        throw TypeError(aa.v()->loc(),"array dimensions do not match");
      Type tt = aa.v()->type();
      tt.dim(0);
      for (unsigned int i=0; i<aa.idx().size(); i++) {
        Expression* aai = aa.idx()[i];
        if (aai->isa<AnonVar>()) {
          aai->type(Type::varint());
        }
        if (aai->type().isset() || (aai->type().bt() != Type::BT_INT && aai->type().bt() != Type::BT_BOOL) || aai->type().dim() != 0) {
          throw TypeError(aai->loc(),"array index must be `int', but is `"+aai->type().toString()+"'");
        }
        aa.idx()[i] = addCoercion(_model, aai, Type::varint())();
        if (aai->type().isopt()) {
          tt.ot(Type::OT_OPTIONAL);
        }
        if (aai->type().isvar()) {
          tt.ti(Type::TI_VAR);
        }
        if (aai->type().cv())
          tt.cv(true);
      }
      aa.type(tt);
    }
    /// Visit array comprehension
    void vComprehension(Comprehension& c) {
      Type tt = c.e()->type();
      for (int i=0; i<c.n_generators(); i++) {
        Expression* g_in = c.in(i);
        const Type& ty_in = g_in->type();
        if (ty_in == Type::varsetint()) {
          tt.ot(Type::OT_OPTIONAL);
          tt.ti(Type::TI_VAR);
        }
        if (ty_in.cv())
          tt.cv(true);
      }
      if (c.where()) {
        if (c.where()->type() == Type::varbool()) {
          tt.ot(Type::OT_OPTIONAL);
          tt.ti(Type::TI_VAR);
        } else if (c.where()->type() != Type::parbool()) {
          throw TypeError(c.where()->loc(),
                          "where clause must be bool, but is `"+
                          c.where()->type().toString()+"'");
        }
        if (c.where()->type().cv())
          tt.cv(true);
      }
      if (c.set()) {
        if (c.e()->type().dim() != 0 || c.e()->type().st() == Type::ST_SET)
          throw TypeError(c.e()->loc(),
              "set comprehension expression must be scalar, but is `"
              +c.e()->type().toString()+"'");
        tt.st(Type::ST_SET);
      } else {
        if (c.e()->type().dim() != 0)
          throw TypeError(c.e()->loc(),
            "array comprehension expression cannot be an array");
        tt.dim(1);
      }
      c.type(tt);
    }
    /// Visit array comprehension generator
    void vComprehensionGenerator(Comprehension& c, int gen_i) {
      Expression* g_in = c.in(gen_i);
      const Type& ty_in = g_in->type();
      if (ty_in != Type::varsetint() && ty_in != Type::parsetint() && ty_in.dim() != 1) {
        throw TypeError(g_in->loc(),
                        "generator expression must be (par or var) set of int or one-dimensional array, but is `"+ty_in.toString()+"'");
      }
      Type ty_id;
      bool needIntLit = false;
      if (ty_in.dim()==0) {
        ty_id = Type::parint();
        needIntLit = true;
      } else {
        ty_id = ty_in;
        ty_id.dim(0);
      }
      for (int j=0; j<c.n_decls(gen_i); j++) {
        if (needIntLit) {
          GCLock lock;
          c.decl(gen_i,j)->e(new IntLit(Location(),0));
        }
        c.decl(gen_i,j)->type(ty_id);
        c.decl(gen_i,j)->ti()->type(ty_id);
      }
    }
    /// Visit if-then-else
    void vITE(ITE& ite) {
      Type tret = ite.e_else()->type();
      bool allpar = !(tret.isvar());
      bool allpresent = !(tret.isopt());
      bool varcond = false;
      for (int i=0; i<ite.size(); i++) {
        Expression* eif = ite.e_if(i);
        Expression* ethen = ite.e_then(i);
        varcond = varcond || (eif->type() == Type::varbool());
        if (eif->type() != Type::parbool() && eif->type() != Type::varbool())
          throw TypeError(eif->loc(),
            "expected bool conditional expression, got `"+
            eif->type().toString()+"'");
        if (tret.isbot()) {
          tret.bt(ethen->type().bt());
        }
        if (eif->type().cv())
          tret.cv(true);
        if ( (!ethen->type().isbot() && !Type::bt_subtype(ethen->type().bt(), tret.bt()) && !Type::bt_subtype(tret.bt(), ethen->type().bt())) ||
            ethen->type().st() != tret.st() ||
            ethen->type().dim() != tret.dim()) {
          throw TypeError(ethen->loc(),
            "type mismatch in branches of conditional. Then-branch has type `"+
            ethen->type().toString()+"', but else branch has type `"+
            tret.toString()+"'");
        }
        if (Type::bt_subtype(tret.bt(), ethen->type().bt())) {
          tret.bt(ethen->type().bt());
        }
        if (ethen->type().isvar()) allpar=false;
        if (ethen->type().isopt()) allpresent=false;
        if (ethen->type().cv())
          tret.cv(true);
      }
      for (int i=0; i<ite.size(); i++) {
        ite.e_then(i, addCoercion(_model,ite.e_then(i), tret)());
      }
      ite.e_else(addCoercion(_model, ite.e_else(), tret)());
      /// TODO: perhaps extend flattener to array types, but for now throw an error
      if (varcond && tret.dim() > 0)
        throw TypeError(ite.loc(), "conditional with var condition cannot have array type");
      if (varcond || !allpar)
        tret.ti(Type::TI_VAR);
      if (!allpresent)
        tret.ot(Type::OT_OPTIONAL);
      ite.type(tret);
    }
    /// Visit binary operator
    void vBinOp(BinOp& bop) {
      std::vector<Expression*> args(2);
      args[0] = bop.lhs(); args[1] = bop.rhs();
      if (FunctionI* fi = _model->matchFn(bop.opToString(),args)) {
        bop.lhs(addCoercion(_model,bop.lhs(),fi->argtype(args, 0))());
        bop.rhs(addCoercion(_model,bop.rhs(),fi->argtype(args, 1))());
        args[0] = bop.lhs(); args[1] = bop.rhs();
        Type ty = fi->rtype(args);
        ty.cv(bop.lhs()->type().cv() || bop.rhs()->type().cv());
        bop.type(ty);
        
        if (fi->e())
          bop.decl(fi);
        else
          bop.decl(NULL);
      } else {
        throw TypeError(bop.loc(),
          std::string("type error in operator application for `")+
          bop.opToString().str()+"'. No matching operator found with left-hand side type `"+bop.lhs()->type().toString()+
                        "' and right-hand side type `"+bop.rhs()->type().toString()+"'");
      }
    }
    /// Visit unary operator
    void vUnOp(UnOp& uop) {
      std::vector<Expression*> args(1);
      args[0] = uop.e();
      if (FunctionI* fi = _model->matchFn(uop.opToString(),args)) {
        uop.e(addCoercion(_model,uop.e(),fi->argtype(args,0))());
        args[0] = uop.e();
        Type ty = fi->rtype(args);
        ty.cv(uop.e()->type().cv());
        uop.type(ty);
        if (fi->e())
          uop.decl(fi);
      } else {
        throw TypeError(uop.loc(),
          std::string("type error in operator application for `")+
          uop.opToString().str()+"'. No matching operator found with type `"+uop.e()->type().toString()+"'");
      }
    }
    /// Visit call
    void vCall(Call& call) {
      std::vector<Expression*> args(call.args().size());
      std::copy(call.args().begin(),call.args().end(),args.begin());
      if (FunctionI* fi = _model->matchFn(call.id(),args)) {
        bool cv = false;
        for (unsigned int i=0; i<args.size(); i++) {
          args[i] = addCoercion(_model,call.args()[i],fi->argtype(args,i))();
          call.args()[i] = args[i];
          cv = cv || args[i]->type().cv();
        }
        Type ty = fi->rtype(args);
        ty.cv(cv);
        call.type(ty);
        call.decl(fi);
      } else {
        std::ostringstream oss;
        oss << "no function or predicate with this signature found: `";
        oss << call.id() << "(";
        for (unsigned int i=0; i<call.args().size(); i++) {
          oss << call.args()[i]->type().toString();
          if (i<call.args().size()-1) oss << ",";
        }
        oss << ")'";
        throw TypeError(call.loc(), oss.str());
      }
    }
    /// Visit let
    void vLet(Let& let) {
      bool cv = false;
      for (unsigned int i=0; i<let.let().size(); i++) {
        Expression* li = let.let()[i];
        cv = cv || li->type().cv();
        if (VarDecl* vdi = li->dyn_cast<VarDecl>()) {
          if (vdi->type().ispar() && vdi->e() == NULL)
            throw TypeError(vdi->loc(),
              "let variable `"+vdi->id()->v().str()+"' must be initialised");
        }
      }
      Type ty = let.in()->type();
      ty.cv(cv);
      let.type(ty);
    }
    /// Visit variable declaration
    void vVarDecl(VarDecl& vd) {
      if (ignoreVarDecl) {
        assert(!vd.type().isunknown());
        if (vd.e()) {
          if (! vd.e()->type().isSubtypeOf(vd.ti()->type())) {
            _typeErrors.push_back(TypeError(vd.e()->loc(),
                                            "initialisation value for `"+vd.id()->str().str()+"' has invalid type-inst: expected `"+
                                            vd.ti()->type().toString()+"', actual `"+vd.e()->type().toString()+"'"));
          } else {
            vd.e(addCoercion(_model, vd.e(), vd.ti()->type())());
          }
        }
      } else {
        vd.type(vd.ti()->type());
        vd.id()->type(vd.type());
      }
    }
    /// Visit type inst
    void vTypeInst(TypeInst& ti) {
      Type tt = ti.type();
      if (ti.ranges().size()>0) {
        bool foundTIId=false;
        for (unsigned int i=0; i<ti.ranges().size(); i++) {
          TypeInst* ri = ti.ranges()[i];
          assert(ri != NULL);
          if (ri->type().cv())
            tt.cv(true);
          if (ri->type() == Type::top()) {
            if (foundTIId) {
              throw TypeError(ri->loc(),
                "only one type-inst variable allowed in array index");
            } else {
              foundTIId = true;
            }
          } else if (ri->type() != Type::parint()) {
            assert(ri->isa<TypeInst>());
            throw TypeError(ri->loc(),
              "invalid type in array index, expected `set of int', actual `"+
              ri->type().toString()+"'");
          }
        }
        tt.dim(foundTIId ? -1 : ti.ranges().size());
      }
      if (ti.domain() && ti.domain()->type().cv())
        tt.cv(true);
      if (ti.domain() && !ti.domain()->isa<TIId>()) {
        if (ti.domain()->type().ti() != Type::TI_PAR ||
            ti.domain()->type().st() != Type::ST_SET)
          throw TypeError(ti.domain()->loc(),
            "type-inst must be par set");
        if (ti.domain()->type().dim() != 0)
          throw TypeError(ti.domain()->loc(),
            "type-inst cannot be an array");
      }
      if (tt.isunknown()) {
        assert(ti.domain());
        switch (ti.domain()->type().bt()) {
        case Type::BT_INT:
        case Type::BT_FLOAT:
          break;
        case Type::BT_BOT:
          {
            Type tidt = ti.domain()->type();
            tidt.bt(Type::BT_INT);
            ti.domain()->type(tidt);
          }
          break;
        default:
          throw TypeError(ti.domain()->loc(),
            "type-inst must be int or float");
        }
        tt.bt(ti.domain()->type().bt());
      } else {
//        assert(ti.domain()==NULL || ti.domain()->isa<TIId>());
      }
      ti.type(tt);
    }
    void vTIId(TIId& id) {}
  };
  
  void typecheck(Model* m, std::vector<TypeError>& typeErrors, bool ignoreUndefinedParameters) {
    TopoSorter ts;
    
    std::vector<FunctionI*> functionItems;
    
    class TSV0 : public ItemVisitor {
    public:
      TopoSorter& ts;
      Model* model;
      std::vector<FunctionI*>& fis;
      TSV0(TopoSorter& ts0, Model* model0, std::vector<FunctionI*>& fis0)
        : ts(ts0), model(model0), fis(fis0) {}
      void vVarDeclI(VarDeclI* i) { ts.add(i->e(), true); }
      void vFunctionI(FunctionI* i) {
        model->registerFn(i);
        fis.push_back(i);
      }
    } _tsv0(ts,m,functionItems);
    iterItems(_tsv0,m);

    class TSV1 : public ItemVisitor {
    public:
      TopoSorter& ts;
      TSV1(TopoSorter& ts0) : ts(ts0) {}
      void vVarDeclI(VarDeclI* i) { ts.run(i->e()); }
      void vAssignI(AssignI* i) {
        VarDecl* vd = ts.checkId(i->id(),i->loc());
        if (vd->e())
          throw TypeError(i->loc(),"multiple assignment to the same variable");
        TopoSorter::PosMap::iterator pi = ts.pos.find(vd);
        int tmp = pi->second;
        pi->second = -1;
        ts.run(i->e());
        pi = ts.pos.find(vd);
        pi->second = tmp;
        i->decl(vd);
      }
      void vConstraintI(ConstraintI* i) { ts.run(i->e()); }
      void vSolveI(SolveI* i) {
        for (ExpressionSetIter it = i->ann().begin(); it != i->ann().end(); ++it)
          ts.run(*it);
        ts.run(i->e());
      }
      void vOutputI(OutputI* i) { ts.run(i->e()); }
      void vFunctionI(FunctionI* fi) {
        ts.run(fi->ti());
        for (unsigned int i=0; i<fi->params().size(); i++)
          ts.run(fi->params()[i]);
        for (ExpressionSetIter it = fi->ann().begin(); it != fi->ann().end(); ++it)
          ts.run(*it);
        for (unsigned int i=0; i<fi->params().size(); i++)
          ts.add(fi->params()[i],false);
        ts.run(fi->e());
        for (unsigned int i=0; i<fi->params().size(); i++)
          ts.remove(fi->params()[i]);
      }
    } _tsv1(ts);
    iterItems(_tsv1,m);

    m->sortFn();

    {
      struct SortByPayload {
        bool operator ()(Item* i0, Item* i1) {
          if (i0->isa<IncludeI>())
            return !i1->isa<IncludeI>();
          if (VarDeclI* vdi0 = i0->dyn_cast<VarDeclI>()) {
            if (VarDeclI* vdi1 = i1->dyn_cast<VarDeclI>()) {
              return vdi0->e()->payload() < vdi1->e()->payload();
            } else {
              return !i1->isa<IncludeI>();
            }
          }
          return false;
        }
      } _sbp;
      
      std::stable_sort(m->begin(), m->end(), _sbp);
    }

    
    {
      Typer<false> ty(m, typeErrors);
      BottomUpIterator<Typer<false> > bu_ty(ty);
      for (unsigned int i=0; i<ts.decls.size(); i++) {
        /// TODO:
        /// Currently only type checks the TypeInst and the actual declaration.
        /// This can be a problem if the TypeInst calls functions, because functions
        /// are only type-checked after the VarDecls and therefore may still contain
        /// unknown types.
        ts.decls[i]->payload(0);
        bu_ty.run(ts.decls[i]->ti());
        ty.vVarDecl(*ts.decls[i]);
      }
      for (unsigned int i=0; i<functionItems.size(); i++) {
        bu_ty.run(functionItems[i]->ti());
        for (unsigned int j=0; j<functionItems[i]->params().size(); j++)
          bu_ty.run(functionItems[i]->params()[j]);
      }
    }
    
    {
      Typer<true> ty(m, typeErrors);
      BottomUpIterator<Typer<true> > bu_ty(ty);
      
      class TSV2 : public ItemVisitor {
      public:
        BottomUpIterator<Typer<true> >& bu_ty;
        std::vector<TypeError>& _typeErrors;
        TSV2(BottomUpIterator<Typer<true> >& b, std::vector<TypeError>& typeErrors) : bu_ty(b), _typeErrors(typeErrors) {}
        void vVarDeclI(VarDeclI* i) { bu_ty.run(i->e()); }
        void vAssignI(AssignI* i) {
          bu_ty.run(i->e());
          if (!i->e()->type().isSubtypeOf(i->decl()->ti()->type())) {
            _typeErrors.push_back(TypeError(i->e()->loc(),
                                           "assignment value for `"+i->decl()->id()->str().str()+"' has invalid type-inst: expected `"+
                                           i->decl()->ti()->type().toString()+"', actual `"+i->e()->type().toString()+"'"));
          }
        }
        void vConstraintI(ConstraintI* i) {
          bu_ty.run(i->e());
          if (!i->e()->type().isSubtypeOf(Type::varbool()))
            throw TypeError(i->e()->loc(), "invalid type of constraint, expected `"+Type::varbool().toString()+"', actual `"+i->e()->type().toString()+"'");
        }
        void vSolveI(SolveI* i) {
          for (ExpressionSetIter it = i->ann().begin(); it != i->ann().end(); ++it) {
            bu_ty.run(*it);
            if (!(*it)->type().isann())
              throw TypeError((*it)->loc(), "expected annotation, got `"+(*it)->type().toString()+"'");
          }
          bu_ty.run(i->e());
          if (i->e()) {
            Type et = i->e()->type();
            if (! (et.isSubtypeOf(Type::varint()) || 
                   et.isSubtypeOf(Type::varfloat())))
              throw TypeError(i->e()->loc(),
                "objective has invalid type, expected int or float, actual `"+et.toString()+"'");
          }
        }
        void vOutputI(OutputI* i) {
          bu_ty.run(i->e());
          if (i->e()->type() != Type::parstring(1) && i->e()->type() != Type::bot(1))
            throw TypeError(i->e()->loc(), "invalid type in output item, expected `"+Type::parstring(1).toString()+"', actual `"+i->e()->type().toString()+"'");
        }
        void vFunctionI(FunctionI* i) {
          for (ExpressionSetIter it = i->ann().begin(); it != i->ann().end(); ++it) {
            bu_ty.run(*it);
            if (!(*it)->type().isann())
              throw TypeError((*it)->loc(), "expected annotation, got `"+(*it)->type().toString()+"'");
          }
          bu_ty.run(i->ti());
          bu_ty.run(i->e());
          if (i->e() && !i->e()->type().isSubtypeOf(i->ti()->type()))
            throw TypeError(i->e()->loc(), "return type of function does not match body, declared type is `"+i->ti()->type().toString()+
                            "', body type is `"+i->e()->type().toString()+"'");
        }
      } _tsv2(bu_ty, typeErrors);
      iterItems(_tsv2,m);
    }
    
    class TSV3 : public ItemVisitor {
    public:
      Model* m;
      TSV3(Model* m0) : m(m0) {}
      void vAssignI(AssignI* i) {
        i->decl()->e(addCoercion(m, i->e(), i->decl()->type())());
      }
    } _tsv3(m);
    iterItems(_tsv3,m);

    for (unsigned int i=0; i<ts.decls.size(); i++) {
      if (ts.decls[i]->toplevel() &&
          ts.decls[i]->type().ispar() && !ts.decls[i]->type().isann() && ts.decls[i]->e()==NULL) {
        if (ts.decls[i]->type().isopt()) {
          ts.decls[i]->e(constants().absent);
        } else if (!ignoreUndefinedParameters) {
          typeErrors.push_back(TypeError(ts.decls[i]->loc(),
                                         "  symbol error: variable `" + ts.decls[i]->id()->str().str()
                                         + "' must be defined (did you forget to specify a data file?)"));
        }
      }
    }

  }
  
  void typecheck(Model* m, AssignI* ai) {
    std::vector<TypeError> typeErrors;
    Typer<true> ty(m, typeErrors);
    BottomUpIterator<Typer<true> > bu_ty(ty);
    bu_ty.run(ai->e());
    if (!typeErrors.empty()) {
      throw typeErrors[0];
    }
    if (!ai->e()->type().isSubtypeOf(ai->decl()->ti()->type())) {
      throw TypeError(ai->e()->loc(),
                      "assignment value for `"+ai->decl()->id()->str().str()+"' has invalid type-inst: expected `"+
                      ai->decl()->ti()->type().toString()+"', actual `"+ai->e()->type().toString()+"'");
    }
    
  }

  void typecheck_fzn(Model* m) {
    ASTStringMap<int>::t declMap;
    for (unsigned int i=0; i<m->size(); i++) {
      if (VarDeclI* vdi = (*m)[i]->dyn_cast<VarDeclI>()) {
        Type t = vdi->e()->type();
        declMap.insert(std::pair<ASTString,int>(vdi->e()->id()->v(), i));
        if (t.isunknown()) {
          if (vdi->e()->ti()->domain()) {
            switch (vdi->e()->ti()->domain()->eid()) {
              case Expression::E_BINOP:
              {
                BinOp* bo = vdi->e()->ti()->domain()->cast<BinOp>();
                if (bo->op()==BOT_DOTDOT) {
                  t.bt(bo->lhs()->type().bt());
                  if (t.isunknown()) {
                    throw TypeError(vdi->e()->loc(), "Cannot determine type of variable declaration");
                  }
                  vdi->e()->type(t);
                } else {
                  throw TypeError(vdi->e()->loc(), "Only ranges allowed in FlatZinc type inst");
                }
              }
              case Expression::E_ID:
              {
                ASTStringMap<int>::t::iterator it = declMap.find(vdi->e()->ti()->domain()->cast<Id>()->v());
                if (it == declMap.end()) {
                  throw TypeError(vdi->e()->loc(), "Cannot determine type of variable declaration");
                }
                t.bt((*m)[it->second]->cast<VarDeclI>()->e()->type().bt());
                if (t.isunknown()) {
                  throw TypeError(vdi->e()->loc(), "Cannot determine type of variable declaration");
                }
                vdi->e()->type(t);
              }
              default:
                throw TypeError(vdi->e()->loc(), "Cannot determine type of variable declaration");
            }
          } else {
            throw TypeError(vdi->e()->loc(), "Cannot determine type of variable declaration");
          }
        }
      }
    }
  }
  
}
