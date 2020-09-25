/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/file_utils.hh>
#include <minizinc/json_parser.hh>
#include <minizinc/parser.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/solver_config.hh>

#include <algorithm>
#include <cctype>
#include <iterator>
#include <set>
#include <sstream>
#include <string>

using namespace std;

namespace MiniZinc {

namespace {
std::string getString(AssignI* ai) {
  if (auto* sl = ai->e()->dynamicCast<StringLit>()) {
    return std::string(sl->v().c_str(), sl->v().size());
  }
  throw ConfigException("invalid configuration item (right hand side must be string)");
}
bool getBool(AssignI* ai) {
  if (auto* bl = ai->e()->dynamicCast<BoolLit>()) {
    return bl->v();
  }
  throw ConfigException("invalid configuration item (right hand side must be bool)");
}
int getInt(AssignI* ai) {
  if (auto* il = ai->e()->dynamicCast<IntLit>()) {
    return static_cast<int>(il->v().toInt());
  }
  throw ConfigException("invalid configuration item (right hand side must be int)");
}
std::vector<std::string> getStringList(AssignI* ai) {
  if (auto* al = ai->e()->dynamicCast<ArrayLit>()) {
    std::vector<std::string> ret;
    for (unsigned int i = 0; i < al->size(); i++) {
      if (auto* sl = (*al)[i]->dynamicCast<StringLit>()) {
        ret.emplace_back(sl->v().c_str(), sl->v().size());
      } else {
        throw ConfigException(
            "invalid configuration item (right hand side must be a list of strings)");
      }
    }
    return ret;
  }
  throw ConfigException("invalid configuration item (right hand side must be a list of strings)");
}
std::vector<std::pair<std::string, std::string> > getStringPairList(AssignI* ai) {
  if (auto* al = ai->e()->dynamicCast<ArrayLit>()) {
    std::vector<std::pair<std::string, std::string> > ret;
    if (al->dims() != 2 || al->min(1) != 1 || al->max(1) != 2) {
      throw ConfigException(
          "invalid configuration item (right hand side must be a 2d array of strings)");
    }
    for (unsigned int i = 0; i < al->size(); i += 2) {
      auto* sl1 = (*al)[i]->dynamicCast<StringLit>();
      auto* sl2 = (*al)[i + 1]->dynamicCast<StringLit>();
      if ((sl1 != nullptr) && (sl2 != nullptr)) {
        ret.emplace_back(std::string(sl1->v().c_str(), sl1->v().size()),
                         std::string(sl2->v().c_str(), sl2->v().size()));
      } else {
        throw ConfigException(
            "invalid configuration item (right hand side must be a 2d array of strings)");
      }
    }
    return ret;
  }
  throw ConfigException(
      "invalid configuration item (right hand side must be a 2d array of strings)");
}
std::vector<std::vector<std::string> > getDefaultOptionList(AssignI* ai) {
  if (auto* al = ai->e()->dynamicCast<ArrayLit>()) {
    std::vector<std::vector<std::string> > ret;
    if (al->size() == 0) {
      return ret;
    }
    if (al->dims() != 2) {
      throw ConfigException(
          "invalid configuration item (right hand side must be a 2d array of strings)");
    }
    int nCols = al->max(1) - al->min(1) + 1;
    if (nCols != 3) {
      throw ConfigException(
          "invalid configuration item (right hand side must be a 2d array of strings with 3 "
          "columns)");
    }
    for (unsigned int i = 0; i < al->size(); i += nCols) {
      auto* sl0 = (*al)[i]->dynamicCast<StringLit>();
      auto* sl1 = (*al)[i + 1]->dynamicCast<StringLit>();
      auto* sl2 = (*al)[i + 2]->dynamicCast<StringLit>();
      if ((sl0 != nullptr) && (sl1 != nullptr) && (sl2 != nullptr)) {
        ret.push_back(std::vector<std::string>({std::string(sl0->v().c_str(), sl0->v().size()),
                                                std::string(sl1->v().c_str(), sl1->v().size()),
                                                std::string(sl2->v().c_str(), sl2->v().size())}));
      } else {
        throw ConfigException(
            "invalid configuration item (right hand side must be a list of strings)");
      }
    }
    return ret;
  }
  throw ConfigException(
      "invalid configuration item (right hand side must be a 2d array of strings)");
}
std::vector<SolverConfig::ExtraFlag> getExtraFlagList(AssignI* ai) {
  if (auto* al = ai->e()->dynamicCast<ArrayLit>()) {
    std::vector<SolverConfig::ExtraFlag> ret;
    if (al->size() == 0) {
      return ret;
    }
    if (al->dims() != 2) {
      throw ConfigException(
          "invalid configuration item (right hand side must be a 2d array of strings)");
    }
    int nCols = al->max(1) - al->min(1) + 1;
    if (nCols < 2 || nCols > 4) {
      throw ConfigException(
          "invalid configuration item (right hand side must be a 2d array of strings)");
    }
    bool haveType = (nCols >= 3);
    bool haveDefault = (nCols >= 4);
    for (unsigned int i = 0; i < al->size(); i += nCols) {
      auto* sl1 = (*al)[i]->dynamicCast<StringLit>();
      auto* sl2 = (*al)[i + 1]->dynamicCast<StringLit>();
      StringLit* sl3 = haveType ? (*al)[i + 2]->dynamicCast<StringLit>() : nullptr;
      StringLit* sl4 = haveDefault ? (*al)[i + 3]->dynamicCast<StringLit>() : nullptr;
      std::string opt_type =
          sl3 != nullptr ? std::string(sl3->v().c_str(), sl3->v().size()) : "bool";
      std::string opt_def;
      if (sl4 != nullptr) {
        opt_def = std::string(sl4->v().c_str(), sl4->v().size());
      } else if (opt_type == "bool") {
        opt_def = "false";
      } else if (opt_type == "int") {
        opt_def = "0";
      } else if (opt_type == "float") {
        opt_def = "0.0";
      }
      if ((sl1 != nullptr) && (sl2 != nullptr)) {
        ret.emplace_back(std::string(sl1->v().c_str(), sl1->v().size()),
                         std::string(sl2->v().c_str(), sl2->v().size()), opt_type, opt_def);
      } else {
        throw ConfigException(
            "invalid configuration item (right hand side must be a 2d array of strings)");
      }
    }
    return ret;
  }
  throw ConfigException(
      "invalid configuration item (right hand side must be a 2d array of strings)");
}

std::string getEnv(const char* v) {
  std::string ret;
#ifdef _MSC_VER
  size_t len;
  getenv_s(&len, NULL, 0, v);
  if (len > 0) {
    char* p = static_cast<char*>(malloc(len * sizeof(char)));
    getenv_s(&len, p, len, v);
    if (len > 0) {
      ret = p;
    }
    free(p);
  }
#else
  char* p = getenv(v);
  if (p != nullptr) {
    ret = p;
  }
#endif
  return ret;
}

char charToLower(char c) { return std::tolower(static_cast<unsigned char>(c)); }
std::string stringToLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), charToLower);
  return s;
}
struct SortByLowercase {
  bool operator()(const std::string& n1, const std::string& n2) {
    for (size_t i = 0; i < n1.size() && i < n2.size(); i++) {
      if (std::tolower(n1[i]) != std::tolower(n2[i])) {
        return std::tolower(n1[i]) < std::tolower(n2[i]);
      }
    }
    return n1.size() < n2.size();
  }
};
struct SortByName {
  const std::vector<SolverConfig>& solvers;
  SortByLowercase sortByLowercase;
  SortByName(const std::vector<SolverConfig>& solvers0) : solvers(solvers0) {}
  bool operator()(int idx1, int idx2) {
    return sortByLowercase(solvers[idx1].name(), solvers[idx2].name());
  }
};

}  // namespace

