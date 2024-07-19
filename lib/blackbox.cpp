/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/astexception.hh>
#include <minizinc/blackbox.hh>
#include <minizinc/copy.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/gc.hh>

#include <vector>

namespace MiniZinc {

namespace {

/// Wrap the target of a `blackbox_exec` / `blackbox_dll` source annotation in a
/// call to the `mzn_resolve_blackbox_source` builtin. That builtin resolves
/// the (platform-independent) executable or library name to an absolute path on
/// the compilation host.
void wrap_blackbox_source(EnvI& env, Call* source, bool isExec) {
  if (source->argCount() == 0) {
    return;
  }
  GCLock lock;
  Expression* arg0 = source->arg(0);
  Expression* isLibrary = env.constants.boollit(!isExec);
  Call* resolve = Call::a(Expression::loc(arg0), env.constants.ids.blackbox.resolve_blackbox_source,
                          {arg0, isLibrary});
  source->arg(0, resolve);
}

}  // namespace

void synthesize_blackbox_bodies(EnvI& env, Model* m, const std::vector<FunctionI*>& functionItems,
                                std::vector<TypeError>& typeErrors) {
  const Constants& c = env.constants;

  for (auto* fi : functionItems) {
    // This pass runs before the annotations have been type checked, so the propagator-kind
    // annotations cannot be matched through the (type-aware) Annotation::contains. Match them by
    // name and keep the actual expression pointers so they can be removed afterwards.
    Expression* valueAnn = nullptr;
    Expression* boundsAnn = nullptr;
    for (ExpressionSetIter it = fi->ann().begin(); it != fi->ann().end(); ++it) {
      if (Id* id = Expression::dynamicCast<Id>(*it)) {
        if (id->v() == c.ann.minizinc_value_propagator->v()) {
          valueAnn = *it;
        } else if (id->v() == c.ann.minizinc_bounds_propagator->v()) {
          boundsAnn = *it;
        }
      }
    }
    const bool isValue = valueAnn != nullptr;
    const bool isBounds = boundsAnn != nullptr;
    if (!isValue && !isBounds) {
      continue;
    }
    if (isValue && isBounds) {
      typeErrors.emplace_back(
          env, fi->loc(), "a black-box propagator cannot be both a value and a bounds propagator");
      continue;
    }
    if (fi->e() != nullptr) {
      typeErrors.emplace_back(env, fi->loc(),
                              "a black-box propagator declaration must not have a body");
      continue;
    }

    // The propagation function must be defined externally: it needs a source annotation.
    Call* source = fi->ann().getCall(c.ann.blackbox_exec);
    const bool isExec = source != nullptr;
    if (source == nullptr) {
      source = fi->ann().getCall(c.ann.blackbox_dll);
    }
    if (source == nullptr) {
      typeErrors.emplace_back(
          env, fi->loc(),
          "MiniZinc-defined black-box propagators are not yet supported; the propagator must "
          "also be annotated with `::blackbox_exec' or `::blackbox_dll'");
      continue;
    }

    // Wrap the executable/library name so that flattening resolves it to an
    // absolute path, making the generated FlatZinc self-contained (see
    // wrap_blackbox_source).
    wrap_blackbox_source(env, source, isExec);

    {
      GCLock lock;
      Location loc = Location().introduce();

      // Classify the parameters. A propagator takes at most one `list of var int` and at most one
      // `list of var float`, plus any number of fixed (par) parameters.
      // - `allArgs`/`allArgTypes` forward the parameters unchanged to the `_reason` function.
      // - `parArgs`/`parArgTypes` describe a call to the `par` overload of the propagator (used to
      //   size a functional propagator's output): the variable lists are replaced by their
      //   element-wise lower bounds (`lb(...)`), the fixed parameters are passed through.
      Expression* intList = nullptr;
      Expression* floatList = nullptr;
      std::vector<Expression*> allArgs;
      std::vector<Type> allArgTypes;
      std::vector<Expression*> parArgs;
      std::vector<Type> parArgTypes;
      bool validArgs = true;
      for (unsigned int i = 0; i < fi->paramCount(); ++i) {
        VarDecl* p = fi->param(i);
        Type t = p->type();
        allArgs.push_back(p->id());
        allArgTypes.push_back(t);
        if (t.isvar()) {
          if (t.dim() == 1 && t.st() == Type::ST_PLAIN && t.bt() == Type::BT_INT) {
            if (intList != nullptr) {
              typeErrors.emplace_back(env, Expression::loc(p),
                                      "a black-box propagator takes at most one `list of var int'");
              validArgs = false;
            }
            intList = p->id();
            parArgs.push_back(Call::a(loc, c.ids.lb, {p->id()}));
            parArgTypes.push_back(Type::parint(1));
          } else if (t.dim() == 1 && t.st() == Type::ST_PLAIN && t.bt() == Type::BT_FLOAT) {
            if (floatList != nullptr) {
              typeErrors.emplace_back(
                  env, Expression::loc(p),
                  "a black-box propagator takes at most one `list of var float'");
              validArgs = false;
            }
            floatList = p->id();
            parArgs.push_back(Call::a(loc, c.ids.lb, {p->id()}));
            parArgTypes.push_back(Type::parfloat(1));
          } else {
            typeErrors.emplace_back(env, Expression::loc(p),
                                    "black-box propagator variable arguments must be `list of var "
                                    "int' or `list of var float'");
            validArgs = false;
          }
        } else {
          parArgs.push_back(p->id());
          parArgTypes.push_back(t);
        }
      }
      if (!validArgs) {
        continue;
      }

      if (intList == nullptr) {
        intList = new ArrayLit(Location().introduce(), std::vector<Expression*>{});
      }
      if (floatList == nullptr) {
        floatList = new ArrayLit(Location().introduce(), std::vector<Expression*>{});
      }
      const Type retType = fi->ti()->type();

      if (isBounds) {
        if (!(retType.dim() == 0 && retType.isvar() && retType.bt() == Type::BT_BOOL)) {
          typeErrors.emplace_back(env, fi->loc(),
                                  "a bounds propagator must be declared as a predicate");
          continue;
        }

        // Reason: use the user-defined `<name>_reason` if there is one, otherwise the conservative
        // default reason over all input bounds.
        const ASTString reasonId(std::string(fi->id().c_str()) + "_reason");
        Expression* reason;
        if (m->matchFn(env, reasonId, allArgTypes, true) != nullptr) {
          reason = Call::a(loc, reasonId, allArgs);
        } else {
          Call* lenInt = Call::a(loc, c.ids.length, {intList});
          Call* lenFloat = Call::a(loc, c.ids.length, {floatList});
          auto* n = new BinOp(loc, lenInt, BOT_PLUS, lenFloat);
          reason = Call::a(loc, c.ids.blackbox.blackbox_default_reason, {n});
        }

        Call* body = Call::a(loc, c.ids.blackbox.blackbox_bounds, {intList, floatList, reason});
        Expression::addAnnotation(body, copy(env, source));
        fi->ann().remove(boundsAnn);
        fi->ann().remove(source);
        fi->e(body);
        continue;
      }

      // Value propagator. Both the relational and functional forms produce an output so that the
      // external computation can actually constrain the model.
      Expression* intOutput;
      Expression* floatOutput;
      Expression* result;
      std::vector<Expression*> letDecls;

      if (retType.dim() == 0 && retType.isvar() && retType.bt() == Type::BT_BOOL) {
        // Relational checker: a 0/1 output fixed to `true` by the predicate context.
        auto* domain = new BinOp(loc, IntLit::a(0), BOT_DOTDOT, IntLit::a(1));
        auto* rti = new TypeInst(loc, Type::varint(), domain);
        auto* r = new VarDecl(loc, rti, env.genId());
        r->toplevel(false);
        intOutput = new ArrayLit(loc, std::vector<Expression*>{r->id()});
        floatOutput = new ArrayLit(loc, std::vector<Expression*>{});
        result = new BinOp(loc, r->id(), BOT_EQ, IntLit::a(1));
        letDecls.push_back(r);
      } else if (retType.dim() == 1 && retType.isvar() &&
                 (retType.bt() == Type::BT_INT || retType.bt() == Type::BT_FLOAT)) {
        // Functional propagator: the output index set is derived from the `par` overload of the
        // propagation function, evaluated on the variables' lower bounds.
        FunctionI* parFn = m->matchFn(env, fi->id(), parArgTypes, true);
        if (parFn == nullptr || !parFn->ti()->type().isPar()) {
          typeErrors.emplace_back(
              env, fi->loc(),
              "a functional black-box propagator `" + std::string(fi->id().c_str()) +
                  "' requires a `par` overload defining the propagation function "
                  "(used to determine the length of its output)");
          continue;
        }
        Call* parCall = Call::a(loc, fi->id(), parArgs);
        Call* idxSet = Call::a(loc, c.ids.index_set, {parCall});
        auto* rangeTI = new TypeInst(loc, Type::parint(), idxSet);
        std::vector<TypeInst*> ranges{rangeTI};
        const Type outType = (retType.bt() == Type::BT_INT) ? Type::varint(1) : Type::varfloat(1);
        auto* outTI = new TypeInst(loc, outType, ranges);
        auto* out = new VarDecl(loc, outTI, env.genId());
        out->toplevel(false);
        if (retType.bt() == Type::BT_INT) {
          intOutput = out->id();
          floatOutput = new ArrayLit(loc, std::vector<Expression*>{});
        } else {
          intOutput = new ArrayLit(loc, std::vector<Expression*>{});
          floatOutput = out->id();
        }
        result = out->id();
        letDecls.push_back(out);
      } else {
        typeErrors.emplace_back(
            env, fi->loc(),
            "a value propagator must be declared as a predicate or as a function "
            "returning `list of var int' or `list of var float'");
        continue;
      }

      Call* bbCall =
          Call::a(loc, c.ids.blackbox.blackbox, {intList, floatList, intOutput, floatOutput});
      Expression::addAnnotation(bbCall, copy(env, source));
      letDecls.push_back(bbCall);
      auto* body = new Let(loc, letDecls, result);
      fi->ann().remove(valueAnn);
      fi->ann().remove(source);
      fi->e(body);
    }
  }
}

}  // namespace MiniZinc
