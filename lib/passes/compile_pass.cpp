/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Kevin Leo <kevin.leo@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/passes/compile_pass.hh>

#include <minizinc/flatten.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/copy.hh>
#include <minizinc/hash.hh>
#include <minizinc/astexception.hh>
#include <minizinc/optimize.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/parser.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/builtins.hh>
#include <minizinc/stl_map_set.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/timer.hh>

#include <minizinc/MIPdomains.hh>

#include <fstream>

namespace MiniZinc {

  using std::string;
  using std::vector;

  inline
  IncludeI* parse_include(Env& e, Model* parent, const IncludeI* inc, const vector<string>& includes, bool verbose,
                          const vector<string>& datafiles, const string& filename) {
    vector<string> filenames {filename};
    Model* inc_mod = parse(e, filenames, datafiles, includes, true, true, verbose, std::cerr);
    IncludeI* new_inc = new IncludeI(inc->loc(), filename);
    new_inc->m(inc_mod);
    inc_mod->setParent(parent);
    return new_inc;
  }

  IncludeI* update_include(Env& e, Model* parent, const IncludeI* inc, const vector<string>& includes, bool verbose=false) {
    string filename = inc->f().str();
    vector<string> datafiles;

    string parentPath = parent->filepath().str();
    parentPath.erase(std::find(parentPath.rbegin(), parentPath.rend(), '/').base(), parentPath.end());

    std::stringstream full_filename;
    full_filename << (!parentPath.empty() ? parentPath + "/" : "") << filename;

    std::ifstream fi(full_filename.str());
    if(fi.is_open()) return parse_include(e, parent, inc, includes, verbose, datafiles, full_filename.str());

    for(unsigned int i=0; i<includes.size(); i++) {
      full_filename.str("");
      string path = includes[i];
      full_filename << path << '/' << filename;
      std::ifstream fi(full_filename.str());
      if(fi.is_open()) return parse_include(e, parent, inc, includes, verbose, datafiles, full_filename.str());
    }
    return nullptr;
  }

  Env* changeLibrary(Env& e, vector<string>& includePaths, string globals_dir, CompilePassFlags& compflags, bool verbose=false) {
    GC::lock();
    CopyMap cm;
    Model* m = e.model();
    Model* new_mod = new Model();
    new_mod->setFilename(m->filename().str());
    new_mod->setFilepath(m->filepath().str());

    vector<string> new_includePaths;

    if(std::find(includePaths.begin(), includePaths.end(), globals_dir) == includePaths.end())
      new_includePaths.push_back(globals_dir);
    new_includePaths.insert(new_includePaths.end(), includePaths.begin(), includePaths.end());

    for(Item* item : *m) {
      if(IncludeI* inc = item->dyn_cast<IncludeI>()) {
        IncludeI* ninc = update_include(e, new_mod, inc, new_includePaths, verbose);
        if(ninc) new_mod->addItem(ninc);
      } else {
        new_mod->addItem(copy(e.envi(),cm,item));
      }
    }

    Env* fenv = new Env(new_mod);
    vector<TypeError> typeErrors;
    MiniZinc::typecheck(*fenv, new_mod, typeErrors, compflags.model_check_only || compflags.model_interface_only, compflags.allow_multi_assign);
    if (typeErrors.size() > 0) {
      for (unsigned int i=0; i<typeErrors.size(); i++) {
        std::cerr << std::endl;
        std::cerr << typeErrors[i].what() << ": " << typeErrors[i].msg() << std::endl;
        std::cerr << typeErrors[i].loc() << std::endl;
      }
      exit(EXIT_FAILURE);
    }
    registerBuiltins(*fenv, new_mod);

    GC::unlock();

    return fenv;
  }

  CompilePass::CompilePass(Env* e,
                           FlatteningOptions& opts,
                           CompilePassFlags& cflags,
                           string globals_library,
                           vector<string> include_paths,
                           bool change_lib = true) :
    env(e),
    fopts(opts),
    compflags(cflags),
    library(globals_library),
    includePaths(include_paths),
    change_library(change_lib) {
  }

  Env* CompilePass::run(Env* store) {
    Timer lasttime;
    if(compflags.verbose)
      std::cerr << "\n\tCompilePass: Flatten with \'" << library << "\' library ...\n";

    Env* new_env;
    if(change_library) {
      new_env = changeLibrary(*env, includePaths, library, compflags, compflags.verbose);
      new_env->envi().copyPathMapsAndState(store->envi());
    } else {
      new_env = env;
    }

    flatten(*new_env, fopts);

    if ( ! compflags.noMIPdomains ) {
      if (compflags.verbose)
        std::cerr << "MIP domains ...";
      MIPdomains(*new_env, compflags.statistics);
      if (compflags.verbose)
        std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
    }

    if (compflags.optimize) {
      if (compflags.verbose)
        std::cerr << "Optimizing ...";
      optimize(*new_env);
      for (unsigned int i=0; i<new_env->warnings().size(); i++) {
        std::cerr << (compflags.werror ? "\n  ERROR: " : "\n  WARNING: ") << new_env->warnings()[i];
      }
      if (compflags.werror && new_env->warnings().size() > 0) {
        exit(EXIT_FAILURE);
      }
      if (compflags.verbose)
        std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
    }

    if (!compflags.newfzn) {
      if (compflags.verbose)
        std::cerr << "Converting to old FlatZinc ...";
      oldflatzinc(*new_env);
      if (compflags.verbose)
        std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
    } else {
      new_env->flat()->compact();
      new_env->output()->compact();
    }

    if(compflags.verbose)
      std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;

    return new_env;
  }

  CompilePass::~CompilePass() {};

}

