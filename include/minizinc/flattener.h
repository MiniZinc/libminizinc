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

using namespace std;

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

namespace MiniZinc {

  class Flattener;
  Flattener* getGlobalFlattener(bool fOutputByDefault=true);
  void cleanupGlobalFlattener(Flattener*);

  class Flattener {
  private:
    unique_ptr<Env>   pEnv;
  public:
    Flattener(bool fOutputByDefault=true);
    virtual ~Flattener();
    virtual bool processOption(int& i, const int argc, const char** argv);
    virtual void printVersion(ostream& );
    virtual void printHelp(ostream& );

    virtual void flatten();
    virtual void printStatistics(ostream& );

    virtual void set_flag_verbose(bool f) { flag_verbose = f; }
    virtual bool get_flag_verbose() const { return flag_verbose; }
    virtual void set_flag_statistics(bool f) { flag_statistics = f; }
    virtual bool get_flag_statistics() const { return flag_statistics; }
    virtual Env* getEnv() const { assert(pEnv.get()); return pEnv.get(); }
    
    SolverInstance::Status status = SolverInstance::UNKNOWN;

  private:

    bool fOutputByDefault = true;      // if the class is used in mzn2fzn, write .fzn+.ozn by default
    vector<string> filenames;
    vector<string> datafiles;
    vector<string> includePaths;
    bool is_flatzinc = false;

    bool flag_ignoreStdlib = false;
    bool flag_typecheck = true;
    bool flag_verbose = false;
    bool flag_newfzn = false;
    bool flag_optimize = true;
    bool flag_werror = false;
    bool flag_only_range_domains = false;
    bool flag_noMIPdomains = false;
    bool flag_no_presolve = false;
    bool flag_statistics = false;
    bool flag_stdinInput = false;

    string std_lib_dir;
    string globals_dir;

    bool flag_no_output_ozn = false;
    string flag_output_base;
    string flag_output_fzn;
    string flag_output_ozn;
    bool flag_output_fzn_stdout = false;
    bool flag_output_ozn_stdout = false;
    bool flag_instance_check_only = false;
    FlatteningOptions fopts;

    clock_t starttime01;
    clock_t lasttime;

  };

}

#endif  // __MINIZINC_FLATTENER_H__