SolverConfig SolverConfig::load(const string& filename) {
  SolverConfig sc;
  sc._configFile = FileUtils::file_path(filename);
  ostringstream errstream;
  try {
    Env confenv;
    Model* m = nullptr;
    if (JSONParser::fileIsJSON(filename)) {
      JSONParser jp(confenv.envi());
      try {
        m = new Model;
        GCLock lock;
        jp.parse(m, filename);
      } catch (JSONError& e) {
        delete m;
        m = nullptr;
        throw ConfigException(e.msg());
      }
    } else {
      vector<string> filenames;
      filenames.push_back(filename);
      m = parse(confenv, filenames, vector<string>(), "", "", vector<string>(), false, true, false,
                false, errstream);
    }
    if (m != nullptr) {
      bool hadId = false;
      bool hadVersion = false;
      bool hadName = false;
      string basePath = FileUtils::dir_name(sc._configFile);
      for (auto& i : *m) {
        if (auto* ai = i->dynamicCast<AssignI>()) {
          if (ai->id() == "id") {
            sc._id = getString(ai);
            hadId = true;
          } else if (ai->id() == "name") {
            sc._name = getString(ai);
            hadName = true;
          } else if (ai->id() == "executable") {
            std::string exePath = getString(ai);
            sc._executable = exePath;
            std::string exe = FileUtils::find_executable(FileUtils::file_path(exePath, basePath));
            int nr_found = (int)(!exe.empty());
            std::string tmp = FileUtils::file_path(FileUtils::find_executable(exePath));
            nr_found += (int)((!tmp.empty()) && tmp != exe);
            exe = exe.empty() ? tmp : exe;
            if (nr_found > 0) {
              sc._executableResolved = exe;
              if (nr_found > 1) {
                std::cerr << "Warning: multiple executables '" << exePath
                          << "' found on the system, using '" << exe << "'" << std::endl;
              }
            }
          } else if (ai->id() == "mznlib") {
            std::string libPath = getString(ai);
            sc._mznlib = libPath;
            if (!libPath.empty()) {
              if (libPath[0] == '-') {
                sc._mznlibResolved = libPath;
              } else if (libPath.size() > 2 && libPath[0] == '.' &&
                         (libPath[1] == '/' || (libPath[1] == '.' && libPath[2] == '/'))) {
                sc._mznlibResolved = FileUtils::file_path(libPath, basePath);
              } else {
                sc._mznlibResolved = FileUtils::file_path(libPath, basePath);
              }
            }
          } else if (ai->id() == "version") {
            sc._version = getString(ai);
            hadVersion = true;
          } else if (ai->id() == "mznlibVersion") {
            sc._mznlibVersion = getInt(ai);
          } else if (ai->id() == "description") {
            sc._description = getString(ai);
          } else if (ai->id() == "contact") {
            sc._contact = getString(ai);
          } else if (ai->id() == "website") {
            sc._website = getString(ai);
          } else if (ai->id() == "supportsMzn") {
            sc._supportsMzn = getBool(ai);
          } else if (ai->id() == "supportsFzn") {
            sc._supportsFzn = getBool(ai);
          } else if (ai->id() == "supportsNL") {
            sc._supportsNL = getBool(ai);
          } else if (ai->id() == "needsSolns2Out") {
            sc._needsSolns2Out = getBool(ai);
          } else if (ai->id() == "isGUIApplication") {
            sc._isGUIApplication = getBool(ai);
          } else if (ai->id() == "needsMznExecutable") {
            sc._needsMznExecutable = getBool(ai);
          } else if (ai->id() == "needsStdlibDir") {
            sc._needsStdlibDir = getBool(ai);
          } else if (ai->id() == "needsPathsFile") {
            sc._needsPathsFile = getBool(ai);
          } else if (ai->id() == "tags") {
            sc._tags = getStringList(ai);
          } else if (ai->id() == "stdFlags") {
            sc._stdFlags = getStringList(ai);
          } else if (ai->id() == "requiredFlags") {
            sc._requiredFlags = getStringList(ai);
          } else if (ai->id() == "extraFlags") {
            sc._extraFlags = getExtraFlagList(ai);
          } else {
            std::ostringstream ss;
            ss << "invalid configuration item (" << ai->id() << ")";
            throw ConfigException(ss.str());
          }
        } else {
          throw ConfigException("invalid configuration item");
        }
      }
      if (!hadId) {
        throw ConfigException("invalid solver configuration (missing id)");
      }
      if (!hadVersion) {
        throw ConfigException("invalid solver configuration (missing version)");
      }
      if (!hadName) {
        throw ConfigException("invalid solver configuration (missing name)");
      }
    } else {
      throw ConfigException(errstream.str());
    }
  } catch (ConfigException&) {
    throw;
  } catch (Exception& e) {
    throw ConfigException(e.what());
  }

  return sc;
}

