/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solver_config.hh>
#include <minizinc/parser.hh>
#include <minizinc/json_parser.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/prettyprinter.hh>

#include <sstream>
#include <algorithm>
#include <iterator>
#include <string>
#include <set>
#include <cctype>

using namespace std;

namespace MiniZinc {
  
  namespace {
    std::string getString(AssignI* ai) {
      if (StringLit* sl = ai->e()->dyn_cast<StringLit>()) {
        return sl->v().str();
      }
      throw ConfigException("invalid configuration item (right hand side must be string)");
    }
    bool getBool(AssignI* ai) {
      if (BoolLit* bl = ai->e()->dyn_cast<BoolLit>()) {
        return bl->v();
      }
      throw ConfigException("invalid configuration item (right hand side must be bool)");
    }
    int getInt(AssignI* ai) {
      if (IntLit* il = ai->e()->dyn_cast<IntLit>()) {
        return static_cast<int>(il->v().toInt());
      }
      throw ConfigException("invalid configuration item (right hand side must be int)");
    }
    std::vector<std::string> getStringList(AssignI* ai) {
      if (ArrayLit* al = ai->e()->dyn_cast<ArrayLit>()) {
        std::vector<std::string> ret;
        for (unsigned int i=0; i<al->size(); i++) {
          if (StringLit* sl = (*al)[i]->dyn_cast<StringLit>()) {
            ret.push_back(sl->v().str());
          } else {
            throw ConfigException("invalid configuration item (right hand side must be a list of strings)");
          }
        }
        return ret;
      }
      throw ConfigException("invalid configuration item (right hand side must be a list of strings)");
    }
    std::vector<std::vector<std::string> > getDefaultOptionList(AssignI* ai) {
      if (ArrayLit* al = ai->e()->dyn_cast<ArrayLit>()) {
        std::vector<std::vector<std::string> > ret;
        if (al->size()==0)
          return ret;
        if (al->dims()!=2) {
          throw ConfigException("invalid configuration item (right hand side must be a 2d array of strings)");
        }
        int nCols = al->max(1)-al->min(1)+1;
        if (nCols != 3) {
          throw ConfigException("invalid configuration item (right hand side must be a 2d array of strings with 3 columns)");
        }
        for (unsigned int i=0; i<al->size(); i+=nCols) {
          StringLit* sl0 = (*al)[i]->dyn_cast<StringLit>();
          StringLit* sl1 = (*al)[i+1]->dyn_cast<StringLit>();
          StringLit* sl2 = (*al)[i+2]->dyn_cast<StringLit>();
          if (sl0 && sl1 && sl2) {
            ret.push_back(std::vector<std::string>({sl0->v().str(),sl1->v().str(),sl2->v().str()}));
          } else {
            throw ConfigException("invalid configuration item (right hand side must be a list of strings)");
          }
        }
        return ret;
      }
      throw ConfigException("invalid configuration item (right hand side must be a 2d array of strings)");
    }
    std::vector<SolverConfig::ExtraFlag> getExtraFlagList(AssignI* ai) {
      if (ArrayLit* al = ai->e()->dyn_cast<ArrayLit>()) {
        std::vector<SolverConfig::ExtraFlag> ret;
        if (al->size()==0)
          return ret;
        if (al->dims()!=2) {
          throw ConfigException("invalid configuration item (right hand side must be a 2d array of strings)");
        }
        int nCols = al->max(1)-al->min(1)+1;
        if (nCols < 2 || nCols > 4) {
          throw ConfigException("invalid configuration item (right hand side must be a 2d array of strings)");
        }
        bool haveType = (nCols == 3);
        bool haveDefault = (nCols == 4);
        for (unsigned int i=0; i<al->size(); i+=nCols) {
          StringLit* sl1 = (*al)[i]->dyn_cast<StringLit>();
          StringLit* sl2 = (*al)[i+1]->dyn_cast<StringLit>();
          StringLit* sl3 = haveType ? (*al)[i+2]->dyn_cast<StringLit>() : NULL;
          StringLit* sl4 = haveDefault ? (*al)[i+3]->dyn_cast<StringLit>() : NULL;
          if (sl1 && sl2) {
            ret.emplace_back(sl1->v().str(),sl2->v().str(),sl3 ? sl3->v().str() : "bool",sl4 ? sl4->v().str() : "false");
          } else {
            throw ConfigException("invalid configuration item (right hand side must be a 2d array of strings)");
          }
        }
        return ret;
      }
      throw ConfigException("invalid configuration item (right hand side must be a 2d array of strings)");
    }

