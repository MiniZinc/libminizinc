/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/flatten_internal.hh>

namespace MiniZinc {
void add_path_annotation(EnvI& env, Expression* e);
bool istrue(EnvI& env, Expression* e);
bool isfalse(EnvI& env, Expression* e);
Ref<Expression> create_dummy_value(EnvI& env, const Type& t);
Ref<TypeInst> eval_typeinst(EnvI& env, const Ctx& ctx, VarDecl* vd);

Ref<Expression> bind(EnvI& env, const Ctx& ctx, VarDecl* vd, Expression* e);
Ref<Expression> conj(EnvI& env, VarDecl* b, const Ctx& ctx, const std::vector<EE>& e);

void flatten_vardecl_annotations(EnvI& env, VarDecl* origVd, VarDeclI* vdi, VarDecl* toAnnotate);
/// The definition of \a origVd as a closure to defer (see defer_bool_def), or nullptr if it cannot
/// be deferred. Adds the flat variables it uses to \a refs.
Ref<Let> deferrable_bool_def(EnvI& env, VarDecl* origVd, std::vector<VarDecl*>& refs);
/// Make \a def the right-hand side of the flat variable \a vd, so that flatten() flattens it once
/// all uses of \a vd are known. Returns false if \a vd already has a right-hand side: then the
/// caller flattens the definition now.
bool defer_bool_def(EnvI& env, VarDecl* vd, Let* def, std::vector<VarDecl*> refs);
/// Defer the `var bool` array element \a e into a new flat variable (see defer_bool_def). Returns
/// the variable, or nullptr if \a e has to be flattened now.
Id* defer_bool_expr(EnvI& env, Expression* e);
/// Record a use of the flat array \a array in context \a c as a use of each deferred element
void add_ctx_ann_elements(EnvI& env, Expression* array, BCtx c);

/// Maps the variables of \a let to flat variables while it is in scope
class LetFlatScope {
  std::vector<std::pair<VarDecl*, Ref<VarDecl>>> _saved;

public:
  LetFlatScope(Let* let);
  ~LetFlatScope();
  LetFlatScope(const LetFlatScope&) = delete;
  LetFlatScope& operator=(const LetFlatScope&) = delete;
  /// Map \a vd to the flat variable that \a e names, or to itself if \a e is a value
  static void bind(VarDecl* vd, Expression* e);
};

Ref<VarDecl> new_vardecl(EnvI& env, const Ctx& ctx, TypeInst* ti, Id* origId, VarDecl* origVd,
                         Expression* rhs, bool flattenAnnotations = true);

Ref<Expression> flat_cv_exp(EnvI& env, Ctx ctx, Expression* e);

void make_defined_var(EnvI& env, VarDecl* vd, Call* c);
void check_index_sets(EnvI& env, VarDecl* vd, Expression* e, bool isArg = false);
/// Create a domain constraint that enforces that `expr` falls within `dom`
///
/// This function might return nullptr if no constraint is required
Ref<Expression> mk_domain_constraint(EnvI& env, Expression* expr, Expression* dom);

class CallArgItem {
public:
  EnvI& env;
  CallArgItem(EnvI& env0);
  ~CallArgItem();
};

}  // namespace MiniZinc
