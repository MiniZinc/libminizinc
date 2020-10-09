/*
 *  Main authors:
 *     Kevin Leo <kevin.leo@monash.edu>
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/exception.hh>
#include <minizinc/solvers/gecode/fzn_space.hh>
#include <minizinc/solvers/gecode/gecode_constraints.hh>
#include <minizinc/solvers/gecode_solverinstance.hh>

#include "aux_brancher.hh"

#include <utility>

using namespace std;
using namespace Gecode;

namespace MiniZinc {

GecodeSolverFactory::GecodeSolverFactory() {
  SolverConfig sc("org.minizinc.gecode_presolver", GECODE_VERSION);
  sc.name("Presolver");
  sc.mznlib("-Ggecode_presolver");
  sc.mznlibVersion(1);
  sc.supportsMzn(false);
  sc.description("Internal Gecode presolver plugin");
  sc.tags({"cp", "float", "api", "set", "gecode_presolver", "__internal__"});
  sc.stdFlags({"-a", "-n"});
  SolverConfigs::registerBuiltinSolver(sc);
}

SolverInstanceBase::Options* GecodeSolverFactory::createOptions() { return new GecodeOptions; }

SolverInstanceBase* GecodeSolverFactory::doCreateSI(Env& env, std::ostream& log,
                                                    SolverInstanceBase::Options* opt) {
  return new GecodeSolverInstance(env, log, opt);
}

string GecodeSolverFactory::getDescription(SolverInstanceBase::Options* /*opt*/) {
  string v = "Gecode solver plugin, compiled " __DATE__ ", using: Gecode version " +
             string(GECODE_VERSION);
  return v;
}

string GecodeSolverFactory::getVersion(SolverInstanceBase::Options* /*opt*/) {
  return string(GECODE_VERSION);
}

bool GecodeSolverFactory::processOption(SolverInstanceBase::Options* opt, int& i,
                                        std::vector<std::string>& argv) {
  auto& _opt = static_cast<GecodeOptions&>(*opt);
  if (string(argv[i]) == "--allow-unbounded-vars") {
    _opt.allowUnboundedVars = true;
  } else if (string(argv[i]) == "--only-range-domains") {
    _opt.onlyRangeDomains = true;
  } else if (string(argv[i]) == "--sac") {
    _opt.sac = true;
  } else if (string(argv[i]) == "--shave") {
    _opt.shave = true;
  } else if (string(argv[i]) == "--pre-passes") {
    if (++i == argv.size()) {
      return false;
    }
    int passes = atoi(argv[i].c_str());
    if (passes >= 0) {
      _opt.prePasses = passes;
    }
  } else if (string(argv[i]) == "-a" || string(argv[i]) == "--all-solutions") {
    _opt.allSolutions = true;
  } else if (string(argv[i]) == "-n") {
    if (++i == argv.size()) {
      return false;
    }
    int n = atoi(argv[i].c_str());
    if (n >= 0) {
      _opt.nSolutions = n;
    }
  } else if (string(argv[i]) == "--node") {
    if (++i == argv.size()) {
      return false;
    }
    int nodes = atoi(argv[i].c_str());
    if (nodes >= 0) {
      _opt.nodes = nodes;
    }
  } else if (string(argv[i]) == "--c_d") {
    if (++i == argv.size()) {
      return false;
    }
    int c_d = atoi(argv[i].c_str());
    if (c_d >= 0) {
      _opt.c_d = static_cast<unsigned int>(c_d);
    }
  } else if (string(argv[i]) == "--a_d") {
    if (++i == argv.size()) {
      return false;
    }
    int a_d = atoi(argv[i].c_str());
    if (a_d >= 0) {
      _opt.a_d = static_cast<unsigned int>(a_d);
    }
  } else if (string(argv[i]) == "--fail") {
    if (++i == argv.size()) {
      return false;
    }
    int fails = atoi(argv[i].c_str());
    if (fails >= 0) {
      _opt.fails = fails;
    }
  } else if (argv[i] == "--solver-time-limit" || argv[i] == "-t") {
    if (++i == argv.size()) {
      return false;
    }
    int time = atoi(argv[i].c_str());
    if (time >= 0) {
      _opt.time = time;
    }
  } else if (string(argv[i]) == "-v" || string(argv[i]) == "--verbose-solving") {
    _opt.verbose = true;
  } else if (string(argv[i]) == "-s" || string(argv[i]) == "--solver-statistics") {
    _opt.statistics = true;
  } else {
    return false;
  }
  return true;
}

void GecodeSolverFactory::printHelp(ostream& os) {
  os << "Gecode solver plugin options:" << std::endl
     << "  --allow-unbounded-vars" << std::endl
     << "    give unbounded variables maximum bounds (this may lead to incorrect behaviour)"
     << std::endl
     << "  --only-range-domains" << std::endl
     << "    only tighten bounds" << std::endl
     << "  --sac" << std ::endl
     << "    singleton arc consistency" << std::endl
     << "  --shave" << std::endl
     << "    shave domains" << std::endl
     << "  --pre-passes <n>" << std::endl
     << "    n passes of sac/shaving, 0 for fixed point" << std::endl
     << "  --c_d <n>" << std::endl
     << "    recomputation commit distance" << std::endl
     << "  --a_d <n>" << std::endl
     << "    recomputation adaption distance" << std::endl
     << "  --node <n>" << std::endl
     << "    node cutoff (0 = none, solution mode)" << std::endl
     << "  --fail <f>" << std::endl
     << "    failure cutoff (0 = none, solution mode)" << std::endl
     << "  --time <ms>" << std::endl
     << "    time (in ms) cutoff (0 = none, solution mode)" << std::endl
     << "  -a, --all-solutions" << std::endl
     << "    print intermediate solutions" << std::endl
     << "  -n <sols>" << std::endl
     << "    number of solutions" << std::endl
     << std::endl;
}

class GecodeEngine {
public:
  virtual FznSpace* next() = 0;
  virtual bool stopped() = 0;
  virtual ~GecodeEngine() {}
  virtual Gecode::Search::Statistics statistics() = 0;
};

template <template <class> class Engine, template <class, template <class> class> class Meta>
class MetaEngine : public GecodeEngine {
  Meta<FznSpace, Engine> _e;

public:
  MetaEngine(FznSpace* s, Search::Options& o) : _e(s, o) {}
  FznSpace* next() override { return _e.next(); }
  bool stopped() override { return _e.stopped(); }
  Gecode::Search::Statistics statistics() override { return _e.statistics(); }
};

GecodeSolverInstance::GecodeSolverInstance(Env& env, std::ostream& log,
                                           SolverInstanceBase::Options* opt)
    : SolverInstanceImpl<GecodeSolver>(env, log, opt),
      _nFoundSolutions(0),
      currentSpace(nullptr),
      solution(nullptr),
      engine(nullptr) {
  registerConstraints();
  _flat = env.flat();
}

GecodeSolverInstance::~GecodeSolverInstance() {
  delete engine;
  // delete currentSpace;
  // delete _solution; // TODO: is this necessary?
}

void GecodeSolverInstance::registerConstraint(const std::string& name, poster p) {
  std::stringstream ss;
  ss << "gecode_" << name;
  _constraintRegistry.add(ss.str(), p);
  std::stringstream ss2;
  ss2 << "fzn_" << name;
  _constraintRegistry.add(ss2.str(), p);
  // TODO: DO NOT USE global names directly
  _constraintRegistry.add(name, p);
}

