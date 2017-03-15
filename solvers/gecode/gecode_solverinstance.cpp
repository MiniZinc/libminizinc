/*
 *  Main authors:
 *     Kevin Leo <kevin.leo@monash.edu>
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/exception.hh>
#include <minizinc/ast.hh>
#include <minizinc/eval_par.hh>

#include <minizinc/solvers/gecode_solverinstance.hh>
#include <minizinc/solvers/gecode/gecode_constraints.hh>
#include "aux_brancher.hh"
#include <minizinc/solvers/gecode/fzn_space.hh>

using namespace std;
using namespace Gecode;

namespace MiniZinc {

  class Gecode_SolverFactory: public SolverFactory {
    Options _options;
  public:
    SolverInstanceBase* doCreateSI(Env& env) {
      return new GecodeSolverInstance(env, _options);
    }
    string getVersion( );
    bool processOption(int& i, int argc, const char** argv);
    void printHelp(std::ostream& os);
  };
  
  SolverFactory* SolverFactory::createF_GECODE() {
    return new Gecode_SolverFactory;
  }

  string Gecode_SolverFactory::getVersion()
  {
    string v = "Gecode solver plugin, compiled  " __DATE__ "  " __TIME__;
    return v;
  }

  bool Gecode_SolverFactory::processOption(int& i, int argc, const char** argv)
  {
    if (string(argv[i])=="--only-range-domains") {
      _options.setBoolParam(std::string("only-range-domains"), true);
    } else if (string(argv[i])=="--sac") {
      _options.setBoolParam(std::string("sac"), true);
    } else if (string(argv[i])=="--shave") {
      _options.setBoolParam(std::string("shave"), true);
    } else if (string(argv[i])=="--pre-passes") {
      if (++i==argc) return false;
      int passes = atoi(argv[i]);
      if(passes >= 0)
        _options.setIntParam(std::string("pre_passes"), passes);
    } else if (string(argv[i])=="-a") {
      _options.setBoolParam(std::string("all_solutions"), true);
    } else if (string(argv[i])=="-n") {
      if (++i==argc) return false;
      int n = atoi(argv[i]);
      if(n >= 0)
        _options.setIntParam(std::string("n_solutions"), n);
    } else if (string(argv[i])=="--node") {
      if (++i==argc) return false;
      int nodes = atoi(argv[i]);
      if(nodes >= 0)
        _options.setIntParam(std::string("nodes"), nodes);
    } else if (string(argv[i])=="--fail") {
      if (++i==argc) return false;
      int fails = atoi(argv[i]);
      if(fails >= 0)
        _options.setIntParam(std::string("fails"), fails);
    } else if (string(argv[i])=="--time") {
      if (++i==argc) return false;
      int time = atoi(argv[i]);
      if(time >= 0)
        _options.setIntParam(std::string("time"), time);
    }
    return true;
  }
  
  void Gecode_SolverFactory::printHelp(ostream& os)
  {
    os
    << "Gecode solver plugin options:" << std::endl
    << "  --only-range-domains" << std::endl
    << "    only tighten bounds" << std::endl
    << "  --sac" << std ::endl
    << "    singleton arc consistency" << std::endl
    << "  --shave" << std::endl
    << "    shave domains" << std::endl
    << "  --pre-passes <n>" << std::endl
    << "    n passes of sac/shaving, 0 for fixed point" << std::endl
    << "  --node <n>" << std::endl
    << "    node cutoff (0 = none, solution mode)" << std::endl
    << "  --fail <f>" << std::endl
    << "    failure cutoff (0 = none, solution mode)" << std::endl
    << "  --time <ms>" << std::endl
    << "    time (in ms) cutoff (0 = none, solution mode)" << std::endl
    << "  -a" << std::endl
    << "    print intermediate solutions" << std::endl
    << "  -n <sols>" << std::endl
    << "    number of solutions" << std::endl
    << std::endl;
  }

  class GecodeEngine {
  public:
    virtual FznSpace* next(void) = 0;
    virtual bool stopped(void) = 0;
    virtual ~GecodeEngine(void) {}
    virtual Gecode::Search::Statistics statistics(void) = 0;
  };

#ifdef HAS_GECODE_VERSION_5
  template<template<class> class Engine,
           template<class, template<class> class> class Meta>
#else 
  template<template<class> class Engine,
           template<template<class> class,class> class Meta>
#endif
  class MetaEngine : public GecodeEngine {
#ifdef HAS_GECODE_VERSION_5
    Meta<FznSpace,Engine> e;
#else
    Meta<Engine,FznSpace> e;
#endif
  public:
    MetaEngine(FznSpace* s, Search::Options& o) : e(s,o) {}
    virtual FznSpace* next(void) { return e.next(); }
    virtual bool stopped(void) { return e.stopped(); }
    virtual Gecode::Search::Statistics statistics(void) { return e.statistics(); }
  };

     GecodeSolverInstance::GecodeSolverInstance(Env& env, const Options& options)
       : SolverInstanceImpl<GecodeSolver>(env,options), _n_found_solutions(0),
       _current_space(NULL),
       _solution(NULL), engine(NULL) {
       registerConstraints();
       _flat = env.flat();
     }

    GecodeSolverInstance::~GecodeSolverInstance(void) {
      delete engine;
      //delete _current_space;
      // delete _solution; // TODO: is this necessary?
    }

    void GecodeSolverInstance::registerConstraint(std::string name, poster p) {
      std::stringstream ss;
      ss << "gecode_" << name;
      _constraintRegistry.add(ASTString(ss.str()), p);
      _constraintRegistry.add(ASTString(name), p);
    }

    void GecodeSolverInstance::registerConstraints(void) {
      GCLock lock;
      registerConstraint("all_different_int", GecodeConstraints::p_distinct);
      registerConstraint("all_different_offset", GecodeConstraints::p_distinctOffset);
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
      //#ifndef GECODE_HAS_SET_VARS
      registerConstraint("set_in", GecodeConstraints::p_int_in);
      registerConstraint("set_in_reif", GecodeConstraints::p_int_in_reif);
      registerConstraint("set_in_imp", GecodeConstraints::p_int_in_imp);
      //#endif

      registerConstraint("array_int_lt", GecodeConstraints::p_array_int_lt);
      registerConstraint("array_int_lq", GecodeConstraints::p_array_int_lq);
      registerConstraint("array_bool_lt", GecodeConstraints::p_array_bool_lt);
      registerConstraint("array_bool_lq", GecodeConstraints::p_array_bool_lq);
      registerConstraint("count", GecodeConstraints::p_count);
      registerConstraint("count_reif", GecodeConstraints::p_count_reif);
      registerConstraint("count_imp", GecodeConstraints::p_count_imp);
      registerConstraint("at_least_int", GecodeConstraints::p_at_least);
      registerConstraint("at_most_int", GecodeConstraints::p_at_most);
      registerConstraint("gecode_bin_packing_load", GecodeConstraints::p_bin_packing_load);
      registerConstraint("global_cardinality", GecodeConstraints::p_global_cardinality);
      registerConstraint("global_cardinality_closed", GecodeConstraints::p_global_cardinality_closed);
      registerConstraint("global_cardinality_low_up", GecodeConstraints::p_global_cardinality_low_up);
      registerConstraint("global_cardinality_low_up_closed", GecodeConstraints::p_global_cardinality_low_up_closed);
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
      registerConstraint("gecode_among_seq_int", GecodeConstraints::p_among_seq_int);
      registerConstraint("gecode_among_seq_bool", GecodeConstraints::p_among_seq_bool);


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

      registerConstraint("gecode_schedule_unary", GecodeConstraints::p_schedule_unary);
      registerConstraint("gecode_schedule_unary_optional", GecodeConstraints::p_schedule_unary_optional);
      registerConstraint("gecode_schedule_cumulative_optional", GecodeConstraints::p_cumulative_opt);

      registerConstraint("gecode_circuit", GecodeConstraints::p_circuit);
      registerConstraint("gecode_circuit_cost_array", GecodeConstraints::p_circuit_cost_array);
      registerConstraint("gecode_circuit_cost", GecodeConstraints::p_circuit_cost);
      registerConstraint("gecode_nooverlap", GecodeConstraints::p_nooverlap);
      registerConstraint("gecode_precede", GecodeConstraints::p_precede);
      registerConstraint("nvalue", GecodeConstraints::p_nvalue);
      registerConstraint("among", GecodeConstraints::p_among);
      registerConstraint("member_int", GecodeConstraints::p_member_int);
      registerConstraint("gecode_member_int_reif", GecodeConstraints::p_member_int_reif);
      registerConstraint("member_bool", GecodeConstraints::p_member_bool);
      registerConstraint("gecode_member_bool_reif", GecodeConstraints::p_member_bool_reif);

#ifdef GECODE_HAS_FLOAT_VARS
      registerConstraint("int2float",GecodeConstraints::p_int2float);
      registerConstraint("float_abs",GecodeConstraints::p_float_abs);
      registerConstraint("float_sqrt",GecodeConstraints::p_float_sqrt);
      registerConstraint("float_eq",GecodeConstraints::p_float_eq);
      registerConstraint("float_eq_reif",GecodeConstraints::p_float_eq_reif);
      registerConstraint("float_le",GecodeConstraints::p_float_le);
      registerConstraint("float_le_reif",GecodeConstraints::p_float_le_reif);
      registerConstraint("float_lt",GecodeConstraints::p_float_lt);
      registerConstraint("float_lt_reif",GecodeConstraints::p_float_lt_reif);
      registerConstraint("float_ne",GecodeConstraints::p_float_ne);
      registerConstraint("float_times",GecodeConstraints::p_float_times);
      registerConstraint("float_div",GecodeConstraints::p_float_div);
      registerConstraint("float_plus",GecodeConstraints::p_float_plus);
      registerConstraint("float_max",GecodeConstraints::p_float_max);
      registerConstraint("float_min",GecodeConstraints::p_float_min);
      registerConstraint("float_lin_eq",GecodeConstraints::p_float_lin_eq);
      registerConstraint("float_lin_eq_reif",GecodeConstraints::p_float_lin_eq_reif);
      registerConstraint("float_lin_le",GecodeConstraints::p_float_lin_le);
      registerConstraint("float_lin_le_reif",GecodeConstraints::p_float_lin_le_reif);
#endif
#ifdef GECODE_HAS_MPFR
      registerConstraint("float_acos",GecodeConstraints::p_float_acos);
      registerConstraint("float_asin",GecodeConstraints::p_float_asin);
      registerConstraint("float_atan",GecodeConstraints::p_float_atan);
      registerConstraint("float_cos",GecodeConstraints::p_float_cos);
      registerConstraint("float_exp",GecodeConstraints::p_float_exp);
      registerConstraint("float_ln",GecodeConstraints::p_float_ln);
      registerConstraint("float_log10",GecodeConstraints::p_float_log10);
      registerConstraint("float_log2",GecodeConstraints::p_float_log2);
      registerConstraint("float_sin",GecodeConstraints::p_float_sin);
      registerConstraint("float_tan",GecodeConstraints::p_float_tan);
#endif		
    }

  inline void GecodeSolverInstance::insertVar(Id* id, GecodeVariable gv) {
    //std::cerr << *id << ": " << id->decl() << std::endl;
    _variableMap.insert(id->decl()->id(), gv);
  }

  inline bool GecodeSolverInstance::valueWithinBounds(double b) {
    long long int bo = round_to_longlong(b);
    return bo >= Gecode::Int::Limits::min && bo <= Gecode::Int::Limits::max;
  }

  void GecodeSolverInstance::processFlatZinc(void) {
    _only_range_domains = _options.getBoolParam(std::string("only-range-domains"), false);
    _run_sac = _options.getBoolParam(std::string("sac"), false);
    _run_shave = _options.getBoolParam(std::string("shave"), false);
    _pre_passes = _options.getIntParam(std::string("pre_passes"), 1);
    _print_stats = _options.getBoolParam(std::string("statistics"), false);
    _all_solutions = _options.getBoolParam(std::string("all_solutions"), false);
    _n_max_solutions = _options.getIntParam(std::string("n_solutions"), 1);
    _current_space = new FznSpace();

    // iterate over VarDecls of the flat model and create variables
    for (VarDeclIterator it = _flat->begin_vardecls(); it != _flat->end_vardecls(); ++it) {
      if (!it->removed() && it->e()->type().isvar()) {
        // check if it has an output-annotation
        VarDecl* vd = it->e();
        if(!vd->ann().isEmpty()) {
          if(vd->ann().containsCall(constants().ann.output_array.aststr()) ||
              vd->ann().contains(constants().ann.output_var)
            ) {
            _varsWithOutput.push_back(vd);
          }
        }

        if (it->e()->type().dim() != 0) {
          // we ignore arrays - all their elements are defined
          continue;
        }
        MiniZinc::TypeInst* ti = it->e()->ti();
        bool isDefined, isIntroduced = false;

        if(vd->type().isint()) {
          if(!it->e()->e()) { // if there is no initialisation expression
            Expression* domain = ti->domain();
            if(domain) {
              IntVar intVar(*this->_current_space, arg2intset(_env.envi(), domain));
              _current_space->iv.push_back(intVar);
              insertVar(it->e()->id(), GecodeVariable(GecodeVariable::INT_TYPE, _current_space->iv.size()-1));
            } else {
              std::stringstream ssm;
              ssm << "GecodeSolverInstance::processFlatZinc: Error: Unbounded Variable: " << *vd << std::endl;
              throw InternalError(ssm.str());
            }
          } else { // there is an initialisation expression
            Expression* init = it->e()->e();
            if (init->isa<Id>() || init->isa<ArrayAccess>()) {
              // root->iv[root->intVarCount++] = root->iv[*(int*)resolveVar(init)];
              GecodeVariable var = resolveVar(init);
              assert(var.isint());
              _current_space->iv.push_back(var.intVar(_current_space));
              insertVar(it->e()->id(), var);
            } else {
              double il = init->cast<IntLit>()->v().toInt();
              if(valueWithinBounds(il)) {
                IntVar intVar(*this->_current_space, il, il);
                _current_space->iv.push_back(intVar);
                insertVar(it->e()->id(), GecodeVariable(GecodeVariable::INT_TYPE,
                      _current_space->iv.size()-1));
              } else {
                std::stringstream ssm;
                ssm << "GecodeSolverInstance::processFlatZinc: Error: Unsafe value for Gecode: " << il << std::endl;
                throw InternalError(ssm.str());
              }
            }
          }
          isIntroduced = it->e()->introduced() || (MiniZinc::getAnnotation(it->e()->ann(), constants().ann.is_introduced.str()) != NULL);
          _current_space->iv_introduced.push_back(isIntroduced);
          isDefined = MiniZinc::getAnnotation(it->e()->ann(), constants().ann.is_defined_var->str().str()) != NULL;
          _current_space->iv_defined.push_back(isDefined);

        } else if(vd->type().isbool()) {
          double lb=0, ub=1;
          if(!it->e()->e()) { // there is NO initialisation expression
            Expression* domain = ti->domain();
            if(domain) {
              IntBounds ib = compute_int_bounds(_env.envi(), domain);
              lb = ib.l.toInt();
              ub = ib.u.toInt();
            } else {
              lb = 0;
              ub = 1;
            }
            BoolVar boolVar(*this->_current_space, lb, ub);
            _current_space->bv.push_back(boolVar);
            insertVar(it->e()->id(), GecodeVariable(GecodeVariable::BOOL_TYPE,
                  _current_space->bv.size()-1));
          } else { // there is an initialisation expression
            Expression* init = it->e()->e();
            if (init->isa<Id>() || init->isa<ArrayAccess>()) {
              // root->bv[root->boolVarCount++] = root->bv[*(int*)resolveVar(init)];
              //int index = *(int*) resolveVar(init);
              GecodeVariable var = resolveVar(init);
              assert(var.isbool());
              _current_space->bv.push_back(var.boolVar(_current_space));
              insertVar(it->e()->id(), var);
            } else {
              double b = (double) init->cast<BoolLit>()->v();
              BoolVar boolVar(*this->_current_space, b, b);
              _current_space->bv.push_back(boolVar);
              insertVar(it->e()->id(), GecodeVariable(GecodeVariable::BOOL_TYPE,
                    _current_space->bv.size()-1));
            }
          }
          isIntroduced = it->e()->introduced() || (MiniZinc::getAnnotation(it->e()->ann(), constants().ann.is_introduced.str()) != NULL);
          _current_space->bv_introduced.push_back(isIntroduced);
          isDefined = MiniZinc::getAnnotation(it->e()->ann(), constants().ann.is_defined_var->str().str()) != NULL;
          _current_space->bv_defined.push_back(isDefined);
        } else if(vd->type().isfloat()) {
          if(it->e()->e() == NULL) { // there is NO initialisation expression
            Expression* domain = ti->domain();
            double lb, ub;
            if (domain) {
              FloatBounds fb = compute_float_bounds(_env.envi(), vd->id());
              lb = fb.l.toDouble();
              ub = fb.u.toDouble();
            } else {
              std::stringstream ssm;
              ssm << "GecodeSolverInstance::processFlatZinc: Error: Unbounded Variable: " << *vd << std::endl;
              throw InternalError(ssm.str());
            }
            FloatVar floatVar(*this->_current_space, lb, ub);
            _current_space->fv.push_back(floatVar);
            insertVar(it->e()->id(), GecodeVariable(GecodeVariable::FLOAT_TYPE,
                  _current_space->fv.size()-1));
          } else {
            Expression* init = it->e()->e();
            if (init->isa<Id>() || init->isa<ArrayAccess>()) {
              // root->fv[root->floatVarCount++] = root->fv[*(int*)resolveVar(init)];
              GecodeVariable var = resolveVar(init);
              assert(var.isfloat());
              _current_space->fv.push_back(var.floatVar(_current_space));
              insertVar(it->e()->id(), var);
            } else {
              double il = init->cast<FloatLit>()->v().toDouble();
              FloatVar floatVar(*this->_current_space, il, il);
              _current_space->fv.push_back(floatVar);
              insertVar(it->e()->id(), GecodeVariable(GecodeVariable::FLOAT_TYPE,
                    _current_space->fv.size()-1));
            }
          }
          isIntroduced = it->e()->introduced() || (MiniZinc::getAnnotation(it->e()->ann(), constants().ann.is_introduced.str()) != NULL);
          _current_space->fv_introduced.push_back(isIntroduced);
          isDefined = MiniZinc::getAnnotation(it->e()->ann(), constants().ann.is_defined_var->str().str()) != NULL;
          _current_space->fv_defined.push_back(isDefined);
        } else {
          std::stringstream ssm;
          ssm << "Type " << *ti << " is currently not supported by Gecode." << std::endl;
          throw InternalError(ssm.str());
        }
      } // end if it is a variable
    } // end for all var decls

    // post the constraints
    for (ConstraintIterator it = _flat->begin_constraints(); it != _flat->end_constraints(); ++it) {
      if(!it->removed()) {
      if (Call* c = it->e()->dyn_cast<Call>()) {
        _constraintRegistry.post(c);
      }
    }
    }

    // objective
    SolveI* si = _flat->solveItem();
    _current_space->_solveType = si->st();
    if(si->e()) {
      _current_space->_optVarIsInt = (si->e()->type().isvarint());
      if(Id* id = si->e()->dyn_cast<Id>()) {
        if (si->e()->type().isvar()) {
          GecodeVariable var = resolveVar(id->decl());
          if(_current_space->_optVarIsInt) {
            IntVar intVar = var.intVar(_current_space);
            for(unsigned int i=0; i<_current_space->iv.size(); i++) {
              if(_current_space->iv[i].same(intVar)) {
                _current_space->_optVarIdx = i;
                break;
              }
            }
            assert(_current_space->_optVarIdx >= 0);
          } else {
            FloatVar floatVar = var.floatVar(_current_space);
            for(unsigned int i=0; i<_current_space->fv.size(); i++) {
              if(_current_space->fv[i].same(floatVar)) {
                _current_space->_optVarIdx = i;
                break;
              }
            }
            assert(_current_space->_optVarIdx >= 0);
          }
        } 
      }
      else { // the solve expression has to be a variable/id
        assert(false);
      }

    }


    //std::cout << "DEBUG: at end of processFlatZinc: " << std::endl
    //          << "iv has " << _current_space->iv.size() << " variables " << std::endl
    //          << "bv has " << _current_space->bv.size() << " variables " << std::endl
    //          << "fv has " << _current_space->fv.size() << " variables " << std::endl
    //          << "sv has " << _current_space->sv.size() << " variables " << std::endl;
  }

  Gecode::IntArgs
  GecodeSolverInstance::arg2intargs(Expression* arg, int offset) {
    if(!arg->isa<Id>() && !arg->isa<ArrayLit>()) {
      std::stringstream ssm; ssm << "Invalid argument in arg2intargs: " << *arg;
      ssm << ". Expected Id or ArrayLit.";
      throw InternalError(ssm.str());
    }
    ArrayLit* a = arg->isa<Id>() ? arg->cast<Id>()->decl()->e()->cast<ArrayLit>() : arg->cast<ArrayLit>();
    IntArgs ia(a->v().size()+offset);
    for (int i=offset; i--;)
        ia[i] = 0;
    for (int i=a->v().size(); i--;) {
        ia[i+offset] = a->v()[i]->cast<IntLit>()->v().toInt();
    }
    return ia;
  }

  Gecode::IntArgs
  GecodeSolverInstance::arg2boolargs(Expression* arg, int offset) {
    if(!arg->isa<Id>() && !arg->isa<ArrayLit>()) {
      std::stringstream ssm; ssm << "Invalid argument in arg2boolargs: " << *arg;
      ssm << ". Expected Id or ArrayLit.";
      throw InternalError(ssm.str());
    }
    ArrayLit* a = arg->isa<Id>() ? arg->cast<Id>()->decl()->e()->cast<ArrayLit>() : arg->cast<ArrayLit>();
    IntArgs ia(a->v().size()+offset);
    for (int i=offset; i--;)
        ia[i] = 0;
    for (int i=a->v().size(); i--;)
        ia[i+offset] = a->v()[i]->cast<BoolLit>()->v();
    return ia;
  }


  class GecodeRangeIter {
  public:
    GecodeSolverInstance& si;
    IntSetRanges& isr;
    GecodeRangeIter(GecodeSolverInstance& gsi, IntSetRanges& isr0) : si(gsi), isr(isr0) {}
    int min(void) const {
      long long int val = isr.min().toInt();
      if(si.valueWithinBounds(val)) {
        return (int)val;
      } else {
        std::stringstream ssm;
        ssm << "GecodeRangeIter::min: Error: " << val << " outside 32-bit int." << std::endl;
        throw InternalError(ssm.str());
      }
    }
    int max(void) const {
      long long int val = isr.max().toInt();
      if(si.valueWithinBounds(val)) {
        return (int)val;
      } else {
        std::stringstream ssm;
        ssm << "GecodeRangeIter::max: Error: " << val << " outside 32-bit int." << std::endl;
        throw InternalError(ssm.str());
      }
    }
    int width(void) const { return isr.width().toInt(); }
    bool operator() (void) { return isr(); }
    void operator++ (void) { ++isr; }
  };

  Gecode::IntSet
  GecodeSolverInstance::arg2intset(EnvI& envi, Expression* arg) {
    GCLock lock;
    IntSetVal* isv = eval_intset(envi, arg);
    IntSetRanges isr(isv);
    GecodeRangeIter isr_g(*this, isr);
    IntSet d(isr_g);
    return d;
   }

  Gecode::IntVarArgs
  GecodeSolverInstance::arg2intvarargs(Expression* arg, int offset) {
    ArrayLit* a = arg2arraylit(arg);
    if (a->v().size() == 0) {
        IntVarArgs emptyIa(0);
        return emptyIa;
    }
    IntVarArgs ia(a->v().size()+offset);
    for (int i=offset; i--;)
        ia[i] = IntVar(*this->_current_space, 0, 0);
    for (int i=a->v().size(); i--;) {
        Expression* e = a->v()[i];
        if (e->type().isvar()) {
            //ia[i+offset] = _current_space->iv[*(int*)resolveVar(getVarDecl(e))];
            GecodeSolver::Variable var = resolveVar(getVarDecl(e));
            assert(var.isint());
            Gecode::IntVar v = var.intVar(_current_space);
            ia[i+offset] = v;
        } else {
            long long int value = e->cast<IntLit>()->v().toInt();
            if(valueWithinBounds(value)) {
              IntVar iv(*this->_current_space, value, value);
              ia[i+offset] = iv;
            } else {
              std::stringstream ssm;
              ssm << "GecodeSolverInstance::arg2intvarargs Error: " << value << " outside 32-bit int." << std::endl;
              throw InternalError(ssm.str());
            }
        }
    }
    return ia;
  }

  Gecode::BoolVarArgs
  GecodeSolverInstance::arg2boolvarargs(Expression* arg, int offset, int siv) {
    ArrayLit* a = arg2arraylit(arg);
    if (a->length() == 0) {
        BoolVarArgs emptyIa(0);
        return emptyIa;
    }
    BoolVarArgs ia(a->length()+offset-(siv==-1?0:1));
    for (int i=offset; i--;)
        ia[i] = BoolVar(*this->_current_space, 0, 0);
    for (int i=0; i<static_cast<int>(a->length()); i++) {
        if (i==siv)
            continue;
        Expression* e = a->v()[i];
        if(e->type().isvar()) {
            GecodeVariable var = resolveVar(getVarDecl(e));
            if (e->type().isvarbool()) {
              assert(var.isbool());
              ia[offset++] = var.boolVar(_current_space);
            } else if(e->type().isvarint() && var.hasBoolAlias()) {
              ia[offset++] = _current_space->bv[var.boolAliasIndex()];
            }
            else {
              std::stringstream ssm;
              ssm << "expected bool-var or alias int var instead of " << *e
                  << " with type " << e->type().toString(env().envi()) ;
              throw InternalError(ssm.str());
            }
        } else {
          if(BoolLit* bl = e->dyn_cast<BoolLit>()) {
            bool value = bl->v();
            BoolVar iv(*this->_current_space, value, value);
            ia[offset++] = iv;
          } else {
            std::stringstream ssm; ssm << "Expected bool literal instead of: " << *e;
            throw new InternalError(ssm.str());
          }
        }
    }
    return ia;
  }

  Gecode::BoolVar
  GecodeSolverInstance::arg2boolvar(Expression* e) {
    BoolVar x0;
    if (e->type().isvar()) {
        //x0 = _current_space->bv[*(int*)resolveVar(getVarDecl(e))];
        GecodeVariable var = resolveVar(getVarDecl(e));
        assert(var.isbool());
        x0 = var.boolVar(_current_space);
    } else {
      if(BoolLit* bl = e->dyn_cast<BoolLit>()) {
        x0 = BoolVar(*this->_current_space, bl->v(), bl->v());
      } else {
        std::stringstream ssm; ssm << "Expected bool literal instead of: " << *e;
        throw new InternalError(ssm.str());
      }
    }
    return x0;
  }

  Gecode::IntVar
  GecodeSolverInstance::arg2intvar(Expression* e) {
    IntVar x0;
    if (e->type().isvar()) {
        //x0 = _current_space->iv[*(int*)resolveVar(getVarDecl(e))];
        GecodeVariable var = resolveVar(getVarDecl(e));
        assert(var.isint());
        x0 = var.intVar(_current_space);
    } else {
        IntVal i;
        if(IntLit* il = e->dyn_cast<IntLit>()) i = il->v().toInt();
        else if(BoolLit* bl = e->dyn_cast<BoolLit>()) i = bl->v();
        else {
          std::stringstream ssm; ssm << "Expected bool or int literal instead of: " << *e;
          throw InternalError(ssm.str());
        }
        x0 = IntVar(*this->_current_space, i.toInt(), i.toInt());
    }
    return x0;
  }

  ArrayLit*
  GecodeSolverInstance::arg2arraylit(Expression* arg) {
    ArrayLit* a;
      if(Id* id = arg->dyn_cast<Id>()) {
          VarDecl* vd = id->decl();
          if(vd->e()) {
              a = vd->e()->cast<ArrayLit>();
          } else {
              std::vector<Expression*>* array = arrayMap[vd];
              std::vector<Expression*> ids;
              for(unsigned int i=0; i<array->size(); i++)
                  ids.push_back(((*array)[i])->cast<VarDecl>()->id());
              a = new ArrayLit(vd->loc(), ids);
          }
      } else if(ArrayLit* al = arg->dyn_cast<ArrayLit>()) {
          a = al;
      } else {
          std::stringstream ssm; ssm << "Invalid argument in arg2arrayLit: " << *arg;
          ssm << ". Expected Id or ArrayLit.";
          throw new InternalError(ssm.str());
      }
      return a;
  }

  bool
  GecodeSolverInstance::isBoolArray(ArrayLit* a, int& singleInt) {
    singleInt = -1;
    if (a->length() == 0)
        return true;
    for (int i=a->length(); i--;) {
        if (a->v()[i]->type().isbool()) {
          continue;
        } else if ((a->v()[i])->type().isvarint()) {
          GecodeVariable var = resolveVar(getVarDecl(a->v()[i]));
          if (var.hasBoolAlias()) {
            if (singleInt != -1) {
              return false;
            }
            singleInt = var.boolAliasIndex();
          }
          else return false;
        } else {
          return false;
        }
    }
    return singleInt==-1 || a->length() > 1;
  }

#ifdef GECODE_HAS_FLOAT_VARS
  Gecode::FloatValArgs
  GecodeSolverInstance::arg2floatargs(Expression* arg, int offset) {
    assert(arg->isa<Id>() || arg->isa<ArrayLit>());
    ArrayLit* a = arg->isa<Id>() ? arg->cast<Id>()->decl()->e()->cast<ArrayLit>() : arg->cast<ArrayLit>();
    FloatValArgs fa(a->v().size()+offset);
    for (int i=offset; i--;)
        fa[i] = 0.0;
    for (int i=a->v().size(); i--;)
        fa[i+offset] = a->v()[i]->cast<FloatLit>()->v().toDouble();
    return fa;
  }

  Gecode::FloatVar
  GecodeSolverInstance::arg2floatvar(Expression* e) {
    FloatVar x0;
    if (e->type().isvar()) {
      GecodeVariable var = resolveVar(getVarDecl(e));
      assert(var.isfloat());
      x0 = var.floatVar(_current_space);
    } else {
        FloatVal i;
        if(IntLit* il = e->dyn_cast<IntLit>()) i = il->v().toInt();
        else if(BoolLit* bl = e->dyn_cast<BoolLit>()) i = bl->v();
        else if(FloatLit* fl = e->dyn_cast<FloatLit>()) i = fl->v();
        else {
          std::stringstream ssm; ssm << "Expected bool, int or float literal instead of: " << *e;
          throw InternalError(ssm.str());
        }
        x0 = FloatVar(*this->_current_space, i.toDouble(), i.toDouble());
    }
    return x0;
  }

  Gecode::FloatVarArgs
  GecodeSolverInstance::arg2floatvarargs(Expression* arg, int offset) {
    ArrayLit* a = arg2arraylit(arg);
    if (a->v().size() == 0) {
        FloatVarArgs emptyFa(0);
        return emptyFa;
    }
    FloatVarArgs fa(a->v().size()+offset);
    for (int i=offset; i--;)
        fa[i] = FloatVar(*this->_current_space, 0.0, 0.0);
    for (int i=a->v().size(); i--;) {
        Expression* e = a->v()[i];
        if (e->type().isvar()) {
            GecodeVariable var = resolveVar(getVarDecl(e));
            assert(var.isfloat());
            fa[i+offset] = var.floatVar(_current_space);
        } else {
          if(FloatLit* fl = e->dyn_cast<FloatLit>()) {
            double value = fl->v().toDouble();
            FloatVar fv(*this->_current_space, value, value);
            fa[i+offset] = fv;
          } else {
            std::stringstream ssm; ssm << "Expected float literal instead of: " << *e;
            throw InternalError(ssm.str());
          }
        }
    }
    return fa;
  }
#endif

  MZ_IntConLevel
  GecodeSolverInstance::ann2icl(const Annotation& ann) {
    if (!ann.isEmpty()) {
      if (getAnnotation(ann, "val"))
          return MZ_ICL_VAL;
      if (getAnnotation(ann, "domain"))
          return MZ_ICL_DOM;
      if (getAnnotation(ann, "bounds") ||
              getAnnotation(ann, "boundsR") ||
              getAnnotation(ann, "boundsD") ||
              getAnnotation(ann, "boundsZ"))
          return MZ_ICL_BND;
    }
    return MZ_ICL_DEF;
  }

  VarDecl*
  GecodeSolverInstance::getVarDecl(Expression* expr) {
    VarDecl* vd=NULL;
    if( (vd = expr->dyn_cast<VarDecl>()) ) {
        vd = expr->cast<VarDecl>();
    } else if(Id* id = expr->dyn_cast<Id>()) {
        vd = id->decl();
    } else if(ArrayAccess* aa = expr->dyn_cast<ArrayAccess>()) {
        vd = resolveArrayAccess(aa);
    } else {
        std::stringstream ssm; ssm << "Can not extract vardecl from " << *expr;
        throw new InternalError(ssm.str());
    }
    return vd;
  }

  VarDecl*
  GecodeSolverInstance::resolveArrayAccess(ArrayAccess* aa) {
    VarDecl* vd = aa->v()->cast<Id>()->decl();
    int idx = aa->idx()[0]->cast<IntLit>()->v().toInt();
    return resolveArrayAccess(vd, idx);
  }

  VarDecl*
  GecodeSolverInstance::resolveArrayAccess(VarDecl* vd, int index) {
    UNORDERED_NAMESPACE::unordered_map<VarDecl*, std::vector<Expression*>* >::iterator it = arrayMap.find(vd);
    if(it != arrayMap.end()) {
        std::vector<Expression*>* exprs = it->second;
        Expression* expr = (*exprs)[index-1];
        return expr->cast<VarDecl>();
    } else {
        std::stringstream ssm; ssm << "Unknown array: " << vd->id();
        throw new InternalError(ssm.str());
    }
  }

  GecodeSolver::Variable
  GecodeSolverInstance::resolveVar(Expression* e) {
    if (Id* id = e->dyn_cast<Id>()) {
        return _variableMap.get(id->decl()->id()); //lookupVar(id->decl());
    } else if (VarDecl* vd = e->dyn_cast<VarDecl>()) {
        return _variableMap.get(vd->id()->decl()->id());
    } else if (ArrayAccess* aa = e->dyn_cast<ArrayAccess>()) {
        return _variableMap.get(resolveArrayAccess(aa)->id()->decl()->id());
    } else {
        std::stringstream ssm;
        ssm << "Expected Id, VarDecl or ArrayAccess instead of \"" << *e << "\"";
        throw InternalError(ssm.str());
    }
  }

  SolverInstance::Status
  GecodeSolverInstance::next(void) {
    prepareEngine();
    
    _solution = engine->next();
    
    if (_solution) {
      assignSolutionToOutput();
      return SolverInstance::SAT;
    } else if (engine->stopped()) {
      return SolverInstance::UNKNOWN;
    } else {
      return SolverInstance::UNSAT;
    }
  }

  void
  GecodeSolverInstance::resetSolver(void) {
    assert(false); // TODO: implement
  }

  Expression*
  GecodeSolverInstance::getSolutionValue(Id* id) {
    id = id->decl()->id();
    if(id->type().isvar()) {
      GecodeVariable var = resolveVar(id->decl()->id());
      switch (id->type().bt()) {
        case Type::BT_INT:
          assert(var.intVar(_solution).assigned());
          return IntLit::a(var.intVar(_solution).val());
        case Type::BT_BOOL:
          assert(var.boolVar(_solution).assigned());
          return new BoolLit(Location(), var.boolVar(_solution).val());
        case Type::BT_FLOAT:
          assert(var.floatVar(_solution).assigned());
          return new FloatLit(Location(), (var.floatVar(_solution).val()).med());
        default: return NULL;
      }
    } else {
      return id->decl()->e();
    }
  }

  void
  GecodeSolverInstance::prepareEngine(void) {
    if (engine==NULL) {
      // TODO: check what we need to do options-wise
      std::vector<Expression*> branch_vars;
      std::vector<Expression*> solve_args;
      Expression* solveExpr = _flat->solveItem()->e();
      Expression* optSearch = NULL;
      
      switch(_current_space->_solveType) {
        case MiniZinc::SolveI::SolveType::ST_MIN:
          assert(solveExpr != NULL);
          branch_vars.push_back(solveExpr);
          solve_args.push_back(new ArrayLit(Location(), branch_vars));
          if (!_current_space->_optVarIsInt) // TODO: why??
            solve_args.push_back(new FloatLit(Location(), 0.0));
          solve_args.push_back(new Id(Location(), "input_order", NULL));
          solve_args.push_back(new Id(Location(), _current_space->_optVarIsInt ? "indomain_min" : "indomain_split", NULL));
          solve_args.push_back(new Id(Location(), "complete", NULL));
          optSearch = new Call(Location(), _current_space->_optVarIsInt ? "int_search" : "float_search", solve_args);
          break;
        case MiniZinc::SolveI::SolveType::ST_MAX:
          branch_vars.push_back(solveExpr);
          solve_args.push_back(new ArrayLit(Location(), branch_vars));
          if (!_current_space->_optVarIsInt)
            solve_args.push_back(new FloatLit(Location(), 0.0));
          solve_args.push_back(new Id(Location(), "input_order", NULL));
          solve_args.push_back(new Id(Location(), _current_space->_optVarIsInt ? "indomain_max" : "indomain_split_reverse", NULL));
          solve_args.push_back(new Id(Location(), "complete", NULL));
          optSearch = new Call(Location(), _current_space->_optVarIsInt ? "int_search" : "float_search", solve_args);
          break;
        case MiniZinc::SolveI::SolveType::ST_SAT:
          break;
        default:
          assert(false);
      }

      int seed = _options.getIntParam("seed", 1);
      double decay = _options.getFloatParam("decay", 0.5);
      
      createBranchers(_flat->solveItem()->ann(), optSearch,
                      seed, decay,
                      false, /* ignoreUnknown */
                      std::cerr);
      
      int nodeStop = _options.getIntParam("nodes", 0);
      int failStop = _options.getIntParam("fails", 0);
      int timeStop = _options.getIntParam("time", 0);

      engine_options.stop = Driver::CombinedStop::create(nodeStop,
                                            failStop,
                                            timeStop,
                                            false);
      // TODO: add presolving part
      if(_current_space->_solveType == MiniZinc::SolveI::SolveType::ST_SAT) {
        engine = new MetaEngine<DFS, Driver::EngineToMeta>(this->_current_space,engine_options);
      } else {
        engine = new MetaEngine<BAB, Driver::EngineToMeta>(this->_current_space,engine_options);
      }
    }
  }

  void GecodeSolverInstance::print_stats(){
      Gecode::Search::Statistics stat = engine->statistics();
      std::cerr << "%%  variables:     " 
        << (_current_space->iv.size() +
            _current_space->bv.size() +
            _current_space->sv.size()) << std::endl
#ifdef HAS_GECODE_VERSION_5
        << "%%  propagators:   " << Gecode::PropagatorGroup::all.size(*_current_space) << endl

#else
        << "%%  propagators:   " << _current_space->propagators() << std::endl
#endif
        << "%%  propagations:  " << stat.propagate << std::endl
        << "%%  nodes:         " << stat.node << std::endl
        << "%%  failures:      " << stat.fail << std::endl
        << "%%  restarts:      " << stat.restart << std::endl
        << "%%  peak depth:    " << stat.depth << std::endl
        << std::endl;
  }

  void GecodeSolverInstance::processSolution(void) {
    if(_solution) {
      assignSolutionToOutput();
      printSolution();
      if (_current_space->_solveType == MiniZinc::SolveI::SolveType::ST_SAT) {
        if (engine->stopped()) {
          _status = SolverInstance::SAT;
        } else {
          _status = SolverInstance::OPT;
        }
      } else {
        if (engine->stopped()) {
          Gecode::Search::Statistics stat = engine->statistics();
          Driver::CombinedStop* cs = static_cast<Driver::CombinedStop*>(engine_options.stop);
          int r = cs->reason(stat, engine_options);
          if (r & Driver::CombinedStop::SR_INT)
            std::cerr << "user interrupt " << std::endl;
          else {
            if (r & Driver::CombinedStop::SR_NODE)
              _status_reason = SolverInstance::SR_LIMIT;
            if (r & Driver::CombinedStop::SR_FAIL)
              _status_reason = SolverInstance::SR_LIMIT;
            if (r & Driver::CombinedStop::SR_TIME)
              _status_reason = SolverInstance::SR_LIMIT;
            std::cerr << "limit reached" << std::endl << std::endl;
          }
          _status = SolverInstance::UNKNOWN;
        } else {
          _status = SolverInstance::OPT;
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

  SolverInstanceBase::Status
  GecodeSolverInstance::solve(void) {

    prepareEngine();

    if(_run_sac || _run_shave) {
      presolve();
    }

    while (FznSpace* next_sol = engine->next()) {
      if(_solution) delete _solution;
      _solution = next_sol;
      _n_found_solutions++;

      if(_all_solutions || _n_found_solutions < _n_max_solutions) {
        processSolution();
        if (_print_stats) print_stats();
      } else {
        if (_current_space->_solveType == MiniZinc::SolveI::SolveType::ST_SAT) {
          break;
        }
      }
    }

    processSolution();
    if (_print_stats) print_stats();
    
    return _status;
  }

  class IntVarComp {
    public:
      std::vector<Gecode::IntVar> iv;
      IntVarComp(std::vector<Gecode::IntVar> b) {iv = b;}
      int operator() (int a, int b) {
        return iv[a].size() < iv[b].size();
      }
  };

  class IntVarRangesBwd : public Int::IntVarImpBwd {
    public:
      IntVarRangesBwd(void) {}
      IntVarRangesBwd(const IntVar& x) : Int::IntVarImpBwd(x.varimp()) {}
      void init(const IntVar& x) {Int::IntVarImpBwd(x.varimp());}
  };

  bool GecodeSolverInstance::sac(bool toFixedPoint = false, bool shaving = false) {
    if(_current_space->status() == SS_FAILED) return false;
    bool modified;
    std::vector<int> sorted_iv;

    for(unsigned int i=0; i<_current_space->iv.size(); i++) if(!_current_space->iv[i].assigned()) sorted_iv.push_back(i);
    IntVarComp ivc(_current_space->iv);
    sort(sorted_iv.begin(), sorted_iv.end(), ivc);

    do {
      modified = false;
      for (unsigned int idx = 0; idx < _current_space->bv.size(); idx++) {
        BoolVar bvar = _current_space->bv[idx];
        if(!bvar.assigned()) {
          for (int val = bvar.min(); val <= bvar.max(); ++val) {
            FznSpace* f = static_cast<FznSpace*>(_current_space->clone());
            rel(*f, f->bv[idx], IRT_EQ, val);
            if(f->status() == SS_FAILED) {
              rel(*_current_space, bvar, IRT_NQ, val);
              modified = true;
              if(_current_space->status() == SS_FAILED)
                return false;
            }
            delete f;
          }
        }
      }

      for (unsigned int i=0; i<sorted_iv.size(); i++) {
        unsigned int idx = sorted_iv[i];
        IntVar ivar = _current_space->iv[idx];
        bool tight = false;
        unsigned int nnq = 0;
        int fwd_min;
        IntArgs nq(ivar.size());
        for (IntVarValues vv(ivar); vv() && !tight; ++vv) {
          FznSpace* f = static_cast<FznSpace*>(_current_space->clone());
          rel(*f, f->iv[idx], IRT_EQ, vv.val());
          if (f->status() == SS_FAILED) {
            nq[nnq++] = vv.val();
          } else {
            fwd_min = vv.val();
            tight = shaving;
          }
          delete f;
        }
        if(shaving) {
          tight = false;
          for (IntVarRangesBwd vr(ivar); vr() && !tight; ++vr) {
            for (int i=vr.max(); i>=vr.min() && i>=fwd_min; i--) {
              FznSpace* f = static_cast<FznSpace*>(_current_space->clone());
              rel(*f, f->iv[idx], IRT_EQ, i);
              if (f->status() == SS_FAILED)
                nq[nnq++] = i;
              else
                tight = true;
              delete f;
            }
          }
        }
        if(nnq) modified = true;
        while (nnq--)
          rel(*_current_space, ivar, IRT_NQ, nq[nnq]);
        if (_current_space->status() == SS_FAILED)
          return false;
      }
    } while(toFixedPoint && modified);
    return true;
  }

  bool GecodeSolverInstance::presolve(Model* orig_model) {
    GCLock lock;
    if(_current_space->status() == SS_FAILED) return false;
    // run SAC?
    if(_run_sac || _run_shave) {
      unsigned int iters = _pre_passes;
      if(iters) {
        for(unsigned int i=0; i<iters; i++)
          sac(false, _run_shave);
      } else {
        sac(true, _run_shave);
      }
    }

    if(orig_model != NULL) {

      UNORDERED_NAMESPACE::unordered_map<std::string, VarDecl*> vds;
      for(VarDeclIterator it = orig_model->begin_vardecls();
          it != orig_model->end_vardecls();
          ++it) {
        VarDecl* vd = it->e();
        vds[vd->id()->str().str()] = vd;
      }

      IdMap<GecodeVariable>::iterator it;
      for(it = _variableMap.begin(); it != _variableMap.end(); it++) {
        VarDecl* vd = it->first->decl();
        long long int old_domsize = 0;
        bool holes = false;

        if(vd->ti()->domain()) {
          if(vd->type().isint()) {
            IntBounds old_bounds = compute_int_bounds(_env.envi(), vd->id());
            long long int old_rangesize = abs(old_bounds.u.toInt() - old_bounds.l.toInt());
            if(vd->ti()->domain()->isa<SetLit>())
              old_domsize = arg2intset(_env.envi(), vd->ti()->domain()).size();
            else
              old_domsize = old_rangesize + 1;
            holes = old_domsize < old_rangesize + 1;
          }
        }

        std::string name = it->first->str().str();


        if(vds.find(name) != vds.end()) {
          VarDecl* nvd = vds[name];
          Type::BaseType bt = vd->type().bt();
          if(bt == Type::BaseType::BT_INT) {
            IntVar intvar = it->second.intVar(_current_space);
            const long long int l = intvar.min(), u = intvar.max();

            if(l==u) {
              if(nvd->e()) {
                nvd->ti()->domain(new SetLit(nvd->loc(), IntSetVal::a(l, u)));
              } else {
                nvd->type(Type::parint());
                nvd->ti(new TypeInst(nvd->loc(), Type::parint()));
                nvd->e(IntLit::a(l));
              }
            } else if(!(l == Gecode::Int::Limits::min || u == Gecode::Int::Limits::max)){
              if(_only_range_domains && !holes) {
                nvd->ti()->domain(new SetLit(nvd->loc(), IntSetVal::a(l, u)));
              } else {
                IntVarRanges ivr(intvar);
                nvd->ti()->domain(new SetLit(nvd->loc(), IntSetVal::ai(ivr)));
              }
            }
          } else if(bt == Type::BaseType::BT_BOOL) {
            BoolVar boolvar = it->second.boolVar(_current_space);
            int l = boolvar.min(),
                u = boolvar.max();
            if(l == u) {
              if(nvd->e()) {
                nvd->ti()->domain(constants().boollit(l));
              } else {
                nvd->type(Type::parbool());
                nvd->ti(new TypeInst(nvd->loc(), Type::parbool()));
                nvd->e(new BoolLit(nvd->loc(), l));
              }
            }
          } else if(bt == Type::BaseType::BT_FLOAT) {
            Gecode::FloatVar floatvar = it->second.floatVar(_current_space);
            if(floatvar.assigned() && !nvd->e()) {
              FloatNum l = floatvar.min();
              nvd->type(Type::parfloat());
              nvd->ti(new TypeInst(nvd->loc(), Type::parfloat()));
              nvd->e(new FloatLit(nvd->loc(), l));
            } else {
              FloatNum l = floatvar.min(),
                       u = floatvar.max();
              nvd->ti()->domain(new BinOp(nvd->loc(),
                    new FloatLit(nvd->loc(), l),
                    BOT_DOTDOT,
                    new FloatLit(nvd->loc(), u)));
            }
          }
        }
      }
    }
    return true;
  }

  void
  GecodeSolverInstance::setSearchStrategyFromAnnotation(std::vector<Expression*> flatAnn, 
                                                        std::vector<bool>& iv_searched, 
                                                        std::vector<bool>& bv_searched,
                                                        std::vector<bool>& sv_searched,
                                                        std::vector<bool>& fv_searched,
                                                        TieBreak<IntVarBranch>& def_int_varsel,
                                                        IntValBranch& def_int_valsel,
                                                        TieBreak<IntVarBranch>& def_bool_varsel,
                                                        IntValBranch& def_bool_valsel,
                                                #ifdef GECODE_HAS_SET_VARS
                                                        SetVarBranch& def_set_varsel,
                                                        SetValBranch& def_set_valsel,
                                                #endif
                                                #ifdef GECODE_HAS_FLOAT_VARS
                                                        TieBreak<FloatVarBranch>& def_float_varsel,
                                                        FloatValBranch& def_float_valsel,
                                                #endif
                                                        Rnd& rnd,
                                                        double decay,
                                                        bool ignoreUnknown,
                                                        std::ostream& err
                                                       )  {    
    for (unsigned int i=0; i<flatAnn.size(); i++) {      
      if (flatAnn[i]->isa<Call>() && flatAnn[i]->cast<Call>()->id().str() == "gecode_search") {
        //Call* c = flatAnn[i]->cast<Call>();
        //branchWithPlugin(c->args); 
        std::cerr << "WARNING: Not supporting search annotation \"gecode_search\" yet." << std::endl;
        return;
      } 
      else if (flatAnn[i]->isa<Call>() && flatAnn[i]->cast<Call>()->id().str() == "int_search") {
        Call* call = flatAnn[i]->cast<Call>();            
        ArrayLit *vars = arg2arraylit(call->args()[0]);
        if(vars->v().size() == 0) { // empty array
          std::cerr << "WARNING: trying to branch on empty array in search annotation: " << *call << std::endl;
          continue;
        }
        int k=vars->v().size();
        for (int i=vars->v().size();i--;)
          if (!(vars->v()[i])->type().isvarint())
            k--;
        IntVarArgs va(k);
        std::vector<std::string> names;
        k=0;
        for (unsigned int i=0; i<vars->v().size(); i++) {          
          if (!(vars->v()[i])->type().isvarint()) {
            continue;
          }          
          int idx = resolveVar(getVarDecl(vars->v()[i])).index();
          va[k++] = _current_space->iv[idx];
          iv_searched[idx] = true;
          names.push_back(getVarDecl(vars->v()[i])->id()->str().str());          
        }        
        std::string r0, r1;
        //BrancherHandle bh = 
        branch(*_current_space, va,
            ann2ivarsel(call->args()[1]->cast<Id>()->str().str(),rnd,decay),
            ann2ivalsel(call->args()[2]->cast<Id>()->str().str(),r0,r1,rnd),
            NULL
            //,&varValPrint<IntVar>
            );
        //branchInfo.add(bh,r0,r1,names);
      } // end int_search
      else if (flatAnn[i]->isa<Call>() && flatAnn[i]->cast<Call>()->id().str() == "int_assign") {
        Call* call = flatAnn[i]->dyn_cast<Call>();
        ArrayLit* vars = arg2arraylit(call->args()[0]);
        int k=vars->v().size();
        for (int i=vars->v().size(); i--;)
          if (!(vars->v()[i])->type().isvarint())
            k--;
        IntVarArgs va(k);
        k=0;
        for (unsigned int i=0; i<vars->v().size(); i++) {
          if (!(vars->v()[i])->type().isvarint())
            continue;
          int idx = resolveVar(getVarDecl(vars->v()[i])).index();
          va[k++] = _current_space->iv[idx];
          iv_searched[idx] = true;
        }
        assign(*_current_space, va, ann2asnivalsel(call->args()[1]->cast<Id>()->str().str(),rnd), NULL
                            //&varValPrint<IntVar>
              );
      } 
      else if (flatAnn[i]->isa<Call>() && flatAnn[i]->cast<Call>()->id().str() == "bool_search") {
        Call* call = flatAnn[i]->dyn_cast<Call>();
        ArrayLit* vars = arg2arraylit(call->args()[0]);
        int k=vars->v().size();
        for (int i=vars->v().size(); i--;)
            if (!(vars->v()[i])->type().isvarbool())
                k--;
        BoolVarArgs va(k);
        k=0;
        std::vector<std::string> names;
        for (unsigned int i=0; i<vars->v().size(); i++) {
            if (!(vars->v()[i])->type().isvarbool())
                continue;
            int idx = resolveVar(getVarDecl(vars->v()[i])).index();
            va[k++] = _current_space->bv[idx];
            bv_searched[idx] = true;
            names.push_back(getVarDecl(vars->v()[i])->id()->str().str());
        }

        std::string r0, r1;
        //BrancherHandle bh = 
        branch(*_current_space, va, 
            ann2ivarsel(call->args()[1]->cast<Id>()->str().str(),rnd,decay),
            ann2ivalsel(call->args()[2]->cast<Id>()->str().str(),r0,r1,rnd), NULL //,
            //&varValPrint<BoolVar>
            );
        //branchInfo.add(bh,r0,r1,names);
      } 
      else if (flatAnn[i]->isa<Call>() && flatAnn[i]->cast<Call>()->id().str() == "int_default_search") {
        Call* call = flatAnn[i]->dyn_cast<Call>();
        def_int_varsel = ann2ivarsel(call->args()[0]->cast<Id>()->str().str(),rnd,decay);
        std::string r0;                                                    
        def_int_valsel = ann2ivalsel(call->args()[1]->cast<Id>()->str().str(),r0,r0,rnd);
      } 
      else if (flatAnn[i]->isa<Call>() && flatAnn[i]->cast<Call>()->id().str() == "bool_default_search") {
        Call* call = flatAnn[i]->dyn_cast<Call>();
        def_bool_varsel = ann2ivarsel(call->args()[0]->cast<Id>()->str().str(),rnd,decay);
        std::string r0;
        def_bool_valsel = ann2ivalsel(call->args()[1]->cast<Id>()->str().str(),r0,r0,rnd);
      } 
      else if (flatAnn[i]->isa<Call>() && flatAnn[i]->cast<Call>()->id().str() == "set_search") {
#ifdef GECODE_HAS_SET_VARS
        Call* call = flatAnn[i]->dyn_cast<Call>();
        ArrayLit* vars = arg2arraylit(call->args()[0]);
        int k=vars->v().size();
        for (int i=vars->v().size(); i--;)
            if (!(vars->v()[i])->type().is_set() || !(vars->v()[i])->type().isvar())
                k--;
        SetVarArgs va(k);
        k=0;
        std::vector<std::string> names;
        for (unsigned int i=0; i<vars->v().size(); i++) {
          if (!(vars->v()[i])->type().is_set() || !(vars->v()[i])->type().isvar())
            continue;
          int idx = resolveVar(getVarDecl(vars->v()[i])).index();
          va[k++] = _current_space->sv[idx];
          sv_searched[idx] = true;
          names.push_back(getVarDecl(vars->v()[i])->id()->str().str());
        }
        std::string r0, r1;
        //BrancherHandle bh =
        branch(*_current_space, va, 
            ann2svarsel(call->args()[1]->cast<Id>()->str().str(),rnd,decay), 
            ann2svalsel(call->args()[2]->cast<Id>()->str().str(),r0,r1,rnd),
            NULL//,
            //&varValPrint<SetVar>
            );
        //branchInfo.add(bh,r0,r1,names);
#else
        if (!ignoreUnknown) {
            err << "Warning, ignored search annotation: ";
            flatAnn[i]->print(err);
            err << std::endl;
        }
#endif
      } 
      else if (flatAnn[i]->isa<Call>() && flatAnn[i]->cast<Call>()->id().str() == "set_default_search") {        
#ifdef GECODE_HAS_SET_VARS
        Call* call = flatAnn[i]->dyn_cast<Call>();
        def_set_varsel = ann2svarsel(call->args()[0]->cast<Id>()->str().str(),rnd,decay);
        std::string r0;
        def_set_valsel = ann2svalsel(call->args()[1]->cast<Id>()->str().str(),r0,r0,rnd);
#else
        if (!ignoreUnknown) {
            err << "Warning, ignored search annotation: ";
            flatAnn[i]->print(err);
            err << std::endl;
        }
#endif
        } 
      else if (flatAnn[i]->isa<Call>() && flatAnn[i]->cast<Call>()->id().str() == "float_default_search") {
#ifdef GECODE_HAS_FLOAT_VARS
        Call* call = flatAnn[i]->dyn_cast<Call>();
        def_float_varsel = ann2fvarsel(call->args()[0]->cast<Id>()->str().str(),rnd,decay);
        std::string r0;
        def_float_valsel = ann2fvalsel(call->args()[1]->cast<Id>()->str().str(),r0,r0);
#else
        if (!ignoreUnknown) {
            err << "Warning, ignored search annotation: float_default_search" << std::endl;
        }
#endif
      } else if (flatAnn[i]->isa<Call>() && flatAnn[i]->cast<Call>()->id().str() == "float_search") {
#ifdef GECODE_HAS_FLOAT_VARS
        Call* call = flatAnn[i]->dyn_cast<Call>();
        ArrayLit* vars = call->args()[0]->cast<ArrayLit>();
        int k=vars->v().size();
        for (int i=vars->v().size(); i--;)
            if (!(vars->v()[i])->type().isvarfloat())
                k--;
        FloatVarArgs va(k);
        k=0;
        std::vector<std::string> names;
        for (unsigned int i=0; i<vars->v().size(); i++) {
            if (!(vars->v()[i])->type().isvarfloat())
              continue;
            int idx = resolveVar(getVarDecl(vars->v()[i])).index();
            va[k++] = _current_space->fv[idx];
            fv_searched[idx] = true;
            names.push_back(getVarDecl(vars->v()[i])->id()->str().str());
        }
        std::string r0, r1;
        //BrancherHandle bh =
        branch(*_current_space, va,
            ann2fvarsel(call->args()[2]->cast<Id>()->str().str(),rnd,decay), 
            ann2fvalsel(call->args()[3]->cast<Id>()->str().str(),r0,r1),
            NULL//,
            //&varValPrintF
            );
        //branchInfo.add(bh,r0,r1,names);
#else
        if (!ignoreUnknown) {
            err << "Warning, ignored search annotation: float_search" << std::endl;
        }
#endif
      } else { 
          if (!ignoreUnknown) {
              err << "Warning, ignored search annotation: " << *flatAnn[i] << std::endl;
          }
      }
    } // end for all annotations  
  }

  void
  GecodeSolverInstance::createBranchers(Annotation& ann, Expression* additionalAnn,
                                        int seed, double decay, bool ignoreUnknown,
                                        std::ostream& err) {
    // default search heuristics
    Rnd rnd(static_cast<unsigned int>(seed));
    TieBreak<IntVarBranch> def_int_varsel = INT_VAR_AFC_SIZE_MAX(0.99);
    IntValBranch def_int_valsel = INT_VAL_MIN();
    TieBreak<IntVarBranch> def_bool_varsel = INT_VAR_AFC_MAX(0.99);
    IntValBranch def_bool_valsel = INT_VAL_MIN();
#ifdef GECODE_HAS_SET_VARS
    SetVarBranch def_set_varsel = SET_VAR_AFC_SIZE_MAX(0.99);
    SetValBranch def_set_valsel = SET_VAL_MIN_INC();
#endif
#ifdef GECODE_HAS_FLOAT_VARS
    TieBreak<FloatVarBranch> def_float_varsel = FLOAT_VAR_SIZE_MIN();
    FloatValBranch def_float_valsel = FLOAT_VAL_SPLIT_MIN();
#endif

    std::vector<bool> iv_searched(_current_space->iv.size());
    for (unsigned int i=_current_space->iv.size(); i--;)
      iv_searched[i] = false;
    std::vector<bool> bv_searched(_current_space->bv.size());
    for (unsigned int i=_current_space->bv.size(); i--;)
      bv_searched[i] = false;
#ifdef GECODE_HAS_SET_VARS
    std::vector<bool> sv_searched(_current_space->sv.size());
    for (unsigned int i=_current_space->sv.size(); i--;)
      sv_searched[i] = false;
#endif
#ifdef GECODE_HAS_FLOAT_VARS
    std::vector<bool> fv_searched(_current_space->fv.size());
    for (unsigned int i=_current_space->fv.size(); i--;)
      fv_searched[i] = false;
#endif

    // solving annotations
    std::vector<Expression*> flatAnn;
    if (!ann.isEmpty()) {
      flattenSearchAnnotations(ann, flatAnn);
    }
    if (additionalAnn != NULL) {
      flatAnn.push_back(additionalAnn);
    }
    if (flatAnn.size() > 0) {
      setSearchStrategyFromAnnotation(flatAnn, iv_searched, bv_searched,sv_searched,fv_searched,
                                      def_int_varsel,def_int_valsel,def_bool_varsel,def_bool_valsel,
                              #ifdef GECODE_HAS_SET_VARS
                                      def_set_varsel,def_set_valsel,
                              #endif
                              #ifdef GECODE_HAS_FLOAT_VARS
                                      def_float_varsel,def_float_valsel,
                              #endif
                                      rnd,decay,ignoreUnknown,err
                                      );
    }

    int introduced = 0;
    int funcdep = 0;
    int searched = 0;
    for (int i=_current_space->iv.size(); i--;) {
      if (iv_searched[i]) {
        searched++;
      } else if (_current_space->iv_introduced[i]) {
          if (_current_space->iv_defined[i]) {
            funcdep++;
          } else {
            introduced++;
          }
      }
    }
    IntVarArgs iv_sol(_current_space->iv.size()-(introduced+funcdep+searched));
    IntVarArgs iv_tmp(introduced);
    for (unsigned int i=_current_space->iv.size(), j=0, k=0; i--;) {
      if (iv_searched[i])
        continue;
      if(_current_space->iv_introduced[i]) {
        if(_current_space->iv_introduced.size() >= i) {
          if (!_current_space->iv_defined[i]) {
            iv_tmp[j++] = _current_space->iv[i];
          }
        }
      } else {
          iv_sol[k++] = _current_space->iv[i];
      }
    }
    // Collecting Boolean variables
    introduced = 0;
    funcdep = 0;
    searched = 0;
    for (int i=_current_space->bv.size(); i--;) {
      if (bv_searched[i]) {
        searched++;
      } else if (_current_space->bv_introduced[i]) {
        if (_current_space->bv_defined[i]) {
          funcdep++;
        } else {
            introduced++;
        }
      }
    }
    BoolVarArgs bv_sol(_current_space->bv.size()-(introduced+funcdep+searched));
    BoolVarArgs bv_tmp(introduced);
    for (int i=_current_space->bv.size(), j=0, k=0; i--;) {
      if (bv_searched[i])
        continue;
      if (_current_space->bv_introduced[i]) {
        if (!_current_space->bv_defined[i]) {
            bv_tmp[j++] = _current_space->bv[i];
        }
      } else {
          bv_sol[k++] = _current_space->bv[i];
      }
    }

    if (iv_sol.size() > 0)
      branch(*this->_current_space, iv_sol, def_int_varsel, def_int_valsel);
    if (bv_sol.size() > 0)
      branch(*this->_current_space, bv_sol, def_bool_varsel, def_bool_valsel);

    //std::cout << "DEBUG: branched over " << iv_sol.size()  << " integer variables."<< std::endl;
    //std::cout << "DEBUG: branched over " << bv_sol.size()  << " Boolean variables."<< std::endl;
#ifdef GECODE_HAS_FLOAT_VARS
    introduced = 0;
    funcdep = 0;
    searched = 0;
    for (int i=_current_space->fv.size(); i--;) {
      if (fv_searched[i]) {
        searched++;
      } else if (_current_space->fv_introduced[i]) {
        if (_current_space->fv_defined[i]) {
          funcdep++;
        } else {
          introduced++;
        }
      }
    }
    FloatVarArgs fv_sol(_current_space->fv.size()-(introduced+funcdep+searched));
    FloatVarArgs fv_tmp(introduced);
    for (int i=_current_space->fv.size(), j=0, k=0; i--;) {
      if (fv_searched[i])
        continue;
      if (_current_space->fv_introduced[i]) {
        if (!_current_space->fv_defined[i]) {
          fv_tmp[j++] = _current_space->fv[i];
        }
      } else {
        fv_sol[k++] = _current_space->fv[i];
      }
    }

    if (fv_sol.size() > 0)
      branch(*this->_current_space, fv_sol, def_float_varsel, def_float_valsel);
#endif
#ifdef GECODE_HAS_SET_VARS
    introduced = 0;
    funcdep = 0;
    searched = 0;
    for (int i=_current_space->sv.size(); i--;) {
      if (sv_searched[i]) {
          searched++;
      } else if (_current_space->sv_introduced[i]) {
          if (_current_space->sv_defined[i]) {
              funcdep++;
          } else {
              introduced++;
          }
      }
    }
    SetVarArgs sv_sol(_current_space->sv.size()-(introduced+funcdep+searched));
    SetVarArgs sv_tmp(introduced);
    for (int i=_current_space->sv.size(), j=0, k=0; i--;) {
      if (sv_searched[i])
          continue;
      if (_current_space->sv_introduced[i]) {
          if (!_current_space->sv_defined[i]) {
              sv_tmp[j++] = _current_space->sv[i];
          }
      } else {
          sv_sol[k++] = _current_space->sv[i];
      }
    }

    if (sv_sol.size() > 0)
      branch(*this->_current_space, sv_sol, def_set_varsel, def_set_valsel);
#endif

    // branching on auxiliary variables
    _current_space->iv_aux = IntVarArray(*this->_current_space, iv_tmp);
    _current_space->bv_aux = BoolVarArray(*this->_current_space, bv_tmp);
    int n_aux = _current_space->iv_aux.size() + _current_space->bv_aux.size();
#ifdef GECODE_HAS_SET_VARS
    _current_space->sv_aux = SetVarArray(*this->_current_space, sv_tmp);
    n_aux += _current_space->sv_aux.size();
#endif
#ifdef GECODE_HAS_FLOAT_VARS
    _current_space->fv_aux = FloatVarArray(*this->_current_space, fv_tmp);
    n_aux += _current_space->fv_aux.size();
#endif
    if (n_aux > 0) {
      AuxVarBrancher::post(*this->_current_space, def_int_varsel, def_int_valsel,
                          def_bool_varsel, def_bool_valsel
#ifdef GECODE_HAS_SET_VARS
                        , def_set_varsel, def_set_valsel
#endif
#ifdef GECODE_HAS_FLOAT_VARS
                        , def_float_varsel, def_float_valsel
#endif
                    ); // end post
      //std::cout << "DEBUG: Posted aux-var-brancher for " << n_aux << " aux-variables" << std::endl;
    } // end if n_aux > 0
    //else
      //std::cout << "DEBUG: No aux vars to branch on." << std::endl;
  }

 
  TieBreak<IntVarBranch>
  GecodeSolverInstance::ann2ivarsel(std::string s, Rnd& rnd, double decay) {
    if (s == "input_order")
        return TieBreak<IntVarBranch>(INT_VAR_NONE());
    if (s == "first_fail")
        return TieBreak<IntVarBranch>(INT_VAR_SIZE_MIN());
    if (s == "anti_first_fail")
        return TieBreak<IntVarBranch>(INT_VAR_SIZE_MAX());
    if (s == "smallest")
        return TieBreak<IntVarBranch>(INT_VAR_MIN_MIN());
    if (s == "largest")
        return TieBreak<IntVarBranch>(INT_VAR_MAX_MAX());
    if (s == "occurrence")
        return TieBreak<IntVarBranch>(INT_VAR_DEGREE_MAX());
    if (s == "max_regret")
        return TieBreak<IntVarBranch>(INT_VAR_REGRET_MIN_MAX());
    if (s == "most_constrained")
        return TieBreak<IntVarBranch>(INT_VAR_SIZE_MIN(),
                INT_VAR_DEGREE_MAX());
    if (s == "random") {
        return TieBreak<IntVarBranch>(INT_VAR_RND(rnd));
    }
    if (s == "afc_min")
        return TieBreak<IntVarBranch>(INT_VAR_AFC_MIN(decay));
    if (s == "afc_max")
        return TieBreak<IntVarBranch>(INT_VAR_AFC_MAX(decay));
    if (s == "afc_size_min")
        return TieBreak<IntVarBranch>(INT_VAR_AFC_SIZE_MIN(decay));
    if (s == "afc_size_max" || s=="dom_w_deg") {
        return TieBreak<IntVarBranch>(INT_VAR_AFC_SIZE_MAX(decay));
    }
    if (s == "activity_min")
        return TieBreak<IntVarBranch>(INT_VAR_ACTIVITY_MIN(decay));
    if (s == "activity_max")
        return TieBreak<IntVarBranch>(INT_VAR_ACTIVITY_MAX(decay));
    if (s == "activity_size_min")
        return TieBreak<IntVarBranch>(INT_VAR_ACTIVITY_SIZE_MIN(decay));
    if (s == "activity_size_max")
        return TieBreak<IntVarBranch>(INT_VAR_ACTIVITY_SIZE_MAX(decay));
    std::cerr << "Warning, ignored search annotation: " << s << std::endl;
    return TieBreak<IntVarBranch>(INT_VAR_NONE());
  }

  Gecode::IntValBranch 
  GecodeSolverInstance::ann2ivalsel(std::string s, std::string& r0, std::string& r1, Rnd& rnd) {
    if (s == "indomain_min") {
        r0 = "="; r1 = "!=";
        return INT_VAL_MIN();
    }
    if (s == "indomain_max") {
        r0 = "="; r1 = "!=";
        return INT_VAL_MAX();
    }
    if (s == "indomain_median") {
        r0 = "="; r1 = "!=";
        return INT_VAL_MED();
    }
    if (s == "indomain_split") {
        r0 = "<="; r1 = ">";
        return INT_VAL_SPLIT_MIN();
    }
    if (s == "indomain_reverse_split") {
        r0 = ">"; r1 = "<=";
        return INT_VAL_SPLIT_MAX();
    }
    if (s == "indomain_random") {
        r0 = "="; r1 = "!=";
        return INT_VAL_RND(rnd);
    }
    if (s == "indomain") {
        r0 = "="; r1 = "=";       
        return INT_VALUES_MIN();
    }
    if (s == "indomain_middle") {
        std::cerr << "Warning, replacing unsupported annotation "
            << "indomain_middle with indomain_median" << std::endl;
        r0 = "="; r1 = "!=";
        return INT_VAL_MED();
    }
    if (s == "indomain_interval") {
        std::cerr << "Warning, replacing unsupported annotation "
            << "indomain_interval with indomain_split" << std::endl;
        r0 = "<="; r1 = ">";
        return INT_VAL_SPLIT_MIN();
    }
    std::cerr << "Warning, ignored search annotation: " << s << std::endl;
    r0 = "="; r1 = "!=";
    return INT_VAL_MIN();
  }
  
  IntAssign 
  GecodeSolverInstance::ann2asnivalsel(std::string s, Rnd& rnd) {
    if (s == "indomain_min")
        return INT_ASSIGN_MIN();
    if (s == "indomain_max")
        return INT_ASSIGN_MAX();
    if (s == "indomain_median")
        return INT_ASSIGN_MED();
    if (s == "indomain_random") {
        return INT_ASSIGN_RND(rnd);
    }
    std::cerr << "Warning, ignored search annotation: " << s << std::endl;
    return INT_ASSIGN_MIN();
  }

  #ifdef GECODE_HAS_SET_VARS
    SetVarBranch 
    GecodeSolverInstance::ann2svarsel(std::string s, Rnd& rnd, double decay) {
        if (s == "input_order")
            return SET_VAR_NONE();
        if (s == "first_fail")
            return SET_VAR_SIZE_MIN();
        if (s == "anti_first_fail")
            return SET_VAR_SIZE_MAX();
        if (s == "smallest")
            return SET_VAR_MIN_MIN();
        if (s == "largest")
            return SET_VAR_MAX_MAX();
        if (s == "afc_min")
            return SET_VAR_AFC_MIN(decay);
        if (s == "afc_max")
            return SET_VAR_AFC_MAX(decay);
        if (s == "afc_size_min")
            return SET_VAR_AFC_SIZE_MIN(decay);
        if (s == "afc_size_max")
            return SET_VAR_AFC_SIZE_MAX(decay);
        if (s == "activity_min")
            return SET_VAR_ACTIVITY_MIN(decay);
        if (s == "activity_max")
            return SET_VAR_ACTIVITY_MAX(decay);
        if (s == "activity_size_min")
            return SET_VAR_ACTIVITY_SIZE_MIN(decay);
        if (s == "activity_size_max")
            return SET_VAR_ACTIVITY_SIZE_MAX(decay);
        if (s == "random") {
            return SET_VAR_RND(rnd);
        }
        std::cerr << "Warning, ignored search annotation: " << s << std::endl;
        return SET_VAR_NONE();
    }

    SetValBranch 
    GecodeSolverInstance::ann2svalsel(std::string s, std::string r0, std::string r1, Rnd& rnd) {
        (void) rnd;
        if (s == "indomain_min") {
            r0 = "in"; r1 = "not in";
            return SET_VAL_MIN_INC();
        }
        if (s == "indomain_max") {
            r0 = "in"; r1 = "not in";
            return SET_VAL_MAX_INC();
        }
        if (s == "outdomain_min") {
            r1 = "in"; r0 = "not in";
            return SET_VAL_MIN_EXC();
        }
        if (s == "outdomain_max") {
            r1 = "in"; r0 = "not in";
            return SET_VAL_MAX_EXC();
        }
        std::cerr << "Warning, ignored search annotation: " << s << std::endl;
        r0 = "in"; r1 = "not in";
        return SET_VAL_MIN_INC();
    }
#endif

#ifdef GECODE_HAS_FLOAT_VARS
    TieBreak<FloatVarBranch> 
    GecodeSolverInstance::ann2fvarsel(std::string s, Rnd& rnd, double decay) {
        if (s == "input_order")
            return TieBreak<FloatVarBranch>(FLOAT_VAR_NONE());
        if (s == "first_fail")
            return TieBreak<FloatVarBranch>(FLOAT_VAR_SIZE_MIN());
        if (s == "anti_first_fail")
            return TieBreak<FloatVarBranch>(FLOAT_VAR_SIZE_MAX());
        if (s == "smallest")
            return TieBreak<FloatVarBranch>(FLOAT_VAR_MIN_MIN());
        if (s == "largest")
            return TieBreak<FloatVarBranch>(FLOAT_VAR_MAX_MAX());
        if (s == "occurrence")
            return TieBreak<FloatVarBranch>(FLOAT_VAR_DEGREE_MAX());
        if (s == "most_constrained")
            return TieBreak<FloatVarBranch>(FLOAT_VAR_SIZE_MIN(),
                    FLOAT_VAR_DEGREE_MAX());
        if (s == "random") {
            return TieBreak<FloatVarBranch>(FLOAT_VAR_RND(rnd));
        }
        if (s == "afc_min")
            return TieBreak<FloatVarBranch>(FLOAT_VAR_AFC_MIN(decay));
        if (s == "afc_max")
            return TieBreak<FloatVarBranch>(FLOAT_VAR_AFC_MAX(decay));
        if (s == "afc_size_min")
            return TieBreak<FloatVarBranch>(FLOAT_VAR_AFC_SIZE_MIN(decay));
        if (s == "afc_size_max")
            return TieBreak<FloatVarBranch>(FLOAT_VAR_AFC_SIZE_MAX(decay));
        if (s == "activity_min")
            return TieBreak<FloatVarBranch>(FLOAT_VAR_ACTIVITY_MIN(decay));
        if (s == "activity_max")
            return TieBreak<FloatVarBranch>(FLOAT_VAR_ACTIVITY_MAX(decay));
        if (s == "activity_size_min")
            return TieBreak<FloatVarBranch>(FLOAT_VAR_ACTIVITY_SIZE_MIN(decay));
        if (s == "activity_size_max")
            return TieBreak<FloatVarBranch>(FLOAT_VAR_ACTIVITY_SIZE_MAX(decay));
        std::cerr << "Warning, ignored search annotation: " << s << std::endl;
        return TieBreak<FloatVarBranch>(FLOAT_VAR_NONE());
    }

    FloatValBranch 
    GecodeSolverInstance::ann2fvalsel(std::string s, std::string r0, std::string r1) {
        if (s == "indomain_split") {
            r0 = "<="; r1 = ">";
            return FLOAT_VAL_SPLIT_MIN();
        }
        if (s == "indomain_reverse_split") {
            r1 = "<="; r0 = ">";
            return FLOAT_VAL_SPLIT_MAX();
        }
        std::cerr << "Warning, ignored search annotation: " << s << std::endl;
        r0 = "<="; r1 = ">";
        return FLOAT_VAL_SPLIT_MIN();
    }
#endif
  }
