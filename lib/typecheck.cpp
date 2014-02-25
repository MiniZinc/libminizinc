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
#include <minizinc/exception.hh>

#include <string>
#include <sstream>

namespace MiniZinc {
  
  struct VarDeclCmp {
    std::unordered_map<VarDecl*,int>& _pos;
    VarDeclCmp(std::unordered_map<VarDecl*,int>& pos) : _pos(pos) {}
    bool operator()(Expression* e0, Expression* e1) {
      if (VarDecl* vd0 = e0->dyn_cast<VarDecl>()) {
        if (VarDecl* vd1 = e1->dyn_cast<VarDecl>()) {
          return _pos.at(vd0) < _pos.at(vd1);
        } else {
          return true;
        }
      } else {
        return false;
      }
    }
  };
  struct ItemCmp {
    std::unordered_map<VarDecl*,int>& _pos;
    ItemCmp(std::unordered_map<VarDecl*,int>& pos) : _pos(pos) {}
    bool operator()(Item* i0, Item* i1) {
      if (VarDeclI* vd0 = i0->cast<VarDeclI>()) {
        if (VarDeclI* vd1 = i1->cast<VarDeclI>()) {
          return _pos.at(vd0->e()) < _pos.at(vd1->e());
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
    DeclMap::iterator vdi = env.find(vd->id()->v());
    if (vdi == env.end()) {
      Decls nd; nd.push_back(vd);
      env.insert(std::pair<ASTString,Decls>(vd->id()->v(),nd));
    } else {
      if (unique)
        throw TypeError(vd->loc(),"identifier `"+vd->id()->v().str()+
                        "' already defined");
      vdi->second.push_back(vd);
    }
  }
  void
  TopoSorter::remove(VarDecl* vd) {
    DeclMap::iterator vdi = env.find(vd->id()->v());
    assert(vdi != env.end());
    vdi->second.pop_back();
    if (vdi->second.empty())
      env.erase(vdi);
  }
  
  VarDecl*
  TopoSorter::checkId(const ASTString& id, const Location& loc) {
    DeclMap::iterator decl = env.find(id);
    if (decl==env.end()) {
      throw TypeError(loc,"undefined identifier "+id.str());
    }
    PosMap::iterator pi = pos.find(decl->second.back());
    if (pi==pos.end()) {
      // new id
      run(decl->second.back());
    } else {
      // previously seen, check if circular
      if (pi->second==-1)
        throw TypeError(loc,"circular definition of "+id.str());
    }
    return decl->second.back();
  }
  
  void
  TopoSorter::run(Expression* e) {
    if (e==NULL)
      return;
    if (e->ann())
      run(e->ann());
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
        e->cast<Id>()->decl(checkId(e->cast<Id>()->v(),e->loc()));
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
        for (unsigned int i=0; i<ce->n_generators(); i++) {
          run(ce->in(i));
          for (unsigned int j=0; j<ce->n_decls(i); j++) {
            add(ce->decl(i,j), false);
          }
        }
        if (ce->where())
          run(ce->where());
        run(ce->e());
        for (unsigned int i=0; i<ce->n_generators(); i++) {
          for (unsigned int j=0; j<ce->n_decls(i); j++) {
            remove(ce->decl(i,j));
          }
        }
      }
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        for (unsigned int i=0; i<ite->size(); i++) {
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
          decls.push_back(ve);
          pi = pos.find(ve);
          pi->second = decls.size()-1;
        } else {
          assert(pi->second != -1);
        }
      }
      break;
    case Expression::E_ANN:
      {
        Annotation* ann = e->cast<Annotation>();
        run(ann->e());
        run(ann->next());
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
  
  template<bool ignoreVarDecl>
  class Typer {
  public:
    Model* _model;
    Typer(Model* model) : _model(model) {}
    bool enter(Expression* e) {
      return ignoreVarDecl || (!e->isa<Annotation>());
    }
    /// Visit integer literal
    void vIntLit(const IntLit&) {}
    /// Visit floating point literal
    void vFloatLit(const FloatLit&) {}
    /// Visit Boolean literal
    void vBoolLit(const BoolLit&) {}
    /// Visit set literal
    void vSetLit(SetLit& sl) {
      Type ty; ty._st = Type::ST_SET;
      for (unsigned int i=0; i<sl.v().size(); i++) {
        if (sl.v()[i]->type().isvar())
          ty._ti = Type::TI_VAR;
        if (ty._bt!=sl.v()[i]->type()._bt) {
          if (ty._bt!=Type::BT_UNKNOWN)
            throw TypeError(sl.loc(),"non-uniform set literal");
          ty._bt = sl.v()[i]->type()._bt;
        }
      }
      if (ty._bt == Type::BT_UNKNOWN)
        ty._bt = Type::BT_BOT;
      sl.type(ty);
    }
    /// Visit string literal
    void vStringLit(const StringLit&) {}
    /// Visit identifier
    void vId(Id& id) {
      assert(!id.decl()->type().isunknown());
      id.type(id.decl()->type());
    }
    /// Visit anonymous variable
    void vAnonVar(const AnonVar&) {}
    /// Visit array literal
    void vArrayLit(ArrayLit& al) {
      Type ty; ty._dim = al.dims();
      std::vector<AnonVar*> anons;
      for (unsigned int i=0; i<al.v().size(); i++) {
        Expression* vi = al.v()[i];
        if (AnonVar* av = vi->dyn_cast<AnonVar>()) {
          ty._ti = Type::TI_VAR;
          anons.push_back(av);
        } else if (vi->type().isvar()) {
          ty._ti = Type::TI_VAR;
        }
        if (vi->type().isopt())
          ty._ot = Type::OT_OPTIONAL;
        if (vi->type().isbot()) {
          // do nothing
        } else if (ty._bt==Type::BT_UNKNOWN) {
          ty._bt = vi->type()._bt;
          assert(ty._bt != Type::BT_UNKNOWN);
          ty._st = vi->type()._st;
        } else if (ty._bt != vi->type()._bt ||
                   ty._st != vi->type()._st) {
          throw TypeError(al.loc(),"non-uniform array literal");
        }
      }
      if (ty._bt == Type::BT_UNKNOWN) {
        ty._bt = Type::BT_BOT;
        if (!anons.empty())
          throw TypeError(al.loc(),"array literal must contain at least one non-anonymous variable");
      } else {
        Type at = ty;
        at._dim = 0;
        for (unsigned int i=0; i<anons.size(); i++) {
          anons[i]->type(at);
        }
      }
      al.type(ty);
    }
    /// Visit array access
    void vArrayAccess(ArrayAccess& aa) {
      if (aa.v()->type()._dim==0)
        throw TypeError(aa.v()->loc(),"not an array in array access");
      if (aa.v()->type()._dim != aa.idx().size())
        throw TypeError(aa.v()->loc(),"array dimensions do not match");
      bool allpar=true;
      bool allpresent=true;
      for (unsigned int i=0; i<aa.idx().size(); i++) {
        Expression* aai = aa.idx()[i];
        if (aai->type().isset() || aai->type()._bt != Type::BT_INT ||
            aai->type()._dim != 0) {
          throw TypeError(aai->loc(),"array index must be int");
        }
        if (aai->type().isopt()) {
          allpresent = false;
        }
        if (aai->type().isvar()) {
          allpar=false;
        }
      }
      Type tt = aa.v()->type();
      tt._dim = 0;
      if (!allpar)
        tt._ti = Type::TI_VAR;
      if (!allpresent)
        tt._ot = Type::OT_OPTIONAL;
      aa.type(tt);
    }
    /// Visit array comprehension
    void vComprehension(Comprehension& c) {
      for (unsigned int i=0; i<c.n_generators(); i++) {
        Expression* g_in = c.in(i);
        const Type& ty_in = g_in->type();
        if (ty_in != Type::parsetint() && !ty_in.isintarray()) {
          throw TypeError(g_in->loc(),
            "generator expression must be par set of int or array[int] of int, but is "+ty_in.toString());
        }
      }
      if (c.where() && c.where()->type() != Type::parbool()) {
        throw TypeError(c.where()->loc(),
                        "where clause must be par bool, but is "+
                        c.where()->type().toString());
        
      }
      Type tt = c.e()->type();
      if (c.set()) {
        if (c.e()->type()._dim != 0 || c.e()->type()._st == Type::ST_SET)
          throw TypeError(c.e()->loc(),
              "set comprehension expression must be scalar, but is "
              +c.e()->type().toString());
        tt._st = Type::ST_SET;
      } else {
        if (c.e()->type()._dim != 0)
          throw TypeError(c.e()->loc(),
            "array comprehension expression cannot be an array");
        tt._dim = 1;
      }
      c.type(tt);
    }
    /// Visit if-then-else
    void vITE(ITE& ite) {
      Type tret = ite.e_else()->type();
      bool allpar = !(tret.isvar());
      bool varcond = false;
      for (unsigned int i=0; i<ite.size(); i++) {
        Expression* eif = ite.e_if(i);
        Expression* ethen = ite.e_then(i);
        varcond = varcond || (eif->type() == Type::varbool());
        if (eif->type() != Type::parbool() && eif->type() != Type::varbool())
          throw TypeError(eif->loc(),
            "expected bool conditional expression, got\n  "+
            eif->type().toString());
        if (tret.isbot()) {
          tret._bt = ethen->type()._bt;
        }
        if ( (!ethen->type().isbot() && ethen->type()._bt != tret._bt) ||
            ethen->type()._st != tret._st ||
            ethen->type()._dim != tret._dim) {
          throw TypeError(ethen->loc(),
            "type mismatch in branches of conditional. Then-branch has type "+
            ethen->type().toString()+", but else branch has type "+
            tret.toString());
        }
        if (ethen->type().isvar()) allpar=false;
      }
      /// TODO: perhaps extend flattener to array types, but for now throw an error
      if (varcond && tret.dim() > 0)
        throw TypeError(ite.loc(), "conditional with var condition cannot have array type");
      if (!allpar)
        tret._ti = Type::TI_VAR;
      ite.type(tret);
    }
    /// Visit binary operator
    void vBinOp(BinOp& bop) {
      std::vector<Expression*> args(2);
      args[0] = bop.lhs(); args[1] = bop.rhs();
      if (bop.op()==BOT_PLUSPLUS &&
        bop.lhs()->type()._dim==1 && bop.rhs()->type()._dim==1 &&
        bop.lhs()->type()._st==bop.rhs()->type()._st &&
        bop.lhs()->type()._bt==bop.rhs()->type()._bt) {
        if (bop.lhs()->type().isvar())
          bop.type(bop.lhs()->type());
        else
          bop.type(bop.rhs()->type());
      } else {
        if (FunctionI* fi = _model->matchFn(bop.opToString(),args)) {
          bop.type(fi->rtype(args));
          if (fi->e())
            bop.decl(fi);
          else
            bop.decl(NULL);
        } else {
          throw TypeError(bop.loc(),
            std::string("type error in operator application for ")+
            bop.opToString().str());
        }
      }
    }
    /// Visit unary operator
    void vUnOp(UnOp& uop) {
      std::vector<Expression*> args(1);
      args[0] = uop.e();
      if (FunctionI* fi = _model->matchFn(uop.opToString(),args)) {
        uop.type(fi->rtype(args));
        if (fi->e())
          uop.decl(fi);
      } else {
        throw TypeError(uop.loc(),
          std::string("type error in operator application for ")+
          uop.opToString().str());
      }
    }
    /// Visit call
    void vCall(Call& call) {
      std::vector<Expression*> args(call.args().size());
      std::copy(call.args().begin(),call.args().end(),args.begin());
      if (FunctionI* fi = _model->matchFn(call.id(),args)) {
        call.type(fi->rtype(args));
        call.decl(fi);
      } else {
        std::ostringstream oss;
        oss << "no function or predicate with this signature found: ";
        oss << call.id() << "(";
        for (unsigned int i=0; i<call.args().size(); i++) {
          oss << call.args()[i]->type().toString();
          if (i<call.args().size()-1) oss << ",";
        }
        oss << ")";
        throw TypeError(call.loc(), oss.str());
      }
    }
    /// Visit let
    void vLet(Let& let) {
      for (unsigned int i=0; i<let.let().size(); i++) {
        Expression* li = let.let()[i];
        if (VarDecl* vdi = li->dyn_cast<VarDecl>()) {
          if (vdi->type().ispar() && vdi->e() == NULL)
            throw TypeError(vdi->loc(),
              "let variable `"+vdi->id()->v().str()+"' must be defined");
        }
      }
      let.type(let.in()->type());
    }
    /// Visit annotation
    void vAnnotation(Annotation& ann) {}
    /// Visit variable declaration
    void vVarDecl(VarDecl& vd) {
      if (ignoreVarDecl) {
        assert(!vd.type().isunknown());
        if (vd.e()) {
          if (! vd.e()->type().isSubtypeOf(vd.ti()->type()))
            throw TypeError(vd.loc(),
              "type error in initialization, LHS is\n  "+
              vd.ti()->type().toString()+"\nbut RHS is\n  "+
              vd.e()->type().toString());
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
          if (ri->type() == Type::top()) {
            if (foundTIId) {
              throw TypeError(ri->loc(),
                "only one type-inst variable allowed in array index");
            } else {
              foundTIId = true;
            }
          } else if (ri->type() != Type::parint()) {
            assert(ri->isa<TypeInst>());
            std::cerr << "expected set of int for array index, but got " <<
              ri->type().toString() << "\n";
            assert(false);
            throw TypeError(ri->loc(),
              "expected set of int for array index, but got\n"+
              ri->type().toString());
          }
        }
        tt._dim = foundTIId ? -1 : ti.ranges().size();
      }
      if (ti.domain() && !ti.domain()->isa<TIId>()) {
        if (ti.domain()->type()._ti != Type::TI_PAR ||
            ti.domain()->type()._st != Type::ST_SET)
          throw TypeError(ti.domain()->loc(),
            "type-inst must be par set");
        if (ti.domain()->type()._dim != 0)
          throw TypeError(ti.domain()->loc(),
            "type-inst cannot be an array");
      }
      if (tt.isunknown()) {
        assert(ti.domain());
        switch (ti.domain()->type()._bt) {
        case Type::BT_INT:
        case Type::BT_FLOAT:
          break;
        case Type::BT_BOT:
          {
            Type tidt = ti.domain()->type();
            tidt._bt = Type::BT_INT;
            ti.domain()->type(tidt);
          }
          break;
        default:
          throw TypeError(ti.domain()->loc(),
            "type-inst must be int or float");
        }
        tt._bt = ti.domain()->type()._bt;
      } else {
//        assert(ti.domain()==NULL || ti.domain()->isa<TIId>());
      }
      ti.type(tt);
    }
    void vTIId(TIId& id) {}
  };
  
  void typecheck(Model* m) {
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
        ts.run(i->e());
        i->decl(ts.checkId(i->id(),i->loc()));
        if (i->decl()->e())
          throw TypeError(i->loc(),"multiple assignment to same variable");
        i->decl()->e(i->e());
      }
      void vConstraintI(ConstraintI* i) { ts.run(i->e()); }
      void vSolveI(SolveI* i) { ts.run(i->ann()); ts.run(i->e()); }
      void vOutputI(OutputI* i) { ts.run(i->e()); }
      void vFunctionI(FunctionI* fi) {
        ts.run(fi->ti());
        for (unsigned int i=0; i<fi->params().size(); i++)
          ts.run(fi->params()[i]);
        ts.run(fi->ann());
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
      Typer<false> ty(m);
      BottomUpIterator<Typer<false> > bu_ty(ty);
      for (unsigned int i=0; i<ts.decls.size(); i++)
        bu_ty.run(ts.decls[i]);
      for (unsigned int i=0; i<functionItems.size(); i++) {
        bu_ty.run(functionItems[i]->ti());
        for (unsigned int j=0; j<functionItems[i]->params().size(); j++)
          bu_ty.run(functionItems[i]->params()[j]);
      }
    }
    
    {
      Typer<true> ty(m);
      BottomUpIterator<Typer<true> > bu_ty(ty);
      
      class TSV2 : public ItemVisitor {
      public:
        BottomUpIterator<Typer<true> >& bu_ty;
        TSV2(BottomUpIterator<Typer<true> >& b) : bu_ty(b) {}
        void vVarDeclI(VarDeclI* i) { bu_ty.run(i->e()); }
        void vAssignI(AssignI* i) {
          bu_ty.run(i->e());
          if (!i->e()->type().isSubtypeOf(i->decl()->ti()->type())) {
            throw TypeError(i->e()->loc(),
              "RHS of assignment does not agree with LHS");
          }
        }
        void vConstraintI(ConstraintI* i) {
          bu_ty.run(i->e());
          if (!i->e()->type().isSubtypeOf(Type::varbool()))
            throw TypeError(i->e()->loc(), "constraint must be var bool");
        }
        void vSolveI(SolveI* i) {
          bu_ty.run(i->ann());
          bu_ty.run(i->e());
          if (i->e()) {
            Type et = i->e()->type();
            if (! (et.isSubtypeOf(Type::varint()) || 
                   et.isSubtypeOf(Type::varfloat())))
              throw TypeError(i->e()->loc(),
                "objective must be int or float");
          }
        }
        void vOutputI(OutputI* i) {
          bu_ty.run(i->e());
          if (i->e()->type() != Type::parstring(1))
            throw TypeError(i->e()->loc(), "output item needs string array");
        }
        void vFunctionI(FunctionI* i) {
          bu_ty.run(i->ann());
          bu_ty.run(i->ti());
          bu_ty.run(i->e());
          if (i->e() && !i->e()->type().isSubtypeOf(i->ti()->type()))
            throw TypeError(i->e()->loc(), "return type of function does not match body");
        }
      } _tsv2(bu_ty);
      iterItems(_tsv2,m);
    }
    
  }
  
}