void GecodeSolverInstance::registerConstraints() {
  GCLock lock;
  registerConstraint("all_different_int", GecodeConstraints::p_distinct);
  registerConstraint("all_different_offset", GecodeConstraints::p_distinct_offset);
  registerConstraint("all_equal_int", GecodeConstraints::p_all_equal);
  registerConstraint("int_eq", GecodeConstraints::p_int_eq);
  registerConstraint("int_ne", GecodeConstraints::p_int_ne);
  registerConstraint("int_ge", GecodeConstraints::p_int_ge);
  registerConstraint("int_gt", GecodeConstraints::p_int_gt);
  registerConstraint("int_le", GecodeConstraints::p_int_le);
  registerConstraint("int_lt", GecodeConstraints::p_int_lt);
  registerConstraint("int_eq_reif", GecodeConstraints::p_int_eq_reif);
  registerConstraint("int_ne_reif", GecodeConstraints::p_int_ne_reif);
  registerConstraint("int_ge_reif", GecodeConstraints::p_int_ge_reif);
  registerConstraint("int_gt_reif", GecodeConstraints::p_int_gt_reif);
  registerConstraint("int_le_reif", GecodeConstraints::p_int_le_reif);
  registerConstraint("int_lt_reif", GecodeConstraints::p_int_lt_reif);
  registerConstraint("int_eq_imp", GecodeConstraints::p_int_eq_imp);
  registerConstraint("int_ne_imp", GecodeConstraints::p_int_ne_imp);
  registerConstraint("int_ge_imp", GecodeConstraints::p_int_ge_imp);
  registerConstraint("int_gt_imp", GecodeConstraints::p_int_gt_imp);
  registerConstraint("int_le_imp", GecodeConstraints::p_int_le_imp);
  registerConstraint("int_lt_imp", GecodeConstraints::p_int_lt_imp);
  registerConstraint("int_lin_eq", GecodeConstraints::p_int_lin_eq);
  registerConstraint("int_lin_eq_reif", GecodeConstraints::p_int_lin_eq_reif);
  registerConstraint("int_lin_eq_imp", GecodeConstraints::p_int_lin_eq_imp);
  registerConstraint("int_lin_ne", GecodeConstraints::p_int_lin_ne);
  registerConstraint("int_lin_ne_reif", GecodeConstraints::p_int_lin_ne_reif);
  registerConstraint("int_lin_ne_imp", GecodeConstraints::p_int_lin_ne_imp);
  registerConstraint("int_lin_le", GecodeConstraints::p_int_lin_le);
  registerConstraint("int_lin_le_reif", GecodeConstraints::p_int_lin_le_reif);
  registerConstraint("int_lin_le_imp", GecodeConstraints::p_int_lin_le_imp);
  registerConstraint("int_lin_lt", GecodeConstraints::p_int_lin_lt);
  registerConstraint("int_lin_lt_reif", GecodeConstraints::p_int_lin_lt_reif);
  registerConstraint("int_lin_lt_imp", GecodeConstraints::p_int_lin_lt_imp);
  registerConstraint("int_lin_ge", GecodeConstraints::p_int_lin_ge);
  registerConstraint("int_lin_ge_reif", GecodeConstraints::p_int_lin_ge_reif);
  registerConstraint("int_lin_ge_imp", GecodeConstraints::p_int_lin_ge_imp);
  registerConstraint("int_lin_gt", GecodeConstraints::p_int_lin_gt);
  registerConstraint("int_lin_gt_reif", GecodeConstraints::p_int_lin_gt_reif);
  registerConstraint("int_lin_gt_imp", GecodeConstraints::p_int_lin_gt_imp);
  registerConstraint("int_plus", GecodeConstraints::p_int_plus);
  registerConstraint("int_minus", GecodeConstraints::p_int_minus);
  registerConstraint("int_times", GecodeConstraints::p_int_times);
  registerConstraint("int_div", GecodeConstraints::p_int_div);
  registerConstraint("int_mod", GecodeConstraints::p_int_mod);
  registerConstraint("int_min", GecodeConstraints::p_int_min);
  registerConstraint("int_max", GecodeConstraints::p_int_max);
  registerConstraint("int_abs", GecodeConstraints::p_abs);
  registerConstraint("int_negate", GecodeConstraints::p_int_negate);
  registerConstraint("bool_eq", GecodeConstraints::p_bool_eq);
  registerConstraint("bool_eq_reif", GecodeConstraints::p_bool_eq_reif);
  registerConstraint("bool_eq_imp", GecodeConstraints::p_bool_eq_imp);
  registerConstraint("bool_ne", GecodeConstraints::p_bool_ne);
  registerConstraint("bool_ne_reif", GecodeConstraints::p_bool_ne_reif);
  registerConstraint("bool_ne_imp", GecodeConstraints::p_bool_ne_imp);
  registerConstraint("bool_ge", GecodeConstraints::p_bool_ge);
  registerConstraint("bool_ge_reif", GecodeConstraints::p_bool_ge_reif);
  registerConstraint("bool_ge_imp", GecodeConstraints::p_bool_ge_imp);
  registerConstraint("bool_le", GecodeConstraints::p_bool_le);
  registerConstraint("bool_le_reif", GecodeConstraints::p_bool_le_reif);
  registerConstraint("bool_le_imp", GecodeConstraints::p_bool_le_imp);
  registerConstraint("bool_gt", GecodeConstraints::p_bool_gt);
  registerConstraint("bool_gt_reif", GecodeConstraints::p_bool_gt_reif);
  registerConstraint("bool_gt_imp", GecodeConstraints::p_bool_gt_imp);
  registerConstraint("bool_lt", GecodeConstraints::p_bool_lt);
  registerConstraint("bool_lt_reif", GecodeConstraints::p_bool_lt_reif);
  registerConstraint("bool_lt_imp", GecodeConstraints::p_bool_lt_imp);
  registerConstraint("bool_or", GecodeConstraints::p_bool_or);
  registerConstraint("bool_or_imp", GecodeConstraints::p_bool_or_imp);
  registerConstraint("bool_and", GecodeConstraints::p_bool_and);
  registerConstraint("bool_and_imp", GecodeConstraints::p_bool_and_imp);
  registerConstraint("bool_xor", GecodeConstraints::p_bool_xor);
  registerConstraint("bool_xor_imp", GecodeConstraints::p_bool_xor_imp);
  registerConstraint("array_bool_and", GecodeConstraints::p_array_bool_and);
  registerConstraint("array_bool_and_imp", GecodeConstraints::p_array_bool_and_imp);
  registerConstraint("array_bool_or", GecodeConstraints::p_array_bool_or);
  registerConstraint("array_bool_or_imp", GecodeConstraints::p_array_bool_or_imp);
  registerConstraint("array_bool_xor", GecodeConstraints::p_array_bool_xor);
  registerConstraint("array_bool_xor_imp", GecodeConstraints::p_array_bool_xor_imp);
  registerConstraint("bool_clause", GecodeConstraints::p_array_bool_clause);
  registerConstraint("bool_clause_reif", GecodeConstraints::p_array_bool_clause_reif);
  registerConstraint("bool_clause_imp", GecodeConstraints::p_array_bool_clause_imp);
  registerConstraint("bool_left_imp", GecodeConstraints::p_bool_l_imp);
  registerConstraint("bool_right_imp", GecodeConstraints::p_bool_r_imp);
  registerConstraint("bool_not", GecodeConstraints::p_bool_not);
  registerConstraint("array_int_element", GecodeConstraints::p_array_int_element);
  registerConstraint("array_var_int_element", GecodeConstraints::p_array_int_element);
  registerConstraint("array_bool_element", GecodeConstraints::p_array_bool_element);
  registerConstraint("array_var_bool_element", GecodeConstraints::p_array_bool_element);
  registerConstraint("bool2int", GecodeConstraints::p_bool2int);
  registerConstraint("int_in", GecodeConstraints::p_int_in);
  registerConstraint("int_in_reif", GecodeConstraints::p_int_in_reif);
  registerConstraint("int_in_imp", GecodeConstraints::p_int_in_imp);

  registerConstraint("array_int_lt", GecodeConstraints::p_array_int_lt);
  registerConstraint("array_int_lq", GecodeConstraints::p_array_int_lq);
  registerConstraint("array_bool_lt", GecodeConstraints::p_array_bool_lt);
  registerConstraint("array_bool_lq", GecodeConstraints::p_array_bool_lq);
  registerConstraint("count", GecodeConstraints::p_count);
  registerConstraint("count_reif", GecodeConstraints::p_count_reif);
  registerConstraint("count_imp", GecodeConstraints::p_count_imp);
  registerConstraint("at_least_int", GecodeConstraints::p_at_least);
  registerConstraint("at_most_int", GecodeConstraints::p_at_most);
  registerConstraint("bin_packing_load", GecodeConstraints::p_bin_packing_load);
  registerConstraint("global_cardinality", GecodeConstraints::p_global_cardinality);
  registerConstraint("global_cardinality_closed", GecodeConstraints::p_global_cardinality_closed);
  registerConstraint("global_cardinality_low_up", GecodeConstraints::p_global_cardinality_low_up);
  registerConstraint("global_cardinality_low_up_closed",
                     GecodeConstraints::p_global_cardinality_low_up_closed);
  registerConstraint("array_int_minimum", GecodeConstraints::p_minimum);
  registerConstraint("array_int_maximum", GecodeConstraints::p_maximum);
  registerConstraint("minimum_arg_int", GecodeConstraints::p_minimum_arg);
  registerConstraint("maximum_arg_int", GecodeConstraints::p_maximum_arg);
  registerConstraint("regular", GecodeConstraints::p_regular);
  registerConstraint("sort", GecodeConstraints::p_sort);
  registerConstraint("inverse_offsets", GecodeConstraints::p_inverse_offsets);
  registerConstraint("increasing_int", GecodeConstraints::p_increasing_int);
  registerConstraint("increasing_bool", GecodeConstraints::p_increasing_bool);
  registerConstraint("decreasing_int", GecodeConstraints::p_decreasing_int);
  registerConstraint("decreasing_bool", GecodeConstraints::p_decreasing_bool);
  registerConstraint("table_int", GecodeConstraints::p_table_int);
  registerConstraint("table_bool", GecodeConstraints::p_table_bool);
  registerConstraint("cumulatives", GecodeConstraints::p_cumulatives);
  registerConstraint("among_seq_int", GecodeConstraints::p_among_seq_int);
  registerConstraint("among_seq_bool", GecodeConstraints::p_among_seq_bool);

  registerConstraint("bool_lin_eq", GecodeConstraints::p_bool_lin_eq);
  registerConstraint("bool_lin_ne", GecodeConstraints::p_bool_lin_ne);
  registerConstraint("bool_lin_le", GecodeConstraints::p_bool_lin_le);
  registerConstraint("bool_lin_lt", GecodeConstraints::p_bool_lin_lt);
  registerConstraint("bool_lin_ge", GecodeConstraints::p_bool_lin_ge);
  registerConstraint("bool_lin_gt", GecodeConstraints::p_bool_lin_gt);

  registerConstraint("bool_lin_eq_reif", GecodeConstraints::p_bool_lin_eq_reif);
  registerConstraint("bool_lin_eq_imp", GecodeConstraints::p_bool_lin_eq_imp);
  registerConstraint("bool_lin_ne_reif", GecodeConstraints::p_bool_lin_ne_reif);
  registerConstraint("bool_lin_ne_imp", GecodeConstraints::p_bool_lin_ne_imp);
  registerConstraint("bool_lin_le_reif", GecodeConstraints::p_bool_lin_le_reif);
  registerConstraint("bool_lin_le_imp", GecodeConstraints::p_bool_lin_le_imp);
  registerConstraint("bool_lin_lt_reif", GecodeConstraints::p_bool_lin_lt_reif);
  registerConstraint("bool_lin_lt_imp", GecodeConstraints::p_bool_lin_lt_imp);
  registerConstraint("bool_lin_ge_reif", GecodeConstraints::p_bool_lin_ge_reif);
  registerConstraint("bool_lin_ge_imp", GecodeConstraints::p_bool_lin_ge_imp);
  registerConstraint("bool_lin_gt_reif", GecodeConstraints::p_bool_lin_gt_reif);
  registerConstraint("bool_lin_gt_imp", GecodeConstraints::p_bool_lin_gt_imp);

  registerConstraint("schedule_unary", GecodeConstraints::p_schedule_unary);
  registerConstraint("schedule_unary_optional", GecodeConstraints::p_schedule_unary_optional);
  registerConstraint("schedule_cumulative_optional", GecodeConstraints::p_cumulative_opt);

  registerConstraint("circuit", GecodeConstraints::p_circuit);
  registerConstraint("circuit_cost_array", GecodeConstraints::p_circuit_cost_array);
  registerConstraint("circuit_cost", GecodeConstraints::p_circuit_cost);
  registerConstraint("nooverlap", GecodeConstraints::p_nooverlap);
  registerConstraint("precede", GecodeConstraints::p_precede);
  registerConstraint("nvalue", GecodeConstraints::p_nvalue);
  registerConstraint("among", GecodeConstraints::p_among);
  registerConstraint("member_int", GecodeConstraints::p_member_int);
  registerConstraint("member_int_reif", GecodeConstraints::p_member_int_reif);
  registerConstraint("member_bool", GecodeConstraints::p_member_bool);
  registerConstraint("member_bool_reif", GecodeConstraints::p_member_bool_reif);

#ifdef GECODE_HAS_FLOAT_VARS
  registerConstraint("int2float", GecodeConstraints::p_int2float);
  registerConstraint("float_abs", GecodeConstraints::p_float_abs);
  registerConstraint("float_sqrt", GecodeConstraints::p_float_sqrt);
  registerConstraint("float_eq", GecodeConstraints::p_float_eq);
  registerConstraint("float_eq_reif", GecodeConstraints::p_float_eq_reif);
  registerConstraint("float_le", GecodeConstraints::p_float_le);
  registerConstraint("float_le_reif", GecodeConstraints::p_float_le_reif);
  registerConstraint("float_lt", GecodeConstraints::p_float_lt);
  registerConstraint("float_lt_reif", GecodeConstraints::p_float_lt_reif);
  registerConstraint("float_ne", GecodeConstraints::p_float_ne);
  registerConstraint("float_times", GecodeConstraints::p_float_times);
  registerConstraint("float_div", GecodeConstraints::p_float_div);
  registerConstraint("float_plus", GecodeConstraints::p_float_plus);
  registerConstraint("float_max", GecodeConstraints::p_float_max);
  registerConstraint("float_min", GecodeConstraints::p_float_min);
  registerConstraint("float_lin_eq", GecodeConstraints::p_float_lin_eq);
  registerConstraint("float_lin_eq_reif", GecodeConstraints::p_float_lin_eq_reif);
  registerConstraint("float_lin_le", GecodeConstraints::p_float_lin_le);
  registerConstraint("float_lin_le_reif", GecodeConstraints::p_float_lin_le_reif);
#endif
#ifdef GECODE_HAS_MPFR
  registerConstraint("float_acos", GecodeConstraints::p_float_acos);
  registerConstraint("float_asin", GecodeConstraints::p_float_asin);
  registerConstraint("float_atan", GecodeConstraints::p_float_atan);
  registerConstraint("float_cos", GecodeConstraints::p_float_cos);
  registerConstraint("float_exp", GecodeConstraints::p_float_exp);
  registerConstraint("float_ln", GecodeConstraints::p_float_ln);
  registerConstraint("float_log10", GecodeConstraints::p_float_log10);
  registerConstraint("float_log2", GecodeConstraints::p_float_log2);
  registerConstraint("float_sin", GecodeConstraints::p_float_sin);
  registerConstraint("float_tan", GecodeConstraints::p_float_tan);
#endif
#ifdef GECODE_HAS_SET_VARS
  registerConstraint("set_eq", GecodeConstraints::p_set_eq);
  registerConstraint("set_le", GecodeConstraints::p_set_le);
  registerConstraint("set_lt", GecodeConstraints::p_set_lt);
  registerConstraint("equal", GecodeConstraints::p_set_eq);
  registerConstraint("set_ne", GecodeConstraints::p_set_ne);
  registerConstraint("set_union", GecodeConstraints::p_set_union);
  registerConstraint("array_set_element", GecodeConstraints::p_array_set_element);
  registerConstraint("array_var_set_element", GecodeConstraints::p_array_set_element);
  registerConstraint("set_intersect", GecodeConstraints::p_set_intersect);
  registerConstraint("set_diff", GecodeConstraints::p_set_diff);
  registerConstraint("set_symdiff", GecodeConstraints::p_set_symdiff);
  registerConstraint("set_subset", GecodeConstraints::p_set_subset);
  registerConstraint("set_superset", GecodeConstraints::p_set_superset);
  registerConstraint("set_card", GecodeConstraints::p_set_card);
  registerConstraint("set_in", GecodeConstraints::p_set_in);
  registerConstraint("set_eq_reif", GecodeConstraints::p_set_eq_reif);
  registerConstraint("set_le_reif", GecodeConstraints::p_set_le_reif);
  registerConstraint("set_lt_reif", GecodeConstraints::p_set_lt_reif);
  registerConstraint("equal_reif", GecodeConstraints::p_set_eq_reif);
  registerConstraint("set_ne_reif", GecodeConstraints::p_set_ne_reif);
  registerConstraint("set_subset_reif", GecodeConstraints::p_set_subset_reif);
  registerConstraint("set_superset_reif", GecodeConstraints::p_set_superset_reif);
  registerConstraint("set_in_reif", GecodeConstraints::p_set_in_reif);
  registerConstraint("set_in_imp", GecodeConstraints::p_set_in_imp);
  registerConstraint("disjoint", GecodeConstraints::p_set_disjoint);
  registerConstraint("link_set_to_booleans", GecodeConstraints::p_link_set_to_booleans);
  registerConstraint("array_set_union", GecodeConstraints::p_array_set_union);
  registerConstraint("array_set_partition", GecodeConstraints::p_array_set_partition);
  registerConstraint("set_convex", GecodeConstraints::p_set_convex);
  registerConstraint("array_set_seq", GecodeConstraints::p_array_set_seq);
  registerConstraint("array_set_seq_union", GecodeConstraints::p_array_set_seq_union);
  registerConstraint("array_set_element_union", GecodeConstraints::p_array_set_element_union);
  registerConstraint("array_set_element_intersect",
                     GecodeConstraints::p_array_set_element_intersect);
  registerConstraint("array_set_element_intersect_in",
                     GecodeConstraints::p_array_set_element_intersect_in);
  registerConstraint("array_set_element_partition",
                     GecodeConstraints::p_array_set_element_partition);
  registerConstraint("int_set_channel", GecodeConstraints::p_int_set_channel);
  registerConstraint("range", GecodeConstraints::p_range);
  registerConstraint("set_weights", GecodeConstraints::p_weights);
  registerConstraint("inverse_set", GecodeConstraints::p_inverse_set);
  registerConstraint("precede_set", GecodeConstraints::p_precede_set);
#endif
}

inline void GecodeSolverInstance::insertVar(Id* id, GecodeVariable gv) {
  // std::cerr << *id << ": " << id->decl() << std::endl;
  _variableMap.insert(id->decl()->id(), gv);
}