std::string SolverConfig::toJSON(const SolverConfigs& configs) const {
  GCLock lock;
  std::ostringstream oss;
  auto def_id = configs.defaultSolver("");
  oss << "{\n";
  oss << "  \"extraInfo\": {\n";
  if (!def_id.empty() && def_id == id()) {
    oss << "    \"isDefault\": true,\n";
  }
  if (!mznlibResolved().empty()) {
    oss << "    \"mznlib\": \"" << Printer::escapeStringLit(mznlibResolved()) << "\",\n";
  }
  if (!executableResolved().empty()) {
    oss << "    \"executable\": \"" << Printer::escapeStringLit(executableResolved()) << "\",\n";
  }
  oss << "    \"configFile\": \"" << Printer::escapeStringLit(configFile()) << "\"";
  if (!defaultFlags().empty()) {
    oss << ",\n    \"defaultFlags\": [";
    for (unsigned int j = 0; j < defaultFlags().size(); j++) {
      oss << "\"" << Printer::escapeStringLit(defaultFlags()[j]) << "\"";
      if (j < defaultFlags().size() - 1) {
        oss << ",";
      }
    }
    oss << "]";
  }
  oss << "\n";
  oss << "  },\n";
  oss << "  \"id\": \"" << Printer::escapeStringLit(id()) << "\",\n";
  oss << "  \"name\": \"" << Printer::escapeStringLit(name()) << "\",\n";
  oss << "  \"version\": \"" << Printer::escapeStringLit(version()) << "\",\n";
  if (!mznlib().empty()) {
    oss << "  \"mznlib\": \"" << Printer::escapeStringLit(mznlib()) << "\",\n";
  }
  if (!executable().empty()) {
    oss << "  \"executable\": \"" << Printer::escapeStringLit(executable()) << "\",\n";
  }
  oss << "  \"mznlibVersion\": " << mznlibVersion() << ",\n";
  if (!description().empty()) {
    oss << "  \"description\": \"" << Printer::escapeStringLit(description()) << "\",\n";
  }
  if (!contact().empty()) {
    oss << "  \"contact\": \"" << Printer::escapeStringLit(contact()) << "\",\n";
  }
  if (!website().empty()) {
    oss << "  \"website\": \"" << Printer::escapeStringLit(website()) << "\",\n";
  }
  if (!requiredFlags().empty()) {
    oss << "  \"requiredFlags\": [";
    for (unsigned int j = 0; j < requiredFlags().size(); j++) {
      oss << "\"" << requiredFlags()[j] << "\"";
      if (j < requiredFlags().size() - 1) {
        oss << ",";
      }
    }
    oss << "],\n";
  }
  if (!stdFlags().empty()) {
    oss << "  \"stdFlags\": [";
    for (unsigned int j = 0; j < stdFlags().size(); j++) {
      oss << "\"" << stdFlags()[j] << "\"";
      if (j < stdFlags().size() - 1) {
        oss << ",";
      }
    }
    oss << "],\n";
  }
  if (!extraFlags().empty()) {
    oss << "  \"extraFlags\": [";
    for (unsigned int j = 0; j < extraFlags().size(); j++) {
      oss << "["
          << "\"" << extraFlags()[j].flag << "\",\"" << extraFlags()[j].description << "\",\"";
      oss << extraFlags()[j].flagType << "\",\"" << extraFlags()[j].defaultValue << "\"]";
      if (j < extraFlags().size() - 1) {
        oss << ",";
      }
    }
    oss << "],\n";
  }

  if (!tags().empty()) {
    oss << "  \"tags\": [";
    for (unsigned int j = 0; j < tags().size(); j++) {
      oss << "\"" << Printer::escapeStringLit(tags()[j]) << "\"";
      if (j < tags().size() - 1) {
        oss << ",";
      }
    }
    oss << "],\n";
  }
  oss << "  \"supportsMzn\": " << (supportsMzn() ? "true" : "false") << ",\n";
  oss << "  \"supportsFzn\": " << (supportsFzn() ? "true" : "false") << ",\n";
  oss << "  \"supportsNL\": " << (supportsNL() ? "true" : "false") << ",\n";
  oss << "  \"needsSolns2Out\": " << (needsSolns2Out() ? "true" : "false") << ",\n";
  oss << "  \"needsMznExecutable\": " << (needsMznExecutable() ? "true" : "false") << ",\n";
  oss << "  \"needsStdlibDir\": " << (needsStdlibDir() ? "true" : "false") << ",\n";
  oss << "  \"needsPathsFile\": " << (needsPathsFile() ? "true" : "false") << ",\n";
  oss << "  \"isGUIApplication\": " << (isGUIApplication() ? "true" : "false") << "\n";
  oss << "}";

  return oss.str();
}

