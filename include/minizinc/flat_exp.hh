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
Expression* create_dummy_value(EnvI& env, const Type& t);
TypeInst* eval_typeinst(EnvI& env, const Ctx& ctx, VarDecl* vd);

KeepAlive bind(EnvI& env, Ctx ctx, VarDecl* vd, Expression* e);
KeepAlive conj(EnvI& env, VarDecl* b, const Ctx& ctx, const std::vector<EE>& e);

void flatten_vardecl_annotations(EnvI& env, VarDecl* origVd, VarDeclI* vdi, VarDecl* toAnnotate);
/// The definition of \a origVd, ready to be deferred (see defer_bool_def): a let that shares the
/// definition, binds the let variables and parameters it uses, and carries the annotations of
/// \a origVd. Adds the flat variables that it uses to \a refs. Returns nullptr if the definition
/// cannot be deferred.
Let* deferrable_bool_def(EnvI& env, VarDecl* origVd, std::vector<VarDecl*>& refs);
/// Defer the flattening of the definition \a def (see deferrable_bool_def) of the flat variable
/// \a vd until all uses of \a vd are known. The definition becomes the right-hand side of \a vd,
/// and the loop over the flat model in flatten() flattens it. Returns false if \a vd already has a
/// definition or a fixed value; then the definition has to be flattened now.
bool defer_bool_def(EnvI& env, VarDecl* vd, Let* def, std::vector<VarDecl*> refs);
/// Defer the flattening of the `var bool` expression \a e, an element of an array whose uses are
/// not known yet, into a new flat variable (see defer_bool_def). Returns the variable, or nullptr
/// if \a e has to be flattened now.
Id* defer_bool_expr(EnvI& env, Expression* e);
/// Add context \a c to the `var bool` elements of the flat array \a array: a use of the whole
/// array is a use of each of its elements
void add_ctx_ann_elements(EnvI& env, Expression* array, BCtx c);

/// Map the variables of \a let to flat variables while it is in scope (see bind), and restore the
/// previous mapping afterwards
class LetFlatScope {
  std::vector<std::pair<VarDecl*, KeepAlive>> _saved;

public:
  LetFlatScope(Let* let);
  ~LetFlatScope();
  LetFlatScope(const LetFlatScope&) = delete;
  LetFlatScope& operator=(const LetFlatScope&) = delete;
  /// Map the let variable \a vd to the flat variable its flat value \a e names, or to itself if
  /// \a e is a value
  static void bind(VarDecl* vd, Expression* e);
};

VarDecl* new_vardecl(EnvI& env, const Ctx& ctx, TypeInst* ti, Id* origId, VarDecl* origVd,
                     Expression* rhs, bool flattenAnnotations = true);

KeepAlive flat_cv_exp(EnvI& env, Ctx ctx, Expression* e);

void make_defined_var(EnvI& env, VarDecl* vd, Call* c);
void check_index_sets(EnvI& env, VarDecl* vd, Expression* e, bool isArg = false);
/// Create a domain constraint that enforces that `expr` falls within `dom`
///
/// This function might return nullptr if no constraint is required
Expression* mk_domain_constraint(EnvI& env, Expression* expr, Expression* dom);

class CallArgItem {
public:
  EnvI& env;
  CallArgItem(EnvI& env0);
  ~CallArgItem();
};

}  // namespace MiniZinc