inline bool GecodeSolverInstance::valueWithinBounds(double b) {
  long long int bo = round_to_longlong(b);
  return bo >= Gecode::Int::Limits::min && bo <= Gecode::Int::Limits::max;
}

void GecodeSolverInstance::processFlatZinc() {
  auto& _opt = static_cast<GecodeOptions&>(*_options);
  _onlyRangeDomains = _opt.onlyRangeDomains;
  _runSac = _opt.sac;
  _runShave = _opt.shave;
  _prePasses = _opt.prePasses;
  _printStats = _opt.statistics;
  _allSolutions = _opt.allSolutions;
  _nMaxSolutions = _opt.nSolutions;
  _allowUnboundedVars = _opt.allowUnboundedVars;
  currentSpace = new FznSpace();

  // iterate over VarDecls of the flat model and create variables
  for (VarDeclIterator it = _flat->vardecls().begin(); it != _flat->vardecls().end(); ++it) {
    if (!it->removed() && it->e()->type().isvar()) {
      // check if it has an output-annotation
      VarDecl* vd = it->e();
      if (!vd->ann().isEmpty()) {
        if (vd->ann().containsCall(constants().ann.output_array.aststr())) {
          auto* al = vd->e()->dynamicCast<ArrayLit>();
          if (al == nullptr) {
            std::stringstream ssm;
            ssm << "GecodeSolverInstance::processFlatZinc: Error: Array without right hand side: "
                << *vd->id() << std::endl;
            throw InternalError(ssm.str());
          }
          for (int i = 0; i < al->size(); i++) {
            if (Id* id = (*al)[i]->dynamicCast<Id>()) {
              GecodeVariable var = resolveVar(id);
              if (var.isint()) {
                currentSpace->ivIntroduced[var.index()] = false;
              } else if (var.isbool()) {
                currentSpace->bvIntroduced[var.index()] = false;
              } else if (var.isfloat()) {
                currentSpace->fvIntroduced[var.index()] = false;
              } else if (var.isset()) {
                currentSpace->svIntroduced[var.index()] = false;
              }
            }
          }
          _varsWithOutput.push_back(vd);
        } else if (vd->ann().contains(constants().ann.output_var)) {
          _varsWithOutput.push_back(vd);
        }
      }

      if (it->e()->type().dim() != 0) {
        // we ignore arrays - all their elements are defined
        continue;
      }
      MiniZinc::TypeInst* ti = it->e()->ti();
      bool isDefined;
      bool isIntroduced = false;

      if (vd->type().isint()) {
        if (it->e()->e() == nullptr) {  // if there is no initialisation expression
          Expression* domain = ti->domain();
          if (domain != nullptr) {
            IntVar intVar(*this->currentSpace, arg2intset(_env.envi(), domain));
            currentSpace->iv.push_back(intVar);
            insertVar(it->e()->id(),
                      GecodeVariable(GecodeVariable::INT_TYPE, currentSpace->iv.size() - 1));
          } else {
            if (_allowUnboundedVars) {
              IntVar intVar(*this->currentSpace, Gecode::Int::Limits::min,
                            Gecode::Int::Limits::max);
              currentSpace->iv.push_back(intVar);
              insertVar(it->e()->id(),
                        GecodeVariable(GecodeVariable::INT_TYPE, currentSpace->iv.size() - 1));
              std::cerr << "% GecodeSolverInstance::processFlatZinc: Warning: Unbounded variable "
                        << *vd->id()
                        << " given maximum integer bounds, this may be incorrect: " << std::endl;
            } else {
              std::stringstream ssm;
              ssm << "GecodeSolverInstance::processFlatZinc: Error: Unbounded variable: "
                  << *vd->id() << ", rerun with --allow-unbounded-vars to add arbitrary bounds."
                  << std::endl;
              throw InternalError(ssm.str());
            }
          }
        } else {  // there is an initialisation expression
          Expression* init = it->e()->e();
          if (init->isa<Id>() || init->isa<ArrayAccess>()) {
            // root->iv[root->intVarCount++] = root->iv[*(int*)resolveVar(init)];
            GecodeVariable var = resolveVar(init);
            assert(var.isint());
            currentSpace->iv.push_back(var.intVar(currentSpace));
            insertVar(it->e()->id(), var);
          } else {
            double il = static_cast<double>(init->cast<IntLit>()->v().toInt());
            if (valueWithinBounds(il)) {
              IntVar intVar(*this->currentSpace, static_cast<int>(il), static_cast<int>(il));
              currentSpace->iv.push_back(intVar);
              insertVar(it->e()->id(),
                        GecodeVariable(GecodeVariable::INT_TYPE, currentSpace->iv.size() - 1));
            } else {
              std::stringstream ssm;
              ssm << "GecodeSolverInstance::processFlatZinc: Error: Unsafe value for Gecode: " << il
                  << std::endl;
              throw InternalError(ssm.str());
            }
          }
        }
        isIntroduced =
            it->e()->introduced() ||
            (MiniZinc::get_annotation(it->e()->ann(), constants().ann.is_introduced) != nullptr);
        currentSpace->ivIntroduced.push_back(isIntroduced);
        isDefined = MiniZinc::get_annotation(it->e()->ann(),
                                             constants().ann.is_defined_var->str()) != nullptr;
        currentSpace->ivDefined.push_back(isDefined);

      } else if (vd->type().isbool()) {
        double lb = 0;
        double ub = 1;
        if (it->e()->e() == nullptr) {  // there is NO initialisation expression
          Expression* domain = ti->domain();
          if (domain != nullptr) {
            IntBounds ib = compute_int_bounds(_env.envi(), domain);
            lb = static_cast<double>(ib.l.toInt());
            ub = static_cast<double>(ib.u.toInt());
          } else {
            lb = 0;
            ub = 1;
          }
          BoolVar boolVar(*this->currentSpace, static_cast<int>(lb), static_cast<int>(ub));
          currentSpace->bv.push_back(boolVar);
          insertVar(it->e()->id(),
                    GecodeVariable(GecodeVariable::BOOL_TYPE, currentSpace->bv.size() - 1));
        } else {  // there is an initialisation expression
          Expression* init = it->e()->e();
          if (init->isa<Id>() || init->isa<ArrayAccess>()) {
            // root->bv[root->boolVarCount++] = root->bv[*(int*)resolveVar(init)];
            // int index = *(int*) resolveVar(init);
            GecodeVariable var = resolveVar(init);
            assert(var.isbool());
            currentSpace->bv.push_back(var.boolVar(currentSpace));
            insertVar(it->e()->id(), var);
          } else {
            auto b = (double)init->cast<BoolLit>()->v();
            BoolVar boolVar(*this->currentSpace, b, b);
            currentSpace->bv.push_back(boolVar);
            insertVar(it->e()->id(),
                      GecodeVariable(GecodeVariable::BOOL_TYPE, currentSpace->bv.size() - 1));
          }
        }
        isIntroduced =
            it->e()->introduced() ||
            (MiniZinc::get_annotation(it->e()->ann(), constants().ann.is_introduced) != nullptr);
        currentSpace->bvIntroduced.push_back(isIntroduced);
        isDefined = MiniZinc::get_annotation(it->e()->ann(),
                                             constants().ann.is_defined_var->str()) != nullptr;
        currentSpace->bvDefined.push_back(isDefined);
#ifdef GECODE_HAS_FLOAT_VARS
      } else if (vd->type().isfloat()) {
        if (it->e()->e() == nullptr) {  // there is NO initialisation expression
          Expression* domain = ti->domain();
          double lb;
          double ub;
          if (domain != nullptr) {
            FloatBounds fb = compute_float_bounds(_env.envi(), vd->id());
            lb = fb.l.toDouble();
            ub = fb.u.toDouble();
          } else {
            if (_allowUnboundedVars) {
              lb = Gecode::Float::Limits::min;
              ub = Gecode::Float::Limits::max;
              std::cerr << "%% GecodeSolverInstance::processFlatZinc: Warning: Unbounded variable "
                        << *vd->id()
                        << " given maximum float bounds, this may be incorrect: " << std::endl;
            } else {
              std::stringstream ssm;
              ssm << "GecodeSolverInstance::processFlatZinc: Error: Unbounded variable: "
                  << *vd->id() << ", rerun with --allow-unbounded-vars to add arbitrary bounds."
                  << std::endl;
              throw InternalError(ssm.str());
            }
          }
          FloatVar floatVar(*this->currentSpace, lb, ub);
          currentSpace->fv.push_back(floatVar);
          insertVar(it->e()->id(),
                    GecodeVariable(GecodeVariable::FLOAT_TYPE, currentSpace->fv.size() - 1));
        } else {
          Expression* init = it->e()->e();
          if (init->isa<Id>() || init->isa<ArrayAccess>()) {
            // root->fv[root->floatVarCount++] = root->fv[*(int*)resolveVar(init)];
            GecodeVariable var = resolveVar(init);
            assert(var.isfloat());
            currentSpace->fv.push_back(var.floatVar(currentSpace));
            insertVar(it->e()->id(), var);
          } else {
            double il = init->cast<FloatLit>()->v().toDouble();
            FloatVar floatVar(*this->currentSpace, il, il);
            currentSpace->fv.push_back(floatVar);
            insertVar(it->e()->id(),
                      GecodeVariable(GecodeVariable::FLOAT_TYPE, currentSpace->fv.size() - 1));
          }
        }
        isIntroduced =
            it->e()->introduced() ||
            (MiniZinc::get_annotation(it->e()->ann(), constants().ann.is_introduced) != nullptr);
        currentSpace->fvIntroduced.push_back(isIntroduced);
        isDefined = MiniZinc::get_annotation(it->e()->ann(),
                                             constants().ann.is_defined_var->str()) != nullptr;
        currentSpace->fvDefined.push_back(isDefined);
#endif
#ifdef GECODE_HAS_SET_VARS
      } else if (vd->type().isIntSet()) {
        Expression* domain = ti->domain();
        auto d = arg2intset(_env.envi(), domain);
        SetVar setVar(*this->currentSpace, Gecode::IntSet::empty, d);
        currentSpace->sv.push_back(setVar);
        isIntroduced =
            it->e()->introduced() ||
            (MiniZinc::get_annotation(it->e()->ann(), constants().ann.is_introduced) != nullptr);
        currentSpace->svIntroduced.push_back(isIntroduced);
        isDefined = MiniZinc::get_annotation(it->e()->ann(),
                                             constants().ann.is_defined_var->str()) != nullptr;
        currentSpace->svDefined.push_back(isDefined);
        insertVar(it->e()->id(),
                  GecodeVariable(GecodeVariable::SET_TYPE, currentSpace->sv.size() - 1));
#endif
      } else {
        std::stringstream ssm;
        ssm << "Type " << *ti << " is currently not supported by Gecode." << std::endl;
        throw InternalError(ssm.str());
      }
    }  // end if it is a variable
  }    // end for all var decls

  // post the constraints
  for (ConstraintIterator it = _flat->constraints().begin(); it != _flat->constraints().end();
       ++it) {
    if (!it->removed()) {
      if (Call* c = it->e()->dynamicCast<Call>()) {
        _constraintRegistry.post(c);
      }
    }
  }

  // objective
  SolveI* si = _flat->solveItem();
  currentSpace->solveType = si->st();
  if (si->e() != nullptr) {
    currentSpace->optVarIsInt = (si->e()->type().isvarint());
    if (Id* id = si->e()->dynamicCast<Id>()) {
      if (si->e()->type().isvar()) {
        GecodeVariable var = resolveVar(id->decl());
        if (currentSpace->optVarIsInt) {
          IntVar intVar = var.intVar(currentSpace);
          for (int i = 0; i < currentSpace->iv.size(); i++) {
            if (currentSpace->iv[i].varimp() == intVar.varimp()) {
              currentSpace->optVarIdx = i;
              break;
            }
          }
          assert(currentSpace->optVarIdx >= 0);
#ifdef GECODE_HAS_FLOAT_VARS
        } else {
          FloatVar floatVar = var.floatVar(currentSpace);
          for (int i = 0; i < currentSpace->fv.size(); i++) {
            if (currentSpace->fv[i].varimp() == floatVar.varimp()) {
              currentSpace->optVarIdx = i;
              break;
            }
          }
          assert(currentSpace->optVarIdx >= 0);
#endif
        }
      }
    } else {  // the solve expression has to be a variable/id
      assert(false);
    }
  }

  // std::cout << "DEBUG: at end of processFlatZinc: " << std::endl
  //          << "iv has " << currentSpace->iv.size() << " variables " << std::endl
  //          << "bv has " << currentSpace->bv.size() << " variables " << std::endl
  //          << "fv has " << currentSpace->fv.size() << " variables " << std::endl
  //          << "sv has " << currentSpace->sv.size() << " variables " << std::endl;
}

Gecode::IntArgs GecodeSolverInstance::arg2intargs(Expression* arg, int offset) {
  if (!arg->isa<Id>() && !arg->isa<ArrayLit>()) {
    std::stringstream ssm;
    ssm << "Invalid argument in arg2intargs: " << *arg;
    ssm << ". Expected Id or ArrayLit.";
    throw InternalError(ssm.str());
  }
  ArrayLit* a =
      arg->isa<Id>() ? arg->cast<Id>()->decl()->e()->cast<ArrayLit>() : arg->cast<ArrayLit>();
  IntArgs ia(static_cast<int>(a->size()) + offset);
  for (int i = offset; (i--) != 0;) {
    ia[i] = 0;
  }
  for (int i = static_cast<int>(a->size()); (i--) != 0;) {
    ia[i + offset] = (*a)[i]->cast<IntLit>()->v().toInt();
  }
  return ia;
}

