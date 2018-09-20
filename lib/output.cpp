/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/output.hh>
#include <minizinc/astiterator.hh>

namespace MiniZinc {

  void outputVarDecls(EnvI& env, Item* ci, Expression* e);
  
  bool cannotUseRHSForOutput(EnvI& env, Expression* e, std::unordered_set<FunctionI*>& seen_functions) {
    if (e==NULL)
      return true;
    
    class V : public EVisitor {
    public:
      EnvI& env;
      std::unordered_set<FunctionI*>& seen_functions;
      bool success;
      V(EnvI& env0, std::unordered_set<FunctionI*>& seen_functions0) : env(env0), seen_functions(seen_functions0), success(true) {}
      /// Visit anonymous variable
      void vAnonVar(const AnonVar&) { success = false; }
      /// Visit array literal
      void vArrayLit(const ArrayLit&) {}
      /// Visit array access
      void vArrayAccess(const ArrayAccess&) {}
      /// Visit array comprehension
      void vComprehension(const Comprehension&) {}
      /// Visit if-then-else
      void vITE(const ITE&) {}
      /// Visit binary operator
      void vBinOp(const BinOp&) {}
      /// Visit unary operator
      void vUnOp(const UnOp&) {}
      /// Visit call
      void vCall(Call& c) {
        std::vector<Type> tv(c.n_args());
        for (unsigned int i=c.n_args(); i--;) {
          tv[i] = c.arg(i)->type();
          tv[i].ti(Type::TI_PAR);
        }
        FunctionI* decl = env.output->matchFn(env,c.id(), tv, false);
        Type t;
        if (decl==NULL) {
          FunctionI* origdecl = env.model->matchFn(env, c.id(), tv, false);
          if (origdecl == NULL) {
            throw FlatteningError(env,c.loc(),"function "+c.id().str()+" is used in output, par version needed");
          }
          bool seen = (seen_functions.find(origdecl) != seen_functions.end());
          if (seen) {
            success = false;
          } else {
            seen_functions.insert(origdecl);
            if (origdecl->e() && cannotUseRHSForOutput(env, origdecl->e(), seen_functions)) {
              success = false;
            } else {
              if (!origdecl->from_stdlib()) {
                decl = copy(env,env.cmap,origdecl)->cast<FunctionI>();
                CollectOccurrencesE ce(env.output_vo,decl);
                topDown(ce, decl->e());
                topDown(ce, decl->ti());
                for (unsigned int i = decl->params().size(); i--;)
                  topDown(ce, decl->params()[i]);
                env.output->registerFn(env, decl);
                env.output->addItem(decl);
                outputVarDecls(env,origdecl,decl->e());
                outputVarDecls(env,origdecl,decl->ti());
              } else {
                decl = origdecl;
              }
              c.decl(decl);
            }
          }
        }
        if (success) {
          t = decl->rtype(env, tv, false);
          if (!t.ispar())
            success = false;
        }
      }
      void vId(const Id& id) {}
      /// Visit let
      void vLet(const Let&) { success = false; }
      /// Visit variable declaration
      void vVarDecl(const VarDecl& vd) {}
      /// Visit type inst
      void vTypeInst(const TypeInst&) {}
      /// Visit TIId
      void vTIId(const TIId&) {}
      /// Determine whether to enter node
      bool enter(Expression* e) { return success; }
    } _v(env, seen_functions);
    topDown(_v, e);
    
    return !_v.success;
  }
  
  bool cannotUseRHSForOutput(EnvI& env, Expression* e) {
    std::unordered_set<FunctionI*> seen_functions;
    return cannotUseRHSForOutput(env, e, seen_functions);
  }

  void removeIsOutput(VarDecl* vd) {
    if (vd==NULL)
      return;
    vd->ann().remove(constants().ann.output_var);
    vd->ann().removeCall(constants().ann.output_array);
  }
  
  void copyOutput(EnvI& e) {
    struct CopyOutput : public EVisitor {
      EnvI& env;
      CopyOutput(EnvI& env0) : env(env0) {}
      void vId(Id& _id) {
        _id.decl(_id.decl()->flat());
      }
      void vCall(Call& c) {
        std::vector<Type> tv(c.n_args());
        for (unsigned int i=c.n_args(); i--;) {
          tv[i] = c.arg(i)->type();
          tv[i].ti(Type::TI_PAR);
        }
        FunctionI* decl = c.decl();
        if (!decl->from_stdlib()) {
          env.flat_addItem(decl);
        }
      }
    };
    
    if (OutputI* oi = e.model->outputItem()) {
      GCLock lock;
      OutputI* noi = copy(e,oi)->cast<OutputI>();
      CopyOutput co(e);
      topDown(co, noi->e());
      e.flat_addItem(noi);
    }
  }
  
  void cleanupOutput(EnvI& env) {
    for (unsigned int i=0; i<env.output->size(); i++) {
      if (VarDeclI* vdi = (*env.output)[i]->dyn_cast<VarDeclI>()) {
        vdi->e()->flat(NULL);
      }
    }
  }
  
