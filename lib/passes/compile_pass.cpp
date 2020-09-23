/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Kevin Leo <kevin.leo@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/MIPdomains.hh>
#include <minizinc/astexception.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/builtins.hh>
#include <minizinc/copy.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/flatten.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/hash.hh>
#include <minizinc/optimize.hh>
#include <minizinc/parser.hh>
#include <minizinc/passes/compile_pass.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/timer.hh>
#include <minizinc/typecheck.hh>

#include <fstream>

namespace MiniZinc {

using std::string;
using std::vector;

Env* changeLibrary(Env& e, vector<string>& includePaths, string globals_dir,
                   CompilePassFlags& compflags, bool verbose = false) {
  GCLock lock;
  CopyMap cm;
  Model* m = e.envi().orig_model != nullptr ? e.envi().orig_model : e.envi().model;
  auto* new_mod = new Model();
  new_mod->setFilename(m->filename());
  new_mod->setFilepath(m->filepath());

  vector<string> new_includePaths;

  if (std::find(includePaths.begin(), includePaths.end(), globals_dir) == includePaths.end())
    new_includePaths.push_back(globals_dir);
  new_includePaths.insert(new_includePaths.end(), includePaths.begin(), includePaths.end());

  // Collect include items
  vector<ASTString> include_names;
  for (Item* item : *m) {
    if (auto* inc = item->dyn_cast<IncludeI>()) {
      include_names.push_back(inc->m()->filepath());
    } else {
      new_mod->addItem(copy(e.envi(), cm, item));
    }
  }

  std::stringstream ss;
  for (auto& name : include_names) {
    ss << "include \"" << Printer::escapeStringLit(name) << "\";";
  }

  vector<SyntaxError> syntax_errors;
  Env* fenv = new Env(new_mod);
  // Model* inc_mod = parse(*fenv, include_names, {}, new_includePaths, true, true, verbose,
  // std::cerr);
  std::ostringstream dummy_file;
  dummy_file << m->filepath() << "_Dummy.mzn";
  Model* inc_mod = parseFromString(*fenv, ss.str(), dummy_file.str(), new_includePaths, false,
                                   false, true, verbose, std::cerr, syntax_errors);
  if (inc_mod == nullptr) {
    for (const SyntaxError& se : syntax_errors) {
      std::cerr << std::endl;
      std::cerr << se.what() << ": " << se.msg() << std::endl;
      std::cerr << se.loc() << std::endl;
    }
    return nullptr;
  }
  auto* new_inc = new IncludeI(Location().introduce(), string("MultiPassDummy.mzn"));
  new_inc->m(inc_mod);
  inc_mod->setParent(new_mod);
  new_mod->addItem(new_inc);

  return fenv;
}

CompilePass::CompilePass(Env* e, FlatteningOptions& opts, CompilePassFlags& cflags,
                         string globals_library, vector<string> include_paths, bool change_lib,
                         bool ignore_unknown)
    : env(e),
      fopts(opts),
      compflags(cflags),
      library(globals_library),
      includePaths(include_paths),
      change_library(change_lib),
      ignore_unknown_ids(ignore_unknown) {}

Env* CompilePass::run(Env* store, std::ostream& log) {
  Timer lasttime;
  if (compflags.verbose) log << "\n\tCompilePass: Flatten with \'" << library << "\' library ...\n";

  Env* new_env;
  if (change_library) {
    new_env = changeLibrary(*env, includePaths, library, compflags, compflags.verbose);
    if (new_env == nullptr) return nullptr;
    new_env->envi().copyPathMapsAndState(store->envi());
  } else {
    new_env = env;
  }
  new_env->envi().ignoreUnknownIds = ignore_unknown_ids;

  vector<TypeError> typeErrors;
  MiniZinc::typecheck(*new_env, new_env->model(), typeErrors,
                      compflags.model_check_only || compflags.model_interface_only,
                      compflags.allow_multi_assign);
  if (typeErrors.size() > 0) {
    std::ostringstream errstream;
    for (auto& typeError : typeErrors) {
      errstream << typeError.what() << ": " << typeError.msg() << std::endl;
      errstream << typeError.loc() << std::endl;
    }
    throw Error(errstream.str());
  }

  registerBuiltins(*new_env);

  try {
    flatten(*new_env, fopts);
  } catch (LocationException& e) {
    if (compflags.verbose) log << std::endl;
    std::ostringstream errstream;
    errstream << e.what() << ": " << std::endl;
    new_env->dumpErrorStack(errstream);
    errstream << "  " << e.msg() << std::endl;
    throw Error(errstream.str());
  }

  if (!compflags.noMIPdomains) {
    if (compflags.verbose) log << "MIP domains ...";
    MIPdomains(*new_env, compflags.statistics);
    if (compflags.verbose) log << " done (" << lasttime.stoptime() << ")" << std::endl;
  }

  if (compflags.optimize) {
    if (compflags.verbose) log << "Optimizing ...";
    optimize(*new_env, compflags.chain_compression);
    if (compflags.verbose) log << " done (" << lasttime.stoptime() << ")" << std::endl;
  }

  for (const auto& i : new_env->warnings()) {
    log << (compflags.werror ? "\n  ERROR: " : "\n  WARNING: ") << i;
  }
  if (compflags.werror && new_env->warnings().size() > 0) {
    throw Error("errors encountered");
  }
  new_env->clearWarnings();

  if (!compflags.newfzn) {
    if (compflags.verbose) log << "Converting to old FlatZinc ...";
    oldflatzinc(*new_env);
    if (compflags.verbose) log << " done (" << lasttime.stoptime() << ")" << std::endl;
  } else {
    new_env->flat()->compact();
    new_env->output()->compact();
  }

  if (compflags.verbose) log << " done (" << lasttime.stoptime() << ")" << std::endl;

  return new_env;
}

CompilePass::~CompilePass(){};

}  // namespace MiniZinc