    std::string getEnv(const char* v) {
      std::string ret;
#ifdef _MSC_VER
      size_t len;
      getenv_s(&len, NULL, 0, v);
      if (len > 0) {
        char* p = static_cast<char*>(malloc(len*sizeof(char)));
        getenv_s(&len, p, len, v);
        if (len > 0) {
          ret = p;
        }
        free(p);
      }
#else
      char* p = getenv(v);
      if (p) {
        ret = p;
      }
#endif
      return ret;
    }
    
    char charToLower(char c) {
      return std::tolower(static_cast<unsigned char>(c));
    }
    std::string stringToLower(std::string s) {
      std::transform(s.begin(), s.end(), s.begin(), charToLower);
      return s;
    }
    struct SortByLowercase {
      bool operator ()(const std::string& n1, const std::string& n2) {
        for (size_t i=0; i<n1.size() && i<n2.size(); i++) {
          if (std::tolower(n1[i]) != std::tolower(n2[i]))
            return std::tolower(n1[i]) < std::tolower(n2[i]);
        }
        return n1.size() < n2.size();
      }
    };
    struct SortByName {
      const std::vector<SolverConfig>& solvers;
      SortByLowercase sortByLowercase;
      SortByName(const std::vector<SolverConfig>& solvers0) : solvers(solvers0) {}
      bool operator ()(int idx1, int idx2) {
        return sortByLowercase(solvers[idx1].name(), solvers[idx2].name());
      }
    };

  }
  
