/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/astiterator.hh>
#include <minizinc/output.hh>
#include <minizinc/typecheck.hh>

namespace MiniZinc {

namespace {

// Test if all parameters and the return type are par
bool is_completely_par(EnvI& env, FunctionI* fi, const std::vector<Type>& tv) {
  if (fi->e() != nullptr) {
    // This is not a builtin, so check parameters
    for (int i = 0; i < fi->paramCount(); i++) {
      if (fi->param(i)->type().isvar() && !fi->param(i)->type().any()) {
        return false;
      }
    }
  }
  return fi->rtype(env, tv, false).isPar();
}

}  // namespace

void check_output_par_fn(EnvI& env, Call* rhs) {
  std::vector<Type> tv(rhs->argCount());
  for (unsigned int i = rhs->argCount(); (i--) != 0U;) {
    tv[i] = rhs->arg(i)->type();
    tv[i].ti(Type::TI_PAR);
  }
  FunctionI* decl = env.output->matchFn(env, rhs->id(), tv, false);
  if (decl == nullptr) {
    FunctionI* origdecl = env.model->matchFn(env, rhs->id(), tv, false);
    if (origdecl == nullptr || !is_completely_par(env, origdecl, tv)) {
      std::ostringstream ss;
      ss << "function " << demonomorphise_identifier(rhs->id())
         << " is used in output, par version needed";
      throw FlatteningError(env, rhs->loc(), ss.str());
    }
    if (!origdecl->fromStdLib()) {
      decl = copy(env, env.cmap, origdecl)->cast<FunctionI>();
      CollectOccurrencesE ce(env, env.outputVarOccurrences, decl);
      top_down(ce, decl->e());
      top_down(ce, decl->ti());
      for (unsigned int i = decl->paramCount(); (i--) != 0U;) {
        top_down(ce, decl->param(i));
      }
      (void)env.output->registerFn(env, decl, true);
      env.output->addItem(decl);
    } else {
      decl = origdecl;
    }
  }
  rhs->type(decl->rtype(env, tv, false));
  rhs->decl(decl);
}

bool cannot_use_rhs_for_output(EnvI& env, Expression* e,
                               std::unordered_set<FunctionI*>& seen_functions) {
  if (e == nullptr) {
    return true;
  }

  class V : public EVisitor {
  public:
    EnvI& env;
    std::unordered_set<FunctionI*>& seenFunctions;
    bool success;
    V(EnvI& env0, std::unordered_set<FunctionI*>& seenFunctions0)
        : env(env0), seenFunctions(seenFunctions0), success(true) {}
    /// Visit anonymous variable
    void vAnonVar(const AnonVar* /*v*/) { success = false; }
    /// Visit array literal
    void vArrayLit(const ArrayLit* /*al*/) {}
    /// Visit array access
    void vArrayAccess(const ArrayAccess* /*aa*/) {}
    /// Visit array comprehension
    void vComprehension(const Comprehension* /*c*/) {}
    /// Visit if-then-else
    void vITE(const ITE* /*ite*/) {}
    /// Visit binary operator
    void vBinOp(const BinOp* /*bo*/) {}
    /// Visit unary operator
    void vUnOp(const UnOp* /*uo*/) {}
    /// Visit call
    void vCall(Call* c) {
      std::vector<Type> tv(c->argCount());
      for (unsigned int i = c->argCount(); (i--) != 0U;) {
        tv[i] = c->arg(i)->type();
        tv[i].ti(Type::TI_PAR);
      }
      FunctionI* decl = env.output->matchFn(env, c->id(), tv, false);
      Type t;
      if (decl == nullptr) {
        FunctionI* origdecl = env.model->matchFn(env, c->id(), tv, false);
        if (origdecl == nullptr) {
          std::ostringstream ss;
          ss << "function " << demonomorphise_identifier(c->id())
             << " is used in output, par version needed";
          throw FlatteningError(env, c->loc(), ss.str());
        }
        bool seen = (seenFunctions.find(origdecl) != seenFunctions.end());
        if (seen) {
          success = false;
        } else {
          seenFunctions.insert(origdecl);
          if ((origdecl->e() != nullptr) &&
              cannot_use_rhs_for_output(env, origdecl->e(), seenFunctions)) {
            success = false;
          } else {
            if (!origdecl->fromStdLib()) {
              decl = copy(env, env.cmap, origdecl)->cast<FunctionI>();
              CollectOccurrencesE ce(env, env.outputVarOccurrences, decl);
              top_down(ce, decl->e());
              top_down(ce, decl->ti());
              for (unsigned int i = decl->paramCount(); (i--) != 0U;) {
                top_down(ce, decl->param(i));
              }
              (void)env.output->registerFn(env, decl, true);
              env.output->addItem(decl);
              output_vardecls(env, origdecl, decl->e());
              output_vardecls(env, origdecl, decl->ti());
            } else {
              decl = origdecl;
            }
            c->decl(decl);
          }
        }
      }
      if (success) {
        t = decl->rtype(env, tv, false);
        if (!t.isPar()) {
          success = false;
        }
      }
    }
    void vId(const Id* /*id*/) {}
    /// Visit let
    void vLet(const Let* /*let*/) { success = false; }
    /// Visit variable declaration
    void vVarDecl(const VarDecl* /*vd*/) {}
    /// Visit type inst
    void vTypeInst(const TypeInst* /*ti*/) {}
    /// Visit TIId
    void vTIId(const TIId* /*tiid*/) {}
    /// Determine whether to enter node
    bool enter(Expression* /*e*/) const { return success; }
  } _v(env, seen_functions);
  top_down(_v, e);

  return !_v.success;
}

bool cannot_use_rhs_for_output(EnvI& env, Expression* e) {
  std::unordered_set<FunctionI*> seen_functions;
  return cannot_use_rhs_for_output(env, e, seen_functions);
}

void remove_is_output(VarDecl* vd) {
  if (vd == nullptr) {
    return;
  }
  vd->ann().remove(Constants::constants().ann.output_var);
  vd->ann().removeCall(Constants::constants().ann.output_array);
}

void copy_output(EnvI& e) {
  struct CopyOutput : public EVisitor {
    EnvI& env;
    CopyOutput(EnvI& env0) : env(env0) {}
    static void vId(Id* _id) { _id->decl(_id->decl()->flat()); }
    void vCall(Call* c) {
      std::vector<Type> tv(c->argCount());
      for (unsigned int i = c->argCount(); (i--) != 0U;) {
        tv[i] = c->arg(i)->type();
        tv[i].ti(Type::TI_PAR);
      }
      FunctionI* decl = c->decl();
      if (!decl->fromStdLib()) {
        env.flatAddItem(decl);
      }
    }
  };

  if (OutputI* oi = e.model->outputItem()) {
    GCLock lock;
    auto* noi = copy(e, oi)->cast<OutputI>();
    CopyOutput co(e);
    top_down(co, noi->e());
    e.flatAddItem(noi);
  }
}

void cleanup_output(EnvI& env) {
  for (auto& i : *env.output) {
    if (auto* vdi = i->dynamicCast<VarDeclI>()) {
      vdi->e()->flat(nullptr);
    }
  }
}

void make_par(EnvI& env, Expression* e) {
  class OutputJSON : public EVisitor {
  public:
    EnvI& env;
    OutputJSON(EnvI& env0) : env(env0) {}
    void vCall(Call* c) {
      if (c->id() == env.constants.ids.outputJSON) {
        bool outputObjective = (c->argCount() == 1 && eval_bool(env, c->arg(0)));
        c->id(ASTString("array1d"));
        Expression* json =
            copy(env, env.cmap, create_json_output(env, outputObjective, false, false));
        std::vector<Expression*> new_args({json});
        new_args[0]->type(Type::parstring(1));
        c->args(new_args);
      }
    }
  } _outputJSON(env);
  top_down(_outputJSON, e);
  class Par : public EVisitor {
  public:
    /// Visit variable declaration
    static void vVarDecl(VarDecl* vd) { vd->ti()->type(vd->type()); }
    /// Determine whether to enter node
    static bool enter(Expression* e) {
      Type t = e->type();
      t.ti(Type::TI_PAR);
      t.cv(false);
      e->type(t);
      return true;
    }
  } _par;
  top_down(_par, e);
  class Decls : public EVisitor {
  public:
    EnvI& env;
    Decls(EnvI& env0) : env(env0) {}
    void vCall(Call* c) {
      if (c->id() == env.constants.ids.format || c->id() == env.constants.ids.show ||
          c->id() == env.constants.ids.showDzn || c->id() == env.constants.ids.showJSON) {
        unsigned int enumId = c->arg(c->argCount() - 1)->type().enumId();
        if (enumId != 0U && c->arg(c->argCount() - 1)->type().dim() != 0) {
          const std::vector<unsigned int>& enumIds = env.getArrayEnum(enumId);
          enumId = enumIds[enumIds.size() - 1];
        }
        if (enumId > 0) {
          GCLock lock;
          Expression* obj = c->arg(c->argCount() - 1);
          Id* ti_id = env.getEnum(enumId)->e()->id();
          std::string enumName = create_enum_to_string_name(ti_id, "_toString_");
          bool is_json = c->id() == env.constants.ids.showJSON;
          const int dimensions = obj->type().dim();
          if (is_json && dimensions > 1) {
            // Create generators for dimensions selection
            std::vector<Expression*> slice_dimensions(dimensions);
            std::vector<Generator> generators;
            generators.reserve(dimensions - 1);
            auto* idx_ti = new TypeInst(Location().introduce(), Type::parint());
            for (int i = 0; i < dimensions - 1; ++i) {
              auto* idx_i = new VarDecl(Location().introduce(), idx_ti, env.genId());
              idx_i->toplevel(false);
              Call* index_set_xx = new Call(
                  Location().introduce(),
                  "index_set_" + std::to_string(i + 1) + "of" + std::to_string(dimensions), {obj});
              index_set_xx->type(Type::parsetint());
              generators.push_back(Generator({idx_i}, index_set_xx, nullptr));
              slice_dimensions[i] =
                  new BinOp(Location().introduce(), idx_i->id(), BOT_DOTDOT, idx_i->id());
              slice_dimensions[i]->type(Type::parsetint());
            }

            // Construct innermost slicing operation
            Call* index_set_n = new Call(
                Location().introduce(),
                "index_set_" + std::to_string(dimensions) + "of" + std::to_string(dimensions),
                {obj});
            index_set_n->type(Type::parsetint());
            slice_dimensions[dimensions - 1] = index_set_n;
            auto* al_slice_dim = new ArrayLit(Location().introduce(), slice_dimensions);
            al_slice_dim->type(Type::parsetint(1));

            auto* slice_call =
                new Call(Location().introduce(), "slice_1d", {obj, al_slice_dim, index_set_n});
            Type tt = obj->type();
            tt.dim(1);
            slice_call->type(tt);
            Call* _toString_ENUM =
                new Call(Location().introduce(), enumName,
                         {slice_call, env.constants.literalFalse, env.constants.literalTrue});
            _toString_ENUM->type(Type::parstring());

            // Build multi-level JSON Array string
            auto* comma = new StringLit(Location().introduce(), ", ");
            comma->type(Type::parstring());
            auto join = [&](Expression* expr, Generator gen) -> Expression* {
              Generators generators;
              generators.g.push_back(gen);
              auto* comp = new Comprehension(Location().introduce(), expr, generators, false);
              comp->type(Type::parstring(1));
              Call* cc = new Call(Location().introduce(), "join", {comma, comp});
              cc->type(Type::parstring());
              return cc;
            };
            auto* sl_open = new StringLit(Location().introduce(), "[");
            sl_open->type(Type::parstring());
            auto* sl_close = new StringLit(Location().introduce(), "]");
            sl_close->type(Type::parstring());

            auto* al_concat = new ArrayLit(
                Location().introduce(),
                std::vector<Expression*>(
                    {sl_open, join(_toString_ENUM, generators[dimensions - 2]), sl_close}));
            al_concat->type(Type::parstring(1));
            for (int i = dimensions - 3; i >= 0; --i) {
              Call* concat = new Call(Location().introduce(), "concat", {al_concat});
              concat->type(Type::parstring());
              al_concat = new ArrayLit(
                  Location().introduce(),
                  std::vector<Expression*>({sl_open, join(concat, generators[i]), sl_close}));
              al_concat->type(Type::parstring(1));
            }
            std::vector<Expression*> args = {al_concat};
            c->args(args);
            c->id(ASTString("concat"));
          } else {
            std::vector<Expression*> args = {
                obj, env.constants.boollit(c->id() == env.constants.ids.showDzn),
                env.constants.boollit(is_json)};
            c->args(args);
            c->id(ASTString(enumName));
          }
        }
        if (c->id() == env.constants.ids.showDzn ||
            (c->id() == env.constants.ids.showJSON && enumId > 0)) {
          c->id(env.constants.ids.show);
        }
      }
      c->decl(env.model->matchFn(env, c, false));
    }
    void vBinOp(BinOp* bo) {
      std::vector<Expression*> args = {bo->lhs(), bo->rhs()};
      bo->decl(env.model->matchFn(env, bo->opToString(), args, false));
    }
    void vUnop(UnOp* uo) {
      std::vector<Expression*> args = {uo->e()};
      uo->decl(env.model->matchFn(env, uo->opToString(), args, false));
    }
  } _decls(env);
  top_down(_decls, e);
}

void check_rename_var(EnvI& e, VarDecl* vd) {
  if (vd->id()->idn() != vd->flat()->id()->idn()) {
    auto* vd_rename_ti = copy(e, e.cmap, vd->ti())->cast<TypeInst>();
    auto* vd_rename =
        new VarDecl(Location().introduce(), vd_rename_ti, vd->flat()->id()->idn(), nullptr);
    vd_rename->flat(vd->flat());
    make_par(e, vd_rename);
    vd->e(vd_rename->id());
    e.output->addItem(new VarDeclI(Location().introduce(), vd_rename));
  }
}

class ClearAnnotations {
public:
  /// Push all elements of \a v onto \a stack
  template <class E>
  static void pushVec(std::vector<Expression*>& stack, ASTExprVec<E> v) {
    for (unsigned int i = 0; i < v.size(); i++) {
      stack.push_back(v[i]);
    }
  }

