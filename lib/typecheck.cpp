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
#include <minizinc/flatten_internal.hh>

#include <string>
#include <sstream>
#include <unordered_map>

#include <minizinc/prettyprinter.hh>

namespace MiniZinc {
  
  Scopes::Scopes(void) {
    s.push_back(Scope());
    s.back().toplevel = true;
  }
  
  void
  Scopes::add(EnvI &env, VarDecl *vd) {
    if (!s.back().toplevel && vd->ti()->isEnum() && vd->e()) {
      throw TypeError(env, vd->loc(), "enums are only allowed at top level");
    }
    if (vd->id()->idn()==-1 && vd->id()->v()=="")
      return;
    DeclMap::iterator vdi = s.back().m.find(vd->id());
    if (vdi == s.back().m.end()) {
      s.back().m.insert(vd->id(),vd);
    } else {
      GCLock lock;
      throw TypeError(env, vd->loc(),"identifier `"+vd->id()->str().str()+
                      "' already defined");
    }
  }
  
  void
  Scopes::push(bool toplevel) {
    s.push_back(Scope());
    s.back().toplevel = toplevel;
  }
  
  void
  Scopes::pop(void) {
    s.pop_back();
  }
  
  VarDecl*
  Scopes::find(Id *ident) {
    int cur = static_cast<int>(s.size())-1;
    for (;;) {
      DeclMap::iterator vdi = s[cur].m.find(ident);
      if (vdi == s[cur].m.end()) {
        if (s[cur].toplevel) {
          if (cur > 0)
            cur = 0;
          else
            return NULL;
        } else {
          cur--;
        }
      } else {
        return vdi->second;
      }
    }
  }
  