Gecode::IntArgs GecodeSolverInstance::arg2boolargs(Expression* arg, int offset) {
  if (!arg->isa<Id>() && !arg->isa<ArrayLit>()) {
    std::stringstream ssm;
    ssm << "Invalid argument in arg2boolargs: " << *arg;
    ssm << ". Expected Id or ArrayLit.";
    throw InternalError(ssm.str());
  }
  ArrayLit* a =
      arg->isa<Id>() ? arg->cast<Id>()->decl()->e()->cast<ArrayLit>() : arg->cast<ArrayLit>();
  IntArgs ia(static_cast<int>(a->size()) + offset);
  for (int i = offset; (i--) != 0;) {
    ia[i] = 0;
  }
  for (int i = static_cast<int>(a->size()); (i--) != 0;) {
    ia[i + offset] = static_cast<int>((*a)[i]->cast<BoolLit>()->v());
  }
  return ia;
}

class GecodeRangeIter {
public:
  GecodeSolverInstance& si;
  IntSetRanges& isr;
  GecodeRangeIter(GecodeSolverInstance& gsi, IntSetRanges& isr0) : si(gsi), isr(isr0) {}
  int min() const {
    long long int val = isr.min().toInt();
    if (si.valueWithinBounds(static_cast<double>(val))) {
      return (int)val;
    }
    std::stringstream ssm;
    ssm << "GecodeRangeIter::min: Error: " << val << " outside 32-bit int." << std::endl;
    throw InternalError(ssm.str());
  }
  int max() const {
    long long int val = isr.max().toInt();
    if (si.valueWithinBounds(static_cast<double>(val))) {
      return (int)val;
    }
    std::stringstream ssm;
    ssm << "GecodeRangeIter::max: Error: " << val << " outside 32-bit int." << std::endl;
    throw InternalError(ssm.str());
  }
  int width() const { return static_cast<int>(isr.width().toInt()); }
  bool operator()() const { return isr(); }
  void operator++() { ++isr; }
};

Gecode::IntSet GecodeSolverInstance::arg2intset(EnvI& envi, Expression* arg) {
  GCLock lock;
  IntSetVal* isv = eval_intset(envi, arg);
  IntSetRanges isr(isv);
  GecodeRangeIter isr_g(*this, isr);
  IntSet d(isr_g);
  return d;
}
IntSetArgs GecodeSolverInstance::arg2intsetargs(EnvI& envi, Expression* arg, int offset) {
  ArrayLit* a = arg2arraylit(arg);
  if (a->size() == 0) {
    IntSetArgs emptyIa(0);
    return emptyIa;
  }
  IntSetArgs ia(static_cast<int>(a->size()) + offset);
  for (int i = offset; (i--) != 0;) {
    ia[i] = IntSet::empty;
  }
  for (int i = static_cast<int>(a->size()); (i--) != 0;) {
    ia[i + offset] = arg2intset(envi, (*a)[i]);
  }
  return ia;
}

Gecode::IntVarArgs GecodeSolverInstance::arg2intvarargs(Expression* arg, int offset) {
  ArrayLit* a = arg2arraylit(arg);
  if (a->size() == 0) {
    IntVarArgs emptyIa(0);
    return emptyIa;
  }
  IntVarArgs ia(static_cast<int>(a->size()) + offset);
  for (int i = offset; (i--) != 0;) {
    ia[i] = IntVar(*this->currentSpace, 0, 0);
  }
  for (int i = static_cast<int>(a->size()); (i--) != 0;) {
    Expression* e = (*a)[i];
    if (e->type().isvar()) {
      // ia[i+offset] = currentSpace->iv[*(int*)resolveVar(getVarDecl(e))];
      GecodeSolver::Variable var = resolveVar(getVarDecl(e));
      assert(var.isint());
      Gecode::IntVar v = var.intVar(currentSpace);
      ia[i + offset] = v;
    } else {
      long long int value = e->cast<IntLit>()->v().toInt();
      if (valueWithinBounds(static_cast<double>(value))) {
        IntVar iv(*this->currentSpace, static_cast<int>(value), static_cast<int>(value));
        ia[i + offset] = iv;
      } else {
        std::stringstream ssm;
        ssm << "GecodeSolverInstance::arg2intvarargs Error: " << value << " outside 32-bit int."
            << std::endl;
        throw InternalError(ssm.str());
      }
    }
  }
  return ia;
}

Gecode::BoolVarArgs GecodeSolverInstance::arg2boolvarargs(Expression* arg, int offset, int siv) {
  ArrayLit* a = arg2arraylit(arg);
  if (a->length() == 0) {
    BoolVarArgs emptyIa(0);
    return emptyIa;
  }
  BoolVarArgs ia(static_cast<int>(a->length()) + offset - (siv == -1 ? 0 : 1));
  for (int i = offset; (i--) != 0;) {
    ia[i] = BoolVar(*this->currentSpace, 0, 0);
  }
  for (int i = 0; i < static_cast<int>(a->length()); i++) {
    if (i == siv) {
      continue;
    }
    Expression* e = (*a)[i];
    if (e->type().isvar()) {
      GecodeVariable var = resolveVar(getVarDecl(e));
      if (e->type().isvarbool()) {
        assert(var.isbool());
        ia[offset++] = var.boolVar(currentSpace);
      } else if (e->type().isvarint() && var.hasBoolAlias()) {
        ia[offset++] = currentSpace->bv[var.boolAliasIndex()];
      } else {
        std::stringstream ssm;
        ssm << "expected bool-var or alias int var instead of " << *e << " with type "
            << e->type().toString(env().envi());
        throw InternalError(ssm.str());
      }
    } else {
      if (auto* bl = e->dynamicCast<BoolLit>()) {
        bool value = bl->v();
        BoolVar iv(*this->currentSpace, static_cast<int>(value), static_cast<int>(value));
        ia[offset++] = iv;
      } else {
        std::stringstream ssm;
        ssm << "Expected bool literal instead of: " << *e;
        throw InternalError(ssm.str());
      }
    }
  }
  return ia;
}

Gecode::BoolVar GecodeSolverInstance::arg2boolvar(Expression* e) {
  BoolVar x0;
  if (e->type().isvar()) {
    // x0 = currentSpace->bv[*(int*)resolveVar(getVarDecl(e))];
    GecodeVariable var = resolveVar(getVarDecl(e));
    assert(var.isbool());
    x0 = var.boolVar(currentSpace);
  } else {
    if (auto* bl = e->dynamicCast<BoolLit>()) {
      x0 = BoolVar(*this->currentSpace, static_cast<int>(bl->v()), static_cast<int>(bl->v()));
    } else {
      std::stringstream ssm;
      ssm << "Expected bool literal instead of: " << *e;
      throw InternalError(ssm.str());
    }
  }
  return x0;
}

Gecode::IntVar GecodeSolverInstance::arg2intvar(Expression* e) {
  IntVar x0;
  if (e->type().isvar()) {
    // x0 = currentSpace->iv[*(int*)resolveVar(getVarDecl(e))];
    GecodeVariable var = resolveVar(getVarDecl(e));
    assert(var.isint());
    x0 = var.intVar(currentSpace);
  } else {
    IntVal i;
    if (auto* il = e->dynamicCast<IntLit>()) {
      i = il->v().toInt();
    } else if (auto* bl = e->dynamicCast<BoolLit>()) {
      i = static_cast<long long>(bl->v());
    } else {
      std::stringstream ssm;
      ssm << "Expected bool or int literal instead of: " << *e;
      throw InternalError(ssm.str());
    }
    int ii = static_cast<int>(i.toInt());
    x0 = IntVar(*this->currentSpace, ii, ii);
  }
  return x0;
}

ArrayLit* GecodeSolverInstance::arg2arraylit(Expression* arg) {
  ArrayLit* a;
  if (Id* id = arg->dynamicCast<Id>()) {
    VarDecl* vd = id->decl();
    if (vd->e() != nullptr) {
      a = vd->e()->cast<ArrayLit>();
    } else {
      std::vector<Expression*>* array = arrayMap[vd];
      std::vector<Expression*> ids;
      for (auto& i : *array) {
        ids.push_back(i->cast<VarDecl>()->id());
      }
      a = new ArrayLit(vd->loc(), ids);
    }
  } else if (auto* al = arg->dynamicCast<ArrayLit>()) {
    a = al;
  } else {
    std::stringstream ssm;
    ssm << "Invalid argument in arg2arrayLit: " << *arg;
    ssm << ". Expected Id or ArrayLit.";
    throw InternalError(ssm.str());
  }
  return a;
}

bool GecodeSolverInstance::isBoolArray(ArrayLit* a, int& singleInt) {
  singleInt = -1;
  if (a->length() == 0) {
    return true;
  }
  for (int i = static_cast<int>(a->length()); (i--) != 0;) {
    if ((*a)[i]->type().isbool()) {
      continue;
    }
    if (((*a)[i])->type().isvarint()) {
      GecodeVariable var = resolveVar(getVarDecl((*a)[i]));
      if (var.hasBoolAlias()) {
        if (singleInt != -1) {
          return false;
        }
        singleInt = var.boolAliasIndex();
      } else {
        return false;
      }
    } else {
      return false;
    }
  }
  return singleInt == -1 || a->length() > 1;
}
#ifdef GECODE_HAS_SET_VARS
SetVar GecodeSolverInstance::arg2setvar(Expression* e) {
  SetVar x0;
  if (!e->type().isvar()) {
    Gecode::IntSet d = arg2intset(_env.envi(), e);
    x0 = SetVar(*this->currentSpace, d, d);
  } else {
    GecodeVariable var = resolveVar(getVarDecl(e));
    assert(var.isset());
    x0 = var.setVar(currentSpace);
  }
  return x0;
}
Gecode::SetVarArgs GecodeSolverInstance::arg2setvarargs(Expression* arg, int offset, int doffset,
                                                        const Gecode::IntSet& od) {
  ArrayLit* a = arg2arraylit(arg);
  SetVarArgs ia(static_cast<int>(a->size()) + offset);
  for (int i = offset; (i--) != 0;) {
    Gecode::IntSet d = i < doffset ? od : Gecode::IntSet::empty;
    ia[i] = SetVar(*this->currentSpace, d, d);
  }
  for (int i = static_cast<int>(a->size()); (i--) != 0;) {
    ia[i + offset] = arg2setvar((*a)[i]);
  }
  return ia;
}
#endif
#ifdef GECODE_HAS_FLOAT_VARS
Gecode::FloatValArgs GecodeSolverInstance::arg2floatargs(Expression* arg, int offset) {
  assert(arg->isa<Id>() || arg->isa<ArrayLit>());
  ArrayLit* a =
      arg->isa<Id>() ? arg->cast<Id>()->decl()->e()->cast<ArrayLit>() : arg->cast<ArrayLit>();
  FloatValArgs fa(static_cast<int>(a->size()) + offset);
  for (int i = offset; (i--) != 0;) {
    fa[i] = 0.0;
  }
  for (int i = static_cast<int>(a->size()); (i--) != 0;) {
    fa[i + offset] = (*a)[i]->cast<FloatLit>()->v().toDouble();
  }
  return fa;
}

Gecode::FloatVar GecodeSolverInstance::arg2floatvar(Expression* e) {
  FloatVar x0;
  if (e->type().isvar()) {
    GecodeVariable var = resolveVar(getVarDecl(e));
    assert(var.isfloat());
    x0 = var.floatVar(currentSpace);
  } else {
    FloatVal i;
    if (auto* il = e->dynamicCast<IntLit>()) {
      i = static_cast<double>(il->v().toInt());
    } else if (auto* bl = e->dynamicCast<BoolLit>()) {
      i = static_cast<double>(bl->v());
    } else if (auto* fl = e->dynamicCast<FloatLit>()) {
      i = fl->v();
    } else {
      std::stringstream ssm;
      ssm << "Expected bool, int or float literal instead of: " << *e;
      throw InternalError(ssm.str());
    }
    x0 = FloatVar(*this->currentSpace, i.toDouble(), i.toDouble());
  }
  return x0;
}

Gecode::FloatVarArgs GecodeSolverInstance::arg2floatvarargs(Expression* arg, int offset) {
  ArrayLit* a = arg2arraylit(arg);
  if (a->size() == 0) {
    FloatVarArgs emptyFa(0);
    return emptyFa;
  }
  FloatVarArgs fa(static_cast<int>(a->size()) + offset);
  for (int i = offset; (i--) != 0;) {
    fa[i] = FloatVar(*this->currentSpace, 0.0, 0.0);
  }
  for (int i = static_cast<int>(a->size()); (i--) != 0;) {
    Expression* e = (*a)[i];
    if (e->type().isvar()) {
      GecodeVariable var = resolveVar(getVarDecl(e));
      assert(var.isfloat());
      fa[i + offset] = var.floatVar(currentSpace);
    } else {
      if (auto* fl = e->dynamicCast<FloatLit>()) {
        double value = fl->v().toDouble();
        FloatVar fv(*this->currentSpace, value, value);
        fa[i + offset] = fv;
      } else {
        std::stringstream ssm;
        ssm << "Expected float literal instead of: " << *e;
        throw InternalError(ssm.str());
      }
    }
  }
  return fa;
}
#endif

