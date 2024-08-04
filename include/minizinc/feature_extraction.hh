#pragma once
#include <minizinc/model.hh>

namespace MiniZinc {
/// Feature Vector of the FlatModel
struct FlatModelFeatureVector {
  /// Number of integer variables
  int n_int_vars;  // NOLINT(readability-identifier-naming)
  /// Number of bool variables
  int n_bool_vars;  // NOLINT(readability-identifier-naming)
  /// Number of set variables
  int n_set_vars;  // NOLINT(readability-identifier-naming)
  /// Number of bool constraints
  int n_bool_ct;  // NOLINT(readability-identifier-naming)
  /// Number of integer constraints
  int n_int_ct;  // NOLINT(readability-identifier-naming)
  /// Number of set constraints
  int n_set_ct;  // NOLINT(readability-identifier-naming)

  double std_dev_domain_size;
  double avg_domain_size;
  double median_domain_size;
  double avg_domain_overlap;
  double avg_decision_vars_in_cts;
  int n_disjoint_domain_pairs;
  int n_meta_ct;
  int n_total_ct;
  std::string constraint_graph;
  std::map<std::string, int> ct_histogram;
  std::map<std::string, int> ann_histogram;

  // mainly for debugging purposes 
  std::map<int, std::string> customIdToVarNameMap; // it's important that the IDs are consecutive [0, 1, ..] , so we can not use idn() here
  std::map<int, std::string> customIdToConstraintNameMap; // same as with customIdToVarNameMap, ids can overlap


  /// Constructor
  FlatModelFeatureVector()
      : n_int_vars(0),
        n_bool_vars(0),
        n_set_vars(0),
        n_bool_ct(0),
        n_int_ct(0),
        n_set_ct(0),
        std_dev_domain_size(0),
        avg_domain_size(0),
        median_domain_size(0),
        avg_domain_overlap(0),
        avg_decision_vars_in_cts(0),
        n_disjoint_domain_pairs(0),
        n_meta_ct(0),
        n_total_ct(0),
        constraint_graph(""),
        customIdToVarNameMap(),
        customIdToConstraintNameMap(),
        ct_histogram(),
        ann_histogram()
        {}
};

/// Extract the features for flat model in \a m
FlatModelFeatureVector extract_feature_vector(Env& m);

}