  struct VarDeclCmp {
    std::unordered_map<VarDecl*,int>& _pos;
    VarDeclCmp(std::unordered_map<VarDecl*,int>& pos) : _pos(pos) {}
    bool operator()(Expression* e0, Expression* e1) {
      if (VarDecl* vd0 = Expression::dyn_cast<VarDecl>(e0)) {
        if (VarDecl* vd1 = Expression::dyn_cast<VarDecl>(e1)) {
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
    std::unordered_map<VarDecl*,int>& _pos;
    ItemCmp(std::unordered_map<VarDecl*,int>& pos) : _pos(pos) {}
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
  
  std::string createEnumToStringName(Id* ident, std::string prefix) {
    std::string name = ident->str().str();
    if (name[0]=='\'') {
      name = "'"+prefix+name.substr(1);
    } else {
      name = prefix+name;
    }
    return name;
  }
  
  AssignI* createEnumMapper(EnvI& env, Model* m, unsigned int enumId, VarDecl* vd, VarDecl* vd_enumToString, Model* enumItems) {

    Id* ident = vd->id();
    SetLit* sl = NULL;
    
    AssignI* ret = NULL;
    
    GCLock lock;
    if (Call* c = vd->e()->dyn_cast<Call>()) {
      if (c->id()!="anon_enum") {
        throw TypeError(env, c->loc(),
                        "invalid initialisation for enum `"+ident->v().str()+"'");
      }
    } else if ( (sl = vd->e()->dyn_cast<SetLit>()) ) {
      for (unsigned int i=0; i<sl->v().size(); i++) {
        if (!sl->v()[i]->isa<Id>()) {
          throw TypeError(env, sl->v()[i]->loc(),
                          "invalid initialisation for enum `"+ident->v().str()+"'");
        }
        TypeInst* ti_id = new TypeInst(sl->v()[i]->loc(),Type::parenum(enumId));
        
        std::vector<Expression*> toEnumArgs(2);
        toEnumArgs[0] = vd->id();
        toEnumArgs[1] = IntLit::a(i+1);
        Call* toEnum = new Call(sl->v()[i]->loc(), ASTString("to_enum"), toEnumArgs);
        toEnum->decl(env.model->matchFn(env, toEnum, false));
        VarDecl* vd_id = new VarDecl(ti_id->loc(),ti_id,sl->v()[i]->cast<Id>()->str(),toEnum);
        enumItems->addItem(new VarDeclI(vd_id->loc(),vd_id));
      }
      SetLit* nsl = new SetLit(vd->loc(), IntSetVal::a(1,sl->v().size()));
      Type tt = nsl->type();
      tt.enumId(vd->type().enumId());
      nsl->type(tt);
      vd->e(nsl);
    } else {
      throw TypeError(env, vd->e()->loc(),
                      "invalid initialisation for enum `"+ident->v().str()+"'");
    }

    
    if (sl) {
      std::string name = createEnumToStringName(ident,"_enum_to_string_");
      std::vector<Expression*> al_args(sl->v().size());
      for (unsigned int i=0; i<sl->v().size(); i++) {
        ASTString str = sl->v()[i]->cast<Id>()->str();
        al_args[i] = new StringLit(Location().introduce(), str);
        env.reverseEnum[str.str()] = i+1;
      }
      ArrayLit* al = new ArrayLit(Location().introduce(),al_args);
      
      if (vd_enumToString) {
        ret = new AssignI(Location().introduce(),name,al);
        ret->decl(vd_enumToString);
      } else {
        std::vector<TypeInst*> ranges(1);
        ranges[0] = new TypeInst(Location().introduce(),Type::parint());
        TypeInst* ti = new TypeInst(Location().introduce(),Type::parstring(1));
        ti->setRanges(ranges);
        vd_enumToString = new VarDecl(Location().introduce(),ti,name,al);
        enumItems->addItem(new VarDeclI(Location().introduce(),vd_enumToString));
      }
      
      Type tx = Type::parint();
      tx.ot(Type::OT_OPTIONAL);
      TypeInst* ti_aa = new TypeInst(Location().introduce(),tx);
      VarDecl* vd_aa = new VarDecl(Location().introduce(),ti_aa,"x");
      vd_aa->toplevel(false);
      TypeInst* ti_ab = new TypeInst(Location().introduce(),Type::parbool());
      VarDecl* vd_ab = new VarDecl(Location().introduce(),ti_ab,"b");
      vd_ab->toplevel(false);
      TypeInst* ti_fi = new TypeInst(Location().introduce(),Type::parstring());
      std::vector<VarDecl*> fi_params(2);
      fi_params[0] = vd_aa;
      fi_params[1] = vd_ab;

      std::vector<Expression*> deopt_args(1);
      deopt_args[0] = vd_aa->id();
      Call* deopt = new Call(Location().introduce(), "deopt", deopt_args);
      Call* occurs = new Call(Location().introduce(), "occurs", deopt_args);
      std::vector<Expression*> aa_args(1);
      aa_args[0] = deopt;
      ArrayAccess* aa = new ArrayAccess(Location().introduce(),vd_enumToString->id(),aa_args);

      StringLit* sl_absent = new StringLit(Location().introduce(),"<>");
      std::vector<Expression*> ite_ifelse(2);
      ite_ifelse[0] = occurs;
      ite_ifelse[1] = aa;
      ITE* ite = new ITE(Location().introduce(),ite_ifelse,sl_absent);
      
      FunctionI* fi = new FunctionI(Location().introduce(),
                                    createEnumToStringName(ident, "_toString_"),
                                    ti_fi,fi_params,ite);
      enumItems->addItem(fi);
    } else {
      if (vd_enumToString) {
        /// TODO: find a better solution (don't introduce the vd_enumToString until we
        ///       know it's a non-anonymous enum)
        vd_enumToString->e(new ArrayLit(Location().introduce(), std::vector<Expression*>()));
      }
      {
        Type tx = Type::parint();
        tx.ot(Type::OT_OPTIONAL);
        TypeInst* ti_aa = new TypeInst(Location().introduce(),tx);
        VarDecl* vd_aa = new VarDecl(Location().introduce(),ti_aa,"x");
        vd_aa->toplevel(false);
        
        TypeInst* ti_ab = new TypeInst(Location().introduce(),Type::parbool());
        VarDecl* vd_ab = new VarDecl(Location().introduce(),ti_ab,"b");
        vd_ab->toplevel(false);

        std::vector<Expression*> deopt_args(1);
        deopt_args[0] = vd_aa->id();
        Call* deopt = new Call(Location().introduce(), "deopt", deopt_args);
        Call* if_absent = new Call(Location().introduce(), "absent", deopt_args);
        StringLit* sl_absent = new StringLit(Location().introduce(),"<>");

        StringLit* sl_dzn = new StringLit(Location().introduce(),
                                          ASTString("to_enum("+ident->str().str()+","));
        std::vector<Expression*> showIntArgs(1);
        showIntArgs[0] = deopt;
        Call* showInt = new Call(Location().introduce(), constants().ids.show, showIntArgs);
        BinOp* construct_string_dzn = new BinOp(Location().introduce(), sl_dzn, BOT_PLUSPLUS, showInt);
        StringLit* closing_bracket = new StringLit(Location().introduce(), ASTString(")"));
        BinOp* construct_string_dzn_2 = new BinOp(Location().introduce(), construct_string_dzn,
                                             BOT_PLUSPLUS, closing_bracket);
        
        StringLit* sl = new StringLit(Location().introduce(), ASTString(ident->str().str()+"_"));
        BinOp* construct_string = new BinOp(Location().introduce(), sl, BOT_PLUSPLUS, showInt);
        
        std::vector<Expression*> if_then(4);
        if_then[0] = if_absent;
        if_then[1] = sl_absent;
        if_then[2] = vd_ab->id();
        if_then[3] = construct_string_dzn_2;
        ITE* ite = new ITE(Location().introduce(), if_then, construct_string);
        
        
        TypeInst* ti_fi = new TypeInst(Location().introduce(),Type::parstring());
        std::vector<VarDecl*> fi_params(2);
        fi_params[0] = vd_aa;
        fi_params[1] = vd_ab;
        FunctionI* fi = new FunctionI(Location().introduce(),
                                      createEnumToStringName(ident, "_toString_"),
                                      ti_fi,fi_params,ite);
        enumItems->addItem(fi);
      }
    }
    
    {
      /*
       
       function _toString_ENUM(array[$U] of opt Foo: x, bool: b) =
         let {
           array[int] of opt ENUM: xx = array1d(x)
         } in "[" ++ join(", ", [ _toString_ENUM(xx[i],b) | i in index_set(xx) ]) ++ "]";
       
       */

      TIId* tiid = new TIId(Location().introduce(),"U");
      TypeInst* ti_range = new TypeInst(Location().introduce(),Type::parint(),tiid);
      std::vector<TypeInst*> ranges(1);
      ranges[0] = ti_range;

      Type tx = Type::parint(-1);
      tx.ot(Type::OT_OPTIONAL);
      TypeInst* x_ti = new TypeInst(Location().introduce(),tx,ranges,ident);
      VarDecl* vd_x = new VarDecl(Location().introduce(),x_ti,"x");
      vd_x->toplevel(false);

      TypeInst* b_ti = new TypeInst(Location().introduce(),Type::parbool());
      VarDecl* vd_b = new VarDecl(Location().introduce(),b_ti,"b");
      vd_b->toplevel(false);

      TypeInst* xx_range = new TypeInst(Location().introduce(),Type::parint(),NULL);
      std::vector<TypeInst*> xx_ranges(1);
      xx_ranges[0] = xx_range;
      TypeInst* xx_ti = new TypeInst(Location().introduce(),tx,xx_ranges,ident);
      
      std::vector<Expression*> array1dArgs(1);
      array1dArgs[0] = vd_x->id();
      Call* array1dCall = new Call(Location().introduce(),"array1d",array1dArgs);
      
      VarDecl* vd_xx = new VarDecl(Location().introduce(),xx_ti,"xx",array1dCall);
      vd_xx->toplevel(false);

      TypeInst* idx_i_ti = new TypeInst(Location().introduce(),Type::parint());
      VarDecl* idx_i = new VarDecl(Location().introduce(),idx_i_ti,"i");
      idx_i->toplevel(false);
      
      std::vector<Expression*> aa_xxi_idx(1);
      aa_xxi_idx[0] = idx_i->id();
      ArrayAccess* aa_xxi = new ArrayAccess(Location().introduce(),vd_xx->id(),aa_xxi_idx);
      
      std::vector<Expression*> _toString_ENUMArgs(2);
      _toString_ENUMArgs[0] = aa_xxi;
      _toString_ENUMArgs[1] = vd_b->id();
      Call* _toString_ENUM = new Call(Location().introduce(),
                                      createEnumToStringName(ident, "_toString_"),
                                      _toString_ENUMArgs);
      
      std::vector<Expression*> index_set_xx_args(1);
      index_set_xx_args[0] = vd_xx->id();
      Call* index_set_xx = new Call(Location().introduce(),"index_set",index_set_xx_args);
      std::vector<VarDecl*> gen_exps(1);
      gen_exps[0] = idx_i;
      Generator gen(gen_exps,index_set_xx,NULL);
      
      Generators generators;
      generators._g.push_back(gen);
      Comprehension* comp = new Comprehension(Location().introduce(),_toString_ENUM,generators,false);
      
      std::vector<Expression*> join_args(2);
      join_args[0] = new StringLit(Location().introduce(),", ");
      join_args[1] = comp;
      Call* join = new Call(Location().introduce(),"join",join_args);
      
      StringLit* sl_open = new StringLit(Location().introduce(),"[");
      BinOp* bopp0 = new BinOp(Location().introduce(),sl_open,BOT_PLUSPLUS,join);
      StringLit* sl_close = new StringLit(Location().introduce(),"]");
      BinOp* bopp1 = new BinOp(Location().introduce(),bopp0,BOT_PLUSPLUS,sl_close);

      std::vector<Expression*> let_args(1);
      let_args[0] = vd_xx;
      Let* let = new Let(Location().introduce(),let_args,bopp1);
      
      TypeInst* ti_fi = new TypeInst(Location().introduce(),Type::parstring());
      std::vector<VarDecl*> fi_params(2);
      fi_params[0] = vd_x;
      fi_params[1] = vd_b;
      FunctionI* fi = new FunctionI(Location().introduce(),
                                    createEnumToStringName(ident, "_toString_"),
                                    ti_fi,fi_params,let);
      enumItems->addItem(fi);
    }
    
    {
      /*
       
       function _toString_ENUM(opt set of ENUM: x, bool: b) =
         if absent(x) then <> else "{" ++ join(", ", [ _toString_ENUM(i,b) | i in x ]) ++ "}" endif;
       
       */
      
      Type argType = Type::parsetenum(ident->type().enumId());
      argType.ot(Type::OT_OPTIONAL);
      TypeInst* x_ti = new TypeInst(Location().introduce(),argType,ident);
      VarDecl* vd_x = new VarDecl(Location().introduce(),x_ti,"x");
      vd_x->toplevel(false);
      
      TypeInst* b_ti = new TypeInst(Location().introduce(),Type::parbool());
      VarDecl* vd_b = new VarDecl(Location().introduce(),b_ti,"b");
      vd_b->toplevel(false);

      TypeInst* idx_i_ti = new TypeInst(Location().introduce(),Type::parint());
      VarDecl* idx_i = new VarDecl(Location().introduce(),idx_i_ti,"i");
      idx_i->toplevel(false);
      
      std::vector<Expression*> _toString_ENUMArgs(2);
      _toString_ENUMArgs[0] = idx_i->id();
      _toString_ENUMArgs[1] = vd_b->id();
      Call* _toString_ENUM = new Call(Location().introduce(),
                                      createEnumToStringName(ident, "_toString_"),
                                      _toString_ENUMArgs);
      
      
      std::vector<Expression*> deopt_args(1);
      deopt_args[0] = vd_x->id();
      Call* deopt = new Call(Location().introduce(), "deopt", deopt_args);
      Call* if_absent = new Call(Location().introduce(), "absent", deopt_args);
      StringLit* sl_absent = new StringLit(Location().introduce(),"<>");

      std::vector<VarDecl*> gen_exps(1);
      gen_exps[0] = idx_i;
      Generator gen(gen_exps,deopt,NULL);
      
      Generators generators;
      generators._g.push_back(gen);
      Comprehension* comp = new Comprehension(Location().introduce(),_toString_ENUM,generators,false);
      
      std::vector<Expression*> join_args(2);
      join_args[0] = new StringLit(Location().introduce(),", ");
      join_args[1] = comp;
      Call* join = new Call(Location().introduce(),"join",join_args);
      
      StringLit* sl_open = new StringLit(Location().introduce(),"{");
      BinOp* bopp0 = new BinOp(Location().introduce(),sl_open,BOT_PLUSPLUS,join);
      StringLit* sl_close = new StringLit(Location().introduce(),"}");
      BinOp* bopp1 = new BinOp(Location().introduce(),bopp0,BOT_PLUSPLUS,sl_close);
      
      
      std::vector<Expression*> if_then(2);
      if_then[0] = if_absent;
      if_then[1] = sl_absent;
      ITE* ite = new ITE(Location().introduce(), if_then, bopp1);
      
      TypeInst* ti_fi = new TypeInst(Location().introduce(),Type::parstring());
      std::vector<VarDecl*> fi_params(2);
      fi_params[0] = vd_x;
      fi_params[1] = vd_b;
      FunctionI* fi = new FunctionI(Location().introduce(),
                                    createEnumToStringName(ident, "_toString_"),
                                    ti_fi,fi_params,ite);
      enumItems->addItem(fi);
    }

    {
      /*
       
       function _toString_ENUM(array[$U] of opt set of ENUM: x, bool: b) =
       let {
       array[int] of opt set of ENUM: xx = array1d(x)
       } in "[" ++ join(", ", [ _toString_ENUM(xx[i],b) | i in index_set(xx) ]) ++ "]";
       
       */
      
      TIId* tiid = new TIId(Location().introduce(),"U");
      TypeInst* ti_range = new TypeInst(Location().introduce(),Type::parint(),tiid);
      std::vector<TypeInst*> ranges(1);
      ranges[0] = ti_range;
      
      Type tx = Type::parsetint(-1);
      tx.ot(Type::OT_OPTIONAL);
      TypeInst* x_ti = new TypeInst(Location().introduce(),tx,ranges,ident);
      VarDecl* vd_x = new VarDecl(Location().introduce(),x_ti,"x");
      vd_x->toplevel(false);
      
      TypeInst* b_ti = new TypeInst(Location().introduce(),Type::parbool());
      VarDecl* vd_b = new VarDecl(Location().introduce(),b_ti,"b");
      vd_b->toplevel(false);
      
      TypeInst* xx_range = new TypeInst(Location().introduce(),Type::parint(),NULL);
      std::vector<TypeInst*> xx_ranges(1);
      xx_ranges[0] = xx_range;
      TypeInst* xx_ti = new TypeInst(Location().introduce(),tx,xx_ranges,ident);
      
      std::vector<Expression*> array1dArgs(1);
      array1dArgs[0] = vd_x->id();
      Call* array1dCall = new Call(Location().introduce(),"array1d",array1dArgs);
      
      VarDecl* vd_xx = new VarDecl(Location().introduce(),xx_ti,"xx",array1dCall);
      vd_xx->toplevel(false);
      
      TypeInst* idx_i_ti = new TypeInst(Location().introduce(),Type::parint());
      VarDecl* idx_i = new VarDecl(Location().introduce(),idx_i_ti,"i");
      idx_i->toplevel(false);
      
      std::vector<Expression*> aa_xxi_idx(1);
      aa_xxi_idx[0] = idx_i->id();
      ArrayAccess* aa_xxi = new ArrayAccess(Location().introduce(),vd_xx->id(),aa_xxi_idx);
      
      std::vector<Expression*> _toString_ENUMArgs(2);
      _toString_ENUMArgs[0] = aa_xxi;
      _toString_ENUMArgs[1] = vd_b->id();
      Call* _toString_ENUM = new Call(Location().introduce(),
                                      createEnumToStringName(ident, "_toString_"),
                                      _toString_ENUMArgs);
      
      std::vector<Expression*> index_set_xx_args(1);
      index_set_xx_args[0] = vd_xx->id();
      Call* index_set_xx = new Call(Location().introduce(),"index_set",index_set_xx_args);
      std::vector<VarDecl*> gen_exps(1);
      gen_exps[0] = idx_i;
      Generator gen(gen_exps,index_set_xx,NULL);
      
      Generators generators;
      generators._g.push_back(gen);
      Comprehension* comp = new Comprehension(Location().introduce(),_toString_ENUM,generators,false);
      
      std::vector<Expression*> join_args(2);
      join_args[0] = new StringLit(Location().introduce(),", ");
      join_args[1] = comp;
      Call* join = new Call(Location().introduce(),"join",join_args);
      
      StringLit* sl_open = new StringLit(Location().introduce(),"[");
      BinOp* bopp0 = new BinOp(Location().introduce(),sl_open,BOT_PLUSPLUS,join);
      StringLit* sl_close = new StringLit(Location().introduce(),"]");
      BinOp* bopp1 = new BinOp(Location().introduce(),bopp0,BOT_PLUSPLUS,sl_close);
      
      std::vector<Expression*> let_args(1);
      let_args[0] = vd_xx;
      Let* let = new Let(Location().introduce(),let_args,bopp1);
      
      TypeInst* ti_fi = new TypeInst(Location().introduce(),Type::parstring());
      std::vector<VarDecl*> fi_params(2);
      fi_params[0] = vd_x;
      fi_params[1] = vd_b;
      FunctionI* fi = new FunctionI(Location().introduce(),
                                    createEnumToStringName(ident, "_toString_"),
                                    ti_fi,fi_params,let);
      enumItems->addItem(fi);
    }

    return ret;
  }

  void
  TopoSorter::add(EnvI& env, VarDeclI* vdi, bool handleEnums, Model* enumItems) {
    VarDecl* vd = vdi->e();
    if (handleEnums && vd->ti()->isEnum()) {
      unsigned int enumId = env.registerEnum(vdi);
      Type vdt = vd->type();
      vdt.enumId(enumId);
      vd->ti()->type(vdt);
      vd->type(vdt);

      if (vd->e()) {
        (void) createEnumMapper(env, model, enumId, vd, NULL, enumItems);
      } else {
        GCLock lock;
        std::string name = createEnumToStringName(vd->id(),"_enum_to_string_");
        std::vector<TypeInst*> ranges(1);
        ranges[0] = new TypeInst(Location().introduce(),Type::parint());
        TypeInst* ti = new TypeInst(Location().introduce(),Type::parstring(1));
        ti->setRanges(ranges);
        VarDecl* vd_enumToString = new VarDecl(Location().introduce(),ti,name,NULL);
        enumItems->addItem(new VarDeclI(Location().introduce(),vd_enumToString));
      }
    }
    scopes.add(env, vd);
  }

  VarDecl*
  TopoSorter::get(EnvI& env, const ASTString& id_v, const Location& loc) {
    GCLock lock;
    Id* id = new Id(Location(), id_v, NULL);
    VarDecl* decl = scopes.find(id);
    if (decl==NULL) {
      throw TypeError(env,loc,"undefined identifier `"+id->str().str()+"'");
    }
    return decl;
  }

  VarDecl*
  TopoSorter::checkId(EnvI& env, Id* ident, const Location& loc) {
    VarDecl* decl = scopes.find(ident);
    if (decl==NULL) {
      GCLock lock;
      throw TypeError(env,loc,"undefined identifier `"+ident->str().str()+"'");
    }
    PosMap::iterator pi = pos.find(decl);
    if (pi==pos.end()) {
      // new id
      scopes.push(true);
      run(env, decl);
      scopes.pop();
    } else {
      // previously seen, check if circular
      if (pi->second==-1) {
        GCLock lock;
        throw TypeError(env,loc,"circular definition of `"+ident->str().str()+"'");
      }
    }
    return decl;
  }

  VarDecl*
  TopoSorter::checkId(EnvI& env, const ASTString& id_v, const Location& loc) {
    GCLock lock;
    Id* id = new Id(loc,id_v,NULL);
    return checkId(env, id, loc);
  }

  void
  TopoSorter::run(EnvI& env, Expression* e) {
    if (e==NULL)
      return;
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
        if(sl->isv()==NULL && sl->fsv()==NULL)
          for (unsigned int i=0; i<sl->v().size(); i++)
            run(env,sl->v()[i]);
      }
      break;
    case Expression::E_ID:
      {
        if (e != constants().absent) {
          VarDecl* vd = checkId(env, e->cast<Id>(),e->loc());
          e->cast<Id>()->decl(vd);
        }
      }
      break;
    case Expression::E_ARRAYLIT:
      {
        ArrayLit* al = e->cast<ArrayLit>();
        for (unsigned int i=0; i<al->size(); i++)
          run(env, (*al)[i]);
      }
      break;
    case Expression::E_ARRAYACCESS:
      {
        ArrayAccess* ae = e->cast<ArrayAccess>();
        run(env, ae->v());
        for (unsigned int i=0; i<ae->idx().size(); i++)
          run(env, ae->idx()[i]);
      }
      break;
    case Expression::E_COMP:
      {
        Comprehension* ce = e->cast<Comprehension>();
        scopes.push(false);
        for (int i=0; i<ce->n_generators(); i++) {
          run(env, ce->in(i));
          for (int j=0; j<ce->n_decls(i); j++) {
            run(env, ce->decl(i,j));
            scopes.add(env, ce->decl(i,j));
          }
          if (ce->where(i))
            run(env, ce->where(i));
        }
        run(env, ce->e());
        scopes.pop();
      }
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        for (int i=0; i<ite->size(); i++) {
          run(env, ite->e_if(i));
          run(env, ite->e_then(i));
        }
        run(env, ite->e_else());
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* be = e->cast<BinOp>();
        std::vector<Expression*> todo;
        todo.push_back(be->lhs());
        todo.push_back(be->rhs());
        while (!todo.empty()) {
          Expression* e = todo.back();
          todo.pop_back();
          if (BinOp* e_bo = e->dyn_cast<BinOp>()) {
            todo.push_back(e_bo->lhs());
            todo.push_back(e_bo->rhs());
            for (ExpressionSetIter it = e_bo->ann().begin(); it != e_bo->ann().end(); ++it)
              run(env, *it);
          } else {
            run(env, e);
          }
        }
      }
      break;
    case Expression::E_UNOP:
      {
        UnOp* ue = e->cast<UnOp>();
        run(env, ue->e());
      }
      break;
    case Expression::E_CALL:
      {
        Call* ce = e->cast<Call>();
        for (unsigned int i=0; i<ce->n_args(); i++)
          run(env, ce->arg(i));
      }
      break;
    case Expression::E_VARDECL:
      {
        VarDecl* ve = e->cast<VarDecl>();
        PosMap::iterator pi = pos.find(ve);
        if (pi==pos.end()) {
          pos.insert(std::pair<VarDecl*,int>(ve,-1));
          run(env, ve->ti());
          run(env, ve->e());
          ve->payload(static_cast<int>(decls.size()));
          decls.push_back(ve);
          pi = pos.find(ve);
          pi->second = static_cast<int>(decls.size())-1;
        } else {
          assert(pi->second != -1);
        }
      }
      break;
    case Expression::E_TI:
      {
        TypeInst* ti = e->cast<TypeInst>();
        for (unsigned int i=0; i<ti->ranges().size(); i++)
          run(env, ti->ranges()[i]);
        run(env, ti->domain());
      }
      break;
    case Expression::E_TIID:
      break;
    case Expression::E_LET:
      {
        Let* let = e->cast<Let>();
        scopes.push(false);
        for (unsigned int i=0; i<let->let().size(); i++) {
          run(env, let->let()[i]);
          if (VarDecl* vd = let->let()[i]->dyn_cast<VarDecl>()) {
            scopes.add(env, vd);
          }
        }
        run(env, let->in());
        VarDeclCmp poscmp(pos);
        std::stable_sort(let->let().begin(), let->let().end(), poscmp);
        for (unsigned int i=0; i<let->let().size(); i++) {
          if (VarDecl* vd = let->let()[i]->dyn_cast<VarDecl>()) {
            let->let_orig()[i] = vd->e();
          } else {
            let->let_orig()[i] = NULL;
          }
        }
        scopes.pop();
      }
      break;
    }
    if (env.ignoreUnknownIds) {
      std::vector<Expression*> toDelete;
      for (ExpressionSetIter it = e->ann().begin(); it != e->ann().end(); ++it) {
          try {
            run(env, *it);
          } catch (TypeError&) {
            toDelete.push_back(*it);
          }
          for (Expression* de : toDelete)
            e->ann().remove(de);
      }
    } else {
      for (ExpressionSetIter it = e->ann().begin(); it != e->ann().end(); ++it) {
        run(env, *it);
      }
    }
  }
  