class BuiltinSolverConfigs {
public:
  std::unordered_map<std::string, SolverConfig> builtinSolvers;
};

BuiltinSolverConfigs& builtinSolverConfigs() {
  static BuiltinSolverConfigs c;
  return c;
}

void SolverConfigs::addConfig(const MiniZinc::SolverConfig& sc) {
  int newIdx = static_cast<int>(_solvers.size());
  _solvers.push_back(sc);
  std::vector<string> sc_tags = sc.tags();
  std::string id = sc.id();
  id = stringToLower(id);
  sc_tags.push_back(id);
  std::string name = sc.name();
  name = stringToLower(name);
  sc_tags.push_back(name);
  for (const auto& t : sc_tags) {
    auto it = _tags.find(t);
    if (it == _tags.end()) {
      _tags.insert(std::make_pair(t, std::vector<int>({newIdx})));
    } else {
      it->second.push_back(newIdx);
    }
  }
}

std::vector<std::string> SolverConfigs::solverConfigsPath() const { return _solverPath; }

SolverConfigs::SolverConfigs(std::ostream& log) {
#ifdef _MSC_VER
  const char* PATHSEP = ";";
#else
  const char* PATHSEP = ":";
#endif
  for (const auto& sc : builtinSolverConfigs().builtinSolvers) {
    addConfig(sc.second);
  }
  std::string mzn_solver_path = getEnv("MZN_SOLVER_PATH");
  while (!mzn_solver_path.empty()) {
    size_t next_sep = mzn_solver_path.find(PATHSEP);
    string cur_path = mzn_solver_path.substr(0, next_sep);
    _solverPath.push_back(cur_path);
    if (next_sep != string::npos) {
      mzn_solver_path = mzn_solver_path.substr(next_sep + 1, string::npos);
    } else {
      mzn_solver_path = "";
    }
  }
  std::string userConfigDir = FileUtils::user_config_dir();
  if (FileUtils::directory_exists(userConfigDir + "/solvers")) {
    _solverPath.push_back(userConfigDir + "/solvers");
  }
  std::vector<std::string> configFiles(
      {FileUtils::global_config_file(), FileUtils::user_config_file()});

  for (auto& cf : configFiles) {
    if (!cf.empty() && FileUtils::file_exists(cf)) {
      ostringstream errstream;
      try {
        Env userconfenv;
        Model* m = nullptr;
        if (JSONParser::fileIsJSON(cf)) {
          JSONParser jp(userconfenv.envi());
          try {
            m = new Model;
            GCLock lock;
            jp.parse(m, cf);
          } catch (JSONError&) {
            delete m;
            m = nullptr;
          }
        }
        if (m != nullptr) {
          for (auto& i : *m) {
            if (auto* ai = i->dynamicCast<AssignI>()) {
              if (ai->id() == "mzn_solver_path") {
                std::vector<std::string> sp = getStringList(ai);
                for (const auto& s : sp) {
                  _solverPath.push_back(s);
                }
              } else if (ai->id() == "mzn_lib_dir") {
                _mznlibDir = getString(ai);
              } else if (ai->id() == "tagDefaults") {
                std::vector<std::pair<std::string, std::string> > tagDefs = getStringPairList(ai);
                for (auto& td : tagDefs) {
                  std::string tag = td.first;
                  std::string solver_id = td.second;
                  _tagDefault[tag] = solver_id;
                }
              } else if (ai->id() == "solverDefaults") {
                std::vector<std::vector<std::string> > solverDefs = getDefaultOptionList(ai);
                for (auto& sd : solverDefs) {
                  assert(sd.size() == 3);
                  std::string solver = sd[0];
                  auto it = _solverDefaultOptions.find(solver);
                  if (it == _solverDefaultOptions.end()) {
                    std::vector<std::string> solverOptions({sd[1], sd[2]});
                    _solverDefaultOptions.insert(std::make_pair(solver, solverOptions));
                  } else {
                    std::vector<std::string>& opts = it->second;
                    bool found = false;
                    for (unsigned int i = 0; i < opts.size(); i += 2) {
                      if (opts[i] == sd[1]) {
                        // Override existing option value
                        opts[i + 1] = sd[2];
                        found = true;
                        break;
                      }
                    }
                    if (!found) {
                      // Option didn't exist, add to end of list
                      opts.push_back(sd[1]);
                      opts.push_back(sd[2]);
                    }
                  }
                }
              } else {
                throw ConfigException("invalid configuration item");
              }
            } else {
              throw ConfigException("invalid configuration item");
            }
          }
        } else {
          std::cerr << errstream.str();
          throw ConfigException("internal error");
        }
      } catch (ConfigException& e) {
        log << "Warning: invalid configuration file: " << e.msg() << "\n";
      } catch (Exception& e) {
        log << "Warning: invalid configuration file: " << e.what() << "\n";
      }
    }
  }

  if (_mznlibDir.empty()) {
    _mznlibDir = FileUtils::file_path(FileUtils::share_directory());
  }
  if (!_mznlibDir.empty()) {
    _solverPath.push_back(_mznlibDir + "/solvers");
  }
#ifndef _MSC_VER
  if (_mznlibDir != "/usr/local/share/minizinc" &&
      FileUtils::directory_exists("/usr/local/share")) {
    _solverPath.emplace_back("/usr/local/share/minizinc/solvers");
  }
  if (_mznlibDir != "/usr/share/minizinc" && FileUtils::directory_exists("/usr/share")) {
    _solverPath.emplace_back("/usr/share/minizinc/solvers");
  }
#endif
  for (const string& cur_path : _solverPath) {
    std::vector<std::string> configFiles = FileUtils::directory_list(cur_path, "msc");
    for (auto& configFile : configFiles) {
      try {
        SolverConfig sc = SolverConfig::load(cur_path + "/" + configFile);
        addConfig(sc);
      } catch (ConfigException& e) {
        log << "Warning: error loading solver configuration from file " << cur_path << "/"
            << configFile << "\n";
        log << "Error was:\n" << e.msg() << "\n";
      }
    }
  }

  // Add default options to loaded solver configurations
  for (auto& sc : _solvers) {
    SolverDefaultMap::const_iterator it = _solverDefaultOptions.find(sc.id());
    if (it != _solverDefaultOptions.end()) {
      std::vector<std::string> defaultOptions;
      for (const auto& df : it->second) {
        if (!df.empty()) {
          defaultOptions.push_back(df);
        }
      }
      sc.defaultFlags(defaultOptions);
    }
  }
}

