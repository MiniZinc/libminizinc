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
#include <utility>

namespace MiniZinc {

using std::string;
using std::vector;

ASTString strip_stdlib_path(const vector<string>& includePaths, const ASTString& fs) {
  std::string f(fs.c_str());
  for (const auto& p : includePaths) {
    if (f.size() > p.size() && f.substr(0, p.size()) == p) {
      f = f.substr(p.size());
      while (!f.empty() && f[0] == '/') {
        f = f.substr(1);
      }
      return ASTString(f);
    }
  }
  return fs;
}

Env* change_library(Env& e, vector<string>& includePaths, const string& globals_dir,
                    CompilePassFlags& compflags, bool verbose = false) {
  GCLock lock;
  CopyMap cm;
  Model* m = e.envi().originalModel != nullptr ? e.envi().originalModel : e.envi().model;
  auto* new_mod = new Model();
  new_mod->setFilename(m->filename());
  new_mod->setFilepath(m->filepath());

  vector<string> new_includePaths;

  if (std::find(includePaths.begin(), includePaths.end(), globals_dir) == includePaths.end()) {
    new_includePaths.push_back(globals_dir);
  }
  new_includePaths.insert(new_includePaths.end(), includePaths.begin(), includePaths.end());

  // Collect include items
  vector<ASTString> include_names;
  for (Item* item : *m) {
    if (auto* inc = item->dynamicCast<IncludeI>()) {
      if (FileUtils::is_absolute(inc->f().c_str())) {
        include_names.push_back(inc->f());
      } else {
        include_names.push_back(strip_stdlib_path(new_includePaths, inc->m()->filepath()));
      }
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
  Model* inc_mod = parse_from_string(*fenv, ss.str(), dummy_file.str(), new_includePaths, false,
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
    : _env(e),
      _fopts(opts),
      _compflags(cflags),
      _library(std::move(globals_library)),
      _includePaths(std::move(include_paths)),
      _changeLibrary(change_lib),
      _ignoreUnknownIds(ignore_unknown) {}

Env* CompilePass::run(Env* store, std::ostream& log) {
  Timer lasttime;
  if (_compflags.verbose) {
    log << "\n\tCompilePass: Flatten with \'" << _library << "\' library ...\n";
  }

  Env* new_env;
  if (_changeLibrary) {
    new_env = change_library(*_env, _includePaths, _library, _compflags, _compflags.verbose);
    if (new_env == nullptr) {
      return nullptr;
    }
    new_env->envi().copyPathMapsAndState(store->envi());
  } else {
    new_env = _env;
  }
  new_env->envi().ignoreUnknownIds = _ignoreUnknownIds;

  vector<TypeError> typeErrors;
  MiniZinc::typecheck(*new_env, new_env->model(), typeErrors,
                      _compflags.modelCheckOnly || _compflags.modelInterfaceOnly,
                      _compflags.allowMultiAssign);
  if (!typeErrors.empty()) {
    std::ostringstream errstream;
    for (auto& typeError : typeErrors) {
      errstream << typeError.what() << ": " << typeError.msg() << std::endl;
      errstream << typeError.loc() << std::endl;
    }
    throw Error(errstream.str());
  }

  register_builtins(*new_env);

  try {
    flatten(*new_env, _fopts);
  } catch (LocationException& e) {
    if (_compflags.verbose) {
      log << std::endl;
    }
    std::ostringstream errstream;
    errstream << e.what() << ": " << std::endl;
    new_env->dumpErrorStack(errstream);
    errstream << "  " << e.msg() << std::endl;
    throw Error(errstream.str());
  }

  if (!_compflags.noMIPdomains) {
    if (_compflags.verbose) {
      log << "MIP domains ...";
    }
    mip_domains(*new_env, _compflags.statistics);
    if (_compflags.verbose) {
      log << " done (" << lasttime.stoptime() << ")" << std::endl;
    }
  }

  if (_compflags.optimize) {
    if (_compflags.verbose) {
      log << "Optimizing ...";
    }
    optimize(*new_env, _compflags.chainCompression);
    if (_compflags.verbose) {
      log << " done (" << lasttime.stoptime() << ")" << std::endl;
    }
  }

  for (const auto& i : new_env->warnings()) {
    log << (_compflags.werror ? "\n  ERROR: " : "\n  WARNING: ") << i;
  }
  if (_compflags.werror && !new_env->warnings().empty()) {
    throw Error("errors encountered");
  }
  new_env->clearWarnings();

  if (!_compflags.newfzn) {
    if (_compflags.verbose) {
      log << "Converting to old FlatZinc ...";
    }
    oldflatzinc(*new_env);
    if (_compflags.verbose) {
      log << " done (" << lasttime.stoptime() << ")" << std::endl;
    }
  } else {
    new_env->flat()->compact();
    new_env->output()->compact();
  }

  if (_compflags.verbose) {
    log << " done (" << lasttime.stoptime() << ")" << std::endl;
  }

  return new_env;
}

CompilePass::~CompilePass(){};

}  // namespace MiniZinc
