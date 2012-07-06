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
          return _pos.at(vd0->_e) < _pos.at(vd1->_e);
        } else {
          return true;
        }
      } else {
        return false;
      }
    }
  };
  
  class TopoSorter {
  public:
    typedef std::vector<VarDecl*> Decls;
    typedef std::unordered_map<CtxStringH,Decls> DeclMap;
    typedef std::unordered_map<VarDecl*,int> PosMap;
    
    Decls decls;
    DeclMap env;
    PosMap pos;
    
    void add(VarDecl* vd, bool unique) {
      DeclMap::iterator vdi = env.find(vd->_id);
      if (vdi == env.end()) {
        Decls nd; nd.push_back(vd);
        env.insert(std::pair<CtxStringH,Decls>(vd->_id,nd));
      } else {
        if (unique)
          throw TypeError(vd->_loc,"identifier `"+vd->_id.str()+
                          "' already defined");
        vdi->second.push_back(vd);
      }
    }
    void remove(VarDecl* vd) {
      DeclMap::iterator vdi = env.find(vd->_id);
      assert(vdi != env.end());
      vdi->second.pop_back();
      if (vdi->second.empty())
        env.erase(vdi);
    }
    
    void run(Expression* e) {
      if (e==NULL)
        return;
      switch (e->_eid) {
      case Expression::E_INTLIT:
      case Expression::E_FLOATLIT:
      case Expression::E_BOOLLIT:
      case Expression::E_STRINGLIT:
      case Expression::E_ANON:
        break;
      case Expression::E_SETLIT:
        {
          SetLit* sl = e->cast<SetLit>();
          for (unsigned int i=0; i<sl->_v->size(); i++)
            run((*sl->_v)[i]);
        }
        break;
      case Expression::E_ID:
        {
          Id* ie = e->cast<Id>();
          DeclMap::iterator decl = env.find(ie->_v);
          if (decl==env.end()) {
            throw TypeError(e->_loc,"undefined identifier "+ie->_v.str());
          }
          PosMap::iterator pi = pos.find(decl->second.back());
          if (pi==pos.end()) {
            // new id
            run(decl->second.back());
          } else {
            // previously seen, check if circular
            if (pi->second==-1)
              throw TypeError(e->_loc,"circular definition of "+ie->_v.str());
          }
          ie->_decl = decl->second.back();
        }
        break;
      case Expression::E_ARRAYLIT:
        {
          ArrayLit* al = e->cast<ArrayLit>();
          for (unsigned int i=0; i<al->_v->size(); i++)
            run((*al->_v)[i]);
        }
        break;
      case Expression::E_ARRAYACCESS:
        {
          ArrayAccess* ae = e->cast<ArrayAccess>();
          run(ae->_v);
          for (unsigned int i=0; i<ae->_idx->size(); i++)
            run((*ae->_idx)[i]);
        }
        break;
      case Expression::E_COMP:
        {
          Comprehension* ce = e->cast<Comprehension>();
          for (unsigned int i=0; i<ce->_g->size(); i++)
            for (unsigned int j=0; j<(*ce->_g)[i]->_v->size(); j++)
              add((*(*ce->_g)[i]->_v)[j],false);
          for (unsigned int i=0; i<ce->_g->size(); i++)
            run((*ce->_g)[i]->_in);
          if (ce->_where)
            run(ce->_where);
          run(ce->_e);
          for (unsigned int i=0; i<ce->_g->size(); i++)
            for (unsigned int j=0; j<(*ce->_g)[i]->_v->size(); j++)
              remove((*(*ce->_g)[i]->_v)[j]);
        }
        break;
      case Expression::E_ITE:
        {
          ITE* ite = e->cast<ITE>();
          for (unsigned int i=0; i<ite->_e_if->size(); i++) {
            run((*ite->_e_if)[i].first);
            run((*ite->_e_if)[i].second);
          }
          run(ite->_e_else);
        }
        break;
      case Expression::E_BINOP:
        {
          BinOp* be = e->cast<BinOp>();
          run(be->_e0);
          run(be->_e1);
        }
        break;
      case Expression::E_UNOP:
        {
          UnOp* ue = e->cast<UnOp>();
          run(ue->_e0);
        }
        break;
      case Expression::E_CALL:
        {
          Call* ce = e->cast<Call>();
          for (unsigned int i=0; i<ce->_args->size(); i++)
            run((*ce->_args)[i]);
        }
        break;
      case Expression::E_VARDECL:
        {
          VarDecl* ve = e->cast<VarDecl>();
          PosMap::iterator pi = pos.find(ve);
          if (pi==pos.end()) {
            pos.insert(std::pair<VarDecl*,int>(ve,-1));
            run(ve->_ti);
            run(ve->_e);
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
          run(ann->_e);
          run(ann->_a);
        }
        break;
      case Expression::E_TI:
        {
          TypeInst* ti = e->cast<TypeInst>();
          if (ti->_ranges)
            for (unsigned int i=0; i<ti->_ranges->size(); i++)
              run((*ti->_ranges)[i]);
          run(ti->_domain);
        }
        break;
      case Expression::E_LET:
        {
          Let* let = e->cast<Let>();
          for (unsigned int i=0; i<let->_let->size(); i++) {
            if (VarDecl* vd = (*let->_let)[i]->dyn_cast<VarDecl>()) {
              add(vd,false);
            }
          }
          for (unsigned int i=0; i<let->_let->size(); i++) {
            run((*let->_let)[i]);
          }
          run(let->_in);
          VarDeclCmp poscmp(pos);
          std::stable_sort(let->_let->begin(), let->_let->end(), poscmp);
          for (unsigned int i=0; i<let->_let->size(); i++) {
            if (VarDecl* vd = (*let->_let)[i]->dyn_cast<VarDecl>()) {
              remove(vd);
            }
          }
        }
        break;
      }
    }
  };
  
  template<bool ignoreVarDecl>
  class Typer {
  public:
    ASTContext& _ctx;
    Typer(ASTContext& ctx) : _ctx(ctx) {}
    /// Visit integer literal
    void vIntLit(const IntLit&) {}
    /// Visit floating point literal
    void vFloatLit(const FloatLit&) {}
    /// Visit Boolean literal
    void vBoolLit(const BoolLit&) {}
    /// Visit set literal
    void vSetLit(SetLit& sl) {
      Type ty; ty._st = Type::ST_SET;
      for (unsigned int i=0; i<sl._v->size(); i++) {
        if ((*sl._v)[i]->_type.isvar())
          ty._ti = Type::TI_VAR;
        if (ty._bt!=(*sl._v)[i]->_type._bt) {
          if (ty._bt!=Type::BT_UNKNOWN)
            throw TypeError(sl._loc,"non-uniform set literal");
          ty._bt = (*sl._v)[i]->_type._bt;
        }
      }
      if (ty._bt == Type::BT_UNKNOWN)
        ty._bt = Type::BT_BOT;
      sl._type = ty;
    }
    /// Visit string literal
    void vStringLit(const StringLit&) {}
    /// Visit identifier
    void vId(Id& id) {
      assert(!id._decl->_type.isunknown());
      id._type = id._decl->_type;
    }
    /// Visit anonymous variable
    void vAnonVar(const AnonVar&) {}
    /// Visit array literal
    void vArrayLit(ArrayLit& al) {
      Type ty; ty._dim = al._dims->size();
      for (unsigned int i=0; i<al._v->size(); i++) {
        Expression* vi = (*al._v)[i];
        if (vi->_type.isvar() || vi->_type.isany())
          ty._ti = Type::TI_VAR;
        if (ty._bt==Type::BT_UNKNOWN) {
          ty._bt = (*al._v)[i]->_type._bt;
          assert(ty._bt != Type::BT_UNKNOWN);
          ty._st = (*al._v)[i]->_type._st;
        } else if (ty._bt != (*al._v)[i]->_type._bt ||
                   ty._st != (*al._v)[i]->_type._st) {
          throw TypeError(al._loc,"non-uniform array literal");
        }
      }
      if (ty._bt == Type::BT_UNKNOWN)
        ty._bt = Type::BT_BOT;
      al._type = ty;
    }
    /// Visit array access
    void vArrayAccess(ArrayAccess& aa) {
      if (aa._v->_type._dim==0)
        throw TypeError(aa._v->_loc,"not an array in array access");
      if (aa._v->_type._dim != aa._idx->size())
        throw TypeError(aa._v->_loc,"array dimensions do not match");
      bool allpar=true;
      for (unsigned int i=0; i<aa._idx->size(); i++) {
        Expression* aai = (*aa._idx)[i];
        if (aai->_type==Type::varint() || aai->_type==Type::any()) {
          allpar=false;
        } else if (aai->_type!=Type::parint()) {
          throw TypeError(aai->_loc,"array index must be int");
        }
      }
      aa._type = aa._v->_type;
      aa._type._dim = 0;
      if (!allpar)
        aa._type._ti = Type::TI_VAR;
    }
    /// Visit array comprehension
    void vComprehension(Comprehension& c) {
      for (unsigned int i=0; i<c._g->size(); i++) {
        const Type& ty_in = (*c._g)[i]->_in->_type;
        if (ty_in != Type::parsetint() && !ty_in.isintarray()) {
          throw TypeError((*c._g)[i]->_in->_loc,
            "generator expression must be par set of int or array[int] of int, but is "+ty_in.toString());
        }
      }
      if (c._where && c._where->_type != Type::parbool()) {
        throw TypeError(c._where->_loc,
                        "where clause must be par bool, but is "+
                        c._where->_type.toString());
        
      }
      c._type = c._e->_type;
      if (c._set) {
        if (c._e->_type._dim != 0 || c._e->_type._st == Type::ST_SET)
          throw TypeError(c._e->_loc,
              "set comprehension expression must be scalar, but is "
              +c._e->_type.toString());
        c._type._st = Type::ST_SET;
      } else {
        if (c._e->_type._dim != 0)
          throw TypeError(c._e->_loc,
            "array comprehension expression cannot be an array");
        c._type._dim = 1;
      }
    }
    /// Visit if-then-else
    void vITE(ITE& ite) {
      Type& telse = ite._e_else->_type;
      bool allpar = !(telse.isvar());
      for (unsigned int i=0; i<ite._e_if->size(); i++) {
        Expression* eif = (*ite._e_if)[i].first;
        Expression* ethen = (*ite._e_if)[i].second;
        if (eif->_type != Type::parbool())
          throw TypeError(eif->_loc,
            "expected par bool conditional expression, got\n  "+
            eif->_type.toString());
        if (ethen->_type._bt != telse._bt ||
            ethen->_type._st != telse._st ||
            ethen->_type._dim != telse._dim) {
          throw TypeError(ethen->_loc,
            "type mismatch in branches of conditional. Then-branch has type "+
            ethen->_type.toString()+", but else branch has type "+
            telse.toString());
        }
        if (ethen->_type.isvar()) allpar=false;
      }
      ite._type = telse;
      if (!allpar) ite._type._ti = Type::TI_VAR;
    }
    /// Visit binary operator
    void vBinOp(BinOp& bop) {
      std::vector<Expression*> args(2);
      args[0] = bop._e0; args[1] = bop._e1;
      if (FunctionI* fi = _ctx.matchFn(bop.opToString(),args)) {
        bop._type = fi->_ti->_type;
      } else if (bop._op==BOT_PLUSPLUS &&
        bop._e0->_type._dim==1 && bop._e1->_type._dim==1 &&
        bop._e0->_type._st==bop._e1->_type._st &&
        bop._e0->_type._bt==bop._e1->_type._bt) {
        if (bop._e0->_type.isvar())
          bop._type = bop._e0->_type;
        else
          bop._type = bop._e1->_type;
      } else {
        throw TypeError(bop._loc,
          std::string("type error in operator application for ")+
          bop.opToString().str());
      }
    }
    /// Visit unary operator
    void vUnOp(UnOp& uop) {
      std::vector<Expression*> args(1);
      args[0] = uop._e0;
      if (FunctionI* fi = _ctx.matchFn(uop.opToString(),args)) {
        uop._type = fi->_ti->_type;
      } else {
        throw TypeError(uop._loc,
          std::string("type error in operator application for ")+
          uop.opToString().str());
      }
    }
    /// Visit call
    void vCall(Call& call) {
      std::vector<Expression*> args(call._args->size());
      for (unsigned int i=call._args->size(); i--;)
        args[i] = (*call._args)[i];
      if (FunctionI* fi = _ctx.matchFn(call._id,args)) {
        call._type = fi->_ti->_type;
        call._decl = fi;
      } else {
        throw TypeError(call._loc,
          "no function or predicate with this signature found");
      }
    }
    /// Visit let
    void vLet(Let& let) {
      for (unsigned int i=0; i<let._let->size(); i++) {
        Expression* li = (*let._let)[i];
        if (VarDecl* vdi = li->dyn_cast<VarDecl>()) {
          if (vdi->_type.ispar() && vdi->_e == NULL)
            throw TypeError(vdi->_loc,
              "let variable `"+vdi->_id.str()+"' must be defined");
        }
      }
      let._type = let._in->_type;
    }
    /// Visit annotation
    void vAnnotation(Annotation& ann) {}
    /// Visit variable declaration
    void vVarDecl(VarDecl& vd) {
      if (ignoreVarDecl) {
        assert(!vd._type.isunknown());
      } else {
        if (vd._e) {
          if (! vd._e->_type.isSubtypeOf(vd._ti->_type))
            throw TypeError(vd._loc,
              "type error in initialization, LHS is\n  "+
              vd._ti->_type.toString()+"\nbut RHS is\n  "+
              vd._e->_type.toString());
        }
        vd._type = vd._ti->_type;
      }
    }
    /// Visit type inst
    void vTypeInst(TypeInst& ti) {
      if (ti._ranges) {
        for (unsigned int i=0; i<ti._ranges->size(); i++) {
          Expression* ri = (*ti._ranges)[i];
          if (ri && ri->_type != Type::parsetint())
            throw TypeError(ri->_loc,
              "expected set of int for array index, but got\n"+
              ri->_type.toString());
        }
        ti._type._dim = ti._ranges->size();
      }
      if (ti._domain) {
        if (ti._domain->_type._ti != Type::TI_PAR ||
            ti._domain->_type._st != Type::ST_SET)
          throw TypeError(ti._domain->_loc,
            "type-inst must be par set");
        if (ti._domain->_type._dim != 0)
          throw TypeError(ti._domain->_loc,
            "type-inst cannot be an array");
      }
      if (ti._type.isunknown()) {
        assert(ti._domain);
        switch (ti._domain->_type._bt) {
        case Type::BT_INT:
        case Type::BT_FLOAT:
          break;
        case Type::BT_BOT:
          ti._domain->_type._bt = Type::BT_INT;
          break;
        default:
          throw TypeError(ti._domain->_loc,
            "type-inst must be int or float");
        }
        ti._type._bt = ti._domain->_type._bt;
      } else {
        assert(ti._domain==NULL);
      }
    }
  };
  
  void typecheck(ASTContext& ctx, Model* m) {
    TopoSorter ts;
    
    std::vector<Model*> models;
    models.push_back(m);
    while (!models.empty()) {
      Model* cm = models.back();
      models.pop_back();
      // Register types of all declared functions
      for (unsigned int i=0; i<cm->_items.size(); i++) {
        switch (cm->_items[i]->_iid) {
        case Item::II_INC:
          if (cm->_items[i]->cast<IncludeI>()->_own)
            models.push_back(cm->_items[i]->cast<IncludeI>()->_m);
          break;
        case Item::II_VD:
          ts.add(cm->_items[i]->cast<VarDeclI>()->_e,true);
          break;
        case Item::II_ASN:
          break;
        case Item::II_CON:
          break;
        case Item::II_SOL:
          break;
        case Item::II_OUT:
          break;
        case Item::II_FUN:
          ctx.registerFn(cm->_items[i]->cast<FunctionI>());
          break;      
        }
      }
    }
    
    models.push_back(m);
    while (!models.empty()) {
      Model* cm = models.back();
      models.pop_back();
      // Run toposort for all expressions
      for (unsigned int i=0; i<cm->_items.size(); i++) {
        switch (cm->_items[i]->_iid) {
        case Item::II_INC:
          if (cm->_items[i]->cast<IncludeI>()->_own)
            models.push_back(cm->_items[i]->cast<IncludeI>()->_m);
          break;
        case Item::II_VD:
          ts.run(cm->_items[i]->cast<VarDeclI>()->_e);
          break;
        case Item::II_ASN:
          ts.run(cm->_items[i]->cast<AssignI>()->_e);
          break;
        case Item::II_CON:
          ts.run(cm->_items[i]->cast<ConstraintI>()->_e);
          break;
        case Item::II_SOL:
          ts.run(cm->_items[i]->cast<SolveI>()->_ann);
          ts.run(cm->_items[i]->cast<SolveI>()->_e);
          break;
        case Item::II_OUT:
          ts.run(cm->_items[i]->cast<OutputI>()->_e);
          break;
        case Item::II_FUN:
          {
            FunctionI* fi = cm->_items[i]->cast<FunctionI>();
            ts.run(fi->_ti);
            for (unsigned int i=0; i<fi->_params->size(); i++)
              ts.run((*fi->_params)[i]);
            ts.run(fi->_ann);
            for (unsigned int i=0; i<fi->_params->size(); i++)
              ts.add((*fi->_params)[i],false);
            ts.run(fi->_e);
            for (unsigned int i=0; i<fi->_params->size(); i++)
              ts.remove((*fi->_params)[i]);
          }
          break;      
        }
      }
    }

    {
      Typer<false> ty(ctx);
      BottomUpIterator<Typer<false> > bu_ty(ty);
      for (TopoSorter::Decls::iterator it=ts.decls.begin(); it!=ts.decls.end(); 
           ++it)
        bu_ty.run(*it);
    }
    
    {
      Typer<true> ty(ctx);
      BottomUpIterator<Typer<true> > bu_ty(ty);
      models.push_back(m);
      while (!models.empty()) {
        Model* cm = models.back();
        models.pop_back();
        // Run type checking for all expressions
        for (unsigned int i=0; i<cm->_items.size(); i++) {
          switch (cm->_items[i]->_iid) {
          case Item::II_INC:
            if (cm->_items[i]->cast<IncludeI>()->_own)
              models.push_back(cm->_items[i]->cast<IncludeI>()->_m);
            break;
          case Item::II_VD:
            break;
          case Item::II_ASN:
            bu_ty.run(cm->_items[i]->cast<AssignI>()->_e);
            /// TODO: check assignment
            break;
          case Item::II_CON:
            bu_ty.run(cm->_items[i]->cast<ConstraintI>()->_e);
            if (!cm->_items[i]->cast<ConstraintI>()->
              _e->_type.isSubtypeOf(Type::varbool()))
              throw TypeError(cm->_items[i]->cast<ConstraintI>()->_e->_loc,
                "constraint must be var bool");
            break;
          case Item::II_SOL:
            {
              bu_ty.run(cm->_items[i]->cast<SolveI>()->_ann);
              bu_ty.run(cm->_items[i]->cast<SolveI>()->_e);
              if (cm->_items[i]->cast<SolveI>()->_e) {
                Type et = cm->_items[i]->cast<SolveI>()->_e->_type;
                if (! (et.isSubtypeOf(Type::varint()) || 
                       et.isSubtypeOf(Type::varfloat())))
                  throw TypeError(cm->_items[i]->cast<SolveI>()->_e->_loc,
                    "objective must be int or float");
              }
            }
            break;
          case Item::II_OUT:
            bu_ty.run(cm->_items[i]->cast<OutputI>()->_e);
            if (cm->_items[i]->cast<OutputI>()->_e->_type != Type::parstring(1))
              throw TypeError(cm->_items[i]->cast<OutputI>()->_e->_loc,
                "output item needs string array");
            break;
          case Item::II_FUN:
            {
              FunctionI* fi = cm->_items[i]->cast<FunctionI>();
              bu_ty.run(fi->_ti);
              for (unsigned int i=0; i<fi->_params->size(); i++)
                bu_ty.run((*fi->_params)[i]);
              bu_ty.run(fi->_ann);
              bu_ty.run(fi->_e);
            }
            break;      
          }
        }
      }
    }
    ctx.sortFn();
    
  }

  VarDecl* param(ASTContext& ctx, const std::string& id, const Type& type) {
    return VarDecl::a(ctx,Location::a(),TypeInst::a(ctx,Location::a(),type),id);
  }
  std::vector<VarDecl*> params(ASTContext& ctx, int n, const Type& type) {
    std::vector<VarDecl*> ret(n);
    for (unsigned int i=0; i<n; i++) {
      std::ostringstream oss;
      oss << "x" << i;
      ret[i] = param(ctx,oss.str(),type);
    }
    return ret;
  }
  void makeFn(ASTContext& ctx, const std::string& id,
              TypeInst* ret, int n, const Type& param) {
    ctx.registerFn(FunctionI::a(ctx,Location::a(),id,ret,params(ctx,n,param)));
  }

  void addOperatorTypes(ASTContext& ctx) {
    TypeInst* tparint = TypeInst::a(ctx,Location::a(),Type::parint());
    TypeInst* tvarint = TypeInst::a(ctx,Location::a(),Type::varint());
    TypeInst* tparfloat = TypeInst::a(ctx,Location::a(),Type::parfloat());
    TypeInst* tvarfloat = TypeInst::a(ctx,Location::a(),Type::varfloat());
    TypeInst* tparbool = TypeInst::a(ctx,Location::a(),Type::parbool());
    TypeInst* tvarbool = TypeInst::a(ctx,Location::a(),Type::varbool());
    TypeInst* tparstring = TypeInst::a(ctx,Location::a(),Type::parstring());    
    TypeInst* tparsetint = TypeInst::a(ctx,Location::a(),Type::parsetint());
    TypeInst* tvarsetint = TypeInst::a(ctx,Location::a(),Type::varsetint());
    TypeInst* tparsetfloat = TypeInst::a(ctx,Location::a(),Type::parsetfloat());
    TypeInst* tparsetbool = TypeInst::a(ctx,Location::a(),Type::parsetbool());
    TypeInst* tparsetstring = 
      TypeInst::a(ctx,Location::a(),Type::parsetstring());

    makeFn(ctx,"..",tparsetint,2,Type::parint());
    makeFn(ctx,"..",tparsetfloat,2,Type::parfloat());

    makeFn(ctx,"+",tparint,2,Type::parint());
    makeFn(ctx,"+",tvarint,2,Type::varint());
    makeFn(ctx,"+",tparfloat,2,Type::parfloat());
    makeFn(ctx,"+",tvarfloat,2,Type::varfloat());

    makeFn(ctx,"-",tparint,2,Type::parint());
    makeFn(ctx,"-",tvarint,2,Type::varint());
    makeFn(ctx,"-",tparfloat,2,Type::parfloat());
    makeFn(ctx,"-",tvarfloat,2,Type::varfloat());

    makeFn(ctx,"*",tparint,2,Type::parint());
    makeFn(ctx,"*",tvarint,2,Type::varint());
    makeFn(ctx,"*",tparfloat,2,Type::parfloat());
    makeFn(ctx,"*",tvarfloat,2,Type::varfloat());

    makeFn(ctx,"/",tparfloat,2,Type::parfloat());
    makeFn(ctx,"/",tvarfloat,2,Type::varfloat());

    makeFn(ctx,"div",tparint,2,Type::parint());
    makeFn(ctx,"div",tvarint,2,Type::varint());

    makeFn(ctx,"mod",tparint,2,Type::parint());
    makeFn(ctx,"mod",tvarint,2,Type::varint());

    makeFn(ctx,"<",tparbool,2,Type::parint());
    makeFn(ctx,"<",tvarbool,2,Type::varint());
    makeFn(ctx,"<",tparbool,2,Type::parfloat());
    makeFn(ctx,"<",tvarbool,2,Type::varfloat());
    makeFn(ctx,"<",tparbool,2,Type::parsetint());
    makeFn(ctx,"<",tvarbool,2,Type::varsetint());
    makeFn(ctx,"<",tparbool,2,Type::parbool());
    makeFn(ctx,"<",tvarbool,2,Type::varbool());

    makeFn(ctx,"<=",tparbool,2,Type::parint());
    makeFn(ctx,"<=",tvarbool,2,Type::varint());
    makeFn(ctx,"<=",tparbool,2,Type::parfloat());
    makeFn(ctx,"<=",tvarbool,2,Type::varfloat());
    makeFn(ctx,"<=",tparbool,2,Type::parsetint());
    makeFn(ctx,"<=",tvarbool,2,Type::varsetint());
    makeFn(ctx,"<=",tparbool,2,Type::parbool());
    makeFn(ctx,"<=",tvarbool,2,Type::varbool());

    makeFn(ctx,">",tparbool,2,Type::parint());
    makeFn(ctx,">",tvarbool,2,Type::varint());
    makeFn(ctx,">",tparbool,2,Type::parfloat());
    makeFn(ctx,">",tvarbool,2,Type::varfloat());
    makeFn(ctx,">",tparbool,2,Type::parsetint());
    makeFn(ctx,">",tvarbool,2,Type::varsetint());
    makeFn(ctx,">",tparbool,2,Type::parbool());
    makeFn(ctx,">",tvarbool,2,Type::varbool());

    makeFn(ctx,">=",tparbool,2,Type::parint());
    makeFn(ctx,">=",tvarbool,2,Type::varint());
    makeFn(ctx,">=",tparbool,2,Type::parfloat());
    makeFn(ctx,">=",tvarbool,2,Type::varfloat());
    makeFn(ctx,">=",tparbool,2,Type::parsetint());
    makeFn(ctx,">=",tvarbool,2,Type::varsetint());
    makeFn(ctx,">=",tparbool,2,Type::parbool());
    makeFn(ctx,">=",tvarbool,2,Type::varbool());

    makeFn(ctx,"=",tparbool,2,Type::parint());
    makeFn(ctx,"=",tvarbool,2,Type::varint());
    makeFn(ctx,"=",tparbool,2,Type::parfloat());
    makeFn(ctx,"=",tvarbool,2,Type::varfloat());
    makeFn(ctx,"=",tparbool,2,Type::parsetint());
    makeFn(ctx,"=",tvarbool,2,Type::varsetint());
    makeFn(ctx,"=",tparbool,2,Type::parbool());
    makeFn(ctx,"=",tvarbool,2,Type::varbool());

    makeFn(ctx,"!=",tparbool,2,Type::parint());
    makeFn(ctx,"!=",tvarbool,2,Type::varint());
    makeFn(ctx,"!=",tparbool,2,Type::parfloat());
    makeFn(ctx,"!=",tvarbool,2,Type::varfloat());
    makeFn(ctx,"!=",tparbool,2,Type::parsetint());
    makeFn(ctx,"!=",tvarbool,2,Type::varsetint());
    makeFn(ctx,"!=",tparbool,2,Type::parbool());
    makeFn(ctx,"!=",tvarbool,2,Type::varbool());

    {
      std::vector<VarDecl*> params(2);
      params[0] = param(ctx,"x0",Type::parint());
      params[1] = param(ctx,"x1",Type::parsetint());
      ctx.registerFn(FunctionI::a(ctx,Location::a(),"in",tparbool,params));
    }
    {
      std::vector<VarDecl*> params(2);
      params[0] = param(ctx,"x0",Type::parfloat());
      params[1] = param(ctx,"x1",Type::parsetfloat());
      ctx.registerFn(FunctionI::a(ctx,Location::a(),"in",tparbool,params));
    }
    {
      std::vector<VarDecl*> params(2);
      params[0] = param(ctx,"x0",Type::parbool());
      params[1] = param(ctx,"x1",Type::parsetbool());
      ctx.registerFn(FunctionI::a(ctx,Location::a(),"in",tparbool,params));
    }
    {
      std::vector<VarDecl*> params(2);
      params[0] = param(ctx,"x0",Type::parstring());
      params[1] = param(ctx,"x1",Type::parsetstring());
      ctx.registerFn(FunctionI::a(ctx,Location::a(),"in",tparbool,params));
    }
    {
      std::vector<VarDecl*> params(2);
      params[0] = param(ctx,"x0",Type::varint());
      params[1] = param(ctx,"x1",Type::varsetint());
      ctx.registerFn(FunctionI::a(ctx,Location::a(),"in",tvarbool,params));
    }

    
    makeFn(ctx,"subset",tvarbool,2,Type::varsetint());
    makeFn(ctx,"subset",tparbool,2,Type::parsetint());
    makeFn(ctx,"subset",tparbool,2,Type::parsetfloat());
    makeFn(ctx,"subset",tparbool,2,Type::parsetbool());
    makeFn(ctx,"subset",tparbool,2,Type::parsetstring());

    makeFn(ctx,"superset",tvarbool,2,Type::varsetint());
    makeFn(ctx,"superset",tparbool,2,Type::parsetint());
    makeFn(ctx,"superset",tparbool,2,Type::parsetfloat());
    makeFn(ctx,"superset",tparbool,2,Type::parsetbool());
    makeFn(ctx,"superset",tparbool,2,Type::parsetstring());

    makeFn(ctx,"union",tvarsetint,2,Type::varsetint());
    makeFn(ctx,"union",tparsetint,2,Type::parsetint());
    makeFn(ctx,"union",tparsetfloat,2,Type::parsetfloat());
    makeFn(ctx,"union",tparsetbool,2,Type::parsetbool());
    makeFn(ctx,"union",tparsetstring,2,Type::parsetstring());

    makeFn(ctx,"diff",tvarsetint,2,Type::varsetint());
    makeFn(ctx,"diff",tparsetint,2,Type::parsetint());
    makeFn(ctx,"diff",tparsetfloat,2,Type::parsetfloat());
    makeFn(ctx,"diff",tparsetbool,2,Type::parsetbool());
    makeFn(ctx,"diff",tparsetstring,2,Type::parsetstring());

    makeFn(ctx,"symdiff",tvarsetint,2,Type::varsetint());
    makeFn(ctx,"symdiff",tparsetint,2,Type::parsetint());
    makeFn(ctx,"symdiff",tparsetfloat,2,Type::parsetfloat());
    makeFn(ctx,"symdiff",tparsetbool,2,Type::parsetbool());
    makeFn(ctx,"symdiff",tparsetstring,2,Type::parsetstring());

    makeFn(ctx,"intersect",tvarsetint,2,Type::varsetint());
    makeFn(ctx,"intersect",tparsetint,2,Type::parsetint());
    makeFn(ctx,"intersect",tparsetfloat,2,Type::parsetfloat());
    makeFn(ctx,"intersect",tparsetbool,2,Type::parsetbool());
    makeFn(ctx,"intersect",tparsetstring,2,Type::parsetstring());

    makeFn(ctx,"++",tparstring,2,Type::parstring());
    
    makeFn(ctx,"<->",tparbool,2,Type::parbool());
    makeFn(ctx,"<->",tvarbool,2,Type::varbool());

    makeFn(ctx,"->",tparbool,2,Type::parbool());
    makeFn(ctx,"->",tvarbool,2,Type::varbool());

    makeFn(ctx,"<-",tparbool,2,Type::parbool());
    makeFn(ctx,"<-",tvarbool,2,Type::varbool());

    makeFn(ctx,"\\/",tparbool,2,Type::parbool());
    makeFn(ctx,"\\/",tvarbool,2,Type::varbool());

    makeFn(ctx,"/\\",tparbool,2,Type::parbool());
    makeFn(ctx,"/\\",tvarbool,2,Type::varbool());

    makeFn(ctx,"xor",tparbool,2,Type::parbool());
    makeFn(ctx,"xor",tvarbool,2,Type::varbool());
  }
  
}
