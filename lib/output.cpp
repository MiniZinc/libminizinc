/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/optimize.hh>
#include <minizinc/output.hh>
#include <minizinc/typecheck.hh>

#include <string>

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
  return fi->rtype(env, tv, nullptr, false).isPar();
}

}  // namespace

void check_output_par_fn(EnvI& env, Call* rhs) {
  std::vector<Type> tv(rhs->argCount());
  for (unsigned int i = rhs->argCount(); (i--) != 0U;) {
    tv[i] = Expression::type(rhs->arg(i));
    tv[i].mkPar(env);
  }
  FunctionI* decl = env.output->matchFn(env, rhs->id(), tv, false);
  if (decl == nullptr) {
    FunctionI* origdecl = env.model->matchFn(env, rhs->id(), tv, false);
    if (origdecl == nullptr || !is_completely_par(env, origdecl, tv)) {
      std::ostringstream ss;
      ss << "function " << demonomorphise_identifier(rhs->id())
         << " is used in output, par version needed";
      throw FlatteningError(env, Expression::loc(rhs), ss.str());
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
  rhs->type(decl->rtype(env, tv, nullptr, false));
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
        tv[i] = Expression::type(c->arg(i));
        tv[i].mkPar(env);
      }
      FunctionI* decl = env.output->matchFn(env, c->id(), tv, false);
      Type t;
      if (decl == nullptr) {
        FunctionI* origdecl = env.model->matchFn(env, c->id(), tv, false);
        if (origdecl == nullptr) {
          std::ostringstream ss;
          ss << "function " << demonomorphise_identifier(c->id())
             << " is used in output, par version needed";
          throw FlatteningError(env, Expression::loc(c), ss.str());
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
            if (origdecl->e() != nullptr && !origdecl->fromStdLib()) {
              decl = copy(env, env.cmap, origdecl)->cast<FunctionI>();
              // We can use RHS for output, so this has to be able to be par
              auto rt = decl->ti()->type();
              rt.mkPar(env);
              decl->ti()->type(rt);

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
        t = decl->rtype(env, tv, nullptr, false);
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

bool rhs_contains_var_comp(EnvI& env, Expression* e) {
  if (e == nullptr) {
    return true;
  }

  class V : public EVisitor {
  public:
    EnvI& env;
    bool success;
    V(EnvI& env0) : env(env0), success(true) {}
    /// Visit anonymous variable
    void vAnonVar(const AnonVar* /*v*/) {}
    /// Visit array literal
    void vArrayLit(const ArrayLit* /*al*/) {}
    /// Visit array access
    void vArrayAccess(const ArrayAccess* /*aa*/) {}
    /// Visit array comprehension
    void vComprehension(const Comprehension* c) {
      for (int i = 0; i < c->numberOfGenerators(); i++) {
        const auto* g_in = c->in(i);
        if (g_in != nullptr) {
          const Type& ty_in = Expression::type(g_in);
          if (ty_in == Type::varsetint()) {
            success = false;
            break;
          }
          if (c->where(i) != nullptr) {
            if (Expression::type(c->where(i)) == Type::varbool()) {
              success = false;
              break;
            }
          }
        }
      }
    }
    /// Visit if-then-else
    void vITE(const ITE* /*ite*/) {}
    /// Visit binary operator
    void vBinOp(const BinOp* /*bo*/) {}
    /// Visit unary operator
    void vUnOp(const UnOp* /*uo*/) {}
    /// Visit call
    void vCall(Call* /*c*/) {}
    void vId(const Id* /*id*/) {}
    /// Visit let
    void vLet(const Let* /*let*/) {}
    /// Visit variable declaration
    void vVarDecl(const VarDecl* /*vd*/) {}
    /// Visit type inst
    void vTypeInst(const TypeInst* /*ti*/) {}
    /// Visit TIId
    void vTIId(const TIId* /*tiid*/) {}
    /// Determine whether to enter node
    bool enter(Expression* /*e*/) const { return success; }
  } _v(env);
  top_down(_v, e);

  return !_v.success;
}

void remove_is_output(VarDecl* vd) {
  if (vd == nullptr) {
    return;
  }
  Expression::ann(vd).remove(Constants::constants().ann.output_var);
  Expression::ann(vd).removeCall(Constants::constants().ann.output_array);
}

void copy_output(EnvI& e) {
  struct CopyOutput : public EVisitor {
    EnvI& env;
    CopyOutput(EnvI& env0) : env(env0) {}
    static void vId(Id* _id) { _id->decl(_id->decl()->flat()); }
    void vCall(Call* c) {
      std::vector<Type> tv(c->argCount());
      for (unsigned int i = c->argCount(); (i--) != 0U;) {
        tv[i] = Expression::type(c->arg(i));
        tv[i].mkPar(env);
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
        c->id(env.constants.ids.array1d);
        Expression* json =
            copy(env, env.cmap, create_json_output(env, outputObjective, false, false));
        std::vector<Expression*> new_args({json});
        Expression::type(new_args[0], Type::parstring(1));
        c->args(new_args);
      }
    }
  } _outputJSON(env);
  top_down(_outputJSON, e);
  class Par : public EVisitor {
  public:
    EnvI& env;
    Par(EnvI& env0) : env(env0) {}
    /// Visit variable declaration
    static void vVarDecl(VarDecl* vd) { vd->ti()->type(vd->type()); }
    /// Determine whether to enter node
    bool enter(Expression* e) {
      Type t = Expression::type(e);
      t.mkPar(env);
      t.cv(false);
      Expression::type(e, t);
      return true;
    }
  } _par(env);
  top_down(_par, e);
  class Decls : public EVisitor {
  public:
    EnvI& env;
    Decls(EnvI& env0) : env(env0) {}
    void vCall(Call* c) {
      if (c->id() == env.constants.ids.format || c->id() == env.constants.ids.show ||
          c->id() == env.constants.ids.showDzn || c->id() == env.constants.ids.showJSON) {
        Type argtype = Expression::type(c->arg(c->argCount() - 1));
        unsigned int typeId = argtype.typeId();
        if (typeId != 0U && argtype.dim() != 0) {
          const std::vector<unsigned int>& typeIds = env.getArrayEnum(typeId);
          typeId = typeIds[typeIds.size() - 1];
        }
        if (typeId > 0 && argtype.bt() == Type::BT_INT) {
          GCLock lock;
          Expression* obj = c->arg(c->argCount() - 1);
          Id* ti_id = env.getEnum(typeId)->e()->id();
          std::string enumName = create_enum_to_string_name(ti_id, "_toString_");
          bool is_json = c->id() == env.constants.ids.showJSON;
          const int dimensions = Expression::type(obj).dim();
          if (is_json && dimensions > 1) {
            // Create generators for dimensions selection
            std::vector<Expression*> slice_dimensions(dimensions);
            std::vector<Generator> generators;
            generators.reserve(dimensions - 1);
            auto* idx_ti = new TypeInst(Location().introduce(), Type::parint());
            for (int i = 0; i < dimensions - 1; ++i) {
              auto* idx_i = new VarDecl(Location().introduce(), idx_ti, env.genId());
              idx_i->toplevel(false);
              Call* index_set_xx = Call::a(
                  Location().introduce(),
                  "index_set_" + std::to_string(i + 1) + "of" + std::to_string(dimensions), {obj});
              index_set_xx->type(Type::parsetint());
              generators.push_back(Generator({idx_i}, index_set_xx, nullptr));
              slice_dimensions[i] =
                  new BinOp(Location().introduce(), idx_i->id(), BOT_DOTDOT, idx_i->id());
              Expression::type(slice_dimensions[i], Type::parsetint());
            }

            // Construct innermost slicing operation
            Call* index_set_n = Call::a(
                Location().introduce(),
                "index_set_" + std::to_string(dimensions) + "of" + std::to_string(dimensions),
                {obj});
            index_set_n->type(Type::parsetint());
            slice_dimensions[dimensions - 1] = index_set_n;
            auto* al_slice_dim = new ArrayLit(Location().introduce(), slice_dimensions);
            al_slice_dim->type(Type::parsetint(1));

            auto* slice_call =
                Call::a(Location().introduce(), "slice_1d", {obj, al_slice_dim, index_set_n});
            Type tt = Type::arrType(env, Type::partop(1), Expression::type(obj));
            slice_call->type(tt);
            Call* _toString_ENUM =
                Call::a(Location().introduce(), enumName,
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
              Call* cc = Call::a(Location().introduce(), "join", {comma, comp});
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
              Call* concat = Call::a(Location().introduce(), "concat", {al_concat});
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
        if (c->id() == env.constants.ids.showJSON && typeId > 0 && argtype.bt() == Type::BT_INT) {
          c->id(env.constants.ids.show);
        }
      }
      auto* fi = env.output->matchFn(env, c, false);
      if (fi != nullptr) {
        c->decl(fi);
      } else {
        c->decl(env.model->matchFn(env, c, false));
      }
    }
    void vBinOp(BinOp* bo) {
      std::vector<Expression*> args = {bo->lhs(), bo->rhs()};
      auto* fi = env.output->matchFn(env, bo->opToString(), args, false);
      if (fi != nullptr) {
        bo->decl(fi);
      } else {
        bo->decl(env.model->matchFn(env, bo->opToString(), args, false));
      }
    }
    void vUnop(UnOp* uo) {
      std::vector<Expression*> args = {uo->e()};
      auto* fi = env.output->matchFn(env, uo->opToString(), args, false);
      if (fi != nullptr) {
        uo->decl(fi);
      } else {
        uo->decl(env.model->matchFn(env, uo->opToString(), args, false));
      }
    }
  } _decls(env);
  top_down(_decls, e);
}

void check_rename_var(EnvI& e, VarDecl* vd, std::vector<Expression*> dimArgs, IntVal size1d) {
  auto* flat_copy = e.cmap.find(vd->flat());
  if (flat_copy != nullptr) {
    // Flat has been copied into the output, so use the copy as the ozn parameter
    if (vd == flat_copy) {
      if (!dimArgs.empty()) {
        Type t(vd->type());
        t.typeId(0);
        t.dim(1);
        auto* newTi = new TypeInst(Location().introduce(), Type::parint());
        newTi->domain(new SetLit(Location().introduce(), IntSetVal::a(1, size1d)));
        std::vector<TypeInst*> newRanges({newTi});
        vd->ti()->type(t);
        vd->ti()->setRanges(newRanges);
        vd->type(vd->ti()->type());
      }
    } else if (vd->id()->idn() != vd->flat()->id()->idn()) {
      // This is the original variable from the model, so just point it to the
      // flat copy which will be (or has been) processed in the above branch
      vd->e(Expression::cast<VarDecl>(flat_copy)->id());
    }
    return;
  }

  if (vd->id()->idn() != vd->flat()->id()->idn()) {
    auto* vd_rename_ti = Expression::cast<TypeInst>(copy(e, e.cmap, vd->ti()));
    if (!dimArgs.empty()) {
      // Change variable with the FlatZinc identifier to be 1d and 1-based
      Type t(vd_rename_ti->type());
      t.typeId(0);
      t.dim(1);
      auto* newTi = new TypeInst(Location().introduce(), Type::parint());
      newTi->domain(new SetLit(Location().introduce(), IntSetVal::a(1, size1d)));
      std::vector<TypeInst*> newRanges({newTi});
      vd_rename_ti->type(t);
      vd_rename_ti->setRanges(newRanges);
    }
    auto* vd_rename = new VarDecl(Location().introduce(), vd_rename_ti, vd->flat()->id()->idn());
    vd_rename->flat(vd->flat());
    make_par(e, vd_rename);
    Expression* vde = vd_rename->id();
    if (!dimArgs.empty()) {
      // Add arrayXd call
      const auto& arrayXdId = e.constants.ids.arrayNd(vd->ti()->type().dim());
      std::vector<Expression*> arrayXdargs;
      for (auto* e : dimArgs) {
        arrayXdargs.emplace_back(e);
      }
      arrayXdargs.emplace_back(vde);
      auto* arrayXd = Call::a(Location().introduce(), arrayXdId, arrayXdargs);
      arrayXd->type(vd->type());
      arrayXd->decl(e.model->matchFn(e, arrayXd, false));
      vde = arrayXd;
    }
    vd->e(vde);
    e.output->addItem(VarDeclI::a(Location().introduce(), vd_rename));
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
      Expression::ann(e).clear();
      switch (Expression::eid(e)) {
        case Expression::E_INTLIT:
        case Expression::E_FLOATLIT:
        case Expression::E_BOOLLIT:
        case Expression::E_STRINGLIT:
        case Expression::E_ID:
        case Expression::E_ANON:
        case Expression::E_TIID:
          break;
        case Expression::E_SETLIT:
          pushVec(stack, Expression::cast<SetLit>(e)->v());
          break;
        case Expression::E_ARRAYLIT:
          for (unsigned int i = 0; i < Expression::cast<ArrayLit>(e)->size(); i++) {
            stack.push_back((*Expression::cast<ArrayLit>(e))[i]);
          }
          break;
        case Expression::E_ARRAYACCESS:
          pushVec(stack, Expression::cast<ArrayAccess>(e)->idx());
          stack.push_back(Expression::cast<ArrayAccess>(e)->v());
          break;
        case Expression::E_FIELDACCESS:
          stack.push_back(Expression::cast<FieldAccess>(e)->v());
          stack.push_back(Expression::cast<FieldAccess>(e)->field());
          break;
        case Expression::E_COMP: {
          auto* comp = Expression::cast<Comprehension>(e);
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
          ITE* ite = Expression::cast<ITE>(e);
          stack.push_back(ite->elseExpr());
          for (int i = 0; i < ite->size(); i++) {
            stack.push_back(ite->ifExpr(i));
            stack.push_back(ite->thenExpr(i));
          }
        } break;
        case Expression::E_BINOP:
          stack.push_back(Expression::cast<BinOp>(e)->rhs());
          stack.push_back(Expression::cast<BinOp>(e)->lhs());
          break;
        case Expression::E_UNOP:
          stack.push_back(Expression::cast<UnOp>(e)->e());
          break;
        case Expression::E_CALL:
          for (unsigned int i = 0; i < Expression::cast<Call>(e)->argCount(); i++) {
            stack.push_back(Expression::cast<Call>(e)->arg(i));
          }
          break;
        case Expression::E_VARDECL:
          stack.push_back(Expression::cast<VarDecl>(e)->e());
          stack.push_back(Expression::cast<VarDecl>(e)->ti());
          break;
        case Expression::E_LET:
          stack.push_back(Expression::cast<Let>(e)->in());
          pushVec(stack, Expression::cast<Let>(e)->let());
          break;
        case Expression::E_TI:
          stack.push_back(Expression::cast<TypeInst>(e)->domain());
          pushVec(stack, Expression::cast<TypeInst>(e)->ranges());
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
    void vId(Id* ident) {
      if (ident == env.constants.absent) {
        return;
      }
      if (ident->decl() == nullptr || !ident->decl()->toplevel()) {
        return;
      }
      VarDecl* vd = ident->decl();
      VarDecl* reallyFlat = vd->flat();
      while (reallyFlat != nullptr && reallyFlat != reallyFlat->flat()) {
        reallyFlat = reallyFlat->flat();
      }
      auto idx = reallyFlat != nullptr ? env.outputFlatVarOccurrences.idx.find(reallyFlat->id())
                                       : std::make_pair(false, nullptr);
      if (idx.first && (*env.output)[*idx.second]->removed()) {
        idx = std::make_pair(false, nullptr);
        env.outputFlatVarOccurrences.idx.remove(reallyFlat->id());
      }
      auto idx2 = env.outputVarOccurrences.idx.find(vd->id());
      if (idx2.first && (*env.output)[*idx2.second]->removed()) {
        idx2 = std::make_pair(false, nullptr);
        env.outputVarOccurrences.idx.remove(vd->id());
      }
      if (!idx.first && !idx2.first) {
        auto* nvi =
            VarDeclI::a(Location().introduce(), Expression::cast<VarDecl>(copy(env, env.cmap, vd)));
        Type t = nvi->e()->ti()->type();
        t.mkPar(env);
        make_par(env, nvi->e());
        nvi->e()->ti()->eraseDomain();
        nvi->e()->flat(vd->flat());
        ClearAnnotations::run(nvi->e());
        nvi->e()->introduced(false);
        if (reallyFlat != nullptr) {
          env.outputFlatVarOccurrences.addIndex(reallyFlat, static_cast<int>(env.output->size()));
        }
        env.outputVarOccurrences.addIndex(nvi, static_cast<int>(env.output->size()));
        env.outputVarOccurrences.add(nvi->e(), ci);
        env.output->addItem(nvi);

        auto it = env.reverseMappers.find(nvi->e()->id());
        if (it != env.reverseMappers.end()) {
          Expression* rhs = copy(env, env.cmap, it->second());
          if (Call* crhs = Expression::dynamicCast<Call>(rhs)) {
            check_output_par_fn(env, crhs);
          }
          output_vardecls(env, nvi, it->second());
          nvi->e()->e(rhs);
        } else if ((reallyFlat != nullptr) && cannot_use_rhs_for_output(env, nvi->e()->e())) {
          assert(nvi->e()->flat());
          nvi->e()->e(nullptr);
          const auto dims =
              (nvi->e()->type().dim() == 0 ? 0 : Expression::type(reallyFlat->e()).dim());
          std::vector<Expression*> args(dims);
          IntVal flatSize = 1;
          if (nvi->e()->type().dim() == 0) {
            Expression::addAnnotation(reallyFlat, env.constants.ann.output_var);
          } else {
            for (unsigned int i = 0; i < args.size(); i++) {
              IntSetVal* range;
              if (nvi->e()->ti()->ranges()[i]->domain() == nullptr) {
                range = eval_intset(env, reallyFlat->ti()->ranges()[i]->domain());
              } else {
                range = eval_intset(env, nvi->e()->ti()->ranges()[i]->domain());
              }
              args[i] = new SetLit(Location().introduce(), range);
              flatSize *= range->empty() ? 0 : (range->max() - range->min() + 1);
            }
            if (env.fopts.ignoreStdlib) {
              // Ensure array?d call output by solver is available in output model
              std::vector<Type> ts(dims + 1);
              for (auto i = 0; i < dims; i++) {
                ts[i] = Type::parsetint();
              }
              ts[dims] = Expression::type(reallyFlat->e());
              std::stringstream ss;
              ss << "array" << dims << "d";
              ASTString ident(ss.str());
              if (env.output->matchFn(env, ident, ts, false) == nullptr) {
                auto* decl = copy(env, env.cmap, env.model->matchFn(env, ident, ts, true))
                                 ->cast<FunctionI>();
                (void)env.output->registerFn(env, decl, true);
                env.output->addItem(decl);
              }
              // Ensure array1d for solver output is available
              ident = env.constants.ids.array1d;
              ts = {Type::parsetint(), Expression::type(reallyFlat->e())};
              if (env.output->matchFn(env, ident, ts, false) == nullptr) {
                auto* decl = copy(env, env.cmap, env.model->matchFn(env, ident, ts, true))
                                 ->cast<FunctionI>();
                (void)env.output->registerFn(env, decl, true);
                env.output->addItem(decl);
              }
            }
            std::vector<Expression*> alArgs(
                {new SetLit(Location().introduce(), IntSetVal::a(1, flatSize))});
            auto* al = new ArrayLit(Location().introduce(), alArgs);
            Expression::addAnnotation(
                reallyFlat, Call::a(Location().introduce(), env.constants.ann.output_array, {al}));
          }
          check_rename_var(env, nvi->e(), args, flatSize);
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
          !Expression::ann(vdi->e()).contains(e.constants.ann.mzn_check_var) &&
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
      if (cur_idx.first) {
        auto* vdi = (*e.output)[*cur_idx.second]->cast<VarDeclI>();
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
    VarOccurrences::Items keptItems;
    for (auto* iit : it) {
      if (!iit->removed()) {
        keptItems.insert(iit);
      }
    }
    it = keptItems;
  }
}

Expression* create_dzn_output(EnvI& e, bool includeObjective, bool includeOutputItem,
                              bool includeChecker) {
  std::vector<Expression*> outputVars;

  for (auto& it : e.outputVars) {
    auto* vd = Expression::cast<VarDecl>(it());

    if (!includeObjective &&
        (vd->id()->str() == "_objective" || vd->id()->str() == "_checker_objective")) {
      // Skip _objective if disabled
      continue;
    }

    std::ostringstream s;
    s << Printer::quoteId(vd->id()->str()) << " = ";
    bool needArrayXd = false;
    if (vd->type().dim() > 0) {
      ArrayLit* al = nullptr;
      if (!Expression::ann(vd).contains(e.constants.ann.output_only)) {
        if ((vd->flat() != nullptr) && (vd->flat()->e() != nullptr)) {
          al = eval_array_lit(e, vd->flat()->e());
        } else if (vd->e() != nullptr) {
          al = eval_array_lit(e, vd->e());
        }
      }
      if (al == nullptr || !al->empty()) {
        // First check if we can use an array literal representation
        // or whether we have to introduce an arrayXd call
        if (vd->type().dim() <= 2 && al != nullptr) {
          if (vd->type().dim() == 2) {
            s << "\n";
          }
          auto* sl = new StringLit(Location().introduce(), s.str());
          outputVars.push_back(sl);

          unsigned int idx1EnumId =
              (vd->type().typeId() != 0 ? e.getArrayEnum(vd->type().typeId())[0] : 0);
          unsigned int xEnumId =
              (vd->type().typeId() != 0 ? e.getArrayEnum(vd->type().typeId())[al->dims()] : 0);

          if (al->dims() == 1) {
            // 1d array
            if (idx1EnumId == 0 && al->min(0) == 1) {
              // We can use a simple 1d array literal
              auto* show = Call::a(Location().introduce(), ASTString("showDzn"), {vd->id()});
              show->type(Type::parstring());
              FunctionI* fi = e.model->matchFn(e, show, false);
              assert(fi);
              show->decl(fi);
              outputVars.push_back(show);
              std::string ends = needArrayXd ? ")" : "";
              ends += ";\n";
              auto* eol = new StringLit(Location().introduce(), ends);
              outputVars.push_back(eol);
              continue;
            }

            /*

             let {
               array[int] of string: idx = [ showDzn(to_enum(..., i)) | i in al->min(0)..al->max(0)
             ] array[int] of string: x = [ showDzn(al[i]) | i in al->min(0)..al->max(0) ] } in
             show_indexed(idx, x)

             */

            Comprehension* indexes;
            Comprehension* values;
            auto* index_set = Call::a(Location().introduce(), "index_set", {vd->id()});
            index_set->type(Type::varsetint());
            index_set->decl(e.model->matchFn(e, index_set, false));

            {
              auto* i_ti = new TypeInst(Location().introduce(), Type::parenum(idx1EnumId));
              auto* i_vd = new VarDecl(Location().introduce(), i_ti, e.genId());
              i_vd->toplevel(false);

              Generators g;
              g.g.emplace_back(std::vector<VarDecl*>({i_vd}), index_set, nullptr);
              auto* show_i = Call::a(Location().introduce(), ASTString("showDzn"), {i_vd->id()});
              show_i->type(Type::parstring());
              FunctionI* fi = e.model->matchFn(e, show_i, false);
              assert(fi);
              show_i->decl(fi);

              indexes = new Comprehension(Location().introduce(), show_i, g, false);
              Expression::type(indexes, Type::parstring(1));
            }
            {
              auto* i_ti = new TypeInst(Location().introduce(), Type::parenum(idx1EnumId));
              auto* i_vd = new VarDecl(Location().introduce(), i_ti, e.genId());
              i_vd->toplevel(false);

              Generators g;
              g.g.emplace_back(std::vector<VarDecl*>({i_vd}), index_set, nullptr);

              auto* aa = new ArrayAccess(Location().introduce(), vd->id(), {i_vd->id()});
              Type vd_t = vd->type();
              vd_t.typeId(0);
              vd_t.dim(0);
              vd_t.typeId(xEnumId);
              aa->type(vd_t);

              auto* show_i = Call::a(Location().introduce(), ASTString("showDzn"), {aa});
              show_i->type(Type::parstring());
              FunctionI* fi = e.model->matchFn(e, show_i, false);
              assert(fi);
              show_i->decl(fi);

              values = new Comprehension(Location().introduce(), show_i, g, false);
              Expression::type(values, Type::parstring(1));
            }

            auto* idx_ti_ti = new TypeInst(Location().introduce(), Type::parint());
            auto* idx_ti = new TypeInst(Location().introduce(), Type::parstring(1));
            idx_ti->setRanges({idx_ti_ti});
            auto* idx_vd = new VarDecl(Location().introduce(), idx_ti, e.genId(), indexes);
            idx_vd->toplevel(false);

            auto* x_ti_ti = new TypeInst(Location().introduce(), Type::parint());
            auto* x_ti = new TypeInst(Location().introduce(), Type::parstring(1));
            x_ti->setRanges({x_ti_ti});
            auto* x_vd = new VarDecl(Location().introduce(), x_ti, e.genId(), values);
            x_vd->toplevel(false);

            auto* show_indexed = Call::a(Location().introduce(), ASTString("show_indexed"),
                                         {idx_vd->id(), x_vd->id()});
            FunctionI* fi = e.model->matchFn(e, show_indexed, false);
            assert(fi);
            show_indexed->decl(fi);
            show_indexed->type(Type::parstring());
            auto* let = new Let(Location().introduce(), {idx_vd, x_vd}, show_indexed);
            let->type(Type::parstring());
            outputVars.push_back(let);

            auto* eol = new StringLit(Location().introduce(), ";\n");
            outputVars.push_back(eol);
          } else {
            // 2d array

            unsigned int idx2EnumId =
                (vd->type().typeId() != 0 ? e.getArrayEnum(vd->type().typeId())[1] : 0);

            /*

             let {
               array[int] of string: idx1 = [ showDzn(to_enum(..., i)) | i in al->min(0)..al->max(0)
             ] array[int] of string: idx2 = [ showDzn(to_enum(..., i)) | i in al->min(1)..al->max(1)
             ] array[int,int] of string: x = [ (i,j) : showDzn(al[i]) | i in al->min(0)..al->max(0),
             j in al->min(1)..al->max(1) ] } in show2d_headers(idx1, idx2, x)

             */
            std::vector<Expression*> indexes(2);
            Comprehension* values;
            Call* index_set[2];

            for (int i = 0; i < 2; i++) {
              index_set[i] =
                  Call::a(Location().introduce(),
                          std::string("index_set_") + std::to_string(i + 1) + std::string("of2"),
                          {vd->id()});
              index_set[i]->type(Type::varsetint());
              index_set[i]->decl(e.model->matchFn(e, index_set[i], false));
              Expression* index;
              if ((i == 0 && idx1EnumId == 0 && al->min(0) == 1) ||
                  (i == 1 && idx2EnumId == 0 && al->min(1) == 1)) {
                index = new ArrayLit(Location().introduce(), std::vector<Expression*>());
              } else {
                auto* i_ti = new TypeInst(Location().introduce(),
                                          Type::parenum(i == 0 ? idx1EnumId : idx2EnumId));
                auto* i_vd = new VarDecl(Location().introduce(), i_ti, e.genId());
                i_vd->toplevel(false);

                Generators g;
                g.g.emplace_back(std::vector<VarDecl*>({i_vd}), index_set[i], nullptr);
                auto* show_i = Call::a(Location().introduce(), ASTString("showDzn"), {i_vd->id()});
                show_i->type(Type::parstring());
                FunctionI* fi = e.model->matchFn(e, show_i, false);
                assert(fi);
                show_i->decl(fi);

                ArrayLit* idxlit =
                    ArrayLit::constructTuple(Location().introduce(), {i_vd->id(), show_i});
                Type tyIdxLit = Type::tuple();
                tyIdxLit.typeId(Type::COMP_INDEX);
                idxlit->type(tyIdxLit);

                index = new Comprehension(Location().introduce(), idxlit, g, false);
                Expression::type(index, Type::parstring(1));
              }
              Expression::type(index, Type::parstring(1));
              indexes[i] = index;
            }
            {
              auto* i_ti = new TypeInst(Location().introduce(), Type::parenum(idx1EnumId));
              auto* i_vd = new VarDecl(Location().introduce(), i_ti, e.genId());
              i_vd->toplevel(false);
              auto* j_ti = new TypeInst(Location().introduce(), Type::parenum(idx2EnumId));
              auto* j_vd = new VarDecl(Location().introduce(), j_ti, e.genId());
              j_vd->toplevel(false);

              Generators g;
              g.g.emplace_back(std::vector<VarDecl*>({i_vd}), index_set[0], nullptr);
              g.g.emplace_back(std::vector<VarDecl*>({j_vd}), index_set[1], nullptr);

              auto* aa =
                  new ArrayAccess(Location().introduce(), vd->id(), {i_vd->id(), j_vd->id()});
              Type vd_t = vd->type();
              vd_t.typeId(0);
              vd_t.dim(0);
              vd_t.typeId(xEnumId);
              aa->type(vd_t);

              auto* show_i = Call::a(Location().introduce(), ASTString("showDzn"), {aa});
              show_i->type(Type::parstring());
              FunctionI* fi = e.model->matchFn(e, show_i, false);
              assert(fi);
              show_i->decl(fi);

              ArrayLit* idxlit = ArrayLit::constructTuple(Location().introduce(),
                                                          {i_vd->id(), j_vd->id(), show_i});
              Type tyIdxLit = Type::tuple();
              tyIdxLit.typeId(Type::COMP_INDEX);
              idxlit->type(tyIdxLit);

              values = new Comprehension(Location().introduce(), idxlit, g, false);
              auto values_type = Type::parstring(2);
              values_type.typeId(
                  e.registerArrayEnum({i_vd->type().typeId(), j_vd->type().typeId(), 0}));
              Expression::type(values, values_type);
            }

            auto* idx1_ti_ti = new TypeInst(Location().introduce(), Type::parint());
            auto* idx1_ti = new TypeInst(Location().introduce(), Type::parstring(1));
            idx1_ti->setRanges({idx1_ti_ti});
            auto* idx1_vd = new VarDecl(Location().introduce(), idx1_ti, e.genId(), indexes[0]);
            idx1_vd->toplevel(false);

            auto* idx2_ti_ti = new TypeInst(Location().introduce(), Type::parint());
            auto* idx2_ti = new TypeInst(Location().introduce(), Type::parstring(1));
            idx2_ti->setRanges({idx2_ti_ti});
            auto* idx2_vd = new VarDecl(Location().introduce(), idx2_ti, e.genId(), indexes[1]);
            idx2_vd->toplevel(false);

            auto* x_ti_ti = new TypeInst(Location().introduce(), Type::parint());
            auto* x_ti = new TypeInst(Location().introduce(), Type::parstring(2));
            x_ti->setRanges({x_ti_ti, x_ti_ti});
            auto* x_vd = new VarDecl(Location().introduce(), x_ti, e.genId(), values);
            x_vd->toplevel(false);

            auto* show_indexed = Call::a(Location().introduce(), ASTString("show2d_indexed"),
                                         {idx1_vd->id(), idx2_vd->id(), x_vd->id()});
            FunctionI* fi = e.model->matchFn(e, show_indexed, false);
            assert(fi);
            show_indexed->decl(fi);
            show_indexed->type(Type::parstring());
            auto* let = new Let(Location().introduce(), {idx1_vd, idx2_vd, x_vd}, show_indexed);
            let->type(Type::parstring());
            outputVars.push_back(let);

            auto* eol = new StringLit(Location().introduce(), ";\n");
            outputVars.push_back(eol);
          }
          continue;
        }
        needArrayXd = true;
        s << "array" << vd->type().dim() << "d(";
        for (int i = 0; i < vd->type().dim(); i++) {
          unsigned int enumId =
              (vd->type().typeId() != 0 ? e.getArrayEnum(vd->type().typeId())[i] : 0);
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
              ASTString toString(std::string("_toString_") +
                                 e.getEnum(enumId)->e()->id()->str().c_str());

              auto* toStringMin =
                  Call::a(Location().introduce(), toString,
                          {IntLit::a(idxMin), e.constants.literalFalse, e.constants.literalFalse});
              toStringMin->type(Type::parstring());
              FunctionI* toStringMin_fi = e.model->matchFn(e, toStringMin, false);
              toStringMin->decl(toStringMin_fi);
              outputVars.push_back(toStringMin);

              sl = new StringLit(Location().introduce(), "..");
              outputVars.push_back(sl);

              auto* toStringMax =
                  Call::a(Location().introduce(), toString,
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
            auto* index_set_xx = Call::a(Location().introduce(), index_set_fn, {vd->id()});
            index_set_xx->type(Type::parsetint());
            auto* i_fi = e.model->matchFn(e, index_set_xx, false);
            assert(i_fi);
            index_set_xx->decl(i_fi);

            auto* show = Call::a(Location().introduce(), e.constants.ids.show, {index_set_xx});
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
    Call* show = Call::a(Location().introduce(), e.constants.ids.showDzn, showArgs);
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
      Call* concat = Call::a(Location().introduce(), ASTString("concat"), {oi->e()});
      concat->type(Type::parstring());
      FunctionI* fi = e.model->matchFn(e, concat, false);
      assert(fi);
      concat->decl(fi);
      Call* show = Call::a(Location().introduce(), ASTString("showDzn"), {concat});
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
    auto* checker_output = Call::a(Location().introduce(), ASTString("showCheckerOutput"), {});
    checker_output->type(Type::parstring());
    FunctionI* fi = e.model->matchFn(e, checker_output, false);
    assert(fi);
    checker_output->decl(fi);
    auto* show = Call::a(Location().introduce(), ASTString("showDzn"), {checker_output});
    show->type(Type::parstring());
    fi = e.model->matchFn(e, show, false);
    assert(fi);
    show->decl(fi);
    outputVars.push_back(show);
    outputVars.push_back(new StringLit(Location().introduce(), ";\n"));
  }

  auto* al = new ArrayLit(Location().introduce(), outputVars);
  al->type(Type::parstring(1));
  return al;
}

ArrayLit* create_json_output(EnvI& e, bool includeObjective, bool includeOutputItem,
                             bool includeChecker) {
  std::vector<Expression*> outputVars;
  outputVars.push_back(new StringLit(Location().introduce(), "{\n"));

  bool firstVar = true;
  for (auto& it : e.outputVars) {
    auto* vd = Expression::cast<VarDecl>(it());

    if (!includeObjective &&
        (vd->id()->str() == "_objective" || vd->id()->str() == "_checker_objective")) {
      // Skip _objective if disabled
      continue;
    }

    std::ostringstream s;
    if (firstVar) {
      firstVar = false;
    } else {
      s << ",\n";
    }
    s << "  \"" << Printer::escapeStringLit(vd->id()->str()) << "\""
      << " : ";
    auto* sl = new StringLit(Location().introduce(), s.str());
    outputVars.push_back(sl);

    std::vector<Expression*> showArgs(1);
    showArgs[0] = vd->id();
    Call* show = Call::a(Location().introduce(), e.constants.ids.showJSON, showArgs);
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
      Call* concat = Call::a(Location().introduce(), ASTString("concat"), {oi->e()});
      concat->type(Type::parstring());
      FunctionI* fi = e.model->matchFn(e, concat, false);
      assert(fi);
      concat->decl(fi);
      Call* show = Call::a(Location().introduce(), ASTString("showJSON"), {concat});
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
    Call* checker_output = Call::a(Location().introduce(), ASTString("showCheckerOutput"), {});
    checker_output->type(Type::parstring());
    FunctionI* fi = e.model->matchFn(e, checker_output, false);
    assert(fi);
    checker_output->decl(fi);
    Call* show = Call::a(Location().introduce(), ASTString("showJSON"), {checker_output});
    show->type(Type::parstring());
    fi = e.model->matchFn(e, show, false);
    assert(fi);
    show->decl(fi);
    outputVars.push_back(show);
  }

  outputVars.push_back(new StringLit(Location().introduce(), "\n}\n"));
  auto* al = new ArrayLit(Location().introduce(), outputVars);
  al->type(Type::parstring(1));
  return al;
}

Expression* create_encapsulated_output(EnvI& e) {
  std::vector<Expression*> es;
  // Output each section as key-value pairs (solns2out will wrap in {})
  es.push_back(new StringLit(Location().introduce(), "\"output\": {"));
  std::stringstream suffix;
  suffix << "}, \"sections\": [";
  bool first = true;
  for (const auto& it : e.outputSections) {
    if (first) {
      es.push_back(new StringLit(Location().introduce(),
                                 "\"" + Printer::escapeStringLit(it.first) + "\": "));
      first = false;
    } else {
      es.push_back(new StringLit(Location().introduce(),
                                 ", \"" + Printer::escapeStringLit(it.first) + "\": "));
      suffix << ", ";
    }
    bool isJSON = it.first == "json" || it.first.endsWith("_json");
    auto* concat = Call::a(Location().introduce(), "concat", {it.second});
    concat->type(Type::parstring());
    concat->decl(e.model->matchFn(e, concat, false));
    if (isJSON) {
      es.push_back(concat);
    } else {
      auto* showJSON = Call::a(Location().introduce(), "showJSON", {concat});
      showJSON->type(Type::parstring());
      showJSON->decl(e.model->matchFn(e, showJSON, false));
      es.push_back(showJSON);
    }
    suffix << "\"" << Printer::escapeStringLit(it.first) << "\"";
  }
  suffix << "]";
  es.push_back(new StringLit(Location().introduce(), suffix.str()));
  auto* al = new ArrayLit(Location().introduce(), es);
  al->type(Type::parstring(1));
  return al;
}

void process_toplevel_output_vars(EnvI& e) {
  GCLock lock;

  bool outputForChecker = e.fopts.outputMode == FlatteningOptions::OutputMode::OUTPUT_CHECKER;

  class OutputVarVisitor : public ItemVisitor {
  private:
    EnvI& _e;
    bool _outputForChecker;
    bool _isChecker;

  public:
    OutputVarVisitor(EnvI& e, bool outputForChecker)
        : _e(e),
          _outputForChecker(outputForChecker),
          _isChecker(e.model->filename().endsWith(".mzc") ||
                     e.model->filename().endsWith(".mzc.mzn")) {}

    bool hasAddToOutput = false;
    std::vector<std::pair<int, VarDecl*>> todo;

    void vVarDeclI(VarDeclI* vdi) {
      auto* vd = vdi->e();
      if (_outputForChecker) {
        if (Expression::ann(vd).contains(_e.constants.ann.mzn_check_var)) {
          _e.outputVars.emplace_back(vd);
        }
      } else {
        if (Expression::ann(vd).contains(_e.constants.ann.add_to_output)) {
          hasAddToOutput = true;
          todo.clear();  // Skip 2nd pass
          _e.outputVars.emplace_back(vd);
        } else if (Expression::ann(vd).contains(_e.constants.ann.output) ||
                   (!_isChecker && vd->id()->idn() == -1 && vd->id()->v() == "_objective") ||
                   ((_isChecker && vd->id()->idn() == -1 &&
                     vd->id()->v() == "_checker_objective"))) {
          // Whether or not to actually include will be determined later
          _e.outputVars.emplace_back(vd);
        } else if (!hasAddToOutput) {
          todo.emplace_back(_e.outputVars.size(), vd);  // Needs to be processed in 2nd pass
        }
      }
      Expression::ann(vd).remove(_e.constants.ann.output);
    }
  } ovv(e, outputForChecker);
  iter_items(ovv, e.model);

  if (!outputForChecker) {
    // Insert implicit output variables
    int inserted = 0;
    for (auto& it : ovv.todo) {
      auto idx = it.first;
      auto* vd = it.second;
      if (Expression::ann(vd).contains(e.constants.ann.no_output) || Expression::type(vd).isPar()) {
        continue;
      }
      if (vd->e() == nullptr || Expression::ann(vd).contains(e.constants.ann.rhs_from_assignment)) {
        // Output anything without a RHS
        e.outputVars.emplace(e.outputVars.begin() + inserted + idx, vd);
        inserted++;
        continue;
      }
      if (auto* al = Expression::dynamicCast<ArrayLit>(vd->e())) {
        // Output array literals containing _
        for (unsigned int i = 0; i < al->size(); i++) {
          if (Expression::isa<AnonVar>((*al)[i])) {
            e.outputVars.emplace(e.outputVars.begin() + inserted + idx, vd);
            inserted++;
            break;
          }
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
  Expression* o = nullptr;
  for (const auto& it : e.outputSections) {
    if (!e.outputSectionEnabled(it.first)) {
      continue;
    }
    if (o == nullptr) {
      o = it.second;
    } else {
      o = new BinOp(Location().introduce(), o, BOT_PLUSPLUS, it.second);
      Expression::type(o, Type::parstring(1));
    }
  }
  if (o != nullptr) {
    e.outputSections.add(ASTString("raw"), o);  // Add to raw section for encapsulation
    e.model->addItem(new OutputI(Location().introduce(), o));
  }

  // Only include _checker output if not JSON encapsulated
  bool includeChecker = hasChecker && !encapsulateJSON;
  switch (outputMode) {
    case FlatteningOptions::OUTPUT_DZN:
      o = create_dzn_output(e, outputObjective, includeOutputItem, includeChecker);
      e.outputSections.add(ASTString("dzn"), o);  // Add to dzn section for encapsulation
      break;
    case FlatteningOptions::OUTPUT_JSON:
      o = create_json_output(e, outputObjective, includeOutputItem, includeChecker);
      e.outputSections.add(ASTString("json"), o);  // Add to json section for encapsulation
      break;
    case FlatteningOptions::OUTPUT_CHECKER:
      o = create_dzn_output(e, true, false, false);
      e.outputSections.add(ASTString("dzn"), o);  // Add to dzn section for encapsulation
      break;
    default:
      if (e.outputSections.noUserDefined()) {
        // If no user defined output, use dzn output (we still want to generate dzn output if there
        // are only vis_json sections for example)
        auto* dzno = create_dzn_output(e, outputObjective, false, false);
        e.outputSections.add(ASTString("dzn"), dzno);  // Add to dzn section for encapsulation
        // Combine with other output so we don't lose other sections
        if (o == nullptr) {
          o = dzno;
        } else {
          o = new BinOp(Location().introduce(), dzno, BOT_PLUSPLUS, o);
          Expression::type(o, Type::parstring(1));
        }
      }
      break;
  }

  if (encapsulateJSON) {
    // Replace output with json stream output
    o = create_encapsulated_output(e);
  }
  if (o == nullptr) {
    // All sections disabled
    o = new ArrayLit(Location().introduce(), std::vector<Expression*>());
    Expression::type(o, Type::parstring(1));
  }
  auto* newOutputItem = new OutputI(Location().introduce(), o);
  e.model->addItem(newOutputItem);

  // Copy output item from model into output model
  outputItem = copy(e, e.cmap, e.model->outputItem())->cast<OutputI>();
  make_par(e, outputItem->e());
  e.output->addItem(outputItem);

  // Copy all function definitions that are required for output into the output model
  class CollectFunctions : public EVisitor {
  public:
    EnvI& env;
    CollectFunctions(EnvI& env0) : env(env0) {}
    bool enter(Expression* e) {
      if (Expression::type(e).isvar()) {
        Type t = Expression::type(e);
        t.mkPar(env);
        Expression::type(e, t);
      }
      return true;
    }
    void vId(Id* i) {
      // Also collect functions from output_only variables we depend on
      if ((i->decl() != nullptr) &&
          Expression::ann(i->decl()).contains(env.constants.ann.output_only)) {
        top_down(*this, i->decl()->e());
      }
    }
    void vCall(Call* c) {
      std::vector<Type> tv(c->argCount());
      for (unsigned int i = c->argCount(); (i--) != 0U;) {
        tv[i] = Expression::type(c->arg(i));
        tv[i].mkPar(env);
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
          throw FlatteningError(env, Expression::loc(c), ss.str());
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

  // Copying the output item and the functions it depends on has created copies
  // of all dependent VarDecls. However the output model does not contain VarDeclIs for
  // these VarDecls yet. This iterator processes all copied variable declarations and
  // creates the corresponding VarDeclI in the output model.
  class CollectVarDecls : public EVisitor {
  public:
    EnvI& env;
    std::unordered_set<FunctionI*> visited;
    CollectVarDecls(EnvI& env0) : env(env0) {}

    void vCall(Call* c) {
      if (c->decl() != nullptr && !c->decl()->fromStdLib()) {
        auto it = visited.emplace(c->decl());
        if (it.second) {
          top_down(*this, c->decl()->e());
        }
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
      top_down(*this, vd->ti());
      auto idx = env.outputVarOccurrences.find(vd);
      if (idx == -1) {
        auto* orig = env.cmap.findOrig(vd);
        if (orig == nullptr) {
          // Did not copy this in (came from rename var)
          // TODO: any other reason?
          return;
        }
        auto* vd_orig = Expression::cast<VarDecl>(orig);
        Location loc = Expression::loc(vd);  // Close enough
        auto* vdi_copy = VarDeclI::a(loc, vd);
        vd->ti()->eraseDomain();
        vd->flat(vd_orig->flat());
        vd->ti()->setIsEnum(false);
        Expression::ann(vd).clear();
        vd->introduced(false);

        if (!vd_orig->type().isPar()) {
          if (vd->flat() == nullptr && vd_orig->e() != nullptr &&
              Expression::type(vd_orig->e()).isPar()) {
            // Don't have a flat version of this variable, but the original has a right hand
            // side that is par, so we can use that.
            Expression* flate = eval_par(env, vd_orig->e());
            output_vardecls(env, vdi_copy, flate);
            vd->e(flate);
          } else {
            auto* vd_followed = Expression::cast<VarDecl>(follow_id_to_decl(vd->id()));
            VarDecl* reallyFlat = vd_followed->flat();
            while ((reallyFlat != nullptr) && reallyFlat != reallyFlat->flat()) {
              reallyFlat = reallyFlat->flat();
            }
            if (reallyFlat == nullptr) {
              // The variable doesn't have a flat version. This can only happen if
              // the original variable had type-inst var, but a right-hand-side that
              // was par, so follow_id_to_decl lead to a par variable.
              assert(vd_followed->e() && Expression::type(vd_followed->e()).isPar());
              Expression* flate = eval_par(env, vd_followed->e());
              output_vardecls(env, vdi_copy, flate);
              vd_followed->e(flate);
            } else if ((vd_followed->flat()->e() != nullptr) &&
                       Expression::type(vd_followed->flat()->e()).isPar()) {
              // We can use the right hand side of the flat version of this variable
              Expression* flate = copy(env, env.cmap, follow_id(reallyFlat->id()));
              output_vardecls(env, vdi_copy, flate);
              vd_followed->e(flate);
            } else {
              auto it = env.reverseMappers.find(vd_followed->id());
              if (it != env.reverseMappers.end()) {
                // Found a reverse mapper, so we need to add the mapping function to the
                // output model to map the FlatZinc value back to the model variable.
                Expression* rhs = copy(env, env.cmap, it->second());
                if (Call* crhs = Expression::dynamicCast<Call>(rhs)) {
                  check_output_par_fn(env, crhs);
                }
                output_vardecls(env, vdi_copy, rhs);
                top_down(*this, rhs);
                vd_followed->e(rhs);
              } else if (reallyFlat == vd_orig ||
                         cannot_use_rhs_for_output(env, vd_followed->e()) ||
                         rhs_contains_var_comp(env, vd_orig->e())) {
                // If the VarDecl does not have a usable right hand side, it needs to be
                // marked as output in the FlatZinc
                vd_followed->e(nullptr);
                assert(vd_followed->flat());
                if (vd_followed->type().dim() == 0) {
                  Expression::addAnnotation(vd_followed->flat(), env.constants.ann.output_var);
                  check_rename_var(env, vd_followed, {}, 0);
                } else {
                  bool needOutputAnn = true;
                  if (auto* al = Expression::dynamicCast<ArrayLit>(reallyFlat->e())) {
                    for (unsigned int i = 0; i < al->size(); i++) {
                      if (Id* id = Expression::dynamicCast<Id>((*al)[i])) {
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
                    const auto dims = vd_orig->type().dim();
                    std::vector<Expression*> args(dims);
                    IntVal flatSize = 1;
                    for (unsigned int i = 0; i < args.size(); i++) {
                      IntSetVal* range;
                      if (vd_orig->ti()->ranges()[i]->domain() == nullptr) {
                        range = eval_intset(env, vd_followed->flat()->ti()->ranges()[i]->domain());
                      } else {
                        range = eval_intset(env, vd_followed->ti()->ranges()[i]->domain());
                      }
                      args[i] = new SetLit(Location().introduce(), range);
                      flatSize *= range->empty() ? 0 : (range->max() - range->min() + 1);
                    }
                    if (env.fopts.ignoreStdlib) {
                      // Ensure array?d call output by solver is available in output model
                      std::vector<Type> ts(dims + 1);
                      for (auto i = 0; i < dims; i++) {
                        ts[i] = Type::parsetint();
                      }
                      ts[dims] = Expression::type(reallyFlat->e());
                      std::stringstream ss;
                      ss << "array" << dims << "d";
                      ASTString ident(ss.str());
                      if (env.output->matchFn(env, ident, ts, false) == nullptr) {
                        auto* decl = copy(env, env.cmap, env.model->matchFn(env, ident, ts, true))
                                         ->cast<FunctionI>();
                        (void)env.output->registerFn(env, decl, true);
                        env.output->addItem(decl);
                      }
                      // Ensure array1d for solver output is available
                      ident = env.constants.ids.array1d;
                      ts = {Type::parsetint(), Expression::type(reallyFlat->e())};
                      if (env.output->matchFn(env, ident, ts, false) == nullptr) {
                        auto* decl = copy(env, env.cmap, env.model->matchFn(env, ident, ts, true))
                                         ->cast<FunctionI>();
                        (void)env.output->registerFn(env, decl, true);
                        env.output->addItem(decl);
                      }
                    }
                    std::vector<Expression*> alArgs(
                        {new SetLit(Location().introduce(), IntSetVal::a(1, flatSize))});
                    auto* al = new ArrayLit(Location().introduce(), alArgs);
                    Expression::addAnnotation(
                        vd_followed->flat(),
                        Call::a(Location().introduce(), env.constants.ann.output_array, {al}));
                    check_rename_var(env, vd_followed, args, flatSize);
                  }
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
            output_vardecls(env, vdi_copy, vd_orig->ti());
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

  // If we are checking solutions using a checker model, all parameters of the checker model
  // have to be made available in the output model
  class OV1 : public ItemVisitor {
  public:
    EnvI& env;
    CollectFunctions& cf;
    CollectVarDecls& cvd;
    OV1(EnvI& env0, CollectFunctions& cf0, CollectVarDecls& cvd0) : env(env0), cf(cf0), cvd(cvd0) {}
    void vVarDeclI(VarDeclI* vdi) {
      if (Expression::ann(vdi->e()).contains(env.constants.ann.mzn_check_var)) {
        auto* output_vd = Expression::cast<VarDecl>(copy(env, env.cmap, vdi->e()));
        top_down(cf, output_vd);
        top_down(cvd, output_vd->id());
        Expression::addAnnotation(output_vd, env.constants.ann.mzn_check_var);
        Call* checkVarEnum =
            Expression::ann(vdi->e()).getCall(env.constants.ann.mzn_check_enum_var);
        if (checkVarEnum != nullptr) {
          Expression::addAnnotation(output_vd, checkVarEnum);
        }
      }
    }
  } _ov1(e, _cf, _cvd);
  iter_items(_ov1, e.model);

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
          GCLock lock;
          VarDecl* reallyFlat = vd->flat();
          while ((reallyFlat != nullptr) && reallyFlat != reallyFlat->flat()) {
            reallyFlat = reallyFlat->flat();
          }
          if (vd->e() == nullptr) {
            if (((vd->flat()->e() != nullptr) && Expression::type(vd->flat()->e()).isPar()) ||
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
            } else {
              auto it = e.reverseMappers.find(vd->id());
              if (it != e.reverseMappers.end()) {
                Call* rhs = Expression::cast<Call>(copy(e, e.cmap, it->second()));
                check_output_par_fn(e, rhs);
                remove_is_output(reallyFlat);

                output_vardecls(e, item, Expression::cast<Call>(it->second()));
                vd->e(rhs);

                if (e.varOccurrences.occurrences(reallyFlat) == 0 && reallyFlat->e() == nullptr) {
                  auto it = e.varOccurrences.idx.find(reallyFlat->id());
                  assert(it.first);
                  e.flatRemoveItem((*e.flat())[*it.second]->cast<VarDeclI>());
                }
              } else {
                // If the VarDecl does not have a usable right hand side, it needs to be
                // marked as output in the FlatZinc
                assert(vd->flat());

                bool needOutputAnn = true;
                if (Id* ident = Expression::dynamicCast<Id>(reallyFlat->e())) {
                  if (e.reverseMappers.find(ident) != e.reverseMappers.end()) {
                    needOutputAnn = false;
                    remove_is_output(vd);
                    remove_is_output(reallyFlat);

                    vd->e(copy(e, e.cmap, ident));
                    Type al_t(Expression::type(vd->e()));
                    al_t.mkPar(e);
                    Expression::type(vd->e(), al_t);

                    output_vardecls(e, item, ident);

                    if (e.varOccurrences.occurrences(reallyFlat) == 0) {
                      auto it = e.varOccurrences.idx.find(reallyFlat->id());
                      assert(it.first);
                      e.flatRemoveItem((*e.flat())[*it.second]->cast<VarDeclI>());
                    }
                  }
                } else if (auto* al = Expression::dynamicCast<ArrayLit>(reallyFlat->e())) {
                  for (unsigned int i = 0; i < al->size(); i++) {
                    if (Id* ident = Expression::dynamicCast<Id>(follow_id_to_value((*al)[i]))) {
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
                      assert(it.first);
                      e.flatRemoveItem((*e.flat())[*it.second]->cast<VarDeclI>());
                    }

                    output_vardecls(e, item, al);
                    vd->e(copy(e, e.cmap, al));
                    Type al_t(Expression::type(vd->e()));
                    al_t.mkPar(e);
                    Expression::type(vd->e(), al_t);
                  }
                }
                if (needOutputAnn) {
                  if (!is_output(vd->flat())) {
                    GCLock lock;
                    const auto dims = vd->type().dim();
                    std::vector<Expression*> args(dims);
                    IntVal flatSize = 1;
                    if (dims == 0) {
                      Expression::addAnnotation(vd->flat(), e.constants.ann.output_var);
                    } else {
                      for (unsigned int i = 0; i < args.size(); i++) {
                        IntSetVal* range;
                        if (vd->ti()->ranges()[i]->domain() == nullptr) {
                          range = eval_intset(e, vd->flat()->ti()->ranges()[i]->domain());
                        } else {
                          range = eval_intset(e, vd->ti()->ranges()[i]->domain());
                        }
                        args[i] = new SetLit(Location().introduce(), range);
                        flatSize *= range->empty() ? 0 : (range->max() - range->min() + 1);
                      }
                      if (e.fopts.ignoreStdlib) {
                        // Ensure array?d call output by solver is available in output model
                        std::vector<Type> ts(dims + 1);
                        for (auto i = 0; i < dims; i++) {
                          ts[i] = Type::parsetint();
                        }
                        ts[dims] = Expression::type(reallyFlat->e());
                        std::stringstream ss;
                        ss << "array" << dims << "d";
                        ASTString ident(ss.str());
                        if (e.output->matchFn(e, ident, ts, false) == nullptr) {
                          auto* decl = copy(e, e.cmap, e.model->matchFn(e, ident, ts, true))
                                           ->cast<FunctionI>();
                          (void)e.output->registerFn(e, decl, true);
                          e.output->addItem(decl);
                        }
                        // Ensure array1d for solver output is available
                        ident = e.constants.ids.array1d;
                        ts = {Type::parsetint(), Expression::type(reallyFlat->e())};
                        if (e.output->matchFn(e, ident, ts, false) == nullptr) {
                          auto* decl = copy(e, e.cmap, e.model->matchFn(e, ident, ts, true))
                                           ->cast<FunctionI>();
                          (void)e.output->registerFn(e, decl, true);
                          e.output->addItem(decl);
                        }
                      }
                      std::vector<Expression*> alArgs(
                          {new SetLit(Location().introduce(), IntSetVal::a(1, flatSize))});
                      auto* al = new ArrayLit(Location().introduce(), alArgs);
                      Expression::addAnnotation(
                          vd->flat(),
                          Call::a(Location().introduce(), e.constants.ann.output_array, {al}));
                    }
                    check_rename_var(e, vd, args, flatSize);
                  }
                }
              }
            }
            vd->flat(nullptr);
            // Remove enum type
            Type vdt = vd->type();
            vdt.typeId(0);
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
    // Right hand side could still contain identifiers pointing to a value in
    // the FlatZinc eventhough it is reversed mapped.
    class RemapOutputRHS : public EVisitor {
      Model& _m;
      VarOccurrences& _occ;

    public:
      RemapOutputRHS(Model& m, VarOccurrences& occ) : _m(m), _occ(occ) {}
      void vId(Id* ident) {
        if (ident->decl() == nullptr) {
          return;
        }
        auto find = _occ.find(ident->decl());
        if (find >= 0) {
          auto* out = _m[find]->cast<VarDeclI>()->e();
          ident->decl(out);
        }
      }
    } visitor(*e.output, e.outputVarOccurrences);
    BottomUpIterator<RemapOutputRHS> bit(visitor);
    for (auto it = e.output->vardecls().begin(); it != e.output->vardecls().end(); ++it) {
      VarDecl* vd = it->e();
      if (vd->e() != nullptr) {
        bit.run(vd->e());
      }
    }
  }
  process_deletions(e);
}
}  // namespace MiniZinc