vector<string> SolverConfigs::solvers() const {
  // Find default solver, if present
  std::string def_id;
  auto def_it = _tagDefault.find("");
  if (def_it != _tagDefault.end()) {
    def_id = def_it->second;
  }
  // Create sorted list of solvers
  vector<string> s;
  for (const auto& sc : _solvers) {
    if (std::find(sc.tags().begin(), sc.tags().end(), "__internal__") != sc.tags().end()) {
      continue;
    }
    std::ostringstream oss;
    oss << sc.name() << " " << sc.version() << " (" << sc.id();
    if (!def_id.empty() && sc.id() == def_id) {
      oss << ", default solver";
    }
    for (const std::string& t : sc.tags()) {
      oss << ", " << t;
    }
    oss << ")";
    s.push_back(oss.str());
  }
  SortByLowercase sortByLowercase;
  std::sort(s.begin(), s.end(), sortByLowercase);
  return s;
}

std::string SolverConfigs::solverConfigsJSON() const {
  std::ostringstream oss;

  SortByName sortByName(_solvers);
  std::vector<int> solversIdx(_solvers.size());
  for (unsigned int i = 0; i < solversIdx.size(); i++) {
    solversIdx[i] = i;
  }
  std::sort(solversIdx.begin(), solversIdx.end(), sortByName);

  bool hadSolver = false;
  oss << "[";
  for (unsigned int i = 0; i < _solvers.size(); i++) {
    const SolverConfig& sc = _solvers[solversIdx[i]];
    if (std::find(sc.tags().begin(), sc.tags().end(), "__internal__") != sc.tags().end()) {
      continue;
    }
    if (hadSolver) {
      oss << ",";
    }
    hadSolver = true;
    std::istringstream iss(sc.toJSON(*this));
    std::string line;
    while (std::getline(iss, line)) {
      oss << "\n  " << line;
    }
  }
  oss << "\n]\n";
  return oss.str();
}

