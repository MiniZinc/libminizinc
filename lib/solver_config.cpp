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
#include <minizinc/file_utils.hh>

#include <sstream>

using namespace std;

namespace MiniZinc {
  
  namespace {
    std::string getString(AssignI* ai) {
      if (StringLit* sl = ai->e()->dyn_cast<StringLit>()) {
        return sl->v().str();
      }
      throw ConfigException("invalid configuration item (right hand side must be string)");
    }
  }
  
  SolverConfig SolverConfig::load(string filename) {
    SolverConfig sc;
    ostringstream errstream;
    try {
      Env confenv;
      vector<string> filenames;
      filenames.push_back(filename);
      Model* m = parse(confenv,filenames, vector<string>(), vector<string>(),
                       true, false, false, errstream);
      if (m) {
        bool hadId = false;
        bool hadVersion = false;
        string basePath = FileUtils::dir_name(filename);
        for (unsigned int i=0; i<m->size(); i++) {
          if (AssignI* ai = (*m)[i]->dyn_cast<AssignI>()) {
            if (ai->id()=="id") {
              sc._id = getString(ai);
              hadId = true;
            } else if (ai->id()=="executable") {
              sc._executable = FileUtils::file_path(getString(ai), basePath);
            } else if (ai->id()=="mznlib") {
              std::string libPath = getString(ai);
              if (!libPath.empty() && libPath[0]=='-') {
                sc._mznlib = libPath;
              } else {
                sc._mznlib = FileUtils::file_path(libPath, basePath);
              }
            } else if (ai->id()=="version") {
              sc._version = getString(ai);
              hadVersion = true;
            } else if (ai->id()=="description") {
              sc._description = getString(ai);
            } else if (ai->id()=="contact") {
              sc._contact = getString(ai);
            } else if (ai->id()=="website") {
              sc._website = getString(ai);
            } else {
              throw ConfigException("invalid configuration item");
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
      } else {
        std::cerr << errstream.str();
        throw ConfigException("internal error");
      }
    } catch (ConfigException& e) {
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
  
  SolverConfigs::SolverConfigs(const string& sp) {
    string solver_path = sp;
#ifdef _MSC_VER
    const char* PATHSEP = ";";
#else
    const char* PATHSEP = ":";
#endif
    for (auto sc : builtinSolverConfigs().builtinSolvers) {
      vector<SolverConfig> configs({sc.second});
      _solvers.insert(make_pair(sc.second.id(), configs));
    }
    if (char* MZNSOLVERPATH = getenv("MZN_SOLVER_PATH")) {
      if (!solver_path.empty())
        solver_path += PATHSEP;
      solver_path += string(MZNSOLVERPATH);
    }
    std::string shareDirectory = FileUtils::share_directory();
    if (!shareDirectory.empty()) {
      if (!solver_path.empty())
        solver_path += PATHSEP;
      solver_path += shareDirectory+"/solvers";
    }
    
    while (!solver_path.empty()) {
      size_t next_sep = solver_path.find(PATHSEP);
      string cur_path = solver_path.substr(0,next_sep);
      std::vector<std::string> configFiles = FileUtils::directory_list(cur_path, "msc");
      for (unsigned int i=0; i<configFiles.size(); i++) {
        SolverConfig sc = SolverConfig::load(cur_path+"/"+configFiles[i]);
        SolverMap::iterator it = _solvers.find(sc.id());
        if (it == _solvers.end()) {
          vector<SolverConfig> configs;
          configs.push_back(sc);
          _solvers.insert(make_pair(sc.id(), configs));
        } else {
          bool found = false;
          for (auto s : it->second) {
            if (s==sc) {
              found = true;
              break;
            }
          }
          if (!found) {
            it->second.push_back(sc);
          }
        }
      }
      if (next_sep != string::npos)
        solver_path = solver_path.substr(next_sep+1,string::npos);
      else
        solver_path = "";
    }
  }
  
  vector<string> SolverConfigs::solvers() const {
    vector<string> s;
    for (SolverMap::const_iterator it = _solvers.begin(); it != _solvers.end(); ++it) {
      for (vector<SolverConfig>::const_iterator sit = it->second.begin();
           sit != it->second.end(); ++sit) {
        s.push_back(sit->id()+" "+sit->version());
      }
    }
    return s;
  }

  const SolverConfig& SolverConfigs::config(std::string solver_id, std::string version) const {
    SolverMap::const_iterator it = _solvers.find(solver_id);
    if (it == _solvers.end() || it->second.size()==0)
      throw ConfigException("solver not found");
    if (version.empty())
      return it->second[0];
    for (unsigned int i=0; i<it->second.size(); i++) {
      if (it->second[i].version()==version)
        return it->second[i];
    }
    throw ConfigException("solver version not found");
  }
  
  void
  SolverConfigs::registerBuiltinSolver(const SolverConfig& sc) {
    builtinSolverConfigs().builtinSolvers.insert(make_pair(sc.id(),sc));
  }
  
}
















