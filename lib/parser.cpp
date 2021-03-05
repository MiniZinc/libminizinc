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
#include <minizinc/json_parser.hh>
#include <minizinc/parser.hh>
#include <minizinc/prettyprinter.hh>

#include <fstream>

using namespace std;

int mzn_yylex_init(void** scanner);
void mzn_yyset_extra(void* user_defined, void* yyscanner);
int mzn_yylex_destroy(void* scanner);

namespace {
// fastest way to read a file into a string (especially big files)
// see: http://insanecoding.blogspot.be/2011/11/how-to-read-in-file-in-c.html
std::string get_file_contents(std::ifstream& in) {
  if (in) {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(static_cast<unsigned int>(in.tellg()));
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    if (!contents.empty() && contents[0] == '@') {
      contents = MiniZinc::FileUtils::decode_base64(contents);
      MiniZinc::FileUtils::inflate_string(contents);
    }
    return (contents);
  }
  throw(errno);
}
}  // namespace

namespace MiniZinc {

std::string ParserState::canonicalFilename(const std::string& f) const {
  if (FileUtils::is_absolute(f) || std::string(filename).empty()) {
    return f;
  }
  for (const auto& ip : includePaths) {
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

void parse(Env& env, Model*& model, const vector<string>& filenames,
           const vector<string>& datafiles, const std::string& modelString,
           const std::string& modelStringName, const vector<string>& ip, bool isFlatZinc,
           bool ignoreStdlib, bool parseDocComments, bool verbose, ostream& err,
           std::vector<SyntaxError>& syntaxErrors) {
  vector<string> includePaths;
  for (const auto& i : ip) {
    includePaths.push_back(i);
  }

  vector<ParseWorkItem> files;
  map<string, Model*> seenModels;

  string workingDir = FileUtils::working_directory();

  if (!filenames.empty()) {
    GCLock lock;
    model->setFilename(FileUtils::base_name(filenames[0]));
    if (FileUtils::is_absolute(filenames[0])) {
      files.emplace_back(model, nullptr, "", filenames[0]);
    } else {
      files.emplace_back(model, nullptr, "", workingDir + "/" + filenames[0]);
    }

    for (unsigned int i = 1; i < filenames.size(); i++) {
      GCLock lock;
      string fullName = filenames[i];
      string baseName = FileUtils::base_name(filenames[i]);
      if (!FileUtils::is_absolute(fullName)) {
        fullName = FileUtils::file_path(workingDir + "/" + fullName);
      }
      bool isFzn = (baseName.compare(baseName.length() - 4, 4, ".fzn") == 0);
      if (isFzn) {
        files.emplace_back(model, nullptr, "", fullName);
      } else {
        auto* includedModel = new Model;
        includedModel->setFilename(baseName);
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
    auto* libinc = new IncludeI(libloc, libname);
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
    bool isFzn;
    if (!isModelString) {
      for (Model* p = m->parent(); p != nullptr; p = p->parent()) {
        if (p->filename() == f) {
          err << "Error: cyclic includes: " << std::endl;
          for (Model* pe = m; pe != nullptr; pe = pe->parent()) {
            err << "  " << pe->filename() << std::endl;
          }
          goto error;
        }
      }
      ifstream file;
      if (FileUtils::is_absolute(f)) {
        fullname = f;
        basename = FileUtils::base_name(fullname);
        if (FileUtils::file_exists(fullname)) {
          file.open(FILE_PATH(fullname), std::ios::binary);
        }
      }
      if (file.is_open() &&
          FileUtils::file_path(FileUtils::dir_name(fullname)) != FileUtils::file_path(workingDir) &&
          FileUtils::file_exists(workingDir + "/" + basename)) {
        err << "Warning: file " << basename
            << " included from library, but also exists in current working directory" << endl;
      }
      for (const auto& includePath : includePaths) {
        std::string deprecatedName = includePath + "/" + basename + ".deprecated.mzn";
        if (FileUtils::file_exists(deprecatedName)) {
          string deprecatedFullPath = FileUtils::file_path(deprecatedName);
          string deprecatedBaseName = FileUtils::base_name(deprecatedFullPath);
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
      }
      if (!file.is_open()) {
        if (np_ii != nullptr) {
          err << np_ii->loc().toString() << ":\n";
          err << "MiniZinc: error in include item, cannot open file '" << f << "'." << endl;
        } else {
          err << "Error: cannot open file '" << f << "'." << endl;
        }
        goto error;
      }
      if (verbose) {
        std::cerr << "processing file '" << fullname << "'" << endl;
      }
      s = get_file_contents(file);

      if (m->filepath().size() == 0) {
        m->setFilepath(fullname);
      }
      isFzn = (fullname.compare(fullname.length() - 4, 4, ".fzn") == 0);
      isFzn |= (fullname.compare(fullname.length() - 4, 4, ".ozn") == 0);
      isFzn |= (fullname.compare(fullname.length() - 4, 4, ".szn") == 0);
      isFzn |= (fullname.compare(fullname.length() - 4, 4, ".mzc") == 0);
    } else {
      isFzn = false;
      fullname = f;
      s = parentPath;
    }
    ParserState pp(fullname, s, err, includePaths, files, seenModels, m, false, isFzn, isSTDLib,
                   parseDocComments);
    mzn_yylex_init(&pp.yyscanner);
    mzn_yyset_extra(&pp, pp.yyscanner);
    mzn_yyparse(&pp);
    if (pp.yyscanner != nullptr) {
      mzn_yylex_destroy(pp.yyscanner);
    }
    if (pp.hadError) {
      for (const auto& syntaxError : pp.syntaxErrors) {
        syntaxErrors.push_back(syntaxError);
      }
      goto error;
    }
  }

  for (const auto& f : datafiles) {
    GCLock lock;
    if (f.size() >= 6 && f.substr(f.size() - 5, string::npos) == ".json") {
      JSONParser jp(env.envi());
      jp.parse(model, f, true);
    } else {
      string s;
      if (f.size() > 5 && f.substr(0, 5) == "cmd:/") {
        s = f.substr(5);
      } else {
        std::ifstream file(FILE_PATH(f), std::ios::binary);
        if (!FileUtils::file_exists(f) || !file.is_open()) {
          err << "Error: cannot open data file '" << f << "'." << endl;
          goto error;
        }
        if (verbose) {
          std::cerr << "processing data file '" << f << "'" << endl;
        }
        s = get_file_contents(file);
      }

      ParserState pp(f, s, err, includePaths, files, seenModels, model, true, false, false,
                     parseDocComments);
      mzn_yylex_init(&pp.yyscanner);
      mzn_yyset_extra(&pp, pp.yyscanner);
      mzn_yyparse(&pp);
      if (pp.yyscanner != nullptr) {
        mzn_yylex_destroy(pp.yyscanner);
      }
      if (pp.hadError) {
        for (const auto& syntaxError : pp.syntaxErrors) {
          syntaxErrors.push_back(syntaxError);
        }
        goto error;
      }
    }
  }

  return;
error:
  delete model;
  model = nullptr;
}

Model* parse(Env& env, const vector<string>& filenames, const vector<string>& datafiles,
             const string& textModel, const string& textModelName,
             const vector<string>& includePaths, bool isFlatZinc, bool ignoreStdlib,
             bool parseDocComments, bool verbose, ostream& err) {
  if (filenames.empty() && textModel.empty()) {
    err << "Error: no model given" << std::endl;
    return nullptr;
  }

  Model* model;
  {
    GCLock lock;
    model = new Model();
  }
  std::vector<SyntaxError> se;
  parse(env, model, filenames, datafiles, textModel, textModelName, includePaths, isFlatZinc,
        ignoreStdlib, parseDocComments, verbose, err, se);
  return model;
}

Model* parse_data(Env& env, Model* model, const vector<string>& datafiles,
                  const vector<string>& includePaths, bool isFlatZinc, bool ignoreStdlib,
                  bool parseDocComments, bool verbose, ostream& err) {
  vector<string> filenames;
  std::vector<SyntaxError> se;
  parse(env, model, filenames, datafiles, "", "", includePaths, isFlatZinc, ignoreStdlib,
        parseDocComments, verbose, err, se);
  return model;
}

Model* parse_from_string(Env& env, const string& text, const string& filename,
                         const vector<string>& includePaths, bool isFlatZinc, bool ignoreStdlib,
                         bool parseDocComments, bool verbose, ostream& err,
                         std::vector<SyntaxError>& syntaxErrors) {
  vector<string> filenames;
  vector<string> datafiles;
  Model* model;
  {
    GCLock lock;
    model = new Model();
  }
  parse(env, model, filenames, datafiles, text, filename, includePaths, isFlatZinc, ignoreStdlib,
        parseDocComments, verbose, err, syntaxErrors);
  return model;
}

}  // namespace MiniZinc