namespace {
std::string getTag(const std::string& t) { return t.substr(0, t.find('@')); }
std::string getVersion(const std::string& t) {
  size_t sep = t.find('@');
  return sep == string::npos ? "" : t.substr(sep + 1);
}
}  // namespace

const SolverConfig& SolverConfigs::config(const std::string& _s) {
  std::string s;
  if (_s.size() > 4 && _s.substr(_s.size() - 4) == ".msc") {
    SolverConfig sc = SolverConfig::load(_s);
    addConfig(sc);
    s = sc.id() + "@" + sc.version();
  } else {
    s = _s;
  }
  std::remove(s.begin(), s.end(), ' ');
  s = stringToLower(s);
  std::vector<std::string> tags;
  std::istringstream iss(s);
  std::string next_s;
  while (std::getline(iss, next_s, ',')) {
    tags.push_back(next_s);
  }
  std::set<std::string> defaultSolvers;
  std::set<int> selectedSolvers;

  std::string firstTag;
  if (tags.empty()) {
    DefaultMap::const_iterator def_it = _tagDefault.find("");
    if (def_it != _tagDefault.end()) {
      firstTag = def_it->second;
    } else {
      throw ConfigException("no solver selected");
    }
  } else {
    firstTag = tags[0];
  }
  TagMap::const_iterator tag_it = _tags.find(getTag(firstTag));

  if (tag_it == _tags.end()) {
    throw ConfigException("no solver with tag " + getTag(firstTag) + " found");
  }
  std::string tv = getVersion(firstTag);
  for (int sidx : tag_it->second) {
    if (tv.empty() || tv == _solvers[sidx].version()) {
      selectedSolvers.insert(sidx);
    }
  }
  DefaultMap::const_iterator def_it = _tagDefault.find(getTag(firstTag));
  if (def_it != _tagDefault.end()) {
    defaultSolvers.insert(def_it->second);
  }
  for (unsigned int i = 1; i < tags.size(); i++) {
    tag_it = _tags.find(getTag(tags[i]));
    if (tag_it == _tags.end()) {
      throw ConfigException("no solver with tag " + tags[i] + " found");
    }
    tv = getVersion(tags[i]);
    std::set<int> newSolvers;
    for (int sidx : tag_it->second) {
      if (tv.empty() || tv == _solvers[sidx].version()) {
        newSolvers.insert(sidx);
      }
    }
    std::set<int> intersection;
    std::set_intersection(selectedSolvers.begin(), selectedSolvers.end(), newSolvers.begin(),
                          newSolvers.end(), std::inserter(intersection, intersection.begin()));
    selectedSolvers = intersection;
    if (selectedSolvers.empty()) {
      throw ConfigException("no solver with tags " + s + " found");
    }
    def_it = _tagDefault.find(getTag(tags[i]));
    if (def_it != _tagDefault.end()) {
      defaultSolvers.insert(def_it->second);
    }
  }
  int selectedSolver = -1;
  if (selectedSolvers.size() > 1) {
    // use default information for the tags to select a solver
    for (int sc_idx : selectedSolvers) {
      if (defaultSolvers.find(_solvers[sc_idx].id()) != defaultSolvers.end()) {
        selectedSolver = sc_idx;
        break;
      }
    }
    if (selectedSolver == -1) {
      selectedSolver = *selectedSolvers.begin();
    }
  } else {
    selectedSolver = *selectedSolvers.begin();
  }
  return _solvers[selectedSolver];
}

void SolverConfigs::registerBuiltinSolver(const SolverConfig& sc) {
  builtinSolverConfigs().builtinSolvers.insert(make_pair(sc.id(), sc));
}

}  // namespace MiniZinc