  void makePar(EnvI& env, Expression* e) {
    class Par : public EVisitor {
    public:
      /// Visit variable declaration
      void vVarDecl(VarDecl& vd) {
        vd.ti()->type(vd.type());
      }
      /// Determine whether to enter node
      bool enter(Expression* e) {
        Type t = e->type();
        t.ti(Type::TI_PAR);
        e->type(t);
        return true;
      }
    } _par;
    topDown(_par, e);
    class Decls : public EVisitor {
    protected:
      static std::string createEnumToStringName(Id* ident, std::string prefix) {
        std::string name = ident->str().str();
        if (name[0]=='\'') {
          name = "'"+prefix+name.substr(1);
        } else {
          name = prefix+name;
        }
        return name;
      }
      
    public:
      EnvI& env;
      Decls(EnvI& env0) : env(env0) {}
      void vCall(Call& c) {
        if (c.id()=="format" || c.id()=="show" || c.id()=="showDzn") {
          int enumId = c.arg(c.n_args()-1)->type().enumId();
          if (enumId != 0 && c.arg(c.n_args()-1)->type().dim() != 0) {
            const std::vector<unsigned int>& enumIds = env.getArrayEnum(enumId);
            enumId = enumIds[enumIds.size()-1];
          }
          if (enumId > 0) {
            Id* ti_id = env.getEnum(enumId)->e()->id();
            GCLock lock;
            std::vector<Expression*> args(2);
            args[0] = c.arg(c.n_args()-1);
            if (args[0]->type().dim() > 1) {
              std::vector<Expression*> a1dargs(1);
              a1dargs[0] = args[0];
              Call* array1d = new Call(Location().introduce(),ASTString("array1d"),a1dargs);
              Type array1dt = args[0]->type();
              array1dt.dim(1);
              array1d->type(array1dt);
              args[0] = array1d;
            }
            args[1] = constants().boollit(c.id()=="showDzn");
            std::string enumName = createEnumToStringName(ti_id, "_toString_");
            c.id(ASTString(enumName));
            c.args(args);
          }
          if (c.id()=="showDzn") {
            c.id(constants().ids.show);
          }
        }
        c.decl(env.model->matchFn(env,&c,false));
      }
    } _decls(env);
    topDown(_decls, e);
  }
  
  void checkRenameVar(EnvI& e, VarDecl* vd) {
    if (vd->id()->idn() != vd->flat()->id()->idn()) {
      TypeInst* vd_rename_ti = copy(e,e.cmap,vd->ti())->cast<TypeInst>();
      VarDecl* vd_rename = new VarDecl(Location().introduce(), vd_rename_ti, vd->flat()->id()->idn(), NULL);
      vd_rename->flat(vd->flat());
      makePar(e,vd_rename);
      vd->e(vd_rename->id());
      e.output->addItem(new VarDeclI(Location().introduce(), vd_rename));
    }
  }

  class ClearAnnotations {
  public:
    /// Push all elements of \a v onto \a stack
    template<class E>
    static void pushVec(std::vector<Expression*>& stack, ASTExprVec<E> v) {
      for (unsigned int i=0; i<v.size(); i++)
        stack.push_back(v[i]);
    }

    static void run(Expression* root) {
      std::vector<Expression*> stack;
      stack.push_back(root);
      while (!stack.empty()) {
        Expression* e = stack.back();
        stack.pop_back();
        if (e==NULL) {
          continue;
        }
        e->ann().clear();
        switch (e->eid()) {
          case Expression::E_INTLIT:
          case Expression::E_FLOATLIT:
          case Expression::E_BOOLLIT:
          case Expression::E_STRINGLIT:
          case Expression::E_ID:
          case Expression::E_ANON:
          case Expression::E_TIID:
            break;
          case Expression::E_SETLIT:
            pushVec(stack, e->template cast<SetLit>()->v());
            break;
          case Expression::E_ARRAYLIT:
            for (unsigned int i=0; i<e->cast<ArrayLit>()->size(); i++) {
              stack.push_back((*e->cast<ArrayLit>())[i]);
            }
            break;
          case Expression::E_ARRAYACCESS:
            pushVec(stack, e->template cast<ArrayAccess>()->idx());
            stack.push_back(e->template cast<ArrayAccess>()->v());
            break;
          case Expression::E_COMP:
          {
            Comprehension* comp = e->template cast<Comprehension>();
            for (unsigned int i=comp->n_generators(); i--; ) {
              stack.push_back(comp->where(i));
              stack.push_back(comp->in(i));
              for (unsigned int j=comp->n_decls(i); j--; ) {
                stack.push_back(comp->decl(i, j));
              }
            }
            stack.push_back(comp->e());
          }
            break;
          case Expression::E_ITE:
          {
            ITE* ite = e->template cast<ITE>();
            stack.push_back(ite->e_else());
            for (int i=0; i<ite->size(); i++) {
              stack.push_back(ite->e_if(i));
              stack.push_back(ite->e_then(i));
            }
          }
            break;
          case Expression::E_BINOP:
            stack.push_back(e->template cast<BinOp>()->rhs());
            stack.push_back(e->template cast<BinOp>()->lhs());
            break;
          case Expression::E_UNOP:
            stack.push_back(e->template cast<UnOp>()->e());
            break;
          case Expression::E_CALL:
            for (unsigned int i=0; i<e->template cast<Call>()->n_args(); i++)
              stack.push_back(e->template cast<Call>()->arg(i));
            break;
          case Expression::E_VARDECL:
            stack.push_back(e->template cast<VarDecl>()->e());
            stack.push_back(e->template cast<VarDecl>()->ti());
            break;
          case Expression::E_LET:
            stack.push_back(e->template cast<Let>()->in());
            pushVec(stack, e->template cast<Let>()->let());
            break;
          case Expression::E_TI:
            stack.push_back(e->template cast<TypeInst>()->domain());
            pushVec(stack,e->template cast<TypeInst>()->ranges());
            break;
        }
      }
      
    }
  };
  
