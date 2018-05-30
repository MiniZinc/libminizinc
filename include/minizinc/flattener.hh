/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_FLATTENER_H__
#define __MINIZINC_FLATTENER_H__

#include <string>
#include <vector>
#include <ctime>
#include <memory>
#include <iomanip>

#include <minizinc/model.hh>
#include <minizinc/parser.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/astexception.hh>

#include <minizinc/flatten.hh>
#include <minizinc/flatten_internal.hh>  // temp., TODO
#include <minizinc/MIPdomains.hh>
#include <minizinc/optimize.hh>
#include <minizinc/builtins.hh>
#include <minizinc/utils.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/solver_instance.hh>
#include <minizinc/options.hh>

#include <minizinc/passes/compile_pass.hh>
#ifdef HAS_GECODE
#include <minizinc/passes/gecode_pass.hh>
#endif

namespace MiniZinc {
  
  class Flattener {
  private:
    std::unique_ptr<Env>   pEnv;
    std::ostream& os;
    std::ostream& log;
  public:
    Flattener(std::ostream& os, std::ostream& log, const std::string& stdlibDir);
    ~Flattener();
    bool processOption(int& i, std::vector<std::string>& argv);
    void printVersion(std::ostream& );
    void printHelp(std::ostream& );

    void flatten(const std::string& modelString = std::string());
    void printStatistics(std::ostream& );
    
    void set_flag_verbose(bool f) { flag_verbose = f; }
    bool get_flag_verbose() const { return flag_verbose; }
    void set_flag_statistics(bool f) { flag_statistics = f; }
    bool get_flag_statistics() const { return flag_statistics; }
    void set_flag_output_by_default(bool f) { fOutputByDefault = f; }
    Env* getEnv() const { assert(pEnv.get()); return pEnv.get(); }
    bool hasInputFiles(void) const { return !filenames.empty() || flag_stdinInput || !flag_solution_check_model.empty(); }
    
    SolverInstance::Status status = SolverInstance::UNKNOWN;
    
  private:
    Env* multiPassFlatten(const std::vector<std::unique_ptr<Pass> >& passes);

    bool fOutputByDefault = false;      // if the class is used in mzn2fzn, write .fzn+.ozn by default
    std::vector<std::string> filenames;
    std::vector<std::string> datafiles;
    std::vector<std::string> includePaths;
    bool is_flatzinc = false;

    bool flag_ignoreStdlib = false;
    bool flag_typecheck = true;
    bool flag_verbose = false;
    bool flag_newfzn = false;
    bool flag_optimize = true;
    bool flag_werror = false;
    bool flag_only_range_domains = false;
    bool flag_allow_unbounded_vars = false;
    bool flag_noMIPdomains = false;
    int  opt_MIPDmaxIntvEE = 0;
    double opt_MIPDmaxDensEE = 0.0;
    bool flag_statistics = false;
    bool flag_stdinInput = false;
    bool flag_allow_multi_assign = false;

    bool flag_gecode = false;
    bool flag_two_pass = false;
    bool flag_sac = false;
    bool flag_shave = false;
    unsigned int flag_pre_passes = 1;

    std::string std_lib_dir;
    std::string globals_dir;

    bool flag_no_output_ozn = false;
    std::string flag_output_base;
    std::string flag_output_fzn;
    std::string flag_output_ozn;
    std::string flag_output_paths;
    bool flag_keep_mzn_paths = false;
    bool flag_output_fzn_stdout = false;
    bool flag_output_ozn_stdout = false;
    bool flag_output_paths_stdout = false;
    bool flag_instance_check_only = false;
    bool flag_model_check_only = false;
    bool flag_model_interface_only = false;
    FlatteningOptions::OutputMode flag_output_mode = FlatteningOptions::OUTPUT_ITEM;
    bool flag_output_objective = false;
    std::string flag_solution_check_model;
    bool flag_compile_solution_check_model = false;
    FlatteningOptions fopts;

    clock_t starttime01;
    clock_t lasttime;

  };

}

#endif  // __MINIZINC_FLATTENER_H__