MZ_IntConLevel GecodeSolverInstance::ann2icl(const Annotation& ann) {
  if (!ann.isEmpty()) {
    if (get_annotation(ann, "val") != nullptr) {
      return MZ_ICL_VAL;
    }
    if (get_annotation(ann, "domain") != nullptr) {
      return MZ_ICL_DOM;
    }
    if ((get_annotation(ann, "bounds") != nullptr) || (get_annotation(ann, "boundsR") != nullptr) ||
        (get_annotation(ann, "boundsD") != nullptr) ||
        (get_annotation(ann, "boundsZ") != nullptr)) {
      return MZ_ICL_BND;
    }
  }
  return MZ_ICL_DEF;
}

VarDecl* GecodeSolverInstance::getVarDecl(Expression* expr) {
  VarDecl* vd = nullptr;
  if ((vd = expr->dynamicCast<VarDecl>()) != nullptr) {
    vd = expr->cast<VarDecl>();
  } else if (Id* id = expr->dynamicCast<Id>()) {
    vd = id->decl();
  } else if (auto* aa = expr->dynamicCast<ArrayAccess>()) {
    vd = resolveArrayAccess(aa);
  } else {
    std::stringstream ssm;
    ssm << "Can not extract vardecl from " << *expr;
    throw InternalError(ssm.str());
  }
  return vd;
}

VarDecl* GecodeSolverInstance::resolveArrayAccess(ArrayAccess* aa) {
  VarDecl* vd = aa->v()->cast<Id>()->decl();
  long long int idx = aa->idx()[0]->cast<IntLit>()->v().toInt();
  return resolveArrayAccess(vd, idx);
}

VarDecl* GecodeSolverInstance::resolveArrayAccess(VarDecl* vd, long long int index) {
  auto it = arrayMap.find(vd);
  if (it != arrayMap.end()) {
    std::vector<Expression*>* exprs = it->second;
    Expression* expr = (*exprs)[index - 1];
    return expr->cast<VarDecl>();
  }
  std::stringstream ssm;
  ssm << "Unknown array: " << vd->id();
  throw InternalError(ssm.str());
}

GecodeSolver::Variable GecodeSolverInstance::resolveVar(Expression* e) {
  if (Id* id = e->dynamicCast<Id>()) {
    return _variableMap.get(id->decl()->id());  // lookupVar(id->decl());
  }
  if (auto* vd = e->dynamicCast<VarDecl>()) {
    return _variableMap.get(vd->id()->decl()->id());
  }
  if (auto* aa = e->dynamicCast<ArrayAccess>()) {
    return _variableMap.get(resolveArrayAccess(aa)->id()->decl()->id());
  }
  std::stringstream ssm;
  ssm << "Expected Id, VarDecl or ArrayAccess instead of \"" << *e << "\"";
  throw InternalError(ssm.str());
}

SolverInstance::Status GecodeSolverInstance::next() {
  GCLock lock;
  prepareEngine();

  solution = engine->next();

  if (solution != nullptr) {
    assignSolutionToOutput();
    return SolverInstance::SAT;
  }
  if (engine->stopped()) {
    return SolverInstance::UNKNOWN;
  }
  return SolverInstance::UNSAT;
}

void GecodeSolverInstance::resetSolver() {
  assert(false);  // TODO: implement
}

Expression* GecodeSolverInstance::getSolutionValue(Id* id) {
  id = id->decl()->id();
  if (id->type().isvar()) {
    GecodeVariable var = resolveVar(id->decl()->id());
#ifdef GECODE_HAS_SET_VARS
    if (id->type().isSet()) {
      SetVar& sv = var.setVar(solution);
      assert(sv.assigned());
      SetVarGlbRanges svr(sv);
      assert(svr());

      IntVal mi = svr.min();
      IntVal ma = svr.max();
      ++svr;
      vector<IntVal> vals;
      if (svr()) {
        SetVarGlbValues svv(sv);
        IntVal i = svv.val();
        vals.push_back(i);
        ++svv;
        for (; svv(); ++svv) {
          vals.emplace_back(svv.val());
        }
        return new SetLit(Location().introduce(), IntSetVal::a(vals));
      }
      return new SetLit(Location().introduce(), IntSetVal::a(mi, ma));
    }
#endif
    switch (id->type().bt()) {
      case Type::BT_INT:
        assert(var.intVar(solution).assigned());
        return IntLit::a(var.intVar(solution).val());
      case Type::BT_BOOL:
        assert(var.boolVar(solution).assigned());
        return constants().boollit(var.boolVar(solution).val() != 0);
#ifdef GECODE_HAS_FLOAT_VARS
      case Type::BT_FLOAT:
        assert(var.floatVar(solution).assigned());
        return FloatLit::a(var.floatVar(solution).val().med());
#endif
      default:
        return nullptr;
    }
  } else {
    return id->decl()->e();
  }
}

void GecodeSolverInstance::prepareEngine() {
  GCLock lock;
  auto& _opt = static_cast<GecodeOptions&>(*_options);
  if (engine == nullptr) {
    // TODO: check what we need to do options-wise
    std::vector<Expression*> branch_vars;
    std::vector<Expression*> solve_args;
    Expression* solveExpr = _flat->solveItem()->e();
    Expression* optSearch = nullptr;

    switch (currentSpace->solveType) {
      case MiniZinc::SolveI::SolveType::ST_MIN:
        assert(solveExpr != nullptr);
        branch_vars.push_back(solveExpr);
        solve_args.push_back(new ArrayLit(Location(), branch_vars));
        if (!currentSpace->optVarIsInt) {  // TODO: why??
          solve_args.push_back(FloatLit::a(0.0));
        }
        solve_args.push_back(new Id(Location(), "input_order", nullptr));
        solve_args.push_back(new Id(
            Location(), currentSpace->optVarIsInt ? "indomain_min" : "indomain_split", nullptr));
        solve_args.push_back(new Id(Location(), "complete", nullptr));
        optSearch = new Call(Location(), currentSpace->optVarIsInt ? "int_search" : "float_search",
                             solve_args);
        break;
      case MiniZinc::SolveI::SolveType::ST_MAX:
        branch_vars.push_back(solveExpr);
        solve_args.push_back(new ArrayLit(Location(), branch_vars));
        if (!currentSpace->optVarIsInt) {
          solve_args.push_back(FloatLit::a(0.0));
        }
        solve_args.push_back(new Id(Location(), "input_order", nullptr));
        solve_args.push_back(
            new Id(Location(),
                   currentSpace->optVarIsInt ? "indomain_max" : "indomain_split_reverse", nullptr));
        solve_args.push_back(new Id(Location(), "complete", nullptr));
        optSearch = new Call(Location(), currentSpace->optVarIsInt ? "int_search" : "float_search",
                             solve_args);
        break;
      case MiniZinc::SolveI::SolveType::ST_SAT:
        break;
      default:
        assert(false);
    }

    engineOptions.c_d = _opt.c_d;
    engineOptions.a_d = _opt.a_d;

    int seed = _opt.seed;
    double decay = _opt.decay;

    createBranchers(_flat->solveItem()->ann(), optSearch, seed, decay, false, /* ignoreUnknown */
                    std::cerr);

    int nodeStop = _opt.nodes;
    int failStop = _opt.fails;
    int timeStop = _opt.time;

    engineOptions.stop = Driver::CombinedStop::create(nodeStop, failStop, timeStop, false);

    // TODO: add presolving part
    if (currentSpace->solveType == MiniZinc::SolveI::SolveType::ST_SAT) {
      engine = new MetaEngine<DFS, Driver::EngineToMeta>(this->currentSpace, engineOptions);
    } else {
      engine = new MetaEngine<BAB, Driver::EngineToMeta>(this->currentSpace, engineOptions);
    }
  }
}

void GecodeSolverInstance::printStatistics() {
  EnvI& env = _env.envi();
  Gecode::Search::Statistics stat = engine->statistics();
  env.outstream << "%%%mzn-stat: variables="
                << (currentSpace->iv.size() + currentSpace->bv.size() + currentSpace->sv.size())
                << std::endl
                << "%%%mzn-stat: propagators=" << Gecode::PropagatorGroup::all.size(*currentSpace)
                << endl
                << "%%%mzn-stat: propagations=" << stat.propagate << std::endl
                << "%%%mzn-stat: nodes=" << stat.node << std::endl
                << "%%%mzn-stat: failures=" << stat.fail << std::endl
                << "%%%mzn-stat: restarts=" << stat.restart << std::endl
                << "%%%mzn-stat: peak_depth=" << stat.depth << std::endl
                << "%%%mzn-stat-end" << std::endl;
}

void GecodeSolverInstance::processSolution(bool last_sol) {
  if (solution != nullptr) {
    assignSolutionToOutput();
    printSolution();
    if (currentSpace->solveType == MiniZinc::SolveI::SolveType::ST_SAT) {
      if (engine->stopped() || !last_sol) {
        _status = SolverInstance::SAT;
      } else {
        _status = SolverInstance::OPT;
      }
    } else {
      if (engine->stopped()) {
        Gecode::Search::Statistics stat = engine->statistics();
        auto* cs = static_cast<Driver::CombinedStop*>(engineOptions.stop);
        std::cerr << "% GecodeSolverInstance: ";
        int r = cs->reason(stat, engineOptions);
        if ((r & Driver::CombinedStop::SR_INT) != 0) {
          std::cerr << "user interrupt " << std::endl;
        } else {
          if ((r & Driver::CombinedStop::SR_NODE) != 0) {
            _statusReason = SolverInstance::SR_LIMIT;
            std::cerr << "node ";
          }
          if ((r & Driver::CombinedStop::SR_FAIL) != 0) {
            _statusReason = SolverInstance::SR_LIMIT;
            std::cerr << "failure ";
          }
          if ((r & Driver::CombinedStop::SR_TIME) != 0) {
            _statusReason = SolverInstance::SR_LIMIT;
            std::cerr << "time ";
          }
          std::cerr << "limit reached" << std::endl << std::endl;
        }
        if (_nFoundSolutions > 0) {
          _status = SolverInstance::SAT;
        } else {
          _status = SolverInstance::UNKNOWN;
        }
      } else {
        _status = last_sol ? SolverInstance::OPT : SolverInstance::SAT;
      }
    }
  } else {
    if (engine->stopped()) {
      _status = SolverInstance::UNKNOWN;
    } else {
      _status = SolverInstance::UNSAT;
    }
  }
}

SolverInstanceBase::Status GecodeSolverInstance::solve() {
  GCLock lock;
  SolverInstanceBase::Status ret;

  prepareEngine();

  if (_runSac || _runShave) {
    presolve();
  }

  int n_max_solutions = _nMaxSolutions;
  if (n_max_solutions == -1) {
    if (_allSolutions) {
      n_max_solutions = 0;
    } else if (currentSpace->solveType == MiniZinc::SolveI::SolveType::ST_SAT) {
      n_max_solutions = 1;
    }
  }

  FznSpace* next_sol = engine->next();
  while (next_sol != nullptr) {
    delete solution;
    solution = next_sol;
    _nFoundSolutions++;

    if (n_max_solutions == 0 || _nFoundSolutions <= n_max_solutions) {
      processSolution();
      if (_printStats) {
        printStatistics();
      }
    }
    if (_nFoundSolutions == n_max_solutions) {
      break;
    }
    next_sol = engine->next();
  }
  if (currentSpace->solveType != MiniZinc::SolveI::SolveType::ST_SAT) {
    if (n_max_solutions == -1) {
      // Print last solution
      processSolution(next_sol == nullptr);
      if (_printStats) {
        printStatistics();
      }
    }
  }
  if (next_sol == nullptr) {
    if (solution != nullptr) {
      ret = engine->stopped() ? SolverInstance::SAT : SolverInstance::OPT;
    } else {
      ret = engine->stopped() ? SolverInstance::UNKNOWN : SolverInstance::UNSAT;
    }
  } else {
    ret = SolverInstance::SAT;
  }
  _pS2Out->stats.nFails = engine->statistics().fail;
  _pS2Out->stats.nNodes = engine->statistics().node;
  delete engine;
  engine = nullptr;
  return ret;
}

class IntVarComp {
public:
  std::vector<Gecode::IntVar> iv;
  IntVarComp(std::vector<Gecode::IntVar> b) { iv = std::move(b); }
  int operator()(int a, int b) { return static_cast<int>(iv[a].size() < iv[b].size()); }
};

class IntVarRangesBwd : public Int::IntVarImpBwd {
public:
  IntVarRangesBwd() {}
  IntVarRangesBwd(const IntVar& x) : Int::IntVarImpBwd(x.varimp()) {}
  static void init(const IntVar& x) { Int::IntVarImpBwd(x.varimp()); }
};