  void outputVarDecls(EnvI& env, Item* ci, Expression* e) {
    class O : public EVisitor {
    public:
      EnvI& env;
      Item* ci;
      O(EnvI& env0, Item* ci0) : env(env0), ci(ci0) {}
      void vId(Id& id) {
        if (&id==constants().absent)
          return;
        if (!id.decl()->toplevel())
          return;
        VarDecl* vd = id.decl();
        VarDecl* reallyFlat = vd->flat();
        while (reallyFlat != NULL && reallyFlat != reallyFlat->flat())
          reallyFlat = reallyFlat->flat();
        IdMap<int>::iterator idx = reallyFlat ? env.output_vo_flat.idx.find(reallyFlat->id()) : env.output_vo_flat.idx.end();
        IdMap<int>::iterator idx2 = env.output_vo.idx.find(vd->id());
        if (idx==env.output_vo_flat.idx.end() && idx2==env.output_vo.idx.end()) {
          VarDeclI* nvi = new VarDeclI(Location().introduce(), copy(env,env.cmap,vd)->cast<VarDecl>());
          Type t = nvi->e()->ti()->type();
          if (t.ti() != Type::TI_PAR) {
            t.ti(Type::TI_PAR);
          }
          makePar(env,nvi->e());
          nvi->e()->ti()->domain(NULL);
          nvi->e()->flat(vd->flat());
          ClearAnnotations::run(nvi->e());
          nvi->e()->introduced(false);
          if (reallyFlat)
            env.output_vo_flat.add_idx(reallyFlat, env.output->size());
          env.output_vo.add_idx(nvi, env.output->size());
          env.output_vo.add(nvi->e(), ci);
          env.output->addItem(nvi);
          
          IdMap<KeepAlive>::iterator it;
          if ( (it = env.reverseMappers.find(nvi->e()->id())) != env.reverseMappers.end()) {
            Call* rhs = copy(env,env.cmap,it->second())->cast<Call>();
            {
              std::vector<Type> tv(rhs->n_args());
              for (unsigned int i=rhs->n_args(); i--;) {
                tv[i] = rhs->arg(i)->type();
                tv[i].ti(Type::TI_PAR);
              }
              FunctionI* decl = env.output->matchFn(env, rhs->id(), tv, false);
              Type t;
              if (decl==NULL) {
                FunctionI* origdecl = env.model->matchFn(env, rhs->id(), tv, false);
                if (origdecl == NULL) {
                  throw FlatteningError(env,rhs->loc(),"function "+rhs->id().str()+" is used in output, par version needed");
                }
                if (!origdecl->from_stdlib()) {
                  decl = copy(env,env.cmap,origdecl)->cast<FunctionI>();
                  CollectOccurrencesE ce(env.output_vo,decl);
                  topDown(ce, decl->e());
                  topDown(ce, decl->ti());
                  for (unsigned int i = decl->params().size(); i--;)
                    topDown(ce, decl->params()[i]);
                  env.output->registerFn(env, decl);
                  env.output->addItem(decl);
                } else {
                  decl = origdecl;
                }
              }
              rhs->decl(decl);
            }
            outputVarDecls(env,nvi,it->second());
            nvi->e()->e(rhs);
          } else if (reallyFlat && cannotUseRHSForOutput(env, reallyFlat->e())) {
            assert(nvi->e()->flat());
            nvi->e()->e(NULL);
            if (nvi->e()->type().dim() == 0) {
              reallyFlat->addAnnotation(constants().ann.output_var);
            } else {
              std::vector<Expression*> args(reallyFlat->e()->type().dim());
              for (unsigned int i=0; i<args.size(); i++) {
                if (nvi->e()->ti()->ranges()[i]->domain() == NULL) {
                  args[i] = new SetLit(Location().introduce(), eval_intset(env,reallyFlat->ti()->ranges()[i]->domain()));
                } else {
                  args[i] = new SetLit(Location().introduce(), eval_intset(env,nvi->e()->ti()->ranges()[i]->domain()));
                }
              }
              ArrayLit* al = new ArrayLit(Location().introduce(), args);
              args.resize(1);
              args[0] = al;
              reallyFlat->addAnnotation(new Call(Location().introduce(),constants().ann.output_array,args));
            }
            checkRenameVar(env, nvi->e());
          } else {
            outputVarDecls(env, nvi, nvi->e()->ti());
            outputVarDecls(env, nvi, nvi->e()->e());
          }
          CollectOccurrencesE ce(env.output_vo,nvi);
          topDown(ce, nvi->e());
        }
      }
    } _o(env,ci);
    topDown(_o, e);
  }