  KeepAlive addCoercion(EnvI& env, Model* m, Expression* e, const Type& funarg_t) {
    if (e->isa<ArrayAccess>() && e->type().dim() > 0) {
      ArrayAccess* aa = e->cast<ArrayAccess>();
      // Turn ArrayAccess into a slicing operation
      std::vector<Expression*> args;
      args.push_back(aa->v());
      args.push_back(NULL);
      std::vector<Expression*> slice;
      GCLock lock;
      for (unsigned int i=0; i<aa->idx().size(); i++) {
        if (aa->idx()[i]->type().is_set()) {
          bool needIdxSet = true;
          bool needInter = true;
          if (SetLit* sl = aa->idx()[i]->dyn_cast<SetLit>()) {
            if (sl->isv() && sl->isv()->size()==1) {
              if (sl->isv()->min().isFinite() && sl->isv()->max().isFinite()) {
                args.push_back(sl);
                needIdxSet = false;
              } else if (sl->isv()->min()==-IntVal::infinity() && sl->isv()->max()==IntVal::infinity()) {
                needInter = false;
              }
            }
          }
          if (needIdxSet) {
            std::ostringstream oss;
            oss << "index_set_" << (i+1) << "of" << aa->idx().size();
            std::vector<Expression*> origIdxsetArgs(1);
            origIdxsetArgs[0] = aa->v();
            Call* origIdxset = new Call(aa->v()->loc(), ASTString(oss.str()), origIdxsetArgs);
            FunctionI* fi = m->matchFn(env, origIdxset, false);
            if (!fi)
              throw TypeError(env, e->loc(), "missing builtin "+oss.str());
            origIdxset->type(fi->rtype(env, origIdxsetArgs, false));
            origIdxset->decl(fi);
            if (needInter) {
              BinOp* inter = new BinOp(aa->idx()[i]->loc(), aa->idx()[i], BOT_INTERSECT, origIdxset);
              inter->type(Type::parsetint());
              args.push_back(inter);
            } else {
              args.push_back(origIdxset);
            }
          }
          slice.push_back(aa->idx()[i]);
        } else {
          BinOp* bo = new BinOp(aa->idx()[i]->loc(),aa->idx()[i],BOT_DOTDOT,aa->idx()[i]);
          bo->type(Type::parsetint());
          slice.push_back(bo);
        }
      }
      ArrayLit* a_slice = new ArrayLit(e->loc(), slice);
      a_slice->type(Type::parsetint(1));
      args[1] = a_slice;
      std::ostringstream oss;
      oss << "slice_" << (args.size()-2) << "d";
      Call* c = new Call(e->loc(), ASTString(oss.str()), args);
      FunctionI* fi = m->matchFn(env, c, false);
      if (!fi)
        throw TypeError(env, e->loc(), "missing builtin "+oss.str());
      c->type(fi->rtype(env, args, false));
      c->decl(fi);
      return c;
    }
    if (e->type().dim()==funarg_t.dim() && (funarg_t.bt()==Type::BT_BOT || funarg_t.bt()==Type::BT_TOP || e->type().bt()==funarg_t.bt() || e->type().bt()==Type::BT_BOT))
      return e;
    std::vector<Expression*> args(1);
    args[0] = e;
    GCLock lock;
    Call* c = NULL;
    if (e->type().dim()==0 && funarg_t.dim()!=0) {
      if (e->type().isvar()) {
        throw TypeError(env, e->loc(),"cannot coerce var set into array");
      }
      if (e->type().isopt()) {
        throw TypeError(env, e->loc(),"cannot coerce opt set into array");
      }
      std::vector<Expression*> set2a_args(1);
      set2a_args[0] = e;
      Call* set2a = new Call(e->loc(), ASTString("set2array"), set2a_args);
      FunctionI* fi = m->matchFn(env, set2a, false);
      assert(fi);
      set2a->type(fi->rtype(env, args, false));
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
      FunctionI* fi = m->matchFn(env, c, false);
      assert(fi);
      c->type(fi->rtype(env, args, false));
      c->decl(fi);
      KeepAlive ka(c);
      return ka;
    }
    throw TypeError(env, e->loc(),"cannot determine coercion from type "+e->type().toString(env)+" to type "+funarg_t.toString(env));
  }
  KeepAlive addCoercion(EnvI& env, Model* m, Expression* e, Expression* funarg) {
    return addCoercion(env, m, e, funarg->type());
  }
  