  SolverConfig SolverConfig::load(string filename) {
    SolverConfig sc;
    sc._configFile = FileUtils::file_path(filename);
    ostringstream errstream;
    try {
      Env confenv;
      Model* m = NULL;
      if (JSONParser::fileIsJSON(filename)) {
        JSONParser jp(confenv.envi());
        try {
          m = new Model;
          GCLock lock;
          jp.parse(m, filename);
        } catch (JSONError& e) {
          delete m;
          m=NULL;
          throw ConfigException(e.msg());
        }
      } else {
        vector<string> filenames;
        filenames.push_back(filename);
        m = parse(confenv,filenames, vector<string>(), vector<string>(),
                  true, false, false, errstream);
      }
      if (m) {
        bool hadId = false;
        bool hadVersion = false;
        bool hadName = false;
        string basePath = FileUtils::dir_name(sc._configFile);
        for (unsigned int i=0; i<m->size(); i++) {
          if (AssignI* ai = (*m)[i]->dyn_cast<AssignI>()) {
            if (ai->id()=="id") {
              sc._id = getString(ai);
              hadId = true;
            } else if (ai->id()=="name") {
              sc._name = getString(ai);
              hadName = true;
            } else if (ai->id()=="executable") {
              std::string exePath = getString(ai);
              sc._executable = exePath;
              std::string exe;
              if (exePath.size() > 2 && exePath[0]=='.' && (exePath[1]=='/' || (exePath[1]=='.' && exePath[2]=='/'))) {
                exe = FileUtils::find_executable(FileUtils::file_path(getString(ai), basePath));
              } else {
                exe = FileUtils::find_executable(exePath);
              }
              if (!exe.empty()) {
                sc._executable_resolved = exe;
              }
            } else if (ai->id()=="mznlib") {
              std::string libPath = getString(ai);
              sc._mznlib = libPath;
              if (!libPath.empty()) {
                if (libPath[0]=='-') {
                  sc._mznlib_resolved = libPath;
                } else {
                  sc._mznlib_resolved = FileUtils::file_path(libPath, basePath);
                }
              }
            } else if (ai->id()=="version") {
              sc._version = getString(ai);
              hadVersion = true;
            } else if (ai->id()=="mznlibVersion") {
              sc._mznlibVersion = getInt(ai);
            } else if (ai->id()=="description") {
              sc._description = getString(ai);
            } else if (ai->id()=="contact") {
              sc._contact = getString(ai);
            } else if (ai->id()=="website") {
              sc._website = getString(ai);
            } else if (ai->id()=="supportsMzn") {
              sc._supportsMzn = getBool(ai);
            } else if (ai->id()=="supportsFzn") {
              sc._supportsFzn = getBool(ai);
            } else if (ai->id()=="needsSolns2Out") {
              sc._needsSolns2Out = getBool(ai);
            } else if (ai->id()=="isGUIApplication") {
              sc._isGUIApplication = getBool(ai);
            } else if (ai->id()=="needsMznExecutable") {
              sc._needsMznExecutable = getBool(ai);
            } else if (ai->id()=="needsStdlibDir") {
              sc._needsStdlibDir = getBool(ai);
            } else if (ai->id()=="needsPathsFile") {
              sc._needsPathsFile = getBool(ai);
            } else if (ai->id()=="tags") {
              sc._tags = getStringList(ai);
            } else if (ai->id()=="stdFlags") {
              sc._stdFlags = getStringList(ai);
            } else if (ai->id()=="requiredFlags") {
              sc._requiredFlags = getStringList(ai);
            } else if (ai->id()=="extraFlags") {
              sc._extraFlags = getExtraFlagList(ai);
            } else {
              throw ConfigException("invalid configuration item ("+ai->id().str()+")");
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
 
  class BuiltinSolverConfigs {
  public:
    std::unordered_map<std::string, SolverConfig> builtinSolvers;
  };
  
  BuiltinSolverConfigs& builtinSolverConfigs(void) {
    static BuiltinSolverConfigs c;
    return c;
  }
  
  void SolverConfigs::addConfig(const MiniZinc::SolverConfig& sc) {
    int newIdx = static_cast<int>(_solvers.size());
    _solvers.push_back(sc);
    std::vector<string> sc_tags = sc.tags();
    sc_tags.push_back(sc.id());
    std::string name = sc.name();
    name = stringToLower(name);
    sc_tags.push_back(name);
    for (auto t : sc_tags) {
      TagMap::iterator it = _tags.find(t);
      if (it==_tags.end()) {
        _tags.insert(std::make_pair(t,std::vector<int>({newIdx})));
      } else {
        it->second.push_back(newIdx);
      }
    }
  }
  
  SolverConfigs::SolverConfigs(std::ostream& log, const string& sp) {
    string solver_path = sp;
#ifdef _MSC_VER
    const char* PATHSEP = ";";
#else
    const char* PATHSEP = ":";
#endif
    for (auto sc : builtinSolverConfigs().builtinSolvers) {
      addConfig(sc.second);
    }
    std::string mzn_solver_path = getEnv("MZN_SOLVER_PATH");
    if (mzn_solver_path.size() != 0) {
      if (!solver_path.empty())
        solver_path += PATHSEP;
      solver_path += mzn_solver_path;
    }
    std::string userConfigDir = FileUtils::user_config_dir();
    if (FileUtils::directory_exists(userConfigDir+"/solvers")) {
      if (!solver_path.empty())
        solver_path += PATHSEP;
      solver_path += userConfigDir+"/solvers";
    }
    std::vector<std::string> configFiles({FileUtils::global_config_file(),FileUtils::user_config_file()});
    
    for (auto& cf : configFiles) {
      if (!cf.empty() && FileUtils::file_exists(cf)) {
        ostringstream errstream;
        try {
          Env userconfenv;
          Model* m = NULL;
          if (JSONParser::fileIsJSON(cf)) {
            JSONParser jp(userconfenv.envi());
            try {
              m = new Model;
              GCLock lock;
              jp.parse(m, cf);
            } catch (JSONError&) {
              delete m;
              m=NULL;
            }
          }
          if (m) {
            for (unsigned int i=0; i<m->size(); i++) {
              if (AssignI* ai = (*m)[i]->dyn_cast<AssignI>()) {
                if (ai->id()=="mzn_solver_path") {
                  std::string sp = getString(ai);
                  if (!solver_path.empty())
                    solver_path += PATHSEP;
                  solver_path += sp;
                } else if (ai->id()=="mzn_lib_dir") {
                  _mznlibDir = getString(ai);
                } else if (ai->id()=="tagDefaults") {
                  std::vector<std::string> tagDefs = getStringList(ai);
                  for (string td: tagDefs) {
                    size_t sep = td.find(':');
                    if (sep==string::npos) {
                      throw ConfigException("invalid configuration item: tag default without colon");
                    }
                    std::string tag = td.substr(0,sep);
                    std::string solver_id = td.substr(sep+1);
                    _tagDefault[tag] = solver_id;
                  }
                } else if (ai->id()=="solverDefaults") {
                  std::vector<std::vector<std::string> > solverDefs = getDefaultOptionList(ai);
                  for (auto& sd: solverDefs) {
                    assert(sd.size()==3);
                    std::string solver = sd[0];
                    SolverDefaultMap::iterator it = _solverDefaultOptions.find(solver);
                    if (it==_solverDefaultOptions.end()) {
                      std::vector<std::string> solverOptions({sd[1],sd[2]});
                      _solverDefaultOptions.insert(std::make_pair(solver, solverOptions));
                    } else {
                      std::vector<std::string>& opts = it->second;
                      bool found=false;
                      for (unsigned int i=0; i<opts.size(); i+=2) {
                        if (opts[i]==sd[1]) {
                          // Override existing option value
                          opts[i+1] = sd[2];
                          found=true;
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
      _mznlibDir = FileUtils::share_directory();
    }
    if (!_mznlibDir.empty()) {
      if (!solver_path.empty())
        solver_path += PATHSEP;
      solver_path += _mznlibDir+"/solvers";
    }
    
    while (!solver_path.empty()) {
      size_t next_sep = solver_path.find(PATHSEP);
      string cur_path = solver_path.substr(0,next_sep);
      std::vector<std::string> configFiles = FileUtils::directory_list(cur_path, "msc");
      for (unsigned int i=0; i<configFiles.size(); i++) {
        try {
          SolverConfig sc = SolverConfig::load(cur_path+"/"+configFiles[i]);
          addConfig(sc);
        } catch (ConfigException& e) {
          log << "Warning: error loading solver configuration from file " << cur_path << "/" << configFiles[i] << "\n";
          log << "Error was:\n" << e.msg() << "\n";
        }
      }
      if (next_sep != string::npos)
        solver_path = solver_path.substr(next_sep+1,string::npos);
      else
        solver_path = "";
    }
    
    // Add default options to loaded solver configurations
    for (auto& sc : _solvers) {
      SolverDefaultMap::const_iterator it = _solverDefaultOptions.find(sc.id());
      if (it != _solverDefaultOptions.end()) {
        std::vector<std::string> defaultOptions;
        for (auto& df : it->second) {
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
    DefaultMap::const_iterator def_it = _tagDefault.find("");
    if (def_it != _tagDefault.end()) {
      def_id = def_it->second;
    }
    // Create sorted list of solvers
    vector<string> s;
    for (auto& sc: _solvers) {
      if (std::find(sc.tags().begin(), sc.tags().end(), "__internal__") != sc.tags().end())
        continue;
      std::ostringstream oss;
      oss << sc.name() << " " << sc.version() << " (" << sc.id();
      if (!def_id.empty() && sc.id()==def_id) {
        oss << ", default solver";
      }
      for (std::string t: sc.tags()) {
        oss << ", " << t;
      }
      oss << ")";
      s.push_back(oss.str());
    }
    SortByLowercase sortByLowercase;
    std::sort(s.begin(),s.end(), sortByLowercase);
    return s;
  }

  std::string SolverConfigs::solverConfigsJSON() const {
    GCLock lock;
    std::ostringstream oss;
    
    // Find default solver, if present
    std::string def_id;
    DefaultMap::const_iterator def_it = _tagDefault.find("");
    if (def_it != _tagDefault.end()) {
      def_id = def_it->second;
    }
    
    SortByName sortByName(_solvers);
    std::vector<int> solversIdx(_solvers.size());
    for (unsigned int i=0; i<solversIdx.size(); i++) {
      solversIdx[i] = i;
    }
    std::sort(solversIdx.begin(), solversIdx.end(), sortByName);
    
    oss << "[\n";
    for (unsigned int i=0; i<_solvers.size(); i++) {
      const SolverConfig& sc = _solvers[solversIdx[i]];
      if (std::find(sc.tags().begin(), sc.tags().end(), "__internal__") != sc.tags().end())
        continue;
      oss << "  {\n";
      oss << "    \"extraInfo\": {\n";
      if (!def_id.empty() && def_id==sc.id()) {
        oss << "      \"isDefault\": true,\n";
      }
      if (sc.mznlib_resolved().size()) {
        oss << "      \"mznlib\": \"" << Printer::escapeStringLit(sc.mznlib_resolved()) << "\",\n";
      }
      if (sc.executable_resolved().size()) {
        oss << "      \"executable\": \"" << Printer::escapeStringLit(sc.executable_resolved()) << "\",\n";
      }
      oss << "      \"configFile\": \"" << Printer::escapeStringLit(sc.configFile()) << "\"";
      if (sc.defaultFlags().size()) {
        oss << ",\n      \"defaultFlags\": [";
        for (unsigned int j=0; j<sc.defaultFlags().size(); j++) {
          oss << "\"" << sc.defaultFlags()[j] << "\"";
          if (j<sc.defaultFlags().size()-1)
            oss << ",";
        }
        oss << "]";
      }
      oss << "\n";
      oss << "    },\n";
      oss << "    \"id\": \"" << Printer::escapeStringLit(sc.id()) << "\",\n";
      oss << "    \"name\": \"" << Printer::escapeStringLit(sc.name()) << "\",\n";
      oss << "    \"version\": \"" << Printer::escapeStringLit(sc.version()) << "\",\n";
      if (sc.mznlib().size()) {
        oss << "    \"mznlib\": \"" << Printer::escapeStringLit(sc.mznlib()) << "\",\n";
      }
      if (sc.executable().size()) {
        oss << "    \"executable\": \"" << Printer::escapeStringLit(sc.executable()) << "\",\n";
      }
      oss << "    \"mznlibVersion\": " << sc.mznlibVersion() << ",\n";
      if (sc.description().size()) {
        oss << "    \"description\": \"" << Printer::escapeStringLit(sc.description()) << "\",\n";
      }
      if (sc.contact().size()) {
        oss << "    \"contact\": \"" << Printer::escapeStringLit(sc.contact()) << "\",\n";
      }
      if (sc.website().size()) {
        oss << "    \"website\": \"" << Printer::escapeStringLit(sc.website()) << "\",\n";
      }
      if (sc.requiredFlags().size()) {
        oss << "    \"requiredFlags\": [";
        for (unsigned int j=0; j<sc.requiredFlags().size(); j++) {
          oss << "\"" << sc.requiredFlags()[j] << "\"";
          if (j<sc.requiredFlags().size()-1)
            oss << ",";
        }
        oss << "],\n";
      }
      if (sc.stdFlags().size()) {
        oss << "    \"stdFlags\": [";
        for (unsigned int j=0; j<sc.stdFlags().size(); j++) {
          oss << "\"" << sc.stdFlags()[j] << "\"";
          if (j<sc.stdFlags().size()-1)
            oss << ",";
        }
        oss << "],\n";
      }
      if (sc.extraFlags().size()) {
        oss << "    \"extraFlags\": [";
        for (unsigned int j=0; j<sc.extraFlags().size(); j++) {
          oss << "[" << "\"" << sc.extraFlags()[j].flag << "\",\"" << sc.extraFlags()[j].description << "\",\"";
          oss << sc.extraFlags()[j].flag_type  << "\",\"" << sc.extraFlags()[j].default_value << "\"]";
          if (j<sc.extraFlags().size()-1)
            oss << ",";
        }
        oss << "],\n";
      }

      if (sc.tags().size()) {
        oss << "    \"tags\": [";
        for (unsigned int j=0; j<sc.tags().size(); j++) {
          oss << "\"" << Printer::escapeStringLit(sc.tags()[j]) << "\"";
          if (j<sc.tags().size()-1)
            oss << ",";
        }
        oss << "],\n";
      }
      oss << "    \"supportsMzn\": " << (sc.supportsMzn() ? "true" : "false") << ",\n";
      oss << "    \"supportsFzn\": " << (sc.supportsFzn() ? "true" : "false") << ",\n";
      oss << "    \"needsSolns2Out\": " << (sc.needsSolns2Out()? "true" : "false") << ",\n";
      oss << "    \"needsMznExecutable\": " << (sc.needsMznExecutable()? "true" : "false") << ",\n";
      oss << "    \"needsStdlibDir\": " << (sc.needsStdlibDir()? "true" : "false") << ",\n";
      oss << "    \"needsPathsFile\": " << (sc.needsPathsFile()? "true" : "false") << ",\n";
      oss << "    \"isGUIApplication\": " << (sc.isGUIApplication()? "true" : "false") << "\n";
      oss << "  }" << (i<_solvers.size()-1 ? ",\n" : "\n");
    }
    oss << "]\n";
    return oss.str();
  }
  
  namespace {
    std::string getTag(const std::string& t) {
      return t.substr(0,t.find('@'));
    }
    std::string getVersion(const std::string& t) {
      size_t sep = t.find('@');
      return sep==string::npos ? "" : t.substr(sep+1);
    }
  }
  
  const SolverConfig& SolverConfigs::config(const std::string& _s) {
    std::string s;
    if (_s.size() > 4 && _s.substr(_s.size()-4)==".msc") {
      SolverConfig sc = SolverConfig::load(_s);
      addConfig(sc);
      s = sc.id()+"@"+sc.version();
    } else {
      s = _s;
    }
    std::remove(s.begin(),s.end(),' ');
    s = stringToLower(s);
    std::vector<std::string> tags;
    std::istringstream iss(s);
    std::string next_s;
    while (std::getline(iss,next_s,',')) {
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
      throw ConfigException("no solver with tag "+firstTag+" found");
    }
    std::string tv = getVersion(firstTag);
    for (int sidx: tag_it->second) {
      if (tv.empty() || tv==_solvers[sidx].version())
        selectedSolvers.insert(sidx);
    }
    DefaultMap::const_iterator def_it = _tagDefault.find(getTag(firstTag));
    if (def_it != _tagDefault.end()) {
      defaultSolvers.insert(def_it->second);
    }
    for (unsigned int i=1; i<tags.size(); i++) {
      tag_it = _tags.find(getTag(tags[i]));
      if (tag_it == _tags.end()) {
        throw ConfigException("no solver with tag "+tags[i]+" found");
      }
      tv = getVersion(tags[i]);
      std::set<int> newSolvers;
      for (int sidx: tag_it->second) {
        if (tv.empty() || tv==_solvers[sidx].version())
          newSolvers.insert(sidx);
      }
      std::set<int> intersection;
      std::set_intersection(selectedSolvers.begin(),selectedSolvers.end(),
                            newSolvers.begin(),newSolvers.end(),
                            std::inserter(intersection, intersection.begin()));
      selectedSolvers = intersection;
      if (selectedSolvers.empty()) {
        throw ConfigException("no solver with tags "+s+" found");
      }
      def_it = _tagDefault.find(getTag(tags[i]));
      if (def_it != _tagDefault.end()) {
        defaultSolvers.insert(def_it->second);
      }
    }
    int selectedSolver=-1;
    if (selectedSolvers.size()>1) {
      // use default information for the tags to select a solver
      for (int sc_idx : selectedSolvers) {
        if (defaultSolvers.find(_solvers[sc_idx].id()) != defaultSolvers.end()) {
          selectedSolver = sc_idx;
          break;
        }
      }
      if (selectedSolver==-1) {
        selectedSolver = *selectedSolvers.begin();
      }
    } else {
      selectedSolver = *selectedSolvers.begin();
    }
    return _solvers[selectedSolver];
  }
  
  void
  SolverConfigs::registerBuiltinSolver(const SolverConfig& sc) {
    builtinSolverConfigs().builtinSolvers.insert(make_pair(sc.id(),sc));
  }
  
}
