  void processDeletions(EnvI& e, std::vector<VarDecl*>& deletedFlatVarDecls) {
    std::vector<VarDecl*> deletedVarDecls;
    for (unsigned int i=0; i<e.output->size(); i++) {
      if (VarDeclI* vdi = (*e.output)[i]->dyn_cast<VarDeclI>()) {
        if (!vdi->removed() && e.output_vo.occurrences(vdi->e())==0 &&
            !vdi->e()->ann().contains(constants().ann.mzn_check_var) &&
            !(vdi->e()->id()->idn()==-1 && vdi->e()->id()->v()=="_mzn_solution_checker")) {
          CollectDecls cd(e.output_vo,deletedVarDecls,vdi);
          topDown(cd, vdi->e()->e());
          removeIsOutput(vdi->e()->flat());
          if (e.output_vo.find(vdi->e())!=-1)
            e.output_vo.remove(vdi->e());
          vdi->remove();
        }
      }
    }
    while (!deletedVarDecls.empty()) {
      VarDecl* cur = deletedVarDecls.back(); deletedVarDecls.pop_back();
      if (e.output_vo.occurrences(cur) == 0) {
        IdMap<int>::iterator cur_idx = e.output_vo.idx.find(cur->id());
        if (cur_idx != e.output_vo.idx.end()) {
          VarDeclI* vdi = (*e.output)[cur_idx->second]->cast<VarDeclI>();
          if (!vdi->removed()) {
            CollectDecls cd(e.output_vo,deletedVarDecls,vdi);
            topDown(cd,cur->e());
            removeIsOutput(vdi->e()->flat());
            if (e.output_vo.find(vdi->e())!=-1)
              e.output_vo.remove(vdi->e());
            vdi->remove();
          }
        }
      }
    }
    
    for (IdMap<VarOccurrences::Items>::iterator it = e.output_vo._m.begin();
         it != e.output_vo._m.end(); ++it) {
      std::vector<Item*> toRemove;
      for (VarOccurrences::Items::iterator iit = it->second.begin();
           iit != it->second.end(); ++iit) {
        if ((*iit)->removed()) {
          toRemove.push_back(*iit);
        }
      }
      for (unsigned int i=0; i<toRemove.size(); i++) {
        it->second.erase(toRemove[i]);
      }
    }
  }
  
  void createDznOutput(EnvI& e, bool outputObjective) {
    std::vector<Expression*> outputVars;
    
    class DZNOVisitor : public ItemVisitor {
    protected:
      EnvI& e;
      bool outputObjective;
      std::vector<Expression*>& outputVars;
      bool had_add_to_output;
    public:
      DZNOVisitor(EnvI& e0, bool outputObjective0, std::vector<Expression*>& outputVars0)
      : e(e0), outputObjective(outputObjective0), outputVars(outputVars0), had_add_to_output(false) {}
      void vVarDeclI(VarDeclI* vdi) {
        VarDecl* vd = vdi->e();
        bool process_var = false;
        if (outputObjective && vd->id()->idn()==-1 && vd->id()->v()=="_objective") {
          process_var = true;
        } else {
          if (vd->ann().contains(constants().ann.add_to_output)) {
            if (!had_add_to_output) {
              outputVars.clear();
            }
            had_add_to_output = true;
            process_var = true;
          } else {
            if (!had_add_to_output) {
              process_var = false;
              if (vd->type().isvar()) {
                if (vd->e()) {
                  if (ArrayLit* al = vd->e()->dyn_cast<ArrayLit>()) {
                    for (unsigned int i=0; i<al->size(); i++) {
                      if ((*al)[i]->isa<AnonVar>()) {
                        process_var = true;
                        break;
                      }
                    }
                  } else if (vd->ann().contains(constants().ann.rhs_from_assignment)) {
                    process_var = true;
                  }
                } else {
                  process_var = true;
                }
              }
            }
          }
        }
        if (process_var) {
          std::ostringstream s;
          s << vd->id()->str().str() << " = ";
          if (vd->type().dim() > 0) {
            ArrayLit* al = NULL;
            if (vd->e())
              al = eval_array_lit(e, vd->e());
            s << "array" << vd->type().dim() << "d(";
            for (int i=0; i<vd->type().dim(); i++) {
              unsigned int enumId = (vd->type().enumId() != 0 ? e.getArrayEnum(vd->type().enumId())[i] : 0);
              if (enumId != 0) {
                s << e.getEnum(enumId)->e()->id()->str() << ", ";
              } else if (al != NULL) {
                s << al->min(i) << ".." << al->max(i) << ", ";
              } else {
                IntSetVal* idxset = eval_intset(e,vd->ti()->ranges()[i]->domain());
                s << *idxset << ", ";
              }
            }
          }
          StringLit* sl = new StringLit(Location().introduce(),s.str());
          outputVars.push_back(sl);
          
          std::vector<Expression*> showArgs(1);
          showArgs[0] = vd->id();
          Call* show = new Call(Location().introduce(),ASTString("showDzn"),showArgs);
          show->type(Type::parstring());
          FunctionI* fi = e.model->matchFn(e, show, false);
          assert(fi);
          show->decl(fi);
          outputVars.push_back(show);
          std::string ends = vd->type().dim() > 0 ? ")" : "";
          ends += ";\n";
          StringLit* eol = new StringLit(Location().introduce(),ends);
          outputVars.push_back(eol);
        }
      }
      void vOutputI(OutputI* oi) {
        oi->remove();
      }
    } dznov(e, outputObjective, outputVars);

    iterItems(dznov, e.model);
    
    OutputI* newOutputItem = new OutputI(Location().introduce(),new ArrayLit(Location().introduce(),outputVars));
    e.model->addItem(newOutputItem);
  }