  template<bool ignoreVarDecl>
  class Typer {
  public:
    EnvI& _env;
    Model* _model;
    std::vector<TypeError>& _typeErrors;
    bool _ignoreUndefined;
    Typer(EnvI& env, Model* model, std::vector<TypeError>& typeErrors, bool ignoreUndefined) : _env(env), _model(model), _typeErrors(typeErrors), _ignoreUndefined(ignoreUndefined) {}
    /// Check annotations when expression is finished
    void exit(Expression* e) {
      for (ExpressionSetIter it = e->ann().begin(); it != e->ann().end(); ++it)
        if (!(*it)->type().isann())
          throw TypeError(_env,(*it)->loc(),"expected annotation, got `"+(*it)->type().toString(_env)+"'");
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
      if (sl.isv()) {
        ty.bt(Type::BT_INT);
        ty.enumId(sl.type().enumId());
        sl.type(ty);
        return;
      }
      unsigned int enumId = sl.v().size() > 0 ? sl.v()[0]->type().enumId() : 0;
      for (unsigned int i=0; i<sl.v().size(); i++) {
        if (sl.v()[i]->type().dim() > 0)
          throw TypeError(_env,sl.v()[i]->loc(),"set literals cannot contain arrays");
        if (sl.v()[i]->type().isvar())
          ty.ti(Type::TI_VAR);
        if (sl.v()[i]->type().isopt())
          throw TypeError(_env,sl.v()[i]->loc(),"set literals cannot contain option type values");
        if (sl.v()[i]->type().cv())
          ty.cv(true);
        if (enumId != sl.v()[i]->type().enumId())
          enumId = 0;
        if (!Type::bt_subtype(sl.v()[i]->type(), ty, true)) {
          if (ty.bt() == Type::BT_UNKNOWN || Type::bt_subtype(ty, sl.v()[i]->type(), true)) {
            ty.bt(sl.v()[i]->type().bt());
          } else {
            throw TypeError(_env,sl.loc(),"non-uniform set literal");
          }
        }
      }
      ty.enumId(enumId);
      if (ty.bt() == Type::BT_UNKNOWN) {
        ty.bt(Type::BT_BOT);
      } else {
        if (ty.isvar() && ty.bt()!=Type::BT_INT) {
          if (ty.bt()==Type::BT_BOOL)
            ty.bt(Type::BT_INT);
          else
            throw TypeError(_env,sl.loc(),"cannot coerce set literal element to var int");
        }
        for (unsigned int i=0; i<sl.v().size(); i++) {
          sl.v()[i] = addCoercion(_env, _model, sl.v()[i], ty)();
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
      for (unsigned int i=0; i<al.size(); i++) {
        Expression* vi = al[i];
        if (vi->type().dim() > 0)
          throw TypeError(_env,vi->loc(),"arrays cannot be elements of arrays");
        
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
              if (ty.st() != vi->type().st() && vi->type().ot()!=Type::OT_OPTIONAL) {
                throw TypeError(_env,al.loc(),"non-uniform array literal");
              }
            } else {
              haveInferredType = true;
              ty.st(vi->type().st());
            }
            if (vi->type().bt() != Type::BT_BOT) {
              ty.bt(vi->type().bt());
              ty.enumId(vi->type().enumId());
            }
          }
        } else {
          if (av == NULL) {
            if (vi->type().bt() == Type::BT_BOT) {
              if (vi->type().st() != ty.st() && vi->type().ot()!=Type::OT_OPTIONAL) {
                throw TypeError(_env,al.loc(),"non-uniform array literal");
              }
              if (vi->type().enumId() != 0 && ty.enumId() != vi->type().enumId()) {
                ty.enumId(0);
              }
            } else {
              unsigned int tyEnumId = ty.enumId();
              ty.enumId(vi->type().enumId());
              if (Type::bt_subtype(ty, vi->type(), true)) {
                ty.bt(vi->type().bt());
              }
              if (tyEnumId != vi->type().enumId())
                ty.enumId(0);
              if (!Type::bt_subtype(vi->type(),ty,true) || ty.st() != vi->type().st()) {
                throw TypeError(_env,al.loc(),"non-uniform array literal");
              }
            }
          }
        }
      }
      if (ty.bt() == Type::BT_UNKNOWN) {
        ty.bt(Type::BT_BOT);
        if (!anons.empty())
          throw TypeError(_env,al.loc(),"array literal must contain at least one non-anonymous variable");
      } else {
        Type at = ty;
        at.dim(0);
        if (at.ti()==Type::TI_VAR && at.st()==Type::ST_SET && at.bt()!=Type::BT_INT) {
          if (at.bt()==Type::BT_BOOL) {
            ty.bt(Type::BT_INT);
            at.bt(Type::BT_INT);
          } else {
            throw TypeError(_env,al.loc(),"cannot coerce array element to var set of int");
          }
        }
        for (unsigned int i=0; i<anons.size(); i++) {
          anons[i]->type(at);
        }
        for (unsigned int i=0; i<al.size(); i++) {
          al.set(i, addCoercion(_env, _model, al[i], at)());
        }
      }
      if (ty.enumId() != 0) {
        std::vector<unsigned int> enumIds(ty.dim()+1);
        for (int i=0; i<ty.dim(); i++)
          enumIds[i] = 0;
        enumIds[ty.dim()] = ty.enumId();
        ty.enumId(_env.registerArrayEnum(enumIds));
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
          aa.v(addCoercion(_env, _model, aa.v(), tv)());
        } else {
          std::ostringstream oss;
          oss << "array access attempted on expression of type `" << aa.v()->type().toString(_env) << "'";
          throw TypeError(_env,aa.v()->loc(),oss.str());
        }
      } else if (aa.v()->isa<ArrayAccess>()) {
        aa.v(addCoercion(_env, _model, aa.v(), aa.v()->type())());
      }
      if (aa.v()->type().dim() != aa.idx().size()) {
        std::ostringstream oss;
        oss << aa.v()->type().dim() << "-dimensional array accessed with "
        << aa.idx().size() << (aa.idx().size()==1 ? " expression" : " expressions");
        throw TypeError(_env,aa.v()->loc(),oss.str());
      }
      Type tt = aa.v()->type();
      if (tt.enumId() != 0) {
        const std::vector<unsigned int>& arrayEnumIds = _env.getArrayEnum(tt.enumId());
        
        for (unsigned int i=0; i<arrayEnumIds.size()-1; i++) {
          if (arrayEnumIds[i] != 0) {
            if (aa.idx()[i]->type().enumId() != arrayEnumIds[i]) {
              std::ostringstream oss;
              oss << "array index ";
              if (aa.idx().size() > 1) {
                oss << (i+1) << " ";
              }
              oss << "must be `" << _env.getEnum(arrayEnumIds[i])->e()->id()->str().str() << "', but is `" << aa.idx()[i]->type().toString(_env) << "'";
              throw TypeError(_env,aa.loc(),oss.str());
            }
          }
        }
        tt.enumId(arrayEnumIds[arrayEnumIds.size()-1]);
      }
      int n_dimensions = 0;
      bool isVarAccess = false;
      bool isSlice = false;
      for (unsigned int i=0; i<aa.idx().size(); i++) {
        Expression* aai = aa.idx()[i];
        if (aai->isa<AnonVar>()) {
          aai->type(Type::varint());
        }
        if ((aai->type().bt() != Type::BT_INT && aai->type().bt() != Type::BT_BOOL) || aai->type().dim() != 0) {
          throw TypeError(_env,aa.loc(),"array index must be `int' or `set of int', but is `"+aai->type().toString(_env)+"'");
        }
        if (aai->type().is_set()) {
          if (isVarAccess || aai->type().isvar()) {
            throw TypeError(_env,aa.loc(),"array slicing with variable range or index not supported");
          }
          isSlice = true;
          aa.idx()[i] = addCoercion(_env, _model, aai, Type::varsetint())();
          n_dimensions++;
        } else {
          aa.idx()[i] = addCoercion(_env, _model, aai, Type::varint())();
        }
        
        if (aai->type().isopt()) {
          tt.ot(Type::OT_OPTIONAL);
        }
        if (aai->type().isvar()) {
          isVarAccess = true;
          if (isSlice) {
            throw TypeError(_env,aa.loc(),"array slicing with variable range or index not supported");
          }
          tt.ti(Type::TI_VAR);
          if (tt.bt()==Type::BT_ANN || tt.bt()==Type::BT_STRING) {
            throw TypeError(_env,aai->loc(),std::string("array access using a variable not supported for array of ")+(tt.bt()==Type::BT_ANN ? "ann" : "string"));
          }
        }
        tt.dim(n_dimensions);
        if (aai->type().cv())
          tt.cv(true);
      }
      aa.type(tt);
    }
    /// Visit array comprehension
    void vComprehension(Comprehension& c) {
      Type tt = c.e()->type();
      typedef std::unordered_map<VarDecl*, std::pair<int,int> > genMap_t;
      typedef std::unordered_map<VarDecl*, std::vector<Expression*> > whereMap_t;
      genMap_t generatorMap;
      whereMap_t whereMap;
      int declCount = 0;
      bool didMoveWheres = false;
      for (int i=0; i<c.n_generators(); i++) {
        for (int j=0; j<c.n_decls(i); j++) {
          generatorMap[c.decl(i,j)] = std::pair<int,int>(i,declCount++);
          whereMap[c.decl(i,j)] = std::vector<Expression*>();
        }
        Expression* g_in = c.in(i);
        if (g_in) {
          const Type& ty_in = g_in->type();
          if (ty_in == Type::varsetint()) {
            tt.ot(Type::OT_OPTIONAL);
            tt.ti(Type::TI_VAR);
          }
          if (ty_in.cv())
            tt.cv(true);
          if (c.where(i)) {
            if (c.where(i)->type() == Type::varbool()) {
              tt.ot(Type::OT_OPTIONAL);
              tt.ti(Type::TI_VAR);
            } else if (c.where(i)->type() != Type::parbool()) {
              throw TypeError(_env,c.where(i)->loc(),
                              "where clause must be bool, but is `"+
                              c.where(i)->type().toString(_env)+"'");
            }
            if (c.where(i)->type().cv())
              tt.cv(true);
            
            // Try to move parts of the where clause to earlier generators
            std::vector<Expression*> wherePartsStack;
            std::vector<Expression*> whereParts;
            wherePartsStack.push_back(c.where(i));
            while (!wherePartsStack.empty()) {
              Expression* e = wherePartsStack.back();
              wherePartsStack.pop_back();
              if (BinOp* bo = e->dyn_cast<BinOp>()) {
                if (bo->op()==BOT_AND) {
                  wherePartsStack.push_back(bo->rhs());
                  wherePartsStack.push_back(bo->lhs());
                } else {
                  whereParts.push_back(e);
                }
              } else {
                whereParts.push_back(e);
              }
            }
            
            for (unsigned int wpi=0; wpi < whereParts.size(); wpi++) {
              Expression* wp = whereParts[wpi];
              class FindLatestGen : public EVisitor {
              public:
                int decl_idx;
                VarDecl* decl;
                const genMap_t& generatorMap;
                Comprehension* comp;
                FindLatestGen(const genMap_t& generatorMap0, Comprehension* comp0) : decl_idx(-1), decl(NULL), generatorMap(generatorMap0), comp(comp0) {}
                void vId(const Id& ident) {
                  genMap_t::const_iterator it = generatorMap.find(ident.decl());
                  if (it != generatorMap.end() && it->second.second > decl_idx) {
                    decl_idx = it->second.second;
                    decl = ident.decl();
                    int gen = it->second.first;
                    while (comp->in(gen) == NULL && gen < comp->n_generators()-1) {
                      decl_idx++;
                      gen++;
                      decl = comp->decl(gen, 0);
                    }
                  }
                }
              } flg(generatorMap,&c);
              topDown(flg, wp);
              whereMap[flg.decl].push_back(wp);
              
              if (flg.decl_idx < declCount-1)
                didMoveWheres = true;
              
            }
          }
        } else {
          assert(c.where(i) != NULL);
          whereMap[c.decl(i,0)].push_back(c.where(i));
        }
      }
      
      if (didMoveWheres) {
        Generators generators;
        for (int i=0; i<c.n_generators(); i++) {
          std::vector<VarDecl*> decls;
          for (int j=0; j<c.n_decls(i); j++) {
            decls.push_back(c.decl(i,j));
            if (whereMap[c.decl(i,j)].size() != 0) {
              // need a generator for all the decls up to this point
              Expression* whereExpr = whereMap[c.decl(i,j)][0];
              for (unsigned int k=1; k<whereMap[c.decl(i,j)].size(); k++) {
                GCLock lock;
                BinOp* bo = new BinOp(Location().introduce(), whereExpr, BOT_AND, whereMap[c.decl(i,j)][k]);
                Type bo_t = whereMap[c.decl(i,j)][k]->type().ispar() && whereExpr->type().ispar() ? Type::parbool() : Type::varbool();
                bo->type(bo_t);
                whereExpr = bo;
              }
              generators._g.push_back(Generator(decls,c.in(i),whereExpr));
              decls.clear();
            } else if (j==c.n_decls(i)-1) {
              generators._g.push_back(Generator(decls,c.in(i),NULL));
              decls.clear();
            }
          }
        }
        GCLock lock;
        c.init(c.e(), generators);
      }
      
      if (c.set()) {
        if (c.e()->type().dim() != 0 || c.e()->type().st() == Type::ST_SET)
          throw TypeError(_env,c.e()->loc(),
              "set comprehension expression must be scalar, but is `"
              +c.e()->type().toString(_env)+"'");
        tt.st(Type::ST_SET);
        if (tt.isvar()) {
          c.e(addCoercion(_env, _model, c.e(), Type::varint())());
          tt.bt(Type::BT_INT);
        }
      } else {
        if (c.e()->type().dim() != 0)
          throw TypeError(_env,c.e()->loc(),
            "array comprehension expression cannot be an array");
        tt.dim(1);
        if (tt.enumId() != 0) {
          std::vector<unsigned int> enumIds(2);
          enumIds[0] = 0;
          enumIds[1] = tt.enumId();
          tt.enumId(_env.registerArrayEnum(enumIds));
        }
      }
      c.type(tt);
    }
    /// Visit array comprehension generator
    void vComprehensionGenerator(Comprehension& c, int gen_i) {
      Expression* g_in = c.in(gen_i);
      if (g_in==NULL) {
        // This is an "assignment generator" (i = expr)
        assert(c.where(gen_i) != NULL);
        assert(c.n_decls(gen_i) == 1);
        const Type& ty_where = c.where(gen_i)->type();
        c.decl(gen_i,0)->type(ty_where);
        c.decl(gen_i,0)->ti()->type(ty_where);
      } else {
        const Type& ty_in = g_in->type();
        if (ty_in != Type::varsetint() && ty_in != Type::parsetint() && ty_in.dim() != 1) {
          throw TypeError(_env,g_in->loc(),
                          "generator expression must be (par or var) set of int or one-dimensional array, but is `"
                          +ty_in.toString(_env)+"'");
        }
        Type ty_id;
        bool needIntLit = false;
        if (ty_in.dim()==0) {
          ty_id = Type::parint();
          ty_id.enumId(ty_in.enumId());
          needIntLit = true;
        } else {
          ty_id = ty_in;
          if (ty_in.enumId() != 0) {
            const std::vector<unsigned int>& enumIds = _env.getArrayEnum(ty_in.enumId());
            ty_id.enumId(enumIds.back());
          }
          ty_id.dim(0);
        }
        for (int j=0; j<c.n_decls(gen_i); j++) {
          if (needIntLit) {
            GCLock lock;
            c.decl(gen_i,j)->e(IntLit::aEnum(0,ty_id.enumId()));
          }
          c.decl(gen_i,j)->type(ty_id);
          c.decl(gen_i,j)->ti()->type(ty_id);
        }
      }
    }
    /// Visit if-then-else
    void vITE(ITE& ite) {
      bool mustBeBool = false;
      if (ite.e_else()==NULL) {
        // this is an "if <cond> then <expr> endif" so the <expr> must be bool
        ite.e_else(constants().boollit(true));
        mustBeBool = true;
      }
      Type tret = ite.e_else()->type();
      std::vector<AnonVar*> anons;
      bool allpar = !(tret.isvar());
      if (tret.isunknown()) {
        if (AnonVar* av = ite.e_else()->dyn_cast<AnonVar>()) {
          allpar = false;
          anons.push_back(av);
        } else {
          throw TypeError(_env,ite.e_else()->loc(), "cannot infer type of expression in `else' branch of conditional");
        }
      }
      bool allpresent = !(tret.isopt());
      bool varcond = false;
      for (int i=0; i<ite.size(); i++) {
        Expression* eif = ite.e_if(i);
        Expression* ethen = ite.e_then(i);
        varcond = varcond || (eif->type() == Type::varbool());
        if (eif->type() != Type::parbool() && eif->type() != Type::varbool())
          throw TypeError(_env,eif->loc(),
            "expected bool conditional expression, got `"+
            eif->type().toString(_env)+"'");
        if (eif->type().cv())
          tret.cv(true);
        if (ethen->type().isunknown()) {
          if (AnonVar* av = ethen->dyn_cast<AnonVar>()) {
            allpar = false;
            anons.push_back(av);
          } else {
            throw TypeError(_env,ethen->loc(), "cannot infer type of expression in `then' branch of conditional");
          }
        } else {
          if (tret.isbot() || tret.isunknown())
            tret.bt(ethen->type().bt());
          if (mustBeBool && (ethen->type().bt() != Type::BT_BOOL ||  ethen->type().dim() > 0 ||
                             ethen->type().st() != Type::ST_PLAIN || ethen->type().ot() != Type::OT_PRESENT)) {
            throw TypeError(_env,ite.loc(), std::string("conditional without `else' branch must have bool type, ")+
                            "but `then' branch has type `"+ethen->type().toString(_env)+"'");
          }
          if ( (!ethen->type().isbot() && !Type::bt_subtype(ethen->type(), tret, true) && !Type::bt_subtype(tret, ethen->type(), true)) ||
              ethen->type().st() != tret.st() ||
              ethen->type().dim() != tret.dim()) {
            throw TypeError(_env,ethen->loc(),
                            "type mismatch in branches of conditional. `then' branch has type `"+
                            ethen->type().toString(_env)+"', but `else' branch has type `"+
                            tret.toString(_env)+"'");
          }
          if (Type::bt_subtype(tret, ethen->type(), true)) {
            tret.bt(ethen->type().bt());
          }
          if (ethen->type().isvar()) allpar=false;
          if (ethen->type().isopt()) allpresent=false;
          if (ethen->type().cv())
            tret.cv(true);
        }
      }
      Type tret_var(tret);
      tret_var.ti(Type::TI_VAR);
      for (unsigned int i=0; i<anons.size(); i++) {
        anons[i]->type(tret_var);
      }
      for (int i=0; i<ite.size(); i++) {
        ite.e_then(i, addCoercion(_env, _model,ite.e_then(i), tret)());
      }
      ite.e_else(addCoercion(_env, _model, ite.e_else(), tret)());
      /// TODO: perhaps extend flattener to array types, but for now throw an error
      if (varcond && tret.dim() > 0)
        throw TypeError(_env,ite.loc(), "conditional with var condition cannot have array type");
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
      if (FunctionI* fi = _model->matchFn(_env,bop.opToString(),args,true)) {
        bop.lhs(addCoercion(_env, _model,bop.lhs(),fi->argtype(_env,args, 0))());
        bop.rhs(addCoercion(_env, _model,bop.rhs(),fi->argtype(_env,args, 1))());
        args[0] = bop.lhs(); args[1] = bop.rhs();
        Type ty = fi->rtype(_env,args,true);
        ty.cv(bop.lhs()->type().cv() || bop.rhs()->type().cv());
        bop.type(ty);
        
        if (fi->e())
          bop.decl(fi);
        else
          bop.decl(NULL);
      } else {
        throw TypeError(_env,bop.loc(),
          std::string("type error in operator application for `")+
          bop.opToString().str()+"'. No matching operator found with left-hand side type `"
                        +bop.lhs()->type().toString(_env)+
                        "' and right-hand side type `"+bop.rhs()->type().toString(_env)+"'");
      }
    }
    /// Visit unary operator
    void vUnOp(UnOp& uop) {
      std::vector<Expression*> args(1);
      args[0] = uop.e();
      if (FunctionI* fi = _model->matchFn(_env,uop.opToString(),args,true)) {
        uop.e(addCoercion(_env, _model,uop.e(),fi->argtype(_env,args,0))());
        args[0] = uop.e();
        Type ty = fi->rtype(_env,args,true);
        ty.cv(uop.e()->type().cv());
        uop.type(ty);
        if (fi->e())
          uop.decl(fi);
      } else {
        throw TypeError(_env,uop.loc(),
          std::string("type error in operator application for `")+
          uop.opToString().str()+"'. No matching operator found with type `"+uop.e()->type().toString(_env)+"'");
      }
    }
    static std::string createEnumToStringName(Id* ident, std::string prefix) {
      std::string name = ident->str().str();
      if (name[0]=='\'') {
        name = "'"+prefix+name.substr(1);
      } else {
        name = prefix+name;
      }
      return name;
    }

