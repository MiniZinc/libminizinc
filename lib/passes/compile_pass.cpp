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

#include <iomanip>

// temporary
#include <minizinc/prettyprinter.hh>

#include <fstream>

namespace MiniZinc {

  std::string stoptime(Timer& start) {
    std::ostringstream oss;
    oss << std::setprecision(0) << std::fixed << start.ms() << " ms";
    start.reset();
    return oss.str();
  }
  
  IncludeI* update_include(Model* parent, IncludeI* inc, std::vector<std::string>& includes, bool verbose=false) {
    std::string filename = inc->f().str();
    std::vector<std::string> datafiles;

    std::string parentPath = parent->filepath().str();
    parentPath.erase(std::find(parentPath.rbegin(), parentPath.rend(), '/').base(), parentPath.end());

    std::stringstream full_filename;

    if(parentPath.empty())
      full_filename << filename;
    else
      full_filename << parentPath << "/" << filename;

    std::ifstream fi(full_filename.str());
    if(fi.is_open()) {
        std::vector<std::string> filenames {full_filename.str()};
        Model* inc_mod = parse(filenames, datafiles, includes, true, true, verbose, std::cerr);
        IncludeI* new_inc = new IncludeI(inc->loc(), filename);
        new_inc->m(inc_mod);
        inc_mod->setParent(parent);
        return new_inc;
    } else {
      for(unsigned int i=0; i<includes.size(); i++) {
        full_filename.str(std::string());
        std::string path = includes[i];
        full_filename << path << '/' << filename;
        std::ifstream fi(full_filename.str());
        if(fi.is_open()) {
          std::vector<std::string> filenames {full_filename.str()};
          Model* inc_mod = parse(filenames, datafiles, includes, true, true, verbose, std::cerr);
          IncludeI* new_inc = new IncludeI(inc->loc(), filename);
          new_inc->m(inc_mod);
          inc_mod->setParent(parent);
          return new_inc;
        }
      }
    }
    return NULL;
  }

  Env* changeLibrary(Env& e, std::vector<std::string>& includePaths, std::string globals_dir, bool verbose=false) {
    GC::lock();
    CopyMap cm;
    Model* m = e.model();
    Model* new_mod = new Model();
    new_mod->setFilename(m->filename().str());
    new_mod->setFilepath(m->filepath().str());

    std::vector<std::string> new_includePaths;

    if(std::find(includePaths.begin(), includePaths.end(), globals_dir) == includePaths.end())
      new_includePaths.push_back(globals_dir);
    new_includePaths.insert(new_includePaths.end(), includePaths.begin(), includePaths.end());

    for(Item* item : *m) {
      if(IncludeI* inc = item->dyn_cast<IncludeI>()) {
        IncludeI* ninc = update_include(new_mod, inc, new_includePaths, verbose);
        if(ninc) new_mod->addItem(ninc);
      } else {
        new_mod->addItem(copy(e.envi(),cm,item));
      }
    }

    Env* fenv = new Env(new_mod);
    std::vector<TypeError> typeErrors;
    MiniZinc::typecheck(*fenv, new_mod, typeErrors);
    if (typeErrors.size() > 0) {
      for (unsigned int i=0; i<typeErrors.size(); i++) {
        std::cerr << std::endl;
        std::cerr << typeErrors[i].what() << ": " << typeErrors[i].msg() << std::endl;
        std::cerr << typeErrors[i].loc() << std::endl;
      }
      exit(EXIT_FAILURE);
    }
    registerBuiltins(*fenv, new_mod);

    fenv->envi().setMaps(e.envi());

    GC::unlock();

    return fenv;
  }

  CompilePass::CompilePass(Env* e,
                           FlatteningOptions& opts,
                           std::string globals_library,
                           std::vector<std::string> include_paths,
                           bool change_lib = true) :
    env(e),
    fopts(opts),
    library(globals_library),
    includePaths(include_paths),
    change_library(change_lib) {
  }

  bool CompilePass::pre(Env* env) {
    return change_library || env->flat()->size() == 0;
  }

  Env* CompilePass::run(Env* store) {
    Timer lasttime;
    if(fopts.verbose)
      std::cerr << "\n\tCompilePass: Flatten with \'" << library << "\' library ...\n";

    Env* new_env;
    if(change_library) {
      new_env = changeLibrary(*env, includePaths, library, fopts.verbose);

      new_env->envi().passes = store->envi().passes;
      new_env->envi().maxPathDepth = store->envi().maxPathDepth;
      new_env->envi().pass = store->envi().pass;
      new_env->envi().setMaps(store->envi());
    } else {
      new_env = env;
    }

    flatten(*new_env, fopts);
    MIPdomains(*new_env, fopts.verbose);
    optimize(*new_env);
    oldflatzinc(*new_env);

    if(fopts.verbose)
      std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;

    return new_env;
  }

  CompilePass::~CompilePass() {};

  Env* multiPassFlatten(Env& e, std::vector<Pass*>& passes) {
    Env* pre_env = &e;
    pre_env->envi().passes = passes.size();
    Timer lasttime;
    bool verbose = false;
    for(unsigned int i=0; i<passes.size(); i++) {
      pre_env->envi().pass = i;
      if(verbose)
        std::cerr << "Start pass " << i << ":\n";

      if(passes[i]->pre(pre_env)) {

        pre_env = passes[i]->run(pre_env);

        if(verbose)
          std::cerr << "Finish pass " << i << ": " << stoptime(lasttime) << "\n";
      }
    }

    return pre_env;
  }
}