bool GecodeSolverInstance::sac(bool toFixedPoint = false, bool shaving = false) const {
  if (currentSpace->status() == SS_FAILED) {
    return false;
  }
  bool modified;
  std::vector<int> sorted_iv;

  for (unsigned int i = 0; i < currentSpace->iv.size(); i++) {
    if (!currentSpace->iv[i].assigned()) {
      sorted_iv.push_back(i);
    }
  }
  IntVarComp ivc(currentSpace->iv);
  sort(sorted_iv.begin(), sorted_iv.end(), ivc);

  do {
    modified = false;
    for (unsigned int idx = 0; idx < currentSpace->bv.size(); idx++) {
      BoolVar bvar = currentSpace->bv[idx];
      if (!bvar.assigned()) {
        for (int val = bvar.min(); val <= bvar.max(); ++val) {
          auto* f = static_cast<FznSpace*>(currentSpace->clone());
          rel(*f, f->bv[idx], IRT_EQ, val);
          if (f->status() == SS_FAILED) {
            rel(*currentSpace, bvar, IRT_NQ, val);
            modified = true;
            if (currentSpace->status() == SS_FAILED) {
              return false;
            }
          }
          delete f;
        }
      }
    }

    for (unsigned int idx : sorted_iv) {
      IntVar ivar = currentSpace->iv[idx];
      bool tight = false;
      int nnq = 0;
      int fwd_min;
      IntArgs nq(static_cast<int>(ivar.size()));
      for (IntVarValues vv(ivar); vv() && !tight; ++vv) {
        auto* f = static_cast<FznSpace*>(currentSpace->clone());
        rel(*f, f->iv[idx], IRT_EQ, vv.val());
        if (f->status() == SS_FAILED) {
          nq[nnq++] = vv.val();
        } else {
          fwd_min = vv.val();
          tight = shaving;
        }
        delete f;
      }
      if (shaving) {
        tight = false;
        for (IntVarRangesBwd vr(ivar); vr() && !tight; ++vr) {
          for (int i = vr.max(); i >= vr.min() && i >= fwd_min; i--) {
            auto* f = static_cast<FznSpace*>(currentSpace->clone());
            rel(*f, f->iv[idx], IRT_EQ, i);
            if (f->status() == SS_FAILED) {
              nq[nnq++] = i;
            } else {
              tight = true;
            }
            delete f;
          }
        }
      }
      if (nnq != 0U) {
        modified = true;
      }
      while ((nnq--) != 0U) {
        rel(*currentSpace, ivar, IRT_NQ, nq[nnq]);
      }
      if (currentSpace->status() == SS_FAILED) {
        return false;
      }
    }
  } while (toFixedPoint && modified);
  return true;
}

bool GecodeSolverInstance::presolve(Model* originalModel) {
  GCLock lock;
  if (currentSpace->status() == SS_FAILED) {
    return false;
  }
  // run SAC?
  if (_runSac || _runShave) {
    unsigned int iters = _prePasses;
    if (iters != 0U) {
      for (unsigned int i = 0; i < iters; i++) {
        sac(false, _runShave);
      }
    } else {
      sac(true, _runShave);
    }
  }

  if (originalModel != nullptr) {
    ASTStringMap<VarDecl*> vds;
    for (VarDeclIterator it = originalModel->vardecls().begin();
         it != originalModel->vardecls().end(); ++it) {
      VarDecl* vd = it->e();
      vds[vd->id()->str()] = vd;
    }

    IdMap<GecodeVariable>::iterator it;
    for (it = _variableMap.begin(); it != _variableMap.end(); it++) {
      VarDecl* vd = it->first->decl();
      long long int old_domsize = 0;
      bool holes = false;

      if (vd->ti()->domain() != nullptr) {
        if (vd->type().isint()) {
          IntBounds old_bounds = compute_int_bounds(_env.envi(), vd->id());
          long long int old_rangesize = abs(old_bounds.u.toInt() - old_bounds.l.toInt());
          if (vd->ti()->domain()->isa<SetLit>()) {
            old_domsize = arg2intset(_env.envi(), vd->ti()->domain()).size();
          } else {
            old_domsize = old_rangesize + 1;
          }
          holes = old_domsize < old_rangesize + 1;
        }
      }

      ASTString name = it->first->str();

      if (vds.find(name) != vds.end()) {
        VarDecl* nvd = vds[name];
        Type::BaseType bt = vd->type().bt();
        if (bt == Type::BaseType::BT_INT && vd->type().st() == Type::ST_PLAIN) {
          IntVar intvar = it->second.intVar(currentSpace);
          const long long int l = intvar.min();
          const long long int u = intvar.max();

          if (l == u) {
            if (nvd->e() != nullptr) {
              nvd->ti()->domain(new SetLit(nvd->loc(), IntSetVal::a(l, u)));
            } else {
              nvd->type(Type::parint());
              nvd->ti(new TypeInst(nvd->loc(), Type::parint()));
              nvd->e(IntLit::a(l));
            }
          } else if (!(l == Gecode::Int::Limits::min || u == Gecode::Int::Limits::max)) {
            if (_onlyRangeDomains && !holes) {
              nvd->ti()->domain(new SetLit(nvd->loc(), IntSetVal::a(l, u)));
            } else {
              IntVarRanges ivr(intvar);
              nvd->ti()->domain(new SetLit(nvd->loc(), IntSetVal::ai(ivr)));
            }
          }
        } else if (bt == Type::BaseType::BT_BOOL) {
          BoolVar boolvar = it->second.boolVar(currentSpace);
          int l = boolvar.min();
          int u = boolvar.max();
          if (l == u) {
            if (nvd->e() != nullptr) {
              nvd->ti()->domain(constants().boollit(l != 0));
            } else {
              nvd->type(Type::parbool());
              nvd->ti(new TypeInst(nvd->loc(), Type::parbool()));
              nvd->e(new BoolLit(nvd->loc(), l != 0));
            }
          }
#ifdef GECODE_HAS_FLOAT_VAR
        } else if (bt == Type::BaseType::BT_FLOAT) {
          Gecode::FloatVar floatvar = it->second.floatVar(currentSpace);
          if (floatvar.assigned() && !nvd->e()) {
            FloatNum l = floatvar.min();
            nvd->type(Type::parfloat());
            nvd->ti(new TypeInst(nvd->loc(), Type::parfloat()));
            nvd->e(FloatLit::a(l));
          } else {
            FloatNum l = floatvar.min(), u = floatvar.max();
            nvd->ti()->domain(new SetLit(nvd->loc(), FloatSetVal::a(l, u)));
          }
#endif
        }
      }
    }
  }
  return true;
}

void GecodeSolverInstance::setSearchStrategyFromAnnotation(
    std::vector<Expression*> flatAnn, std::vector<bool>& iv_searched,
    std::vector<bool>& bv_searched,
#ifdef GECODE_HAS_SET_VARS
    std::vector<bool>& sv_searched,
#endif
#ifdef GECODE_HAS_FLOAT_VARS
    std::vector<bool>& fv_searched,
#endif
    TieBreak<IntVarBranch>& def_int_varsel, IntValBranch& def_int_valsel,
    TieBreak<BoolVarBranch>& def_bool_varsel, BoolValBranch& def_bool_valsel,
#ifdef GECODE_HAS_SET_VARS
    SetVarBranch& def_set_varsel, SetValBranch& def_set_valsel,
#endif
#ifdef GECODE_HAS_FLOAT_VARS
    TieBreak<FloatVarBranch>& def_float_varsel, FloatValBranch& def_float_valsel,
#endif
    Rnd& rnd, double decay, bool ignoreUnknown, std::ostream& err) {
  for (auto& i : flatAnn) {
    if (i->isa<Call>() && i->cast<Call>()->id() == "gecode_search") {
      // Call* c = flatAnn[i]->cast<Call>();
      // branchWithPlugin(c->args);
      std::cerr << "WARNING: Not supporting search annotation \"gecode_search\" yet." << std::endl;
      return;
    }
    if (i->isa<Call>() && i->cast<Call>()->id() == "int_search") {
      Call* call = i->cast<Call>();
      ArrayLit* vars = arg2arraylit(call->arg(0));
      if (vars->size() == 0) {  // empty array
        std::cerr << "WARNING: trying to branch on empty array in search annotation: " << *call
                  << std::endl;
        continue;
      }
      int k = static_cast<int>(vars->size());
      for (int i = static_cast<int>(vars->size()); (i--) != 0;) {
        if (!(*vars)[i]->type().isvarint()) {
          k--;
        }
      }
      IntVarArgs va(k);
      std::vector<ASTString> names;
      k = 0;
      for (unsigned int i = 0; i < vars->size(); i++) {
        if (!(*vars)[i]->type().isvarint()) {
          continue;
        }
        unsigned int idx = resolveVar(getVarDecl((*vars)[i])).index();
        va[k++] = currentSpace->iv[idx];
        iv_searched[idx] = true;
        names.push_back(getVarDecl((*vars)[i])->id()->str());
      }
      std::string r0;
      std::string r1;
      // BrancherHandle bh =
      branch(*currentSpace, va, ann2ivarsel(call->arg(1)->cast<Id>()->str(), rnd, decay),
             ann2ivalsel(call->arg(2)->cast<Id>()->str(), r0, r1, rnd), nullptr
             //,&varValPrint<IntVar>
      );
      // branchInfo.add(bh,r0,r1,names);
    }  // end int_search
    else if (i->isa<Call>() && i->cast<Call>()->id() == "int_assign") {
      Call* call = i->dynamicCast<Call>();
      ArrayLit* vars = arg2arraylit(call->arg(0));
      int k = static_cast<int>(vars->size());
      for (int i = static_cast<int>(vars->size()); (i--) != 0;) {
        if (!((*vars)[i])->type().isvarint()) {
          k--;
        }
      }
      IntVarArgs va(k);
      k = 0;
      for (unsigned int i = 0; i < vars->size(); i++) {
        if (!((*vars)[i])->type().isvarint()) {
          continue;
        }
        unsigned int idx = resolveVar(getVarDecl((*vars)[i])).index();
        va[k++] = currentSpace->iv[idx];
        iv_searched[idx] = true;
      }
      assign(*currentSpace, va, ann2asnivalsel(call->arg(1)->cast<Id>()->str(), rnd), nullptr
             //&varValPrint<IntVar>
      );
    } else if (i->isa<Call>() && i->cast<Call>()->id() == "bool_search") {
      Call* call = i->dynamicCast<Call>();
      ArrayLit* vars = arg2arraylit(call->arg(0));
      int k = static_cast<int>(vars->size());
      for (int i = static_cast<int>(vars->size()); (i--) != 0;) {
        if (!((*vars)[i])->type().isvarbool()) {
          k--;
        }
      }
      BoolVarArgs va(k);
      k = 0;
      std::vector<ASTString> names;
      for (unsigned int i = 0; i < vars->size(); i++) {
        if (!((*vars)[i])->type().isvarbool()) {
          continue;
        }
        unsigned int idx = resolveVar(getVarDecl((*vars)[i])).index();
        va[k++] = currentSpace->bv[idx];
        bv_searched[idx] = true;
        names.push_back(getVarDecl((*vars)[i])->id()->str());
      }

      std::string r0;
      std::string r1;
      // BrancherHandle bh =
      branch(*currentSpace, va, ann2bvarsel(call->arg(1)->cast<Id>()->str(), rnd, decay),
             ann2bvalsel(call->arg(2)->cast<Id>()->str(), r0, r1, rnd), nullptr  //,
             //&varValPrint<BoolVar>
      );
      // branchInfo.add(bh,r0,r1,names);
    } else if (i->isa<Call>() && i->cast<Call>()->id() == "int_default_search") {
      Call* call = i->dynamicCast<Call>();
      def_int_varsel = ann2ivarsel(call->arg(0)->cast<Id>()->str(), rnd, decay);
      std::string r0;
      def_int_valsel = ann2ivalsel(call->arg(1)->cast<Id>()->str(), r0, r0, rnd);
    } else if (i->isa<Call>() && i->cast<Call>()->id() == "bool_default_search") {
      Call* call = i->dynamicCast<Call>();
      std::string r0;
      def_bool_varsel = ann2bvarsel(call->arg(0)->cast<Id>()->str(), rnd, decay);
      def_bool_valsel = ann2bvalsel(call->arg(1)->cast<Id>()->str(), r0, r0, rnd);
    } else if (i->isa<Call>() && i->cast<Call>()->id() == "set_search") {
#ifdef GECODE_HAS_SET_VARS
      Call* call = i->dynamicCast<Call>();
      ArrayLit* vars = arg2arraylit(call->arg(0));
      int k = static_cast<int>(vars->size());
      for (int i = static_cast<int>(vars->size()); (i--) != 0;) {
        if (!((*vars)[i])->type().isSet() || !((*vars)[i])->type().isvar()) {
          k--;
        }
      }
      SetVarArgs va(k);
      k = 0;
      std::vector<ASTString> names;
      for (unsigned int i = 0; i < vars->size(); i++) {
        if (!((*vars)[i])->type().isSet() || !((*vars)[i])->type().isvar()) {
          continue;
        }
        unsigned int idx = resolveVar(getVarDecl((*vars)[i])).index();
        va[k++] = currentSpace->sv[idx];
        sv_searched[idx] = true;
        names.push_back(getVarDecl((*vars)[i])->id()->str());
      }
      std::string r0;
      std::string r1;
      // BrancherHandle bh =
      branch(*currentSpace, va, ann2svarsel(call->arg(1)->cast<Id>()->str(), rnd, decay),
             ann2svalsel(call->arg(2)->cast<Id>()->str(), r0, r1, rnd),
             nullptr  //,
                      //&varValPrint<SetVar>
      );
      // branchInfo.add(bh,r0,r1,names);
#else
      if (!ignoreUnknown) {
        err << "Warning, ignored search annotation: ";
        flatAnn[i]->print(err);
        err << std::endl;
      }
#endif
    } else if (i->isa<Call>() && i->cast<Call>()->id() == "set_default_search") {
#ifdef GECODE_HAS_SET_VARS
      Call* call = i->dynamicCast<Call>();
      def_set_varsel = ann2svarsel(call->arg(0)->cast<Id>()->str(), rnd, decay);
      std::string r0;
      def_set_valsel = ann2svalsel(call->arg(1)->cast<Id>()->str(), r0, r0, rnd);
#else
      if (!ignoreUnknown) {
        err << "Warning, ignored search annotation: ";
        flatAnn[i]->print(err);
        err << std::endl;
      }
#endif
    } else if (i->isa<Call>() && i->cast<Call>()->id() == "float_default_search") {
#ifdef GECODE_HAS_FLOAT_VARS
      Call* call = i->dynamicCast<Call>();
      def_float_varsel = ann2fvarsel(call->arg(0)->cast<Id>()->str(), rnd, decay);
      std::string r0;
      def_float_valsel = ann2fvalsel(call->arg(1)->cast<Id>()->str(), r0, r0);
#else
      if (!ignoreUnknown) {
        err << "Warning, ignored search annotation: float_default_search" << std::endl;
      }
#endif
    } else if (i->isa<Call>() && i->cast<Call>()->id() == "float_search") {
#ifdef GECODE_HAS_FLOAT_VARS
      Call* call = i->dynamicCast<Call>();
      auto* vars = call->arg(0)->cast<ArrayLit>();
      int k = static_cast<int>(vars->size());
      for (int i = static_cast<int>(vars->size()); (i--) != 0;) {
        if (!((*vars)[i])->type().isvarfloat()) {
          k--;
        }
      }
      FloatVarArgs va(k);
      k = 0;
      std::vector<ASTString> names;
      for (unsigned int i = 0; i < vars->size(); i++) {
        if (!((*vars)[i])->type().isvarfloat()) {
          continue;
        }
        unsigned int idx = resolveVar(getVarDecl((*vars)[i])).index();
        va[k++] = currentSpace->fv[idx];
        fv_searched[idx] = true;
        names.push_back(getVarDecl((*vars)[i])->id()->str());
      }
      std::string r0;
      std::string r1;
      // BrancherHandle bh =
      branch(*currentSpace, va, ann2fvarsel(call->arg(2)->cast<Id>()->str(), rnd, decay),
             ann2fvalsel(call->arg(3)->cast<Id>()->str(), r0, r1),
             nullptr  //,
                      //&varValPrintF
      );
      // branchInfo.add(bh,r0,r1,names);
#else
      if (!ignoreUnknown) {
        err << "Warning, ignored search annotation: float_search" << std::endl;
      }
#endif
    } else {
      if (!ignoreUnknown) {
        err << "Warning, ignored search annotation: " << *i << std::endl;
      }
    }
  }  // end for all annotations
}