  static void run(Expression* root) {
    std::vector<Expression*> stack;
    stack.push_back(root);
    while (!stack.empty()) {
      Expression* e = stack.back();
      stack.pop_back();
      if (e == nullptr) {
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
          for (unsigned int i = 0; i < e->cast<ArrayLit>()->size(); i++) {
            stack.push_back((*e->cast<ArrayLit>())[i]);
          }
          break;
        case Expression::E_ARRAYACCESS:
          pushVec(stack, e->template cast<ArrayAccess>()->idx());
          stack.push_back(e->template cast<ArrayAccess>()->v());
          break;
        case Expression::E_COMP: {
          auto* comp = e->template cast<Comprehension>();
          for (unsigned int i = comp->numberOfGenerators(); (i--) != 0U;) {
            stack.push_back(comp->where(i));
            stack.push_back(comp->in(i));
            for (unsigned int j = comp->numberOfDecls(i); (j--) != 0U;) {
              stack.push_back(comp->decl(i, j));
            }
          }
          stack.push_back(comp->e());
        } break;
        case Expression::E_ITE: {
          ITE* ite = e->template cast<ITE>();
          stack.push_back(ite->elseExpr());
          for (int i = 0; i < ite->size(); i++) {
            stack.push_back(ite->ifExpr(i));
            stack.push_back(ite->thenExpr(i));
          }
        } break;
        case Expression::E_BINOP:
          stack.push_back(e->template cast<BinOp>()->rhs());
          stack.push_back(e->template cast<BinOp>()->lhs());
          break;
        case Expression::E_UNOP:
          stack.push_back(e->template cast<UnOp>()->e());
          break;
        case Expression::E_CALL:
          for (unsigned int i = 0; i < e->template cast<Call>()->argCount(); i++) {
            stack.push_back(e->template cast<Call>()->arg(i));
          }
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
          pushVec(stack, e->template cast<TypeInst>()->ranges());
          break;
      }
    }
  }
};

void output_vardecls(EnvI& env, Item* ci, Expression* e) {
  class O : public EVisitor {
  public:
    EnvI& env;
    Item* ci;
    O(EnvI& env0, Item* ci0) : env(env0), ci(ci0) {}
    void vId(Id* id) {
      if (id == env.constants.absent) {
        return;
      }
      if (!id->decl()->toplevel()) {
        return;
      }
      VarDecl* vd = id->decl();
      VarDecl* reallyFlat = vd->flat();
      while (reallyFlat != nullptr && reallyFlat != reallyFlat->flat()) {
        reallyFlat = reallyFlat->flat();
      }
      auto idx = reallyFlat != nullptr ? env.outputFlatVarOccurrences.idx.find(reallyFlat->id())
                                       : env.outputFlatVarOccurrences.idx.end();
      auto idx2 = env.outputVarOccurrences.idx.find(vd->id());
      if (idx == env.outputFlatVarOccurrences.idx.end() &&
          idx2 == env.outputVarOccurrences.idx.end()) {
        auto* nvi = new VarDeclI(Location().introduce(), copy(env, env.cmap, vd)->cast<VarDecl>());
        Type t = nvi->e()->ti()->type();
        if (t.ti() != Type::TI_PAR) {
          t.ti(Type::TI_PAR);
        }
        make_par(env, nvi->e());
        nvi->e()->ti()->domain(nullptr);
        nvi->e()->flat(vd->flat());
        ClearAnnotations::run(nvi->e());
        nvi->e()->introduced(false);
        if (reallyFlat != nullptr) {
          env.outputFlatVarOccurrences.addIndex(reallyFlat, static_cast<int>(env.output->size()));
        }
        env.outputVarOccurrences.addIndex(nvi, static_cast<int>(env.output->size()));
        env.outputVarOccurrences.add(nvi->e(), ci);
        env.output->addItem(nvi);

        IdMap<KeepAlive>::iterator it;
        if ((it = env.reverseMappers.find(nvi->e()->id())) != env.reverseMappers.end()) {
          Call* rhs = copy(env, env.cmap, it->second())->cast<Call>();
          check_output_par_fn(env, rhs);
          output_vardecls(env, nvi, it->second());
          nvi->e()->e(rhs);
        } else if ((reallyFlat != nullptr) && cannot_use_rhs_for_output(env, reallyFlat->e())) {
          assert(nvi->e()->flat());
          nvi->e()->e(nullptr);
          if (nvi->e()->type().dim() == 0) {
            reallyFlat->addAnnotation(env.constants.ann.output_var);
          } else {
            std::vector<Expression*> args(reallyFlat->e()->type().dim());
            for (unsigned int i = 0; i < args.size(); i++) {
              if (nvi->e()->ti()->ranges()[i]->domain() == nullptr) {
                args[i] = new SetLit(Location().introduce(),
                                     eval_intset(env, reallyFlat->ti()->ranges()[i]->domain()));
              } else {
                args[i] = new SetLit(Location().introduce(),
                                     eval_intset(env, nvi->e()->ti()->ranges()[i]->domain()));
              }
            }
            auto* al = new ArrayLit(Location().introduce(), args);
            args.resize(1);
            args[0] = al;
            reallyFlat->addAnnotation(
                new Call(Location().introduce(), env.constants.ann.output_array, args));
          }
          check_rename_var(env, nvi->e());
        } else {
          output_vardecls(env, nvi, nvi->e()->ti());
          output_vardecls(env, nvi, nvi->e()->e());
        }
        CollectOccurrencesE ce(env, env.outputVarOccurrences, nvi);
        top_down(ce, nvi->e());
      }
    }
  } _o(env, ci);
  top_down(_o, e);
}

void process_deletions(EnvI& e) {
  std::vector<VarDecl*> deletedVarDecls;
  for (unsigned int i = 0; i < e.output->size(); i++) {
    if (auto* vdi = (*e.output)[i]->dynamicCast<VarDeclI>()) {
      if (!vdi->removed() && e.outputVarOccurrences.occurrences(vdi->e()) == 0 &&
          !vdi->e()->ann().contains(e.constants.ann.mzn_check_var) &&
          !(vdi->e()->id()->idn() == -1 && (vdi->e()->id()->v() == "_mzn_solution_checker" ||
                                            vdi->e()->id()->v() == "_mzn_stats_checker"))) {
        CollectDecls cd(e, e.outputVarOccurrences, deletedVarDecls, vdi);
        top_down(cd, vdi->e()->e());
        remove_is_output(vdi->e()->flat());
        if (e.outputVarOccurrences.find(vdi->e()) != -1) {
          e.outputVarOccurrences.remove(vdi->e());
        }
        vdi->remove();
      }
    }
  }
  while (!deletedVarDecls.empty()) {
    VarDecl* cur = deletedVarDecls.back();
    deletedVarDecls.pop_back();
    if (e.outputVarOccurrences.occurrences(cur) == 0) {
      auto cur_idx = e.outputVarOccurrences.idx.find(cur->id());
      if (cur_idx != e.outputVarOccurrences.idx.end()) {
        auto* vdi = (*e.output)[cur_idx->second]->cast<VarDeclI>();
        if (!vdi->removed()) {
          CollectDecls cd(e, e.outputVarOccurrences, deletedVarDecls, vdi);
          top_down(cd, cur->e());
          remove_is_output(vdi->e()->flat());
          if (e.outputVarOccurrences.find(vdi->e()) != -1) {
            e.outputVarOccurrences.remove(vdi->e());
          }
          vdi->remove();
        }
      }
    }
  }

  for (auto& it : e.outputVarOccurrences.itemMap) {
    std::vector<Item*> toRemove;
    for (auto* iit : it.second) {
      if (iit->removed()) {
        toRemove.push_back(iit);
      }
    }
    for (auto& i : toRemove) {
      it.second.erase(i);
    }
  }
}

void create_dzn_output_item(EnvI& e, bool includeObjective, bool includeOutputItem,
                            bool includeChecker) {
  std::vector<Expression*> outputVars;

  for (auto& it : e.outputVars) {
    auto name = it.first;
    auto* vd = it.second()->cast<VarDecl>();

    if (!includeObjective && (name == "_objective" || name == "_checker_objective")) {
      // Skip _objective if disabled
      continue;
    }

    std::ostringstream s;
    s << Printer::quoteId(name) << " = ";
    bool needArrayXd = false;
    if (vd->type().dim() > 0) {
      ArrayLit* al = nullptr;
      if (!vd->ann().contains(e.constants.ann.output_only)) {
        if ((vd->flat() != nullptr) && (vd->flat()->e() != nullptr)) {
          al = eval_array_lit(e, vd->flat()->e());
        } else if (vd->e() != nullptr) {
          al = eval_array_lit(e, vd->e());
        }
      }
      if (al == nullptr || !al->empty()) {
        needArrayXd = true;
        s << "array" << vd->type().dim() << "d(";
        for (int i = 0; i < vd->type().dim(); i++) {
          unsigned int enumId =
              (vd->type().enumId() != 0 ? e.getArrayEnum(vd->type().enumId())[i] : 0);
          if (al != nullptr || vd->ti()->ranges()[i]->domain() != nullptr) {
            if (enumId != 0) {
              IntVal idxMin;
              IntVal idxMax;

              if (al != nullptr) {
                idxMin = al->min(i);
                idxMax = al->max(i);
              } else {
                IntSetVal* idxset = eval_intset(e, vd->ti()->ranges()[i]->domain());
                idxMin = idxset->min();
                idxMax = idxset->max();
              }

              auto* sl = new StringLit(Location().introduce(), s.str());
              outputVars.push_back(sl);
              ASTString toString =
                  std::string("_toString_") + e.getEnum(enumId)->e()->id()->str().c_str();

              auto* toStringMin =
                  new Call(Location().introduce(), toString,
                           {IntLit::a(idxMin), e.constants.literalFalse, e.constants.literalFalse});
              toStringMin->type(Type::parstring());
              FunctionI* toStringMin_fi = e.model->matchFn(e, toStringMin, false);
              toStringMin->decl(toStringMin_fi);
              outputVars.push_back(toStringMin);

              sl = new StringLit(Location().introduce(), "..");
              outputVars.push_back(sl);

              auto* toStringMax =
                  new Call(Location().introduce(), toString,
                           {IntLit::a(idxMax), e.constants.literalFalse, e.constants.literalFalse});
              toStringMax->type(Type::parstring());
              FunctionI* toStringMax_fi = e.model->matchFn(e, toStringMax, false);
              toStringMax->decl(toStringMax_fi);
              outputVars.push_back(toStringMax);
              s.str("");
              s << ", ";
            } else if (al != nullptr) {
              s << al->min(i) << ".." << al->max(i) << ", ";
            } else {
              IntSetVal* idxset = eval_intset(e, vd->ti()->ranges()[i]->domain());
              s << *idxset << ", ";
            }
          } else {
            // Don't know index set range - have to compute in solns2out
            auto* sl = new StringLit(Location().introduce(), s.str());
            outputVars.push_back(sl);

            std::string index_set_fn = "index_set";
            if (vd->type().dim() > 1) {
              index_set_fn += "_" + std::to_string(i + 1) + "of" + std::to_string(vd->type().dim());
            }
            auto* index_set_xx = new Call(Location().introduce(), index_set_fn, {vd->id()});
            index_set_xx->type(Type::parsetint());
            auto* i_fi = e.model->matchFn(e, index_set_xx, false);
            assert(i_fi);
            index_set_xx->decl(i_fi);

            auto* show = new Call(Location().introduce(), e.constants.ids.show, {index_set_xx});
            show->type(Type::parstring());
            FunctionI* s_fi = e.model->matchFn(e, show, false);
            assert(s_fi);
            show->decl(s_fi);

            outputVars.push_back(show);
            s.str("");
            s << ", ";
          }
        }
      }
    }
    auto* sl = new StringLit(Location().introduce(), s.str());
    outputVars.push_back(sl);

    std::vector<Expression*> showArgs(1);
    showArgs[0] = vd->id();
    Call* show = new Call(Location().introduce(), ASTString("showDzn"), showArgs);
    show->type(Type::parstring());
    FunctionI* fi = e.model->matchFn(e, show, false);
    assert(fi);
    show->decl(fi);
    outputVars.push_back(show);
    std::string ends = needArrayXd ? ")" : "";
    ends += ";\n";
    auto* eol = new StringLit(Location().introduce(), ends);
    outputVars.push_back(eol);
  }

  auto* oi = e.model->outputItem();
  if (oi != nullptr) {
    if (includeOutputItem) {
      outputVars.push_back(new StringLit(Location().introduce(), "_output = "));
      Call* concat = new Call(Location().introduce(), ASTString("concat"), {oi->e()});
      concat->type(Type::parstring());
      FunctionI* fi = e.model->matchFn(e, concat, false);
      assert(fi);
      concat->decl(fi);
      Call* show = new Call(Location().introduce(), ASTString("showDzn"), {concat});
      show->type(Type::parstring());
      fi = e.model->matchFn(e, show, false);
      assert(fi);
      show->decl(fi);
      outputVars.push_back(show);
      outputVars.push_back(new StringLit(Location().introduce(), ";\n"));
    }

    oi->remove();
  }

  if (includeChecker) {
    outputVars.push_back(new StringLit(Location().introduce(), "_checker = "));
    auto* checker_output = new Call(Location().introduce(), ASTString("showCheckerOutput"), {});
    checker_output->type(Type::parstring());
    FunctionI* fi = e.model->matchFn(e, checker_output, false);
    assert(fi);
    checker_output->decl(fi);
    auto* show = new Call(Location().introduce(), ASTString("showDzn"), {checker_output});
    show->type(Type::parstring());
    fi = e.model->matchFn(e, show, false);
    assert(fi);
    show->decl(fi);
    outputVars.push_back(show);
    outputVars.push_back(new StringLit(Location().introduce(), ";\n"));
  }

  auto* al = new ArrayLit(Location().introduce(), outputVars);
  e.addOutputToSection(ASTString("dzn"), al);  // Add output to dzn section for encapsulation
  auto* newOutputItem = new OutputI(Location().introduce(), al);
  e.model->addItem(newOutputItem);
}

ArrayLit* create_json_output(EnvI& e, bool includeObjective, bool includeOutputItem,
                             bool includeChecker) {
  std::vector<Expression*> outputVars;
  outputVars.push_back(new StringLit(Location().introduce(), "{\n"));

  bool firstVar = true;
  for (auto it : e.outputVars) {
    auto name = it.first;
    auto* vd = it.second()->cast<VarDecl>();

    if (!includeObjective && (name == "_objective" || name == "_checker_objective")) {
      // Skip _objective if disabled
      continue;
    }

    std::ostringstream s;
    if (firstVar) {
      firstVar = false;
    } else {
      s << ",\n";
    }
    s << "  \"" << Printer::escapeStringLit(name) << "\""
      << " : ";
    auto* sl = new StringLit(Location().introduce(), s.str());
    outputVars.push_back(sl);

    std::vector<Expression*> showArgs(1);
    showArgs[0] = vd->id();
    Call* show = new Call(Location().introduce(), "showJSON", showArgs);
    show->type(Type::parstring());
    FunctionI* fi = e.model->matchFn(e, show, false);
    assert(fi);
    show->decl(fi);
    outputVars.push_back(show);
  }

  auto* oi = e.model->outputItem();
  if (oi != nullptr) {
    if (includeOutputItem) {
      std::ostringstream s;
      if (firstVar) {
        firstVar = false;
      } else {
        s << ",\n";
      }
      s << "  \"_output\""
        << " : ";
      auto* sl = new StringLit(Location().introduce(), s.str());
      outputVars.push_back(sl);
      Call* concat = new Call(Location().introduce(), ASTString("concat"), {oi->e()});
      concat->type(Type::parstring());
      FunctionI* fi = e.model->matchFn(e, concat, false);
      assert(fi);
      concat->decl(fi);
      Call* show = new Call(Location().introduce(), ASTString("showJSON"), {concat});
      show->type(Type::parstring());
      fi = e.model->matchFn(e, show, false);
      assert(fi);
      show->decl(fi);
      outputVars.push_back(show);
    }

    oi->remove();
  }

  if (includeChecker) {
    std::ostringstream s;
    if (firstVar) {
      firstVar = false;
    } else {
      s << ",\n";
    }
    s << "  \"_checker\""
      << " : ";
    auto* sl = new StringLit(Location().introduce(), s.str());
    outputVars.push_back(sl);
    Call* checker_output = new Call(Location().introduce(), ASTString("showCheckerOutput"), {});
    checker_output->type(Type::parstring());
    FunctionI* fi = e.model->matchFn(e, checker_output, false);
    assert(fi);
    checker_output->decl(fi);
    Call* show = new Call(Location().introduce(), ASTString("showJSON"), {checker_output});
    show->type(Type::parstring());
    fi = e.model->matchFn(e, show, false);
    assert(fi);
    show->decl(fi);
    outputVars.push_back(show);
  }

  outputVars.push_back(new StringLit(Location().introduce(), "\n}\n"));
  return new ArrayLit(Location().introduce(), outputVars);
}
void create_json_output_item(EnvI& e, bool ioncludeObjective, bool includeOutputItem,
                             bool includeChecker) {
  auto* al = create_json_output(e, ioncludeObjective, includeOutputItem, includeChecker);
  e.addOutputToSection(ASTString("json"), al);  // Add output to json section for encapsulation
  auto* newOutputItem = new OutputI(Location().introduce(), al);
  e.model->addItem(newOutputItem);
}

void create_encapsulated_output_item(EnvI& e) {
  // Inner portion of encapsulated output
  auto* al = e.model->outputItem()->e();
  al->type(Type::parstring(1));
  std::vector<Expression*> es;

  // Output each section as key-value pairs in an object
  es.push_back(new StringLit(Location().introduce(), "{"));
  bool first = true;
  for (const auto& it : e.outputSections()) {
    if (first) {
      es.push_back(new StringLit(Location().introduce(),
                                 "\"" + Printer::escapeStringLit(it.first) + "\": "));
      first = false;
    } else {
      es.push_back(new StringLit(Location().introduce(),
                                 ", \"" + Printer::escapeStringLit(it.first) + "\": "));
    }
    bool isJSON = it.first == "json" || it.first.endsWith("_json");
    auto* concat = new Call(Location().introduce(), "concat", {it.second});
    concat->type(Type::parstring());
    concat->decl(e.model->matchFn(e, concat, false));
    if (isJSON) {
      es.push_back(concat);
    } else {
      auto* showJSON = new Call(Location().introduce(), "showJSON", {concat});
      showJSON->type(Type::parstring());
      showJSON->decl(e.model->matchFn(e, showJSON, false));
      es.push_back(showJSON);
    }
  }
  es.push_back(new StringLit(Location().introduce(), "}"));

  auto* newOutputItem =
      new OutputI(Location().introduce(), new ArrayLit(Location().introduce(), es));
  e.model->addItem(newOutputItem);
}

void process_toplevel_output_vars(EnvI& e) {
  GCLock lock;

  class OutputVarVisitor : public ItemVisitor {
  private:
    EnvI& _e;
    bool _outputForChecker;
    bool _isChecker;

  public:
    OutputVarVisitor(EnvI& e)
        : _e(e),
          _outputForChecker(e.fopts.outputMode == FlatteningOptions::OutputMode::OUTPUT_CHECKER),
          _isChecker(e.model->filename().endsWith(".mzc") ||
                     e.model->filename().endsWith(".mzc.mzn")) {}

    bool hasAddToOutput = false;
    std::vector<std::pair<int, VarDecl*>> todo;

    void vVarDeclI(VarDeclI* vdi) {
      auto* vd = vdi->e();
      if (_outputForChecker) {
        if (vd->ann().contains(_e.constants.ann.mzn_check_var)) {
          _e.outputVars.emplace_back(vd->id()->str(), vd);
        }
      } else {
        if (vd->ann().contains(_e.constants.ann.add_to_output)) {
          hasAddToOutput = true;
          todo.clear();  // Skip 2nd pass
          _e.outputVars.emplace_back(vd->id()->str(), vd);
        } else if (vd->ann().contains(_e.constants.ann.output) ||
                   (!_isChecker && vd->id()->idn() == -1 && vd->id()->v() == "_objective") ||
                   ((_isChecker && vd->id()->idn() == -1 &&
                     vd->id()->v() == "_checker_objective"))) {
          // Whether or not to actually include will be determined later
          _e.outputVars.emplace_back(vd->id()->str(), vd);
        } else if (!hasAddToOutput) {
          todo.emplace_back(_e.outputVars.size(), vd);  // Needs to be processed in 2nd pass
        }
      }
      vd->ann().remove(_e.constants.ann.output);
    }
  } ovv(e);
  iter_items(ovv, e.model);

  // Insert implicit output variables
  int inserted = 0;
  for (auto& it : ovv.todo) {
    auto idx = it.first;
    auto* vd = it.second;
    if (vd->ann().contains(e.constants.ann.no_output) || vd->type().isPar()) {
      continue;
    }
    if (vd->e() == nullptr || vd->ann().contains(e.constants.ann.rhs_from_assignment)) {
      // Output anything without a RHS
      e.outputVars.emplace(e.outputVars.begin() + inserted + idx, vd->id()->str(), vd);
      inserted++;
      continue;
    }
    if (auto* al = vd->e()->dynamicCast<ArrayLit>()) {
      // Output array literals containing _
      for (unsigned int i = 0; i < al->size(); i++) {
        if ((*al)[i]->isa<AnonVar>()) {
          e.outputVars.emplace(e.outputVars.begin() + inserted + idx, vd->id()->str(), vd);
          inserted++;
          break;
        }
      }
    }
  }
}

void create_output(EnvI& e, FlatteningOptions::OutputMode outputMode, bool outputObjective,
                   bool includeOutputItem, bool hasChecker, bool encapsulateJSON) {
  // Create new output model
  OutputI* outputItem = nullptr;
  GCLock lock;

  // Combine output sections into one string
  if (!e.outputSections().empty()) {
    Expression* combinedOutput = nullptr;
    auto* empty_array = new ArrayLit(Location().introduce(), std::vector<Expression*>());
    for (const auto& it : e.outputSections()) {
      auto* section_enabled = new Call(Location().introduce(), e.constants.ids.mzn_section_enabled,
                                       {new StringLit(Location().introduce(), it.first)});
      section_enabled->type(Type::parbool());
      section_enabled->decl(e.model->matchFn(e, section_enabled, false));
      auto* part = new ITE(Location().introduce(), {section_enabled, it.second}, empty_array);
      part->type(Type::parstring(1));
      if (combinedOutput == nullptr) {
        combinedOutput = part;
      } else {
        combinedOutput = new BinOp(Location().introduce(), combinedOutput, BOT_PLUSPLUS, part);
        combinedOutput->type(Type::parstring(1));
      }
    }
    e.addOutputToSection(ASTString("raw"), combinedOutput);  // Add to raw section for encapsulation
    e.model->addItem(new OutputI(Location().introduce(), combinedOutput));
  }

  // Only include _checker output if not JSON encapsulated
  bool includeChecker = hasChecker && !encapsulateJSON;
  switch (outputMode) {
    case FlatteningOptions::OUTPUT_DZN:
      create_dzn_output_item(e, outputObjective, includeOutputItem, includeChecker);
      break;
    case FlatteningOptions::OUTPUT_JSON:
      create_json_output_item(e, outputObjective, includeOutputItem, includeChecker);
      break;
    case FlatteningOptions::OUTPUT_CHECKER:
      create_dzn_output_item(e, true, false, false);
      break;
    default:
      if (e.model->outputItem() == nullptr) {
        create_dzn_output_item(e, outputObjective, false, false);
      }
      break;
  }

  if (encapsulateJSON) {
    // Encapsulate the created output item in JSON
    create_encapsulated_output_item(e);
  }

  // Copy output item from model into output model
  outputItem = copy(e, e.cmap, e.model->outputItem())->cast<OutputI>();
  make_par(e, outputItem->e());
  e.output->addItem(outputItem);

  // Copy all function definitions that are required for output into the output model
  class CollectFunctions : public EVisitor {
  public:
    EnvI& env;
    CollectFunctions(EnvI& env0) : env(env0) {}
    static bool enter(Expression* e) {
      if (e->type().isvar()) {
        Type t = e->type();
        t.ti(Type::TI_PAR);
        e->type(t);
      }
      return true;
    }
    void vId(Id* i) {
      // Also collect functions from output_only variables we depend on
      if ((i->decl() != nullptr) && i->decl()->ann().contains(env.constants.ann.output_only)) {
        top_down(*this, i->decl()->e());
      }
    }
    void vCall(Call* c) {
      std::vector<Type> tv(c->argCount());
      for (unsigned int i = c->argCount(); (i--) != 0U;) {
        tv[i] = c->arg(i)->type();
        tv[i].ti(Type::TI_PAR);
      }
      FunctionI* decl = env.output->matchFn(env, c->id(), tv, false);
      FunctionI* origdecl = env.model->matchFn(env, c->id(), tv, false);
      bool canReuseDecl = (decl != nullptr);
      if (canReuseDecl && (origdecl != nullptr)) {
        // Check if this is the exact same overloaded declaration as in the model
        for (unsigned int i = 0; i < decl->paramCount(); i++) {
          if (decl->param(i)->type() != origdecl->param(i)->type()) {
            // no, the types don't match, so we have to copy the original decl
            canReuseDecl = false;
            break;
          }
        }
      }
      Type t;
      if (!canReuseDecl) {
        if (origdecl == nullptr || !is_completely_par(env, origdecl, tv)) {
          std::ostringstream ss;
          ss << "function " << demonomorphise_identifier(c->id())
             << " is used in output, par version needed";
          throw FlatteningError(env, c->loc(), ss.str());
        }
        if (!origdecl->fromStdLib()) {
          auto* decl_copy = copy(env, env.cmap, origdecl)->cast<FunctionI>();
          if (decl_copy != decl) {
            decl = decl_copy;
            (void)env.output->registerFn(env, decl, true);
            env.output->addItem(decl);
            if (decl->e() != nullptr) {
              make_par(env, decl->e());
              top_down(*this, decl->e());
            }
            CollectOccurrencesE ce(env, env.outputVarOccurrences, decl);
            top_down(ce, decl->e());
            top_down(ce, decl->ti());
            for (unsigned int i = decl->paramCount(); (i--) != 0U;) {
              top_down(ce, decl->param(i));
            }
          }
        } else {
          decl = origdecl;
        }
      }
      c->decl(decl);
    }
  } _cf(e);
  top_down(_cf, outputItem->e());

  // If we are checking solutions using a checker model, all parameters of the checker model
  // have to be made available in the output model
  class OV1 : public ItemVisitor {
  public:
    EnvI& env;
    CollectFunctions& cf;
    OV1(EnvI& env0, CollectFunctions& cf0) : env(env0), cf(cf0) {}
    void vVarDeclI(VarDeclI* vdi) {
      if (vdi->e()->ann().contains(env.constants.ann.mzn_check_var)) {
        auto* output_vd = copy(env, env.cmap, vdi->e())->cast<VarDecl>();
        top_down(cf, output_vd);
      }
    }
  } _ov1(e, _cf);
  iter_items(_ov1, e.model);

  // Copying the output item and the functions it depends on has created copies
  // of all dependent VarDecls. However the output model does not contain VarDeclIs for
  // these VarDecls yet. This iterator processes all copied variable declarations and
  // creates the corresponding VarDeclI in the output model.
  class CollectVarDecls : public EVisitor {
  public:
    EnvI& env;
    CollectVarDecls(EnvI& env0) : env(env0) {}

    void vCall(Call* c) {
      if (!c->decl()->fromStdLib()) {
        top_down(*this, c->decl()->e());
      }
    }

    void vId(Id* i) {
      auto* vd = i->decl();
      if (vd == nullptr || !vd->toplevel()) {
        return;
      }
      if (vd->e() != nullptr) {
        top_down(*this, vd->e());
      }
      auto idx = env.outputVarOccurrences.find(vd);
      if (idx == -1) {
        auto* orig = env.cmap.findOrig(vd);
        if (orig == nullptr) {
          // Did not copy this in (came from rename var)
          // TODO: any other reason?
          return;
        }
        auto* vd_orig = orig->cast<VarDecl>();
        Location loc = vd->loc();  // Close enough
        auto* vdi_copy = new VarDeclI(loc, vd);
        Type t = vd->ti()->type();
        t.ti(Type::TI_PAR);
        vd->ti()->domain(nullptr);
        vd->flat(vd_orig->flat());
        bool isCheckVar = vd->ann().contains(env.constants.ann.mzn_check_var);
        Call* checkVarEnum = vd->ann().getCall(env.constants.ann.mzn_check_enum_var);
        vd->ann().clear();
        if (isCheckVar) {
          vd->addAnnotation(env.constants.ann.mzn_check_var);
        }
        if (checkVarEnum != nullptr) {
          vdi_copy->e()->addAnnotation(checkVarEnum);
        }
        vdi_copy->e()->introduced(false);

        IdMap<KeepAlive>::iterator it;
        if (!vd_orig->type().isPar()) {
          if (vd->flat() == nullptr && vd_orig->e() != nullptr && vd_orig->e()->type().isPar()) {
            // Don't have a flat version of this variable, but the original has a right hand
            // side that is par, so we can use that.
            Expression* flate = eval_par(env, vd_orig->e());
            output_vardecls(env, vdi_copy, flate);
            vd->e(flate);
          } else {
            auto* vd_followed = follow_id_to_decl(vd->id())->cast<VarDecl>();
            VarDecl* reallyFlat = vd_followed->flat();
            while ((reallyFlat != nullptr) && reallyFlat != reallyFlat->flat()) {
              reallyFlat = reallyFlat->flat();
            }
            if (reallyFlat == nullptr) {
              // The variable doesn't have a flat version. This can only happen if
              // the original variable had type-inst var, but a right-hand-side that
              // was par, so follow_id_to_decl lead to a par variable.
              assert(vd_followed->e() && vd_followed->e()->type().isPar());
              Expression* flate = eval_par(env, vd_followed->e());
              output_vardecls(env, vdi_copy, flate);
              vd_followed->e(flate);
            } else if ((vd_followed->flat()->e() != nullptr) &&
                       vd_followed->flat()->e()->type().isPar()) {
              // We can use the right hand side of the flat version of this variable
              Expression* flate = copy(env, env.cmap, follow_id(reallyFlat->id()));
              output_vardecls(env, vdi_copy, flate);
              vd_followed->e(flate);
            } else if ((it = env.reverseMappers.find(vd_followed->id())) !=
                       env.reverseMappers.end()) {
              // Found a reverse mapper, so we need to add the mapping function to the
              // output model to map the FlatZinc value back to the model variable.
              Call* rhs = copy(env, env.cmap, it->second())->cast<Call>();
              check_output_par_fn(env, rhs);
              output_vardecls(env, vdi_copy, rhs);
              vd_followed->e(rhs);
            } else if (reallyFlat == vd_orig || cannot_use_rhs_for_output(env, vd_followed->e())) {
              // If the VarDecl does not have a usable right hand side, it needs to be
              // marked as output in the FlatZinc
              vd_followed->e(nullptr);
              assert(vd_followed->flat());
              if (vd_followed->type().dim() == 0) {
                vd_followed->flat()->addAnnotation(env.constants.ann.output_var);
                check_rename_var(env, vd_followed);
              } else {
                bool needOutputAnn = true;
                if ((reallyFlat->e() != nullptr) && reallyFlat->e()->isa<ArrayLit>()) {
                  auto* al = reallyFlat->e()->cast<ArrayLit>();
                  for (unsigned int i = 0; i < al->size(); i++) {
                    if (Id* id = (*al)[i]->dynamicCast<Id>()) {
                      if (env.reverseMappers.find(id) != env.reverseMappers.end()) {
                        needOutputAnn = false;
                        break;
                      }
                    }
                  }
                  if (!needOutputAnn) {
                    output_vardecls(env, vdi_copy, al);
                    vd_followed->e(copy(env, env.cmap, al));
                  }
                }
                if (needOutputAnn) {
                  std::vector<Expression*> args(vd_orig->type().dim());
                  for (unsigned int i = 0; i < args.size(); i++) {
                    if (vd_orig->ti()->ranges()[i]->domain() == nullptr) {
                      args[i] = new SetLit(
                          Location().introduce(),
                          eval_intset(env, vd_followed->flat()->ti()->ranges()[i]->domain()));
                    } else {
                      args[i] =
                          new SetLit(Location().introduce(),
                                     eval_intset(env, vd_followed->ti()->ranges()[i]->domain()));
                    }
                  }
                  auto* al = new ArrayLit(Location().introduce(), args);
                  args.resize(1);
                  args[0] = al;
                  vd_followed->flat()->addAnnotation(
                      new Call(Location().introduce(), env.constants.ann.output_array, args));
                  check_rename_var(env, vd_followed);
                }
              }
            }
            if ((reallyFlat != nullptr) && env.outputFlatVarOccurrences.find(reallyFlat) == -1) {
              env.outputFlatVarOccurrences.addIndex(reallyFlat,
                                                    static_cast<int>(env.output->size()));
            }
          }
        } else {
          if (vd->flat() == nullptr && vd_orig->e() != nullptr) {
            // Need to process right hand side of variable, since it may contain
            // identifiers that are only in the FlatZinc and that we would
            // therefore fail to copy into the output model
            output_vardecls(env, vdi_copy, vd_orig->e());
          }
        }
        make_par(env, vdi_copy->e());
        env.outputVarOccurrences.addIndex(vdi_copy, static_cast<int>(env.output->size()));
        CollectOccurrencesE ce(env, env.outputVarOccurrences, vdi_copy);
        top_down(ce, vdi_copy->e());
        env.output->addItem(vdi_copy);
      } else {
        // Either this ID's decl is already the one in the output model,
        // or this ID is from the flat model and can be made to point to the
        // existing one in the output model.
        auto* output_vdi = (*env.output)[idx]->cast<VarDeclI>();
        i->decl(output_vdi->e());
      }
    }
  } _cvd(e);
  top_down(_cvd, outputItem->e());

  CollectOccurrencesE ce(e, e.outputVarOccurrences, outputItem);
  top_down(ce, outputItem->e());

  e.model->mergeStdLib(e, e.output);
  process_deletions(e);
}

Expression* is_fixed_domain(EnvI& env, VarDecl* vd) {
  if (vd->type() != Type::varbool() && vd->type() != Type::varint() &&
      vd->type() != Type::varfloat()) {
    return nullptr;
  }
  Expression* e = vd->ti()->domain();
  if (e == env.constants.literalTrue || e == env.constants.literalFalse) {
    return e;
  }
  if (auto* sl = Expression::dynamicCast<SetLit>(e)) {
    if (sl->type().bt() == Type::BT_INT) {
      IntSetVal* isv = eval_intset(env, sl);
      return isv->min() == isv->max() ? IntLit::a(isv->min()) : nullptr;
    }
    if (sl->type().bt() == Type::BT_FLOAT) {
      FloatSetVal* fsv = eval_floatset(env, sl);
      return fsv->min() == fsv->max() ? FloatLit::a(fsv->min()) : nullptr;
    }
  }
  return nullptr;
}

void finalise_output(EnvI& e) {
  if (!e.output->empty()) {
    // Adapt existing output model
    // (generated by repeated flattening)
    e.outputVarOccurrences.clear();
    for (unsigned int i = 0; i < e.output->size(); i++) {
      Item* item = (*e.output)[i];
      if (item->removed()) {
        continue;
      }
      switch (item->iid()) {
        case Item::II_VD: {
          VarDecl* vd = item->cast<VarDeclI>()->e();
          IdMap<KeepAlive>::iterator it;
          GCLock lock;
          VarDecl* reallyFlat = vd->flat();
          while ((reallyFlat != nullptr) && reallyFlat != reallyFlat->flat()) {
            reallyFlat = reallyFlat->flat();
          }
          if (vd->e() == nullptr) {
            if (((vd->flat()->e() != nullptr) && vd->flat()->e()->type().isPar()) ||
                (is_fixed_domain(e, vd->flat()) != nullptr)) {
              VarDecl* reallyFlat = vd->flat();
              while (reallyFlat != reallyFlat->flat()) {
                reallyFlat = reallyFlat->flat();
              }
              remove_is_output(reallyFlat);
              Expression* flate;
              if (Expression* fd = is_fixed_domain(e, vd->flat())) {
                flate = fd;
              } else {
                flate = copy(e, e.cmap, follow_id(reallyFlat->id()));
              }
              output_vardecls(e, item, flate);
              vd->e(flate);
            } else if ((it = e.reverseMappers.find(vd->id())) != e.reverseMappers.end()) {
              Call* rhs = copy(e, e.cmap, it->second())->cast<Call>();
              check_output_par_fn(e, rhs);
              remove_is_output(reallyFlat);

              output_vardecls(e, item, it->second()->cast<Call>());
              vd->e(rhs);

              if (e.varOccurrences.occurrences(reallyFlat) == 0 && reallyFlat->e() == nullptr) {
                auto it = e.varOccurrences.idx.find(reallyFlat->id());
                assert(it != e.varOccurrences.idx.end());
                e.flatRemoveItem((*e.flat())[it->second]->cast<VarDeclI>());
              }
            } else {
              // If the VarDecl does not have a usable right hand side, it needs to be
              // marked as output in the FlatZinc
              assert(vd->flat());

              bool needOutputAnn = true;
              if ((reallyFlat->e() != nullptr) && reallyFlat->e()->isa<Id>()) {
                Id* ident = reallyFlat->e()->cast<Id>();
                if (e.reverseMappers.find(ident) != e.reverseMappers.end()) {
                  needOutputAnn = false;
                  remove_is_output(vd);
                  remove_is_output(reallyFlat);

                  vd->e(copy(e, e.cmap, ident));
                  Type al_t(vd->e()->type());
                  al_t.ti(Type::TI_PAR);
                  vd->e()->type(al_t);

                  output_vardecls(e, item, ident);

                  if (e.varOccurrences.occurrences(reallyFlat) == 0) {
                    auto it = e.varOccurrences.idx.find(reallyFlat->id());
                    assert(it != e.varOccurrences.idx.end());
                    e.flatRemoveItem((*e.flat())[it->second]->cast<VarDeclI>());
                  }
                }
              } else if ((reallyFlat->e() != nullptr) && reallyFlat->e()->isa<ArrayLit>()) {
                auto* al = reallyFlat->e()->cast<ArrayLit>();
                for (unsigned int i = 0; i < al->size(); i++) {
                  if (Id* ident = follow_id_to_value((*al)[i])->dynamicCast<Id>()) {
                    if (e.reverseMappers.find(ident) != e.reverseMappers.end()) {
                      needOutputAnn = false;
                      break;
                    }
                  }
                }
                if (!needOutputAnn) {
                  remove_is_output(vd);
                  remove_is_output(reallyFlat);
                  if (e.varOccurrences.occurrences(reallyFlat) == 0) {
                    auto it = e.varOccurrences.idx.find(reallyFlat->id());
                    assert(it != e.varOccurrences.idx.end());
                    e.flatRemoveItem((*e.flat())[it->second]->cast<VarDeclI>());
                  }

                  output_vardecls(e, item, al);
                  vd->e(copy(e, e.cmap, al));
                  Type al_t(vd->e()->type());
                  al_t.ti(Type::TI_PAR);
                  vd->e()->type(al_t);
                }
              }
              if (needOutputAnn) {
                if (!is_output(vd->flat())) {
                  GCLock lock;
                  if (vd->type().dim() == 0) {
                    vd->flat()->addAnnotation(e.constants.ann.output_var);
                  } else {
                    std::vector<Expression*> args(vd->type().dim());
                    for (unsigned int i = 0; i < args.size(); i++) {
                      if (vd->ti()->ranges()[i]->domain() == nullptr) {
                        args[i] =
                            new SetLit(Location().introduce(),
                                       eval_intset(e, vd->flat()->ti()->ranges()[i]->domain()));
                      } else {
                        args[i] = new SetLit(Location().introduce(),
                                             eval_intset(e, vd->ti()->ranges()[i]->domain()));
                      }
                    }
                    auto* al = new ArrayLit(Location().introduce(), args);
                    args.resize(1);
                    args[0] = al;
                    vd->flat()->addAnnotation(
                        new Call(Location().introduce(), e.constants.ann.output_array, args));
                  }
                  check_rename_var(e, vd);
                }
              }
            }
            vd->flat(nullptr);
            // Remove enum type
            Type vdt = vd->type();
            vdt.enumId(0);
            vd->type(vdt);
            vd->ti()->type(vdt);
          }
          e.outputVarOccurrences.addIndex(item->cast<VarDeclI>(), static_cast<int>(i));
          CollectOccurrencesE ce(e, e.outputVarOccurrences, item);
          top_down(ce, vd);
        } break;
        case Item::II_OUT: {
          CollectOccurrencesE ce(e, e.outputVarOccurrences, item);
          top_down(ce, item->cast<OutputI>()->e());
        } break;
        case Item::II_FUN: {
          CollectOccurrencesE ce(e, e.outputVarOccurrences, item);
          top_down(ce, item->cast<FunctionI>()->e());
          top_down(ce, item->cast<FunctionI>()->ti());
          for (unsigned int i = item->cast<FunctionI>()->paramCount(); (i--) != 0U;) {
            top_down(ce, item->cast<FunctionI>()->param(i));
          }
        } break;
        default:
          throw FlatteningError(e, item->loc(), "invalid item in output model");
      }
    }
  }
  process_deletions(e);
}
}  // namespace MiniZinc