  void createJSONOutput(EnvI& e, bool outputObjective) {
    std::vector<Expression*> outputVars;
    outputVars.push_back(new StringLit(Location().introduce(), "{\n"));

    class JSONOVisitor : public ItemVisitor {
    protected:
      EnvI& e;
      bool outputObjective;
      std::vector<Expression*>& outputVars;
      bool had_add_to_output;
      bool first_var;
    public:
      JSONOVisitor(EnvI& e0, bool outputObjective0, std::vector<Expression*>& outputVars0)
      : e(e0), outputObjective(outputObjective0), outputVars(outputVars0), had_add_to_output(false), first_var(true) {}
      void vVarDeclI(VarDeclI* vdi) {
        VarDecl* vd = vdi->e();
        bool process_var = false;
        if (outputObjective && vd->id()->idn()==-1 && vd->id()->v()=="_objective") {
          process_var = true;
        } else {
          if (vd->ann().contains(constants().ann.add_to_output)) {
            if (!had_add_to_output) {
              outputVars.clear();
              outputVars.push_back(new StringLit(Location().introduce(), "{\n"));
              first_var = true;
            }
            had_add_to_output = true;
            process_var = true;
          } else {
            if (!had_add_to_output) {
              process_var = vd->type().isvar() && (vd->e()==NULL || vd->ann().contains(constants().ann.rhs_from_assignment));
            }
          }
        }
        if (process_var) {
          std::ostringstream s;
          if (first_var) {
            first_var = false;
          } else {
            s << ",\n";
          }
          s << "  \"" << vd->id()->str().str() << "\"" << " : ";
          StringLit* sl = new StringLit(Location().introduce(),s.str());
          outputVars.push_back(sl);
          
          std::vector<Expression*> showArgs(1);
          showArgs[0] = vd->id();
          Call* show = new Call(Location().introduce(),"showJSON",showArgs);
          show->type(Type::parstring());
          FunctionI* fi = e.model->matchFn(e, show, false);
          assert(fi);
          show->decl(fi);
          outputVars.push_back(show);
        }
      }
      void vOutputI(OutputI* oi) {
        oi->remove();
      }
    } jsonov(e, outputObjective, outputVars);
    
    iterItems(jsonov, e.model);

    outputVars.push_back(new StringLit(Location().introduce(), "\n}\n"));
    
    OutputI* newOutputItem = new OutputI(Location().introduce(),new ArrayLit(Location().introduce(),outputVars));
    e.model->addItem(newOutputItem);
  }
  
