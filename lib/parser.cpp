/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* This (main) file coordinates flattening and solving.
 * The corresponding modules are flexibly plugged in
 * as derived classes, prospectively from DLLs.
 * A flattening module should provide MinZinc::GetFlattener()
 * A solving module should provide an object of a class derived from SolverFactory.
 * Need to get more flexible for multi-pass & multi-solving stuff  TODO
 */

#include <minizinc/file_utils.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/json_parser.hh>
#include <minizinc/library_bundle.hh>
#include <minizinc/parser.hh>
#include <minizinc/prettyprinter.hh>

#include <cstring>
#include <fstream>
#include <regex>

using namespace std;

int mzn_yylex_init(void** scanner);
void mzn_yyset_extra(void* user_defined, void* yyscanner);
int mzn_yylex_destroy(void* scanner);

namespace MiniZinc {
// Temporary: goes away with the bison parser.
// `getenv` is not declared `noexcept`, so the check fires even though the lambda
// is; suppressed as in `Location::nonalloc` and friends in ast.cpp.
bool use_tree_sitter_parser = []() noexcept {  // NOLINT(bugprone-throwing-static-initialization)
  const char* e = getenv("MZN_TREE_SITTER_PARSER");
  return e != nullptr && e[0] != '\0' && std::strcmp(e, "0") != 0 && std::strcmp(e, "false") != 0;
}();
}  // namespace MiniZinc

namespace {
void run_parser(MiniZinc::ParserState& pp) {
  if (MiniZinc::use_tree_sitter_parser) {
    MiniZinc::parse_tree_sitter(pp);
  } else {
    mzn_yylex_init(&pp.yyscanner);
    mzn_yyset_extra(&pp, pp.yyscanner);
    mzn_yyparse(&pp);
    if (pp.yyscanner != nullptr) {
      mzn_yylex_destroy(pp.yyscanner);
    }
  }
  // A failed parse may have seen the keyword in syntax that is not supported yet.
  if (pp.futureKeyword != nullptr && !pp.hadError) {
    MiniZinc::GCLock lock;
    pp.addWarning(MiniZinc::Location(pp.futureKeywordLoc),
                  std::string("`") + pp.futureKeyword +
                      "' will become a reserved word in a future version of MiniZinc; rename "
                      "this identifier");
  }
}
}  // namespace