    /// Visit call
    void vCall(Call& call) {
      std::vector<Expression*> args(call.n_args());
      for (unsigned int i=static_cast<unsigned int>(args.size()); i--;)
        args[i] = call.arg(i);
      if (FunctionI* fi = _model->matchFn(_env,call.id(),args,true)) {
        bool cv = false;
        for (unsigned int i=0; i<args.size(); i++) {
          if(Comprehension* c = call.arg(i)->dyn_cast<Comprehension>()) {
            Type t_before = c->e()->type();
            Type t = fi->argtype(_env,args,i);
            t.dim(0);
            c->e(addCoercion(_env, _model, c->e(), t)());
            Type t_after = c->e()->type();
            if (t_before != t_after) {
              Type ct = c->type();
              ct.bt(t_after.bt());
              c->type(ct);
            }
          } else {
            args[i] = addCoercion(_env, _model,call.arg(i),fi->argtype(_env,args,i))();
            call.arg(i, args[i]);
          }
          cv = cv || args[i]->type().cv();
        }
        // Replace par enums with their string versions
        if (call.id()=="format" || call.id()=="show" || call.id()=="showDzn") {
          if (call.arg(call.n_args()-1)->type().ispar()) {
            int enumId = call.arg(call.n_args()-1)->type().enumId();
            if (enumId != 0 && call.arg(call.n_args()-1)->type().dim() != 0) {
              const std::vector<unsigned int>& enumIds = _env.getArrayEnum(enumId);
              enumId = enumIds[enumIds.size()-1];
            }
            if (enumId > 0) {
              VarDecl* enumDecl = _env.getEnum(enumId)->e();
              if (enumDecl->e()) {
                Id* ti_id = _env.getEnum(enumId)->e()->id();
                GCLock lock;
                std::vector<Expression*> args(2);
                args[0] = call.arg(call.n_args()-1);
                if (args[0]->type().dim() > 1) {
                  std::vector<Expression*> a1dargs(1);
                  a1dargs[0] = args[0];
                  Call* array1d = new Call(Location().introduce(),ASTString("array1d"),a1dargs);
                  Type array1dt = args[0]->type();
                  array1dt.dim(1);
                  array1d->type(array1dt);
                  args[0] = array1d;
                }
                args[1] = constants().boollit(call.id()=="showDzn");
                ASTString enumName(createEnumToStringName(ti_id, "_toString_"));
                call.id(enumName);
                call.args(args);
                if (call.id()=="showDzn") {
                  call.id(constants().ids.show);
                }
                fi = _model->matchFn(_env,&call,false);
                if (fi==NULL) {
                  std::ostringstream oss;
                  oss << "no function or predicate with this signature found: `";
                  oss << call.id() << "(";
                  for (unsigned int i=0; i<call.n_args(); i++) {
                    oss << call.arg(i)->type().toString(_env);
                    if (i<call.n_args()-1) oss << ",";
                  }
                  oss << ")'";
                  throw TypeError(_env,call.loc(), oss.str());
                }
              }
            }
          }
        }

        // Set type and decl
        Type ty = fi->rtype(_env,args,true);
        ty.cv(cv);
        call.type(ty);
        call.decl(fi);
      } else {
        std::ostringstream oss;
        oss << "no function or predicate with this signature found: `";
        oss << call.id() << "(";
        for (unsigned int i=0; i<call.n_args(); i++) {
          oss << call.arg(i)->type().toString(_env);
          if (i<call.n_args()-1) oss << ",";
        }
        oss << ")'";
        throw TypeError(_env,call.loc(), oss.str());
      }
    }
    /// Visit let
    void vLet(Let& let) {
      bool cv = false;
      for (unsigned int i=0; i<let.let().size(); i++) {
        Expression* li = let.let()[i];
        cv = cv || li->type().cv();
        if (VarDecl* vdi = li->dyn_cast<VarDecl>()) {
          if (vdi->e()==NULL && vdi->type().is_set() && vdi->type().isvar() &&
              vdi->ti()->domain()==NULL) {
            _typeErrors.push_back(TypeError(_env,vdi->loc(),
                                            "set element type for `"+vdi->id()->str().str()+"' is not finite"));
          }
          if (vdi->type().ispar() && vdi->e() == NULL)
            throw TypeError(_env,vdi->loc(),
              "let variable `"+vdi->id()->v().str()+"' must be initialised");
          if (vdi->ti()->hasTiVariable()) {
            _typeErrors.push_back(TypeError(_env,vdi->loc(),
                                            "type-inst variables not allowed in type-inst for let variable `"+vdi->id()->str().str()+"'"));
          }
          let.let_orig()[i] = vdi->e();
        }
      }
      Type ty = let.in()->type();
      ty.cv(cv);
      let.type(ty);
    }
    /// Visit variable declaration
    void vVarDecl(VarDecl& vd) {
      if (ignoreVarDecl) {
        if (vd.e()) {
          Type vdt = vd.ti()->type();
          Type vet = vd.e()->type();
          if (vdt.enumId() != 0 && vdt.dim() > 0 &&
              (vd.e()->isa<ArrayLit>() || vd.e()->isa<Comprehension>() ||
               (vd.e()->isa<BinOp>() && vd.e()->cast<BinOp>()->op()==BOT_PLUSPLUS))) {
            // Special case: index sets of array literals and comprehensions automatically
            // coerce to any enum index set
            const std::vector<unsigned int>& enumIds = _env.getArrayEnum(vdt.enumId());
            if (enumIds[enumIds.size()-1]==0) {
              vdt.enumId(0);
            } else {
              std::vector<unsigned int> nEnumIds(enumIds.size());
              for (unsigned int i=0; i<nEnumIds.size()-1; i++)
                nEnumIds[i] = 0;
              nEnumIds[nEnumIds.size()-1] = enumIds[enumIds.size()-1];
              vdt.enumId(_env.registerArrayEnum(nEnumIds));
            }
          } else if (vd.ti()->isEnum() && vd.e()->isa<Call>()) {
            if (vd.e()->cast<Call>()->id()=="anon_enum") {
              vet.enumId(vdt.enumId());
            }
          }
          
          if (vd.type().isunknown()) {
            vd.ti()->type(vet);
            vd.type(vet);
          } else if (! _env.isSubtype(vet,vdt,true)) {
            _typeErrors.push_back(TypeError(_env,vd.e()->loc(),
                                            "initialisation value for `"+vd.id()->str().str()+"' has invalid type-inst: expected `"+
                                            vd.ti()->type().toString(_env)+"', actual `"+vd.e()->type().toString(_env)+"'"));
          } else {
            vd.e(addCoercion(_env, _model, vd.e(), vd.ti()->type())());
          }
        } else {
          assert(!vd.type().isunknown());
        }
      } else {
        vd.type(vd.ti()->type());
        vd.id()->type(vd.type());
      }
    }
    /// Visit type inst
    void vTypeInst(TypeInst& ti) {
      Type tt = ti.type();
      bool foundEnum = ti.ranges().size()>0 && ti.domain() && ti.domain()->type().enumId() != 0;
      if (ti.ranges().size()>0) {
        bool foundTIId=false;
        for (unsigned int i=0; i<ti.ranges().size(); i++) {
          TypeInst* ri = ti.ranges()[i];
          assert(ri != NULL);
          if (ri->type().cv())
            tt.cv(true);
          if (ri->type().enumId() != 0) {
            foundEnum = true;
          }
          if (ri->type() == Type::top()) {
//            if (foundTIId) {
//              throw TypeError(_env,ri->loc(),
//                "only one type-inst variable allowed in array index");
//            } else {
              foundTIId = true;
//            }
          } else if (ri->type() != Type::parint()) {
            assert(ri->isa<TypeInst>());
            TypeInst* riti = ri->cast<TypeInst>();
            if (riti->domain()) {
              throw TypeError(_env,ri->loc(),
                "array index set expression has invalid type, expected `set of int', actual `set of "+
                ri->type().toString(_env)+"'");
            } else {
              throw TypeError(_env,ri->loc(),
                              "cannot use `"+ri->type().toString(_env)+"' as array index set (did you mean `int'?)");
            }
          }
        }
        tt.dim(foundTIId ? -1 : ti.ranges().size());
      }
      if (ti.domain() && ti.domain()->type().cv())
        tt.cv(true);
      if (ti.domain()) {
        if (TIId* tiid = ti.domain()->dyn_cast<TIId>()) {
          if (tiid->isEnum()) {
            tt.bt(Type::BT_INT);
          }
        } else {
          if (ti.domain()->type().ti() != Type::TI_PAR ||
              ti.domain()->type().st() != Type::ST_SET)
            throw TypeError(_env,ti.domain()->loc(),
                            "type-inst must be par set but is `"+ti.domain()->type().toString(_env)+"'");
          if (ti.domain()->type().dim() != 0)
            throw TypeError(_env,ti.domain()->loc(),
                            "type-inst cannot be an array");
        }
      }
      if (tt.isunknown() && ti.domain()) {
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
          throw TypeError(_env,ti.domain()->loc(),
            "type-inst must be int or float");
        }
        tt.bt(ti.domain()->type().bt());
        tt.enumId(ti.domain()->type().enumId());
      } else {
//        assert(ti.domain()==NULL || ti.domain()->isa<TIId>());
      }
      if (foundEnum) {
        std::vector<unsigned int> enumIds(ti.ranges().size()+1);
        for (unsigned int i=0; i<ti.ranges().size(); i++) {
          enumIds[i] = ti.ranges()[i]->type().enumId();
        }
        enumIds[ti.ranges().size()] = ti.domain() ? ti.domain()->type().enumId() : 0;
        int arrayEnumId = _env.registerArrayEnum(enumIds);
        tt.enumId(arrayEnumId);
      }