  void createOutput(EnvI& e, std::vector<VarDecl*>& deletedFlatVarDecls,
                    FlatteningOptions::OutputMode outputMode, bool outputObjective) {
    // Create new output model
    OutputI* outputItem = NULL;
    GCLock lock;
    
    switch (outputMode) {
      case FlatteningOptions::OUTPUT_DZN:
        createDznOutput(e,outputObjective);
        break;
      case FlatteningOptions::OUTPUT_JSON:
        createJSONOutput(e,outputObjective);
      default:
        if (e.model->outputItem()==NULL) {
          createDznOutput(e,outputObjective);
        }
        break;
    }
    
    // Copy output item from model into output model
    outputItem = copy(e,e.cmap, e.model->outputItem())->cast<OutputI>();
    makePar(e,outputItem->e());
    e.output->addItem(outputItem);
    
    // Copy all function definitions that are required for output into the output model
    class CollectFunctions : public EVisitor {
    public:
      EnvI& env;
      CollectFunctions(EnvI& env0) : env(env0) {}
      bool enter(Expression* e) {
        if (e->type().isvar()) {
          Type t = e->type();
          t.ti(Type::TI_PAR);
          e->type(t);
        }
        return true;
      }
      void vCall(Call& c) {
        std::vector<Type> tv(c.n_args());
        for (unsigned int i=c.n_args(); i--;) {
          tv[i] = c.arg(i)->type();
          tv[i].ti(Type::TI_PAR);
        }
        FunctionI* decl = env.output->matchFn(env, c.id(), tv, false);
        Type t;
        if (decl==NULL) {
          FunctionI* origdecl = env.model->matchFn(env, c.id(), tv, false);
          if (origdecl == NULL || !origdecl->rtype(env, tv, false).ispar()) {
            throw FlatteningError(env,c.loc(),"function "+c.id().str()+" is used in output, par version needed");
          }
          if (!origdecl->from_stdlib()) {
            decl = copy(env,env.cmap,origdecl)->cast<FunctionI>();
            CollectOccurrencesE ce(env.output_vo,decl);
            topDown(ce, decl->e());
            topDown(ce, decl->ti());
            for (unsigned int i = decl->params().size(); i--;)
              topDown(ce, decl->params()[i]);
            env.output->registerFn(env, decl);
            env.output->addItem(decl);
            if (decl->e()) {
              makePar(env, decl->e());
              topDown(*this, decl->e());
            }
          } else {
            decl = origdecl;
          }
        }
        c.decl(decl);
      }
    } _cf(e);
    topDown(_cf, outputItem->e());

    // If we are checking solutions using a checker model, all parameters of the checker model
    // have to be made available in the output model
    class OV1 : public ItemVisitor {
    public:
      EnvI& env;
      CollectFunctions& _cf;
      OV1(EnvI& env0, CollectFunctions& cf) : env(env0), _cf(cf) {}
      void vVarDeclI(VarDeclI* vdi) {
        if (vdi->e()->ann().contains(constants().ann.mzn_check_var)) {
          VarDecl* output_vd = copy(env,env.cmap,vdi->e())->cast<VarDecl>();
          topDown(_cf, output_vd);
        }
      }
    } _ov1(e, _cf);
    iterItems(_ov1, e.model);
    
    // Copying the output item and the functions it depends on has created copies
    // of all dependent VarDecls. However the output model does not contain VarDeclIs for
    // these VarDecls yet. This iterator processes all variable declarations of the
    // original model, and if they were copied (i.e., if the output model depends on them),
    // the corresponding VarDeclI is created in the output model.
    class OV2 : public ItemVisitor {
    public:
      EnvI& env;
      OV2(EnvI& env0) : env(env0) {}
      void vVarDeclI(VarDeclI* vdi) {
        IdMap<int>::iterator idx = env.output_vo.idx.find(vdi->e()->id());
        if (idx!=env.output_vo.idx.end())
          return;
        if (Expression* vd_e = env.cmap.find(vdi->e())) {
          // We found a copied VarDecl, now need to create a VarDeclI
          VarDecl* vd = vd_e->cast<VarDecl>();
          VarDeclI* vdi_copy = copy(env,env.cmap,vdi)->cast<VarDeclI>();
          
          Type t = vdi_copy->e()->ti()->type();
          t.ti(Type::TI_PAR);
          vdi_copy->e()->ti()->domain(NULL);
          vdi_copy->e()->flat(vdi->e()->flat());
          bool isCheckVar = vdi_copy->e()->ann().contains(constants().ann.mzn_check_var);
          Call* checkVarEnum = vdi_copy->e()->ann().getCall(constants().ann.mzn_check_enum_var);
          vdi_copy->e()->ann().clear();
          if (isCheckVar) {
            vdi_copy->e()->ann().add(constants().ann.mzn_check_var);
          }
          if (checkVarEnum) {
            vdi_copy->e()->ann().add(checkVarEnum);
          }
          vdi_copy->e()->introduced(false);
          IdMap<KeepAlive>::iterator it;
          if (!vdi->e()->type().ispar()) {
            if (vd->flat() == NULL && vdi->e()->e()!=NULL && vdi->e()->e()->type().ispar()) {
              // Don't have a flat version of this variable, but the original has a right hand
              // side that is par, so we can use that.
              Expression* flate = eval_par(env, vdi->e()->e());
              outputVarDecls(env,vdi_copy,flate);
              vd->e(flate);
            } else {
              vd = follow_id_to_decl(vd->id())->cast<VarDecl>();
              VarDecl* reallyFlat = vd->flat();
              while (reallyFlat && reallyFlat!=reallyFlat->flat())
                reallyFlat=reallyFlat->flat();
              if (reallyFlat==NULL) {
                // The variable doesn't have a flat version. This can only happen if
                // the original variable had type-inst var, but a right-hand-side that
                // was par, so follow_id_to_decl lead to a par variable.
                assert(vd->e() && vd->e()->type().ispar());
                Expression* flate = eval_par(env, vd->e());
                outputVarDecls(env, vdi_copy, flate);
                vd->e(flate);
              } else if (vd->flat()->e() && vd->flat()->e()->type().ispar()) {
                // We can use the right hand side of the flat version of this variable
                Expression* flate = copy(env,env.cmap,follow_id(reallyFlat->id()));
                outputVarDecls(env,vdi_copy,flate);
                vd->e(flate);
              } else if ( (it = env.reverseMappers.find(vd->id())) != env.reverseMappers.end()) {
                // Found a reverse mapper, so we need to add the mapping function to the
                // output model to map the FlatZinc value back to the model variable.
                Call* rhs = copy(env,env.cmap,it->second())->cast<Call>();
                {
                  std::vector<Type> tv(rhs->n_args());
                  for (unsigned int i=rhs->n_args(); i--;) {
                    tv[i] = rhs->arg(i)->type();
                    tv[i].ti(Type::TI_PAR);
                  }
                  FunctionI* decl = env.output->matchFn(env, rhs->id(), tv, false);
                  if (decl==NULL) {
                    FunctionI* origdecl = env.model->matchFn(env, rhs->id(), tv, false);
                    if (origdecl == NULL) {
                      throw FlatteningError(env,rhs->loc(),"function "+rhs->id().str()+" is used in output, par version needed");
                    }
                    if (!origdecl->from_stdlib()) {
                      decl = copy(env,env.cmap,origdecl)->cast<FunctionI>();
                      CollectOccurrencesE ce(env.output_vo,decl);
                      topDown(ce, decl->e());
                      topDown(ce, decl->ti());
                      for (unsigned int i = decl->params().size(); i--;)
                        topDown(ce, decl->params()[i]);
                      env.output->registerFn(env, decl);
                      env.output->addItem(decl);
                    } else {
                      decl = origdecl;
                    }
                  }
                  rhs->decl(decl);
                }
                outputVarDecls(env,vdi_copy,rhs);
                vd->e(rhs);
              } else if (cannotUseRHSForOutput(env,vd->e())) {
                // If the VarDecl does not have a usable right hand side, it needs to be
                // marked as output in the FlatZinc
                vd->e(NULL);
                assert(vd->flat());
                if (vd->type().dim() == 0) {
                  vd->flat()->addAnnotation(constants().ann.output_var);
                  checkRenameVar(env, vd);
                } else {
                  bool needOutputAnn = true;
                  if (reallyFlat->e() && reallyFlat->e()->isa<ArrayLit>()) {
                    ArrayLit* al = reallyFlat->e()->cast<ArrayLit>();
                    for (unsigned int i=0; i<al->size(); i++) {
                      if (Id* id = (*al)[i]->dyn_cast<Id>()) {
                        if (env.reverseMappers.find(id) != env.reverseMappers.end()) {
                          needOutputAnn = false;
                          break;
                        }
                      }
                    }
                    if (!needOutputAnn) {
                      outputVarDecls(env, vdi_copy, al);
                      vd->e(copy(env,env.cmap,al));
                    }
                  }
                  if (needOutputAnn) {
                    std::vector<Expression*> args(vdi->e()->type().dim());
                    for (unsigned int i=0; i<args.size(); i++) {
                      if (vdi->e()->ti()->ranges()[i]->domain() == NULL) {
                        args[i] = new SetLit(Location().introduce(), eval_intset(env,vd->flat()->ti()->ranges()[i]->domain()));
                      } else {
                        args[i] = new SetLit(Location().introduce(), eval_intset(env,vd->ti()->ranges()[i]->domain()));
                      }
                    }
                    ArrayLit* al = new ArrayLit(Location().introduce(), args);
                    args.resize(1);
                    args[0] = al;
                    vd->flat()->addAnnotation(new Call(Location().introduce(),constants().ann.output_array,args));
                    checkRenameVar(env, vd);
                  }
                }
              }
              if (reallyFlat && env.output_vo_flat.find(reallyFlat) == -1)
                env.output_vo_flat.add_idx(reallyFlat, env.output->size());
            }
          }
          makePar(env,vdi_copy->e());
          env.output_vo.add_idx(vdi_copy, env.output->size());
          CollectOccurrencesE ce(env.output_vo,vdi_copy);
          topDown(ce, vdi_copy->e());
          env.output->addItem(vdi_copy);
        }
      }
    } _ov2(e);
    iterItems(_ov2,e.model);
    
    CollectOccurrencesE ce(e.output_vo,outputItem);
    topDown(ce, outputItem->e());
  
    e.model->mergeStdLib(e, e.output);
    processDeletions(e, deletedFlatVarDecls);
  }

