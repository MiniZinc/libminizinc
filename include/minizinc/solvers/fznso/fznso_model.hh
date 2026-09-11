/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/ast.hh>
#include <minizinc/model.hh>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <minizinc/_thirdparty/fznso.hpp>

namespace MiniZinc {

/// Presents MiniZinc's flat model to an FZnSO solver, in place.
///
/// Nothing is copied. A decision is an index into a vector of the flat model's
/// own `VarDecl*`s, a constraint an index into one of its `Call*`s, and every
/// value handed across the interface *is* the MiniZinc expression, behind a
/// method table that reads it where it lies.
///
/// The model is presented as a single permanent layer: this interface is used
/// for one-shot solving, not for incremental re-solving.
class FznsoModel final : public fznso::Model {
public:
  /// Index the flat model of \a env. Borrows it, and stays valid for as long as
  /// the flat model does. \a searchAnn are the solve item's annotations with
  /// `seq_search` already spliced into the top level, as
  /// `SolverInstanceBase::flattenSearchAnnotations` produces them.
  FznsoModel(Env& env, std::vector<Expression*> searchAnn);

  /// The decision index of \a vd, or -1 if it is not a decision variable (a
  /// parameter, an array, or a variable fixed to a literal).
  static int decisionOf(const VarDecl* vd);
  /// The variable declaration behind decision \a index.
  VarDecl* decl(std::size_t index) const { return _decisions[index]; }

  // --- Layers ---

  std::size_t layer_count() const override { return 1; }
  std::size_t layer_permanent() const override { return 1; }
  std::size_t layer_unchanged() const override { return 0; }
  std::size_t layer_redundant_count() const override { return 0; }
  std::size_t layer_redundant_index(std::size_t /*n*/) const override { return 0; }

  // --- Decision variables ---

  std::size_t decision_count() const override { return _decisions.size(); }
  std::size_t decision_layer_end(std::size_t /*layer*/) const override { return _decisions.size(); }
  FznsoType decision_type(fznso::Decision decision) const override;
  fznso::Value decision_domain(fznso::Decision decision) const override;
  std::optional<std::string_view> decision_name(fznso::Decision decision) const override;
  bool decision_defined(fznso::Decision decision) const override;
  bool decision_in_solution(fznso::Decision decision) const override;
  std::size_t decision_annotation_count(fznso::Decision decision) const override;
  fznso::AnnotationRef decision_annotation(fznso::Decision decision,
                                           std::size_t index) const override;

  // --- Constraints ---

  std::size_t constraint_count() const override { return _constraints.size(); }
  std::size_t constraint_layer_end(std::size_t /*layer*/) const override {
    return _constraints.size();
  }
  std::string_view constraint_ident(fznso::Constraint constraint) const override;
  std::size_t constraint_argument_count(fznso::Constraint constraint) const override;
  fznso::Value constraint_argument(fznso::Constraint constraint, std::size_t index) const override;
  std::optional<fznso::Decision> constraint_defines(fznso::Constraint constraint) const override;
  std::size_t constraint_annotation_count(fznso::Constraint constraint) const override;
  fznso::AnnotationRef constraint_annotation(fznso::Constraint constraint,
                                             std::size_t index) const override;

  // --- Objective ---

  std::string_view objective_ident() const override { return _objectiveIdent; }
  fznso::Value objective_arg() const override;
  std::size_t objective_annotation_count() const override { return _objectiveAnn.size(); }
  fznso::AnnotationRef objective_annotation(std::size_t index) const override;

private:
  /// The decision variables, in index order.
  std::vector<VarDecl*> _decisions;
  /// The name of each decision, empty for the ones that have none.
  ///
  /// For debugging only, which is all a name is for. What the solver has to
  /// assign is \a _inSolution.
  std::vector<std::string> _names;
  /// Whether a solution is read back for each decision: the variables annotated
  /// `output_var`, and the elements of an `output_array`. A solver may leave
  /// any of the others open if the constraints do not pin them down.
  std::vector<char> _inSolution;
  /// The constraints, in index order.
  std::vector<Call*> _constraints;
  /// The search annotations, with `seq_search` spliced into the top level.
  std::vector<Expression*> _objectiveAnn;
  /// The objective strategy identifier, empty for a satisfaction problem.
  std::string _objectiveIdent;
  /// The objective's argument, or null for a satisfaction problem.
  Expression* _objectiveArg = nullptr;
};

}  // namespace MiniZinc