void GecodeSolverInstance::createBranchers(Annotation& ann, Expression* additionalAnn, int seed,
                                           double decay, bool ignoreUnknown, std::ostream& err) {
  // default search heuristics
  Rnd rnd(static_cast<unsigned int>(seed));
  TieBreak<IntVarBranch> def_int_varsel = INT_VAR_AFC_SIZE_MAX(0.99);
  IntValBranch def_int_valsel = INT_VAL_MIN();
  TieBreak<BoolVarBranch> def_bool_varsel = BOOL_VAR_AFC_MAX(0.99);
  BoolValBranch def_bool_valsel = BOOL_VAL_MIN();
#ifdef GECODE_HAS_SET_VARS
  SetVarBranch def_set_varsel = SET_VAR_AFC_SIZE_MAX(0.99);
  SetValBranch def_set_valsel = SET_VAL_MIN_INC();
#endif
#ifdef GECODE_HAS_FLOAT_VARS
  TieBreak<FloatVarBranch> def_float_varsel = FLOAT_VAR_SIZE_MIN();
  FloatValBranch def_float_valsel = FLOAT_VAL_SPLIT_MIN();
#endif

  std::vector<bool> iv_searched(currentSpace->iv.size());
  for (unsigned int i = currentSpace->iv.size(); (i--) != 0U;) {
    iv_searched[i] = false;
  }
  std::vector<bool> bv_searched(currentSpace->bv.size());
  for (unsigned int i = currentSpace->bv.size(); (i--) != 0U;) {
    bv_searched[i] = false;
  }
#ifdef GECODE_HAS_SET_VARS
  std::vector<bool> sv_searched(currentSpace->sv.size());
  for (unsigned int i = currentSpace->sv.size(); (i--) != 0U;) {
    sv_searched[i] = false;
  }
#endif
#ifdef GECODE_HAS_FLOAT_VARS
  std::vector<bool> fv_searched(currentSpace->fv.size());
  for (unsigned int i = currentSpace->fv.size(); (i--) != 0U;) {
    fv_searched[i] = false;
  }
#endif

  // solving annotations
  std::vector<Expression*> flatAnn;
  if (!ann.isEmpty()) {
    flattenSearchAnnotations(ann, flatAnn);
  }
  if (additionalAnn != nullptr) {
    flatAnn.push_back(additionalAnn);
  }
  if (!flatAnn.empty()) {
    setSearchStrategyFromAnnotation(flatAnn, iv_searched, bv_searched,
#ifdef GECODE_HAS_SET_VARS
                                    sv_searched,
#endif
#ifdef GECODE_HAS_FLOAT_VARS
                                    fv_searched,
#endif
                                    def_int_varsel, def_int_valsel, def_bool_varsel,
                                    def_bool_valsel,
#ifdef GECODE_HAS_SET_VARS
                                    def_set_varsel, def_set_valsel,
#endif
#ifdef GECODE_HAS_FLOAT_VARS
                                    def_float_varsel, def_float_valsel,
#endif
                                    rnd, decay, ignoreUnknown, err);
  }

  int introduced = 0;
  int funcdep = 0;
  int searched = 0;
  for (int i = currentSpace->iv.size(); (i--) != 0;) {
    if (iv_searched[i]) {
      searched++;
    } else if (currentSpace->ivIntroduced[i]) {
      if (currentSpace->ivDefined[i]) {
        funcdep++;
      } else {
        introduced++;
      }
    }
  }
  IntVarArgs iv_sol(static_cast<int>(currentSpace->iv.size()) - (introduced + funcdep + searched));
  IntVarArgs iv_tmp(introduced);
  for (int i = currentSpace->iv.size(), j = 0, k = 0; (i--) != 0U;) {
    if (iv_searched[i]) {
      continue;
    }
    if (currentSpace->ivIntroduced[i]) {
      if (currentSpace->ivIntroduced.size() >= i) {
        if (!currentSpace->ivDefined[i]) {
          iv_tmp[j++] = currentSpace->iv[i];
        }
      }
    } else {
      iv_sol[k++] = currentSpace->iv[i];
    }
  }
  // Collecting Boolean variables
  introduced = 0;
  funcdep = 0;
  searched = 0;
  for (int i = currentSpace->bv.size(); (i--) != 0;) {
    if (bv_searched[i]) {
      searched++;
    } else if (currentSpace->bvIntroduced[i]) {
      if (currentSpace->bvDefined[i]) {
        funcdep++;
      } else {
        introduced++;
      }
    }
  }
  BoolVarArgs bv_sol(static_cast<int>(currentSpace->bv.size()) - (introduced + funcdep + searched));
  BoolVarArgs bv_tmp(introduced);
  for (int i = currentSpace->bv.size(), j = 0, k = 0; (i--) != 0;) {
    if (bv_searched[i]) {
      continue;
    }
    if (currentSpace->bvIntroduced[i]) {
      if (!currentSpace->bvDefined[i]) {
        bv_tmp[j++] = currentSpace->bv[i];
      }
    } else {
      bv_sol[k++] = currentSpace->bv[i];
    }
  }

  if (iv_sol.size() > 0) {
    branch(*this->currentSpace, iv_sol, def_int_varsel, def_int_valsel);
  }
  if (bv_sol.size() > 0) {
    branch(*this->currentSpace, bv_sol, def_bool_varsel, def_bool_valsel);
  }

  // std::cout << "DEBUG: branched over " << iv_sol.size()  << " integer variables."<< std::endl;
  // std::cout << "DEBUG: branched over " << bv_sol.size()  << " Boolean variables."<< std::endl;
#ifdef GECODE_HAS_FLOAT_VARS
  introduced = 0;
  funcdep = 0;
  searched = 0;
  for (int i = currentSpace->fv.size(); (i--) != 0;) {
    if (fv_searched[i]) {
      searched++;
    } else if (currentSpace->fvIntroduced[i]) {
      if (currentSpace->fvDefined[i]) {
        funcdep++;
      } else {
        introduced++;
      }
    }
  }
  FloatVarArgs fv_sol(static_cast<int>(currentSpace->fv.size()) -
                      (introduced + funcdep + searched));
  FloatVarArgs fv_tmp(introduced);
  for (int i = currentSpace->fv.size(), j = 0, k = 0; (i--) != 0;) {
    if (fv_searched[i]) {
      continue;
    }
    if (currentSpace->fvIntroduced[i]) {
      if (!currentSpace->fvDefined[i]) {
        fv_tmp[j++] = currentSpace->fv[i];
      }
    } else {
      fv_sol[k++] = currentSpace->fv[i];
    }
  }

  if (fv_sol.size() > 0) {
    branch(*this->currentSpace, fv_sol, def_float_varsel, def_float_valsel);
  }
#endif
#ifdef GECODE_HAS_SET_VARS
  introduced = 0;
  funcdep = 0;
  searched = 0;
  for (int i = currentSpace->sv.size(); (i--) != 0;) {
    if (sv_searched[i]) {
      searched++;
    } else if (currentSpace->svIntroduced[i]) {
      if (currentSpace->svDefined[i]) {
        funcdep++;
      } else {
        introduced++;
      }
    }
  }
  SetVarArgs sv_sol(static_cast<int>(currentSpace->sv.size()) - (introduced + funcdep + searched));
  SetVarArgs sv_tmp(introduced);
  for (int i = currentSpace->sv.size(), j = 0, k = 0; (i--) != 0;) {
    if (sv_searched[i]) {
      continue;
    }
    if (currentSpace->svIntroduced[i]) {
      if (!currentSpace->svDefined[i]) {
        sv_tmp[j++] = currentSpace->sv[i];
      }
    } else {
      sv_sol[k++] = currentSpace->sv[i];
    }
  }

  if (sv_sol.size() > 0) {
    branch(*this->currentSpace, sv_sol, def_set_varsel, def_set_valsel);
  }
#endif

  // branching on auxiliary variables
  currentSpace->ivAux = IntVarArray(*this->currentSpace, iv_tmp);
  currentSpace->bvAux = BoolVarArray(*this->currentSpace, bv_tmp);
  int n_aux = currentSpace->ivAux.size() + currentSpace->bvAux.size();
#ifdef GECODE_HAS_SET_VARS
  currentSpace->svAux = SetVarArray(*this->currentSpace, sv_tmp);
  n_aux += currentSpace->svAux.size();
#endif
#ifdef GECODE_HAS_FLOAT_VARS
  currentSpace->fvAux = FloatVarArray(*this->currentSpace, fv_tmp);
  n_aux += currentSpace->fvAux.size();
#endif
  if (n_aux > 0) {
    AuxVarBrancher::post(*this->currentSpace, def_int_varsel, def_int_valsel, def_bool_varsel,
                         def_bool_valsel
#ifdef GECODE_HAS_SET_VARS
                         ,
                         def_set_varsel, def_set_valsel
#endif
#ifdef GECODE_HAS_FLOAT_VARS
                         ,
                         def_float_varsel, def_float_valsel
#endif
    );  // end post
    // std::cout << "DEBUG: Posted aux-var-brancher for " << n_aux << " aux-variables" << std::endl;
  }  // end if n_aux > 0
  // else
  // std::cout << "DEBUG: No aux vars to branch on." << std::endl;
}