  Expression* isFixedDomain(EnvI& env, VarDecl* vd) {
    if (vd->type()!=Type::varbool() && vd->type()!=Type::varint() && vd->type()!=Type::varfloat())
      return NULL;
    Expression* e = vd->ti()->domain();
    if (e==constants().lit_true || e==constants().lit_false)
      return e;
    if (SetLit* sl = Expression::dyn_cast<SetLit>(e)) {
      if (sl->type().bt()==Type::BT_INT) {
        IntSetVal* isv = eval_intset(env, sl);
        return isv->min()==isv->max() ? IntLit::a(isv->min()) : NULL;
      } else if (sl->type().bt()==Type::BT_FLOAT) {
        FloatSetVal* fsv = eval_floatset(env, sl);
        return fsv->min()==fsv->max() ? FloatLit::a(fsv->min()) : NULL;
      }
    }
    return NULL;
  }
  
  void finaliseOutput(EnvI& e, std::vector<VarDecl*>& deletedFlatVarDecls) {
    if (e.output->size() > 0) {
      // Adapt existing output model
      // (generated by repeated flattening)
      e.output_vo.clear();
      for (unsigned int i=0; i<e.output->size(); i++) {
        Item* item = (*e.output)[i];
        if (item->removed())
          continue;
        switch (item->iid()) {
          case Item::II_VD:
          {
            VarDecl* vd = item->cast<VarDeclI>()->e();
            IdMap<KeepAlive>::iterator it;
            GCLock lock;
            VarDecl* reallyFlat = vd->flat();
            while (reallyFlat && reallyFlat!=reallyFlat->flat())
              reallyFlat=reallyFlat->flat();
            if (vd->e()==NULL) {
              if ( (vd->flat()->e() && vd->flat()->e()->type().ispar()) || isFixedDomain(e, vd->flat()) ) {
                VarDecl* reallyFlat = vd->flat();
                while (reallyFlat!=reallyFlat->flat())
                  reallyFlat=reallyFlat->flat();
                removeIsOutput(reallyFlat);
                Expression* flate;
                if (Expression* fd = isFixedDomain(e, vd->flat())) {
                  flate = fd;
                } else {
                  flate = copy(e,e.cmap,follow_id(reallyFlat->id()));
                }
                outputVarDecls(e,item,flate);
                vd->e(flate);
              } else if ( (it = e.reverseMappers.find(vd->id())) != e.reverseMappers.end()) {
                Call* rhs = copy(e,e.cmap,it->second())->cast<Call>();
                std::vector<Type> tv(rhs->n_args());
                for (unsigned int i=rhs->n_args(); i--;) {
                  tv[i] = rhs->arg(i)->type();
                  tv[i].ti(Type::TI_PAR);
                }
                FunctionI* decl = e.output->matchFn(e, rhs->id(), tv, false);
                if (decl==NULL) {
                  FunctionI* origdecl = e.model->matchFn(e, rhs->id(), tv, false);
                  if (origdecl == NULL) {
                    throw FlatteningError(e,rhs->loc(),"function "+rhs->id().str()+" is used in output, par version needed");
                  }
                  if (!origdecl->from_stdlib()) {
                    decl = copy(e,e.cmap,origdecl)->cast<FunctionI>();
                    CollectOccurrencesE ce(e.output_vo,decl);
                    topDown(ce, decl->e());
                    topDown(ce, decl->ti());
                    for (unsigned int i = decl->params().size(); i--;)
                      topDown(ce, decl->params()[i]);
                    e.output->registerFn(e, decl);
                    e.output->addItem(decl);
                  } else {
                    decl = origdecl;
                  }
                }
                rhs->decl(decl);
                removeIsOutput(reallyFlat);
                
                if (e.vo.occurrences(reallyFlat)==0 && reallyFlat->e()==NULL) {
                  deletedFlatVarDecls.push_back(reallyFlat);
                }
                
                outputVarDecls(e,item,it->second()->cast<Call>());
                vd->e(rhs);
              } else {
                // If the VarDecl does not have a usable right hand side, it needs to be
                // marked as output in the FlatZinc
                assert(vd->flat());
                
                bool needOutputAnn = true;
                if (reallyFlat->e() && reallyFlat->e()->isa<ArrayLit>()) {
                  ArrayLit* al = reallyFlat->e()->cast<ArrayLit>();
                  for (unsigned int i=0; i<al->size(); i++) {
                    if (Id* id = (*al)[i]->dyn_cast<Id>()) {
                      if (e.reverseMappers.find(id) != e.reverseMappers.end()) {
                        needOutputAnn = false;
                        break;
                      }
                    }
                  }
                  if (!needOutputAnn) {
                    removeIsOutput(vd);
                    removeIsOutput(reallyFlat);
                    if (e.vo.occurrences(reallyFlat)==0) {
                      deletedFlatVarDecls.push_back(reallyFlat);
                    }
                    
                    outputVarDecls(e, item, al);
                    vd->e(copy(e,e.cmap,al));
                  }
                }
                if (needOutputAnn) {
                  if (!isOutput(vd->flat())) {
                    GCLock lock;
                    if (vd->type().dim() == 0) {
                      vd->flat()->addAnnotation(constants().ann.output_var);
                    } else {
                      std::vector<Expression*> args(vd->type().dim());
                      for (unsigned int i=0; i<args.size(); i++) {
                        if (vd->ti()->ranges()[i]->domain() == NULL) {
                          args[i] = new SetLit(Location().introduce(), eval_intset(e,vd->flat()->ti()->ranges()[i]->domain()));
                        } else {
                          args[i] = new SetLit(Location().introduce(), eval_intset(e,vd->ti()->ranges()[i]->domain()));
                        }
                      }
                      ArrayLit* al = new ArrayLit(Location().introduce(), args);
                      args.resize(1);
                      args[0] = al;
                      vd->flat()->addAnnotation(new Call(Location().introduce(),constants().ann.output_array,args));
                    }
                    checkRenameVar(e, vd);
                  }
                }
              }
              vd->flat(NULL);
              // Remove enum type
              Type vdt = vd->type();
              vdt.enumId(0);
              vd->type(vdt);
              vd->ti()->type(vdt);
            }
            e.output_vo.add_idx(item->cast<VarDeclI>(), i);
            CollectOccurrencesE ce(e.output_vo,item);
            topDown(ce, vd);
          }
            break;
          case Item::II_OUT:
          {
            CollectOccurrencesE ce(e.output_vo,item);
            topDown(ce, item->cast<OutputI>()->e());
          }
            break;
          case Item::II_FUN:
          {
            CollectOccurrencesE ce(e.output_vo,item);
            topDown(ce, item->cast<FunctionI>()->e());
            topDown(ce, item->cast<FunctionI>()->ti());
            for (unsigned int i = item->cast<FunctionI>()->params().size(); i--;)
              topDown(ce, item->cast<FunctionI>()->params()[i]);
          }
            break;
          default:
            throw FlatteningError(e,item->loc(), "invalid item in output model");
        }
      }
    }
    processDeletions(e, deletedFlatVarDecls);
  }
}