namespace MiniZinc {

std::string ParserState::canonicalFilename(const std::string& f) const {
  if (FileUtils::is_absolute(f) || std::string(filename).empty()) {
    return f;
  }
  for (const auto& ip : includePaths) {
    const LibraryBundle* bundle = bundles.get(ip);
    if (bundle != nullptr) {
      if (bundle->entry(f) != nullptr) {
        return bundle->qualifiedName(f);
      }
      continue;
    }
    std::string fullname = FileUtils::file_path(ip + "/" + f);
    if (FileUtils::file_exists(fullname)) {
      return fullname;
    }
  }
  std::string parentPath = FileUtils::dir_name(filename);
  if (parentPath.empty()) {
    parentPath = ".";
  }
  std::string fullname = FileUtils::file_path(parentPath + "/" + f);
  if (FileUtils::file_exists(fullname)) {
    return fullname;
  }
  return f;
}

namespace {
/// The filenames directly included by globals.mzn, looked up in \a stdPath (the
/// include path of the standard library, which may be a library bundle)
std::unordered_set<std::string> global_includes(const std::string& stdPath,
                                                LibraryBundleCache& bundles) {
  GCLock lock;
  // Read globals file. If it cannot be found, act as if there are no bad files.
  std::string content;
  const LibraryBundle* bundle = bundles.get(stdPath);
  if (bundle != nullptr) {
    const auto* e = bundle->entry("globals.mzn");
    if (e == nullptr) {
      return {};
    }
    content.assign(e->data, e->size);
  } else {
    if (!FileUtils::file_exists(stdPath + "/globals.mzn")) {
      return {};
    }
    std::ifstream ifs(FILE_PATH(FileUtils::file_path(stdPath + "/globals.mzn")));
    content.assign((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
  }

  // Regular expression to find include items (assumes no include statements in comments)
  std::regex include_item("include[[:space:]]+\"([^\"]+)\"", std::regex_constants::egrep);

  // Collect all files directly included in globals.mzn
  std::unordered_set<std::string> files;
  for (auto inc = std::sregex_token_iterator(content.begin(), content.end(), include_item, 1);
       inc != std::sregex_token_iterator(); ++inc) {
    files.emplace(inc->str());
  }
  return files;
}
}  // namespace

void ParserState::addWarning(const Location& loc, const std::string& msg) {
  env.addWarning(loc, msg, false);
}

void parse(Env& env, Model*& model, const vector<string>& filenames,
           const vector<string>& datafiles, const std::string& modelString,
           const std::string& modelStringName, const vector<string>& ip, bool checkGlobalOverrides,
           bool isFlatZinc, bool ignoreStdlib, bool parseDocComments, bool verbose, ostream& err) {
  vector<string> includePaths;
  for (const auto& i : ip) {
    includePaths.push_back(i);
  }

  vector<ParseWorkItem> files;
  map<string, Model*> seenModels;
  // Owned by this parse, so no bundle stays resident once it returns
  LibraryBundleCache bundles;
  std::unordered_set<std::string> globalInc;
  if (checkGlobalOverrides && !includePaths.empty()) {
    globalInc = global_includes(includePaths.back(), bundles);
  }

  string workingDir = FileUtils::working_directory();

  if (!filenames.empty()) {
    GCLock lock;
    auto rootFileName = FileUtils::file_path(filenames[0], workingDir);
    model->setFilename(rootFileName);
    files.emplace_back(model, nullptr, "", rootFileName);

    for (unsigned int i = 1; i < filenames.size(); i++) {
      GCLock lock;
      auto fullName = FileUtils::file_path(filenames[i], workingDir);
      bool isFzn = (fullName.compare(fullName.length() - 4, 4, ".fzn") == 0);
      if (isFzn) {
        files.emplace_back(model, nullptr, "", fullName);
      } else {
        auto* includedModel = new Model;
        includedModel->setFilename(fullName);
        files.emplace_back(includedModel, nullptr, "", fullName);
        seenModels.insert(pair<string, Model*>(fullName, includedModel));
        Location loc(ASTString(filenames[i]), 0, 0, 0, 0);
        auto* inc = new IncludeI(loc, includedModel->filename());
        inc->m(includedModel, true);
        model->addItem(inc);
      }
    }
    if (!modelString.empty()) {
      auto* includedModel = new Model;
      includedModel->setFilename(modelStringName);
      files.emplace_back(includedModel, nullptr, modelString, modelStringName, false, true);
      seenModels.insert(pair<string, Model*>(modelStringName, includedModel));
      Location loc(ASTString(modelStringName), 0, 0, 0, 0);
      auto* inc = new IncludeI(loc, includedModel->filename());
      inc->m(includedModel, true);
      model->addItem(inc);
    }
  } else if (!modelString.empty()) {
    GCLock lock;
    model->setFilename(modelStringName);
    files.emplace_back(model, nullptr, modelString, modelStringName, false, true);
  }

  auto include_file = [&](const std::string& libname, bool builtin) {
    GCLock lock;
    auto* lib = new Model;
    std::string fullname;
    for (const auto& ip : includePaths) {
      const LibraryBundle* bundle = bundles.get(ip);
      if (bundle != nullptr) {
        if (bundle->entry(libname) != nullptr) {
          fullname = bundle->qualifiedName(libname);
          break;
        }
        continue;
      }
      std::string n = FileUtils::file_path(ip + "/" + libname);
      if (FileUtils::file_exists(n)) {
        fullname = n;
        break;
      }
    }
    lib->setFilename(fullname);
    files.emplace_back(lib, nullptr, "./", fullname, builtin);
    seenModels.insert(pair<string, Model*>(fullname, lib));
    Location libloc(ASTString(model->filename()), 0, 0, 0, 0);
    auto* libinc = new IncludeI(libloc, ASTString(libname));
    libinc->m(lib, true);
    model->addItem(libinc);
  };

  // TODO: It should be possible to use just flatzinc builtins instead of stdlib when parsing
  // FlatZinc if (!isFlatZinc) {
  if (!ignoreStdlib) {
    include_file("solver_redefinitions.mzn", false);
    include_file("stdlib.mzn", true);  // Added last, so it is processed first
  }
  // } else {
  //   include_file("flatzincbuiltins.mzn", true);
  // }

  while (!files.empty()) {
    GCLock lock;
    ParseWorkItem& np = files.back();
    string parentPath = np.dirName;
    Model* m = np.m;
    bool isModelString = np.isModelString;
    bool isSTDLib = np.isSTDLib;
    IncludeI* np_ii = np.ii;
    string f(np.fileName);
    files.pop_back();

    std::string s;
    std::string fullname;
    std::string basename;
    unsigned int lineOffset = 0;
    bool isFzn;
    if (!isModelString) {
      for (Model* p = m->parent(); p != nullptr; p = p->parent()) {
        if (p->filename() == f) {
          std::vector<ASTString> cycle;
          for (Model* pe = m; pe != nullptr; pe = pe->parent()) {
            cycle.push_back(pe->filename());
          }
          throw CyclicIncludeError(cycle);
        }
      }
      const LibraryBundle* bundle = nullptr;
      const LibraryBundle::Entry* bundled = bundles.lookup(f, &bundle);
      bool found = false;
      if (bundled != nullptr) {
        // File is part of a library bundle rather than a file of its own
        fullname = bundled->location;
        basename = FileUtils::base_name(f);
        lineOffset = bundled->lineOffset;
        found = true;
      } else if (FileUtils::is_absolute(f)) {
        fullname = f;
        basename = FileUtils::base_name(fullname);
        found = FileUtils::file_exists(fullname);
      }
      if (found &&
          FileUtils::file_path(FileUtils::dir_name(fullname)) != FileUtils::file_path(workingDir) &&
          FileUtils::file_exists(workingDir + "/" + basename)) {
        std::ostringstream w;
        w << "file \"" << basename
          << "\" included from library, but also exists in current working directory.";
        env.envi().addWarning(w.str());
      } else if (found && globalInc.find(basename) != globalInc.end() &&
                 f.find(includePaths.back()) == std::string::npos) {
        std::ostringstream w;
        w << "included file \"" << basename
          << "\" overrides a global constraint file from the standard library. This is "
             "deprecated. For a solver-specific redefinition of a global constraint, override "
             "\"fzn_<global>.mzn\" instead.";
        env.envi().addWarning(w.str());
      }
      for (const auto& includePath : includePaths) {
        std::string deprecatedName;
        string deprecatedFullPath;
        const LibraryBundle* deprecatedBundle = bundles.get(includePath);
        if (deprecatedBundle != nullptr) {
          std::string key = basename + ".deprecated.mzn";
          if (deprecatedBundle->entry(key) == nullptr) {
            continue;
          }
          deprecatedName = deprecatedBundle->qualifiedName(key);
          deprecatedFullPath = deprecatedName;
        } else {
          deprecatedName = includePath + "/" + basename + ".deprecated.mzn";
          if (!FileUtils::file_exists(deprecatedName)) {
            continue;
          }
          deprecatedFullPath = FileUtils::file_path(deprecatedName);
        }
        string deprecatedDirName = FileUtils::dir_name(deprecatedFullPath);
        auto* includedModel = new Model;
        includedModel->setFilename(deprecatedName);
        files.emplace_back(includedModel, nullptr, "", deprecatedName, isSTDLib, false);
        seenModels.insert(pair<string, Model*>(deprecatedName, includedModel));
        Location loc(ASTString(deprecatedName), 0, 0, 0, 0);
        auto* inc = new IncludeI(loc, includedModel->filename());
        inc->m(includedModel, true);
        m->addItem(inc);
        files.emplace_back(includedModel, inc, deprecatedDirName, deprecatedFullPath, isSTDLib,
                           false);
      }
      if (!found) {
        if (np_ii != nullptr) {
          throw IncludeError(env.envi(), np_ii->loc(), "Cannot open file '" + f + "'.");
        }
        throw Error("Cannot open file '" + f + "'.");
      }
      if (verbose) {
        if (bundled != nullptr) {
          // Bracketed, so that the bundle and the file inside it do not read as
          // one path. `f` is "<bundle path>/<file>", so the file starts just
          // past the bundle's own path.
          std::cerr << "processing file '" << bundle->path() << "' ["
                    << (f.c_str() + bundle->path().size() + 1) << "]" << endl;
        } else {
          std::cerr << "processing file '" << fullname << "'" << endl;
        }
      }
      if (bundled != nullptr) {
        s.assign(bundled->data, bundled->size);
      } else {
        s = FileUtils::read_file_contents(fullname);
      }

      if (m->filepath().empty()) {
        // For bundled files this is the qualified name, so that include paths
        // can still be stripped off it
        m->setFilepath(bundled != nullptr ? f : fullname);
      }
      isFzn = (fullname.compare(fullname.length() - 4, 4, ".fzn") == 0);
    } else {
      isFzn = false;
      fullname = f;
      s = parentPath;
    }
    ParserState pp(fullname, s, env.envi(), err, includePaths, files, seenModels, bundles, m,
                   env.envi().dataFileCalls, false, isFzn, isSTDLib, parseDocComments, lineOffset);
    run_parser(pp);
    if (pp.hadError) {
      throw MultipleErrors<SyntaxError>(pp.syntaxErrors);
    }
  }

  for (const auto& f : datafiles) {
    GCLock lock;
    if (f.size() >= 6 && f.substr(f.size() - 5, string::npos) == ".json") {
      JSONParser jp(env.envi());
      jp.parse(model, f, true);
    } else if (f.size() >= 6 && f.substr(0, 6) == "json:/") {
      JSONParser jp(env.envi());
      jp.parseFromString(model, f.substr(6), true);
    } else {
      string s;
      if (f.size() > 5 && f.substr(0, 5) == "cmd:/") {
        s = f.substr(5);
      } else {
        if (!FileUtils::file_exists(f)) {
          throw Error("Cannot open data file '" + f + "'.");
        }
        if (verbose) {
          std::cerr << "processing data file '" << f << "'" << endl;
        }
        s = FileUtils::read_file_contents(f);
      }

      ParserState pp(f, s, env.envi(), err, includePaths, files, seenModels, bundles, model,
                     env.envi().dataFileCalls, true, false, false, parseDocComments);
      run_parser(pp);
      if (pp.hadError) {
        throw MultipleErrors<SyntaxError>(pp.syntaxErrors);
      }
    }
  }
}

Model* parse(Env& env, const vector<string>& filenames, const vector<string>& datafiles,
             const string& textModel, const string& textModelName,
             const vector<string>& includePaths, bool checkGlobalOverrides, bool isFlatZinc,
             bool ignoreStdlib, bool parseDocComments, bool verbose, ostream& err) {
  if (filenames.empty() && textModel.empty()) {
    throw Error("No model given.");
  }

  Model* model;
  {
    GCLock lock;
    model = new Model();
  }
  try {
    parse(env, model, filenames, datafiles, textModel, textModelName, includePaths,
          checkGlobalOverrides, isFlatZinc, ignoreStdlib, parseDocComments, verbose, err);
  } catch (Exception& /*e*/) {
    env.envi().dataFileCalls.clear();
    delete model;
    throw;
  }
  return model;
}

Model* parse_data(Env& env, Model* model, const vector<string>& datafiles,
                  const vector<string>& includePaths, bool isFlatZinc, bool ignoreStdlib,
                  bool parseDocComments, bool verbose, ostream& err) {
  vector<string> filenames;
  parse(env, model, filenames, datafiles, "", "", includePaths, false, isFlatZinc, ignoreStdlib,
        parseDocComments, verbose, err);
  return model;
}

Model* parse_from_string(Env& env, const string& text, const string& filename,
                         const vector<string>& includePaths, bool isFlatZinc, bool ignoreStdlib,
                         bool parseDocComments, bool verbose, ostream& err) {
  vector<string> filenames;
  vector<string> datafiles;
  Model* model;
  {
    GCLock lock;
    model = new Model();
  }
  try {
    parse(env, model, filenames, datafiles, text, filename, includePaths, false, isFlatZinc,
          ignoreStdlib, parseDocComments, verbose, err);
  } catch (Exception& /*e*/) {
    env.envi().dataFileCalls.clear();
    delete model;
    throw;
  }
  return model;
}

}  // namespace MiniZinc