TieBreak<IntVarBranch> GecodeSolverInstance::ann2ivarsel(const ASTString s, Rnd& rnd,
                                                         double decay) {
  if (s == "input_order") {
    return TieBreak<IntVarBranch>(INT_VAR_NONE());
  }
  if (s == "first_fail") {
    return TieBreak<IntVarBranch>(INT_VAR_SIZE_MIN());
  }
  if (s == "anti_first_fail") {
    return TieBreak<IntVarBranch>(INT_VAR_SIZE_MAX());
  }
  if (s == "smallest") {
    return TieBreak<IntVarBranch>(INT_VAR_MIN_MIN());
  }
  if (s == "largest") {
    return TieBreak<IntVarBranch>(INT_VAR_MAX_MAX());
  }
  if (s == "occurrence") {
    return TieBreak<IntVarBranch>(INT_VAR_DEGREE_MAX());
  }
  if (s == "max_regret") {
    return TieBreak<IntVarBranch>(INT_VAR_REGRET_MIN_MAX());
  }
  if (s == "most_constrained") {
    return TieBreak<IntVarBranch>(INT_VAR_SIZE_MIN(), INT_VAR_DEGREE_MAX());
  }
  if (s == "random") {
    return TieBreak<IntVarBranch>(INT_VAR_RND(rnd));
  }
  if (s == "afc_min") {
    return TieBreak<IntVarBranch>(INT_VAR_AFC_MIN(decay));
  }
  if (s == "afc_max") {
    return TieBreak<IntVarBranch>(INT_VAR_AFC_MAX(decay));
  }
  if (s == "afc_size_min") {
    return TieBreak<IntVarBranch>(INT_VAR_AFC_SIZE_MIN(decay));
  }
  if (s == "afc_size_max" || s == "dom_w_deg") {
    return TieBreak<IntVarBranch>(INT_VAR_AFC_SIZE_MAX(decay));
  }
  if (s == "action_min") {
    return TieBreak<IntVarBranch>(INT_VAR_ACTION_MIN(decay));
  }
  if (s == "action_max") {
    return TieBreak<IntVarBranch>(INT_VAR_ACTION_MAX(decay));
  }
  if (s == "action_size_min") {
    return TieBreak<IntVarBranch>(INT_VAR_ACTION_SIZE_MIN(decay));
  }
  if (s == "action_size_max") {
    return TieBreak<IntVarBranch>(INT_VAR_ACTION_SIZE_MAX(decay));
  }
  std::cerr << "Warning, ignored search annotation: " << s << std::endl;
  return TieBreak<IntVarBranch>(INT_VAR_NONE());
}

Gecode::IntValBranch GecodeSolverInstance::ann2ivalsel(const ASTString s, std::string& r0,
                                                       std::string& r1, Rnd& rnd) {
  if (s == "indomain_min") {
    r0 = "=";
    r1 = "!=";
    return INT_VAL_MIN();
  }
  if (s == "indomain_max") {
    r0 = "=";
    r1 = "!=";
    return INT_VAL_MAX();
  }
  if (s == "indomain_median") {
    r0 = "=";
    r1 = "!=";
    return INT_VAL_MED();
  }
  if (s == "indomain_split") {
    r0 = "<=";
    r1 = ">";
    return INT_VAL_SPLIT_MIN();
  }
  if (s == "indomain_reverse_split") {
    r0 = ">";
    r1 = "<=";
    return INT_VAL_SPLIT_MAX();
  }
  if (s == "indomain_random") {
    r0 = "=";
    r1 = "!=";
    return INT_VAL_RND(rnd);
  }
  if (s == "indomain") {
    r0 = "=";
    r1 = "=";
    return INT_VALUES_MIN();
  }
  if (s == "indomain_middle") {
    std::cerr << "Warning, replacing unsupported annotation "
              << "indomain_middle with indomain_median" << std::endl;
    r0 = "=";
    r1 = "!=";
    return INT_VAL_MED();
  }
  if (s == "indomain_interval") {
    std::cerr << "Warning, replacing unsupported annotation "
              << "indomain_interval with indomain_split" << std::endl;
    r0 = "<=";
    r1 = ">";
    return INT_VAL_SPLIT_MIN();
  }
  std::cerr << "Warning, ignored search annotation: " << s << std::endl;
  r0 = "=";
  r1 = "!=";
  return INT_VAL_MIN();
}

TieBreak<BoolVarBranch> GecodeSolverInstance::ann2bvarsel(const ASTString s, Rnd& rnd,
                                                          double decay) {
  if ((s == "input_order") || (s == "first_fail") || (s == "anti_first_fail") ||
      (s == "smallest") || (s == "largest") || (s == "max_regret")) {
    return TieBreak<BoolVarBranch>(BOOL_VAR_NONE());
  }
  if ((s == "occurrence") || (s == "most_constrained")) {
    return TieBreak<BoolVarBranch>(BOOL_VAR_DEGREE_MAX());
  }
  if (s == "random") {
    return TieBreak<BoolVarBranch>(BOOL_VAR_RND(rnd));
  }
  if ((s == "afc_min") || (s == "afc_size_min")) {
    return TieBreak<BoolVarBranch>(BOOL_VAR_AFC_MIN(decay));
  }
  if ((s == "afc_max") || (s == "afc_size_max") || (s == "dom_w_deg")) {
    return TieBreak<BoolVarBranch>(BOOL_VAR_AFC_MAX(decay));
  }
  if ((s == "action_min") && (s == "action_size_min")) {
    return TieBreak<BoolVarBranch>(BOOL_VAR_ACTION_MIN(decay));
  }
  if ((s == "action_max") || (s == "action_size_max")) {
    return TieBreak<BoolVarBranch>(BOOL_VAR_ACTION_MAX(decay));
  }
  std::cerr << "Warning, ignored search annotation: " << s << std::endl;
  return TieBreak<BoolVarBranch>(BOOL_VAR_NONE());
}

BoolValBranch GecodeSolverInstance::ann2bvalsel(const ASTString s, std::string& r0, std::string& r1,
                                                Rnd& rnd) {
  if (s == "indomain_min") {
    r0 = "=";
    r1 = "!=";
    return BOOL_VAL_MIN();
  }
  if (s == "indomain_max") {
    r0 = "=";
    r1 = "!=";
    return BOOL_VAL_MAX();
  }
  if (s == "indomain_median") {
    r0 = "=";
    r1 = "!=";
    return BOOL_VAL_MIN();
  }
  if (s == "indomain_split") {
    r0 = "<=";
    r1 = ">";
    return BOOL_VAL_MIN();
  }
  if (s == "indomain_reverse_split") {
    r0 = ">";
    r1 = "<=";
    return BOOL_VAL_MAX();
  }
  if (s == "indomain_random") {
    r0 = "=";
    r1 = "!=";
    return BOOL_VAL_RND(rnd);
  }
  if (s == "indomain") {
    r0 = "=";
    r1 = "=";
    return BOOL_VAL_MIN();
  }
  if (s == "indomain_middle") {
    std::cerr << "Warning, replacing unsupported annotation "
              << "indomain_middle with indomain_median" << std::endl;
    r0 = "=";
    r1 = "!=";
    return BOOL_VAL_MIN();
  }
  if (s == "indomain_interval") {
    std::cerr << "Warning, replacing unsupported annotation "
              << "indomain_interval with indomain_split" << std::endl;
    r0 = "<=";
    r1 = ">";
    return BOOL_VAL_MIN();
  }
  std::cerr << "Warning, ignored search annotation: " << s << "\n";
  r0 = "=";
  r1 = "!=";
  return BOOL_VAL_MIN();
}

BoolAssign GecodeSolverInstance::ann2asnbvalsel(const ASTString s, Rnd& rnd) {
  if ((s == "indomain_min") || (s == "indomain_median")) {
    return BOOL_ASSIGN_MIN();
  }
  if (s == "indomain_max") {
    return BOOL_ASSIGN_MAX();
  }
  if (s == "indomain_random") {
    return BOOL_ASSIGN_RND(rnd);
  }
  std::cerr << "Warning, ignored search annotation: " << s << "\n";
  return BOOL_ASSIGN_MIN();
}

IntAssign GecodeSolverInstance::ann2asnivalsel(const ASTString s, Rnd& rnd) {
  if (s == "indomain_min") {
    return INT_ASSIGN_MIN();
  }
  if (s == "indomain_max") {
    return INT_ASSIGN_MAX();
  }
  if (s == "indomain_median") {
    return INT_ASSIGN_MED();
  }
  if (s == "indomain_random") {
    return INT_ASSIGN_RND(rnd);
  }
  std::cerr << "Warning, ignored search annotation: " << s << std::endl;
  return INT_ASSIGN_MIN();
}

#ifdef GECODE_HAS_SET_VARS
SetVarBranch GecodeSolverInstance::ann2svarsel(const ASTString s, Rnd& rnd, double decay) {
  if (s == "input_order") {
    return SET_VAR_NONE();
  }
  if (s == "first_fail") {
    return SET_VAR_SIZE_MIN();
  }
  if (s == "anti_first_fail") {
    return SET_VAR_SIZE_MAX();
  }
  if (s == "smallest") {
    return SET_VAR_MIN_MIN();
  }
  if (s == "largest") {
    return SET_VAR_MAX_MAX();
  }
  if (s == "afc_min") {
    return SET_VAR_AFC_MIN(decay);
  }
  if (s == "afc_max") {
    return SET_VAR_AFC_MAX(decay);
  }
  if (s == "afc_size_min") {
    return SET_VAR_AFC_SIZE_MIN(decay);
  }
  if (s == "afc_size_max") {
    return SET_VAR_AFC_SIZE_MAX(decay);
  }
  if (s == "action_min") {
    return SET_VAR_ACTION_MIN(decay);
  }
  if (s == "action_max") {
    return SET_VAR_ACTION_MAX(decay);
  }
  if (s == "action_size_min") {
    return SET_VAR_ACTION_SIZE_MIN(decay);
  }
  if (s == "action_size_max") {
    return SET_VAR_ACTION_SIZE_MAX(decay);
  }
  if (s == "random") {
    return SET_VAR_RND(rnd);
  }
  std::cerr << "Warning, ignored search annotation: " << s << std::endl;
  return SET_VAR_NONE();
}

SetValBranch GecodeSolverInstance::ann2svalsel(const ASTString s, std::string& r0, std::string& r1,
                                               Rnd& rnd) {
  (void)rnd;
  if (s == "indomain_min") {
    r0 = "in";
    r1 = "not in";
    return SET_VAL_MIN_INC();
  }
  if (s == "indomain_max") {
    r0 = "in";
    r1 = "not in";
    return SET_VAL_MAX_INC();
  }
  if (s == "outdomain_min") {
    r1 = "in";
    r0 = "not in";
    return SET_VAL_MIN_EXC();
  }
  if (s == "outdomain_max") {
    r1 = "in";
    r0 = "not in";
    return SET_VAL_MAX_EXC();
  }
  std::cerr << "Warning, ignored search annotation: " << s << std::endl;
  r0 = "in";
  r1 = "not in";
  return SET_VAL_MIN_INC();
}
#endif

#ifdef GECODE_HAS_FLOAT_VARS
TieBreak<FloatVarBranch> GecodeSolverInstance::ann2fvarsel(const ASTString s, Rnd& rnd,
                                                           double decay) {
  if (s == "input_order") {
    return TieBreak<FloatVarBranch>(FLOAT_VAR_NONE());
  }
  if (s == "first_fail") {
    return TieBreak<FloatVarBranch>(FLOAT_VAR_SIZE_MIN());
  }
  if (s == "anti_first_fail") {
    return TieBreak<FloatVarBranch>(FLOAT_VAR_SIZE_MAX());
  }
  if (s == "smallest") {
    return TieBreak<FloatVarBranch>(FLOAT_VAR_MIN_MIN());
  }
  if (s == "largest") {
    return TieBreak<FloatVarBranch>(FLOAT_VAR_MAX_MAX());
  }
  if (s == "occurrence") {
    return TieBreak<FloatVarBranch>(FLOAT_VAR_DEGREE_MAX());
  }
  if (s == "most_constrained") {
    return TieBreak<FloatVarBranch>(FLOAT_VAR_SIZE_MIN(), FLOAT_VAR_DEGREE_MAX());
  }
  if (s == "random") {
    return TieBreak<FloatVarBranch>(FLOAT_VAR_RND(rnd));
  }
  if (s == "afc_min") {
    return TieBreak<FloatVarBranch>(FLOAT_VAR_AFC_MIN(decay));
  }
  if (s == "afc_max") {
    return TieBreak<FloatVarBranch>(FLOAT_VAR_AFC_MAX(decay));
  }
  if (s == "afc_size_min") {
    return TieBreak<FloatVarBranch>(FLOAT_VAR_AFC_SIZE_MIN(decay));
  }
  if (s == "afc_size_max") {
    return TieBreak<FloatVarBranch>(FLOAT_VAR_AFC_SIZE_MAX(decay));
  }
  if (s == "action_min") {
    return TieBreak<FloatVarBranch>(FLOAT_VAR_ACTION_MIN(decay));
  }
  if (s == "action_max") {
    return TieBreak<FloatVarBranch>(FLOAT_VAR_ACTION_MAX(decay));
  }
  if (s == "action_size_min") {
    return TieBreak<FloatVarBranch>(FLOAT_VAR_ACTION_SIZE_MIN(decay));
  }
  if (s == "action_size_max") {
    return TieBreak<FloatVarBranch>(FLOAT_VAR_ACTION_SIZE_MAX(decay));
  }
  std::cerr << "Warning, ignored search annotation: " << s << std::endl;
  return TieBreak<FloatVarBranch>(FLOAT_VAR_NONE());
}

FloatValBranch GecodeSolverInstance::ann2fvalsel(const ASTString s, std::string& r0,
                                                 std::string& r1) {
  if (s == "indomain_split") {
    r0 = "<=";
    r1 = ">";
    return FLOAT_VAL_SPLIT_MIN();
  }
  if (s == "indomain_reverse_split") {
    r1 = "<=";
    r0 = ">";
    return FLOAT_VAL_SPLIT_MAX();
  }
  std::cerr << "Warning, ignored search annotation: " << s << std::endl;
  r0 = "<=";
  r1 = ">";
  return FLOAT_VAL_SPLIT_MIN();
}
#endif
}  // namespace MiniZinc