      if (tt.st()==Type::ST_SET && tt.ti()==Type::TI_VAR && tt.bt() != Type::BT_INT && tt.bt() != Type::BT_TOP)
        throw TypeError(_env,ti.loc(), "var set element types other than `int' not allowed");
      ti.type(tt);
    }
    void vTIId(TIId& id) {}
  };
  
  void typecheck(Env& env, Model* origModel, std::vector<TypeError>& typeErrors, bool ignoreUndefinedParameters, bool allowMultiAssignment, bool isFlatZinc) {
    Model* m;
    if (!isFlatZinc && origModel==env.model()) {
      // Combine all items into single model
      Model* combinedModel = new Model;
      class Combiner : public ItemVisitor {
      public:
        Model* m;
        Combiner(Model* m0) : m(m0) {}
        bool enter(Item* i) {
          if (!i->isa<IncludeI>())
            m->addItem(i);
          return true;
        }
      } _combiner(combinedModel);
      iterItems(_combiner, origModel);
      env.envi().orig_model = origModel;
      env.envi().model = combinedModel;
      m=combinedModel;
    } else {
      m = origModel;
    }
    
    // Topological sorting
    TopoSorter ts(m);
    
    std::vector<FunctionI*> functionItems;
    std::vector<AssignI*> assignItems;
    Model* enumItems = new Model;

    class TSVFuns : public ItemVisitor {
    public:
      EnvI& env;
      Model* model;
      std::vector<FunctionI*>& fis;
      TSVFuns(EnvI& env0, Model* model0, std::vector<FunctionI*>& fis0)
      : env(env0), model(model0), fis(fis0) {}
      void vFunctionI(FunctionI* i) {
        model->registerFn(env, i);
        fis.push_back(i);
      }
    } _tsvf(env.envi(),m,functionItems);
    iterItems(_tsvf,m);

    class TSV0 : public ItemVisitor {
    public:
      EnvI& env;
      TopoSorter& ts;
      Model* model;
      bool hadSolveItem;
      std::vector<AssignI*>& ais;
      VarDeclI* objective;
      Model* enumis;
      TSV0(EnvI& env0, TopoSorter& ts0, Model* model0, std::vector<AssignI*>& ais0,
           Model* enumis0)
        : env(env0), ts(ts0), model(model0), hadSolveItem(false), ais(ais0), objective(NULL), enumis(enumis0) {}
      void vAssignI(AssignI* i) { ais.push_back(i); }
      void vVarDeclI(VarDeclI* i) { ts.add(env, i, true, enumis); }
      void vSolveI(SolveI* si) {
        if (hadSolveItem)
          throw TypeError(env,si->loc(),"Only one solve item allowed");
        hadSolveItem = true;
        if (si->e()) {
          GCLock lock;
          TypeInst* ti = new TypeInst(Location().introduce(), Type());
          VarDecl* obj = new VarDecl(Location().introduce(), ti, "_objective", si->e());
          si->e(obj->id());
          objective = new VarDeclI(Location().introduce(), obj);
        }
        
      }
    } _tsv0(env.envi(),ts,m,assignItems,enumItems);
    iterItems(_tsv0,m);
    if (_tsv0.objective) {
      m->addItem(_tsv0.objective);
      ts.add(env.envi(), _tsv0.objective, true, enumItems);
    }

    for (unsigned int i=0; i<enumItems->size(); i++) {
      if (AssignI* ai = (*enumItems)[i]->dyn_cast<AssignI>()) {
        assignItems.push_back(ai);
      } else if (VarDeclI* vdi = (*enumItems)[i]->dyn_cast<VarDeclI>()) {
        m->addItem(vdi);
        ts.add(env.envi(), vdi, false, enumItems);
      } else {
        FunctionI* fi = (*enumItems)[i]->dyn_cast<FunctionI>();
        m->addItem(fi);
        m->registerFn(env.envi(),fi);
        functionItems.push_back(fi);
      }
    }

    Model* enumItems2 = new Model;
    
    for (unsigned int i=0; i<assignItems.size(); i++) {
      AssignI* ai = assignItems[i];
      VarDecl* vd = NULL;
      if (env.envi().ignoreUnknownIds) {
        try {
          vd = ts.get(env.envi(),ai->id(),ai->loc());
        } catch (TypeError&) {}
      } else {
        vd = ts.get(env.envi(),ai->id(),ai->loc());
      }
      if (vd) {
        if (vd->e()) {
          if (allowMultiAssignment) {
            GCLock lock;
            m->addItem(new ConstraintI(ai->loc(),
                                       new BinOp(ai->loc(),
                                                 new Id(Location().introduce(),ai->id(),vd), BOT_EQ, ai->e())));
          } else {
            throw TypeError(env.envi(),ai->loc(),"multiple assignment to the same variable");
          }
        } else {
          vd->e(ai->e());
          vd->ann().add(constants().ann.rhs_from_assignment);
          if (vd->ti()->isEnum()) {
            GCLock lock;
            ASTString name(createEnumToStringName(vd->id(),"_enum_to_string_"));
            VarDecl* vd_enum = ts.get(env.envi(),name,vd->loc());
            if (vd_enum->e())
              throw TypeError(env.envi(),ai->loc(),"multiple definition of the same enum");
            AssignI* ai_enum = createEnumMapper(env.envi(), m, vd->ti()->type().enumId(), vd, vd_enum, enumItems2);
            if (ai_enum) {
              vd_enum->e(ai_enum->e());
              ai_enum->remove();
            }
          }
        }
      }
      ai->remove();
    }
    
    for (unsigned int i=0; i<enumItems2->size(); i++) {
      if (VarDeclI* vdi = (*enumItems2)[i]->dyn_cast<VarDeclI>()) {
        m->addItem(vdi);
        ts.add(env.envi(), vdi, false, enumItems);
      } else {
        FunctionI* fi = (*enumItems2)[i]->cast<FunctionI>();
        m->addItem(fi);
        m->registerFn(env.envi(),fi);
        functionItems.push_back(fi);
      }
    }
    
    delete enumItems;
    delete enumItems2;
    
    class TSV1 : public ItemVisitor {
    public:
      EnvI& env;
      TopoSorter& ts;
      TSV1(EnvI& env0, TopoSorter& ts0) : env(env0), ts(ts0) {}
      void vVarDeclI(VarDeclI* i) { ts.run(env,i->e()); }
      void vAssignI(AssignI* i) {}
      void vConstraintI(ConstraintI* i) { ts.run(env,i->e()); }
      void vSolveI(SolveI* i) {
        for (ExpressionSetIter it = i->ann().begin(); it != i->ann().end(); ++it)
          ts.run(env,*it);
        ts.run(env,i->e());
      }
      void vOutputI(OutputI* i) { ts.run(env,i->e()); }
      void vFunctionI(FunctionI* fi) {
        ts.run(env,fi->ti());
        for (unsigned int i=0; i<fi->params().size(); i++)
          ts.run(env,fi->params()[i]);
        for (ExpressionSetIter it = fi->ann().begin(); it != fi->ann().end(); ++it)
          ts.run(env,*it);
        ts.scopes.push(false);
        for (unsigned int i=0; i<fi->params().size(); i++)
          ts.scopes.add(env,fi->params()[i]);
        ts.run(env,fi->e());
        ts.scopes.pop();
      }
    } _tsv1(env.envi(),ts);
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
      Typer<false> ty(env.envi(), m, typeErrors, ignoreUndefinedParameters);
      BottomUpIterator<Typer<false> > bu_ty(ty);
      for (unsigned int i=0; i<ts.decls.size(); i++) {
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
    
    m->fixFnMap();
    
    {
      Typer<true> ty(env.envi(), m, typeErrors, ignoreUndefinedParameters);
      BottomUpIterator<Typer<true> > bu_ty(ty);
      
      class TSV2 : public ItemVisitor {
      public:
        EnvI& env;
        Model* m;
        BottomUpIterator<Typer<true> >& bu_ty;
        std::vector<TypeError>& _typeErrors;
        TSV2(EnvI& env0, Model* m0,
             BottomUpIterator<Typer<true> >& b,
             std::vector<TypeError>& typeErrors) : env(env0), m(m0), bu_ty(b), _typeErrors(typeErrors) {}
        void vVarDeclI(VarDeclI* i) {
          bu_ty.run(i->e());
          if (i->e()->ti()->hasTiVariable()) {
            _typeErrors.push_back(TypeError(env, i->e()->loc(),
                                            "type-inst variables not allowed in type-inst for `"+i->e()->id()->str().str()+"'"));
          }
          VarDecl* vdi = i->e();
          if (vdi->e()==NULL && vdi->type().is_set() && vdi->type().isvar() &&
              vdi->ti()->domain()==NULL) {
            _typeErrors.push_back(TypeError(env,vdi->loc(),
                                            "set element type for `"+vdi->id()->str().str()+"' is not finite"));
          }
          if (i->e()->ann().contains(constants().ann.output_only) && vdi->e()->type().isvar()) {
            _typeErrors.push_back(TypeError(env,vdi->loc(),"variables annotated with ::output_only must be par"));
          }
        }
        void vAssignI(AssignI* i) {
          bu_ty.run(i->e());
          if (!env.isSubtype(i->e()->type(),i->decl()->ti()->type(),true)) {
            _typeErrors.push_back(TypeError(env, i->loc(),
                                           "assignment value for `"+i->decl()->id()->str().str()+"' has invalid type-inst: expected `"+
                                           i->decl()->ti()->type().toString(env)+"', actual `"+i->e()->type().toString(env)+"'"));
            // Assign to "true" constant to avoid generating further errors that the parameter
            // is undefined
            i->decl()->e(constants().lit_true);
          }
        }
        void vConstraintI(ConstraintI* i) {
          bu_ty.run(i->e());
          if (!env.isSubtype(i->e()->type(),Type::varbool(),true))
            throw TypeError(env, i->loc(), "invalid type of constraint, expected `"
                            +Type::varbool().toString(env)+"', actual `"+i->e()->type().toString(env)+"'");
        }
        void vSolveI(SolveI* i) {
          for (ExpressionSetIter it = i->ann().begin(); it != i->ann().end(); ++it) {
            bu_ty.run(*it);
            if (!(*it)->type().isann())
              throw TypeError(env, (*it)->loc(), "expected annotation, got `"+(*it)->type().toString(env)+"'");
          }
          bu_ty.run(i->e());
          if (i->e()) {
            Type et = i->e()->type();

            bool needOptCoercion = et.isopt() && et.isint();
            if (needOptCoercion) {
              et.ot(Type::OT_PRESENT);
            }
            
            if (! (env.isSubtype(et,Type::varint(),true) ||
                   env.isSubtype(et,Type::varfloat(),true)))
              throw TypeError(env, i->e()->loc(),
                "objective has invalid type, expected int or float, actual `"+et.toString(env)+"'");

            if (needOptCoercion) {
              GCLock lock;
              std::vector<Expression*> args(2);
              args[0] = i->e();
              args[1] = constants().boollit(i->st()==SolveI::ST_MAX);
              Call* c = new Call(Location().introduce(), ASTString("objective_deopt_"), args);
              c->decl(env.model->matchFn(env, c, false));
              assert(c->decl());
              c->type(et);
              i->e(c);
            }
          }
        }
        void vOutputI(OutputI* i) {
          bu_ty.run(i->e());
          if (i->e()->type() != Type::parstring(1) && i->e()->type() != Type::bot(1))
            throw TypeError(env, i->e()->loc(), "invalid type in output item, expected `"
                            +Type::parstring(1).toString(env)+"', actual `"+i->e()->type().toString(env)+"'");
        }
        void vFunctionI(FunctionI* i) {
          for (ExpressionSetIter it = i->ann().begin(); it != i->ann().end(); ++it) {
            bu_ty.run(*it);
            if (!(*it)->type().isann())
              throw TypeError(env, (*it)->loc(), "expected annotation, got `"+(*it)->type().toString(env)+"'");
          }
          bu_ty.run(i->ti());
          bu_ty.run(i->e());
          if (i->e() && !env.isSubtype(i->e()->type(),i->ti()->type(),true))
            throw TypeError(env, i->e()->loc(), "return type of function does not match body, declared type is `"
                            +i->ti()->type().toString(env)+
                            "', body type is `"+i->e()->type().toString(env)+"'");
          if (i->e() && i->e()->type().ispar() && i->ti()->type().isvar()) {
            // this is a par function declared as var, so change declared return type
            Type i_t = i->ti()->type();
            i_t.ti(Type::TI_PAR);
            i->ti()->type(i_t);
          }
          if (i->e())
            i->e(addCoercion(env, m, i->e(), i->ti()->type())());
        }
      } _tsv2(env.envi(), m, bu_ty, typeErrors);
      iterItems(_tsv2,m);
    }
    
    class TSV3 : public ItemVisitor {
    public:
      EnvI& env;
      Model* m;
      OutputI* outputItem;
      TSV3(EnvI& env0, Model* m0) : env(env0), m(m0), outputItem(NULL) {}
      void vAssignI(AssignI* i) {
        i->decl()->e(addCoercion(env, m, i->e(), i->decl()->type())());
      }
      void vOutputI(OutputI* oi) {
        if (outputItem==NULL) {
          outputItem = oi;
        } else {
          GCLock lock;
          BinOp* bo = new BinOp(Location().introduce(),outputItem->e(),BOT_PLUSPLUS,oi->e());
          bo->type(Type::parstring(1));
          outputItem->e(bo);
          oi->remove();
          m->setOutputItem(outputItem);
        }
      }
    } _tsv3(env.envi(),m);
    if (typeErrors.empty()) {
      iterItems(_tsv3,m);
    }

    try {
      m->checkFnOverloading(env.envi());
    } catch (TypeError& e) {
      typeErrors.push_back(e);
    }
    
    for (unsigned int i=0; i<ts.decls.size(); i++) {
      if (ts.decls[i]->toplevel() &&
          ts.decls[i]->type().ispar() && !ts.decls[i]->type().isann() && ts.decls[i]->e()==NULL) {
        if (ts.decls[i]->type().isopt()) {
          ts.decls[i]->e(constants().absent);
        } else if (!ignoreUndefinedParameters) {
          typeErrors.push_back(TypeError(env.envi(), ts.decls[i]->loc(),
                                         "  symbol error: variable `" + ts.decls[i]->id()->str().str()
                                         + "' must be defined (did you forget to specify a data file?)"));
        }
      }
      if (ts.decls[i]->ti()->isEnum()) {
        ts.decls[i]->ti()->setIsEnum(false);
        Type vdt = ts.decls[i]->ti()->type();
        vdt.enumId(0);
        ts.decls[i]->ti()->type(vdt);
      }
    }

    for (auto vd_k : env.envi().checkVars) {
      try {
        VarDecl* vd = ts.get(env.envi(), vd_k()->cast<VarDecl>()->id()->str(), vd_k()->cast<VarDecl>()->loc());
        vd->ann().add(constants().ann.mzn_check_var);
        if (vd->type().enumId() != 0) {
          GCLock lock;
          int enumId = vd->type().enumId();
          if (vd->type().dim() > 0) {
            const std::vector<unsigned int>& arrayEnumIds = env.envi().getArrayEnum(vd->type().enumId());
            enumId = arrayEnumIds[arrayEnumIds.size()-1];
          }
          if (enumId > 0) {
            std::vector<Expression*> args({env.envi().getEnum(enumId)->e()->id()});
            Call* checkEnum = new Call(Location().introduce(), constants().ann.mzn_check_enum_var, args);
            checkEnum->type(Type::ann());
            checkEnum->decl(env.envi().model->matchFn(env.envi(), checkEnum, false));
            vd->ann().add(checkEnum);
          }
        }
        Type vdktype = vd_k()->type();
        vdktype.ti(Type::TI_VAR);
        if (!vd_k()->type().isSubtypeOf(vd->type(), false)) {
          GCLock lock;
          
          typeErrors.push_back(TypeError(env.envi(), vd->loc(),
                                         "Solution checker requires `"+vd->id()->str().str()+"' to be of type `"+
                                         vdktype.toString(env.envi())+"'"));
        }
      } catch (TypeError& e) {
        typeErrors.push_back(TypeError(env.envi(), e.loc(), e.msg()+" (required by solution checker model)"));
      }
    }
    
  }
  
  void typecheck(Env& env, Model* m, AssignI* ai) {
    std::vector<TypeError> typeErrors;
    Typer<true> ty(env.envi(), m, typeErrors, false);
    BottomUpIterator<Typer<true> > bu_ty(ty);
    bu_ty.run(ai->e());
    if (!typeErrors.empty()) {
      throw typeErrors[0];
    }
    if (!env.envi().isSubtype(ai->e()->type(),ai->decl()->ti()->type(),true)) {
      throw TypeError(env.envi(), ai->e()->loc(),
                      "assignment value for `"+ai->decl()->id()->str().str()+"' has invalid type-inst: expected `"+
                      ai->decl()->ti()->type().toString(env.envi())+"', actual `"+ai->e()->type().toString(env.envi())+"'");
    }
    
  }

  void output_var_desc_json(Env& env, VarDecl* vd, std::ostream& os, bool extra = false) {
    os << "    \"" << *vd->id() << "\" : {";
    os << "\"type\" : ";
    switch (vd->type().bt()) {
      case Type::BT_INT: os << "\"int\""; break;
      case Type::BT_BOOL: os << "\"bool\""; break;
      case Type::BT_FLOAT: os << "\"float\""; break;
      case Type::BT_STRING: os << "\"string\""; break;
      case Type::BT_ANN: os << "\"ann\""; break;
      default: os << "\"?\""; break;
    }
    if (vd->type().ot()==Type::OT_OPTIONAL) {
      os << ", \"optional\" : true";
    }
    if (vd->type().st()==Type::ST_SET) {
      os << ", \"set\" : true";
    }
    if (vd->type().dim() > 0) {
      os << ", \"dim\" : " << vd->type().dim();

      if(extra) {
        os << ", \"dims\" : [";
        bool had_dim = false;
        ASTExprVec<TypeInst> ranges = vd->ti()->ranges();
        for(int i=0; i<static_cast<int>(ranges.size()); i++) {
          if(ranges[i]->type().enumId() > 0) {
            os << (had_dim ? "," : "")
               << "\"" << *env.envi().getEnum(ranges[i]->type().enumId())->e()->id() << "\"";
          } else {
            os << (had_dim ? "," : "") << "\"int\"";
          }
          had_dim = true;
        }
        os << "]";

        if (vd->type().enumId() > 0) {
          const std::vector<unsigned int>& enumIds = env.envi().getArrayEnum(vd->type().enumId());
          if(enumIds.back() > 0) {
            os << ", \"enum_type\" : \"" << *env.envi().getEnum(enumIds.back())->e()->id() << "\"";
          }
        }
      }

    } else {
      if(extra) {
        if (vd->type().enumId() > 0) {
          os << ", \"enum_type\" : \"" << *env.envi().getEnum(vd->type().enumId())->e()->id() << "\"";
        }
      }
    }
    os << "}";
  }

  void output_model_variable_types(Env& env, Model* m, std::ostream& os) {
    class VInfVisitor : public ItemVisitor {
    public:
      Env& env;
      bool had_var;
      bool had_enum;
      std::ostringstream oss_vars;
      std::ostringstream oss_enums;
      VInfVisitor(Env& env0) : env(env0), had_var(false), had_enum(false) {}
      bool enter(Item* i) {
        if (IncludeI* ii = i->dyn_cast<IncludeI>()) {
          std::string prefix = ii->m()->filepath().str().substr(0,ii->m()->filepath().size()-ii->f().size());
          return (prefix.empty() || prefix == "./");
        }
        return true;
      }
      void vVarDeclI(VarDeclI* vdi) {
        if (!vdi->e()->type().isann() && !vdi->e()->ti()->isEnum()) {
          if (had_var) oss_vars << ",\n";
          output_var_desc_json(env, vdi->e(), oss_vars, true);
          had_var = true;
        } else if (vdi->e()->type().st()==Type::ST_SET && vdi->e()->type().enumId() != 0 && !vdi->e()->type().isann()) {
          if (had_enum) oss_enums << ", ";
          oss_enums << "\"" << *env.envi().getEnum(vdi->e()->type().enumId())->e()->id() << "\"";
          had_enum = true;
        }
      }
    } _vinf(env);
    iterItems(_vinf, m);
    os << "{\"var_types\": {";
    os << "\n  \"vars\": {\n" << _vinf.oss_vars.str() << "\n  },";
    os << "\n  \"enums\": [" << _vinf.oss_enums.str() << "]\n";
    os << "}}\n";
  }


  void output_model_interface(Env& env, Model* m, std::ostream& os) {
    class IfcVisitor : public ItemVisitor {
    public:
      Env& env;
      bool had_input;
      bool had_output;
      std::ostringstream oss_input;
      std::ostringstream oss_output;
      std::string method;
      IfcVisitor(Env& env0) : env(env0), had_input(false), had_output(false), method("sat") {}
      bool enter(Item* i) {
        if (IncludeI* ii = i->dyn_cast<IncludeI>()) {
          std::string prefix = ii->m()->filepath().str().substr(0,ii->m()->filepath().size()-ii->f().size());
          return (prefix.empty() || prefix == "./");
        }
        return true;
      }
      void vVarDeclI(VarDeclI* vdi) {
        if (vdi->e()->type().ispar() && !vdi->e()->type().isann() && (vdi->e()->e()==NULL || vdi->e()->e()==constants().absent)) {
          if (had_input) oss_input << ",\n";
          output_var_desc_json(env, vdi->e(), oss_input);
          had_input = true;
        } else if (vdi->e()->type().isvar() && (vdi->e()->e()==NULL || vdi->e()->ann().contains(constants().ann.add_to_output))) {
          if (had_output) oss_output << ",\n";
          output_var_desc_json(env, vdi->e(), oss_output);
          had_output = true;
        }
      }
      void vSolveI(SolveI* si) {
        switch (si->st()) {
          case SolveI::ST_MIN: method = "min"; break;
          case SolveI::ST_MAX: method = "max"; break;
          case SolveI::ST_SAT: method = "sat"; break;
        }
      }
    } _ifc(env);
    iterItems(_ifc, m);
    os << "{\n  \"input\" : {\n" << _ifc.oss_input.str() << "\n  },\n  \"output\" : {\n" << _ifc.oss_output.str() << "\n  }";
    os << ",\n  \"method\": \"";
    os << _ifc.method;
    os << "\"";
    os << "\n}\n";
  }
  
}
