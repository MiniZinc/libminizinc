#pragma once
#include <minizinc/model.hh>
#include <regex>

namespace MiniZinc {

static std::regex featureVectorOptionsRegex("(\\d*v)?(\\d*c)?(f)?", std::regex_constants::ECMAScript |
                                                              std::regex_constants::icase);

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
  std::vector<long long> domain_widths; // indices in this array match keys in customIdToVarNameMap

  // mainly for debugging purposes
  // it's important that the IDs are consecutive [0, 1, ..] , so we can not use idn() here
  // example: we want the first variable defined in the file to have id 0, the second one 1 ...
  // same goes for constraints, the first constraint gets id 0
  std::map<int, std::string> customIdToVarNameMap; 
  std::map<int, std::string> customIdToConstraintNameMap;


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
        ann_histogram(),
        domain_widths()
        {}
  
  struct Options {
    int vDimensions = -1;       // for making uniform dimensions of the constraint graph. -1 will not apply padding / cropping
    int cDimensions = -1;
    bool ignoreFloats = true;  // decide whether to ignore floats in all features

    static Options parse_from_string(const std::string input) {
      Options opts;

      std::smatch match;
      std::regex_match(input, match, featureVectorOptionsRegex);
      std::string vars_limit = match[1].str();
      std::string constraints_limit = match[2].str();

      auto vpos = vars_limit.find_last_of('v');
      auto cpos = constraints_limit.find_last_of('c');

      if (vpos != std::string::npos) {
        vars_limit = vars_limit.erase(vpos, std::string::npos);
      }
      if (cpos != std::string::npos) {
        constraints_limit = constraints_limit.erase(cpos, std::string::npos);
      }

      opts.vDimensions = atoi(vars_limit.c_str());
      opts.cDimensions = atoi(constraints_limit.c_str());
      opts.ignoreFloats = match[3].matched;

      return opts;
    }

    static bool is_valid_options_regex(const std::string input) {
      return std::regex_match(input, featureVectorOptionsRegex);
    }
  };
};

/// Extract the features for flat model in \a m
FlatModelFeatureVector extract_feature_vector(Env& m, FlatModelFeatureVector::Options& o);

}

