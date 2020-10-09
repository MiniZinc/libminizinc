/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/astexception.hh>
#include <minizinc/model.hh>

namespace MiniZinc {

/// Exception thrown for errors during flattening
class FlatteningError : public LocationException {
public:
  FlatteningError(EnvI& env, const Location& loc, const std::string& msg);
  ~FlatteningError() throw() override {}
  const char* what() const throw() override { return "MiniZinc: flattening error"; }
};

/// Options for the flattener
struct FlatteningOptions {
  /// Keep output in resulting flat model
  bool keepOutputInFzn;
  /// Verbose output during flattening
  bool verbose;
  /// Only use paths for variables introduced by file 0 (the MiniZinc model)
  bool onlyToplevelPaths;
  /// Construct and collect mzn_paths for expressions and VarDeclI during flattening
  bool collectMznPaths;
  /// Do not apply domain changes but insert them as constraints (useful for debugging)
  bool recordDomainChanges;
  /// Only range domains for old linearization. Set from redefs to true if not here
  bool onlyRangeDomains;
  /// Allow the use of Half Reifications
  bool enableHalfReification;
  /// Timeout for flattening in milliseconds (0 means no timeout)
  unsigned long long int timeout;
  /// Create standard, DZN or JSON output
  enum OutputMode { OUTPUT_ITEM, OUTPUT_DZN, OUTPUT_JSON, OUTPUT_CHECKER } outputMode;
  /// Output objective value (only for DZN and JSON mode)
  bool outputObjective;
  /// Output original output item as string (only for DZN and JSON mode)
  bool outputOutputItem;
  /// Model is being compiled with a solution checker
  bool hasChecker;
  /// Output detailed timing information for flattening
  bool detailedTiming;
  /// Default constructor
  FlatteningOptions()
      : keepOutputInFzn(false),
        verbose(false),
        onlyToplevelPaths(false),
        collectMznPaths(false),
        recordDomainChanges(false),
        onlyRangeDomains(false),
        enableHalfReification(true),
        timeout(0),
        outputMode(OUTPUT_ITEM),
        outputObjective(false),
        outputOutputItem(false),
        detailedTiming(false) {}
};

class Pass {
public:
  Pass(){};
  virtual Env* run(Env* env, std::ostream& log) = 0;
  virtual ~Pass(){};
};

/// Flatten model in environment \a e
void flatten(Env& e, FlatteningOptions opt = FlatteningOptions());

/// Translate model in environment \a e into old FlatZinc syntax
void oldflatzinc(Env& e);

/// Populate FlatZinc output model
void populate_output(Env& e);

/// Statistics on flat models
struct FlatModelStatistics {
  /// Number of integer variables
  int n_int_vars;  // NOLINT(readability-identifier-naming)
  /// Number of bool variables
  int n_bool_vars;  // NOLINT(readability-identifier-naming)
  /// Number of float variables
  int n_float_vars;  // NOLINT(readability-identifier-naming)
  /// Number of set variables
  int n_set_vars;  // NOLINT(readability-identifier-naming)
  /// Number of bool constraints
  int n_bool_ct;  // NOLINT(readability-identifier-naming)
  /// Number of integer constraints
  int n_int_ct;  // NOLINT(readability-identifier-naming)
  /// Number of float constraints
  int n_float_ct;  // NOLINT(readability-identifier-naming)
  /// Number of set constraints
  int n_set_ct;  // NOLINT(readability-identifier-naming)
  /// Number of reified constraints evaluated
  int n_reif_ct;  // NOLINT(readability-identifier-naming)
  /// Number of half-reified constraints evaluated
  int n_imp_ct;  // NOLINT(readability-identifier-naming)
  /// Number of implications eliminated using path compression
  int n_imp_del;  // NOLINT(readability-identifier-naming)
  /// Number of linear expressions eliminated using path compression
  int n_lin_del;  // NOLINT(readability-identifier-naming)
  /// Constructor
  FlatModelStatistics()
      : n_int_vars(0),
        n_bool_vars(0),
        n_float_vars(0),
        n_set_vars(0),
        n_bool_ct(0),
        n_int_ct(0),
        n_float_ct(0),
        n_set_ct(0),
        n_reif_ct(0),
        n_imp_ct(0),
        n_imp_del(0),
        n_lin_del(0) {}
};

/// Compute statistics for flat model in \a m
FlatModelStatistics statistics(Env& m);

}  // namespace MiniZinc
