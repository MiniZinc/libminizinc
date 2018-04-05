/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_SOLVER_CONFIG_HH__
#define __MINIZINC_SOLVER_CONFIG_HH__

#include <string>
#include <vector>
#include <minizinc/stl_map_set.hh>
#include <minizinc/exception.hh>

namespace MiniZinc {

  /**
   * \brief Configuration data for individual MiniZinc solvers
   */
  class SolverConfig {
  protected:
    /// The unique identifier for the solver
    std::string _id;
    /// Name of the solver (used for output)
    std::string _name;
    /// The path to the executable
    std::string _executable;
    /// The path to the solver's MiniZinc library
    std::string _mznlib;
    /// Version string
    std::string _version;
    /// MiniZinc library version
    int _mznlibVersion=1;
    /// Short description
    std::string _description;
    /// Contact email
    std::string _contact;
    /// URL for more information
    std::string _website;
    /// Whether solver supports MiniZinc input
    bool _supportsMzn=false;
    /// Whether solver supports FlatZinc input
    bool _supportsFzn=true;
    /// Whether solver requires solutions2out processing
    bool _needsSolns2Out=true;
    /// Supported standard command line flags
    std::vector<std::string> _stdFlags;
    /// Supported extra command line flags (flag and description)
    std::vector<std::pair<std::string,std::string> > _extraFlags;
    /// Tags
    std::vector<std::string> _tags;
  public:
    /// Load solver configuration from \a filename
    static SolverConfig load(std::string filename);
    /// Default constructor
    SolverConfig() {}
    /// Constructor
    SolverConfig(const std::string& id, const std::string& name, const std::string& executable, const std::string& mznlib, int mznlibVersion,
                 const std::string& version,
                 bool mzn, bool fzn, bool s2o,
                 const std::string& description, const std::string& contact,
                 const std::string& website,
                 const std::vector<std::string>& stdFlags = std::vector<std::string>(),
                 const std::vector<std::pair<std::string,std::string> > extraFlags = std::vector<std::pair<std::string,std::string> >(),
                 const std::vector<std::string>& tags = std::vector<std::string>())
    : _id(id), _name(name), _executable(executable), _mznlib(mznlib), _version(version), _mznlibVersion(mznlibVersion),
      _description(description), _contact(contact), _website(website),
      _supportsMzn(mzn), _supportsFzn(fzn), _needsSolns2Out(s2o),
      _stdFlags(stdFlags), _extraFlags(extraFlags), _tags(tags) {}
    /// Return identifier
    std::string id(void) const { return _id; }
    /// Return name
    std::string name(void) const { return _name; }
    /// Return executable path
    std::string executable(void) const { return _executable; }
    /// Return MiniZinc library path
    std::string mznlib(void) const { return _mznlib; }
    /// Return version string
    std::string version(void) const { return _version; }
    /// Return required MiniZinc library version
    int mznlibVersion(void) const { return _mznlibVersion; }
    /// Whether solver supports MiniZinc input
    bool supportsMzn(void) const { return _supportsMzn; }
    /// Whether solver supports FlatZinc input
    bool supportsFzn(void) const { return _supportsFzn; }
    /// Whether solver requires solutions2out processing
    bool needsSolns2Out(void) const { return _needsSolns2Out; }
    /// Return short description
    std::string description(void) const { return _description; }
    /// Return contact email
    std::string contact(void) const { return _contact; }
    /// Return web site URL
    std::string website(void) const { return _website; }
    /// Return supported standard command line flags
    const std::vector<std::string>& stdFlags(void) const { return _stdFlags; }
    /// Return supported extra command line flags
    const std::vector<std::pair<std::string,std::string> >& extraFlags(void) const { return _extraFlags; }
    /// Return tags
    const std::vector<std::string>& tags(void) const { return _tags; }
    /// Test equality
    bool operator==(const SolverConfig& sc) const {
      return _id==sc.id() && _version==sc.version();
    }
  };
  
  /// A container for solver configurations
  class SolverConfigs {
  protected:
    /// The solvers
    std::vector<SolverConfig> _solvers;
    typedef UNORDERED_NAMESPACE::unordered_map<std::string,std::vector<int> > TagMap;
    /// Mapping tags to vectors of solvers (indexed into _solvers)
    TagMap _tags;
    /// The default solver
    std::string _defaultSolver;
    /// The MiniZinc library directory
    std::string _mznlibDir;
    typedef UNORDERED_NAMESPACE::unordered_map<std::string,std::string> TagDefaultMap;
    /// Mapping from tag to default solver for that tag
    TagDefaultMap _tagDefault;
    /// Add new solver configuration \a sc
    void addConfig(const SolverConfig& sc);
  public:
    /** \brief Constructor loading configurations from \a solverpath
     *
     * Configuration files must be called config.msc and the path
     * uses platform specific separators (: on Unix-like systems, ; on Windows).
     */
    SolverConfigs(const std::string& solverpath=std::string());
    /// Return configuration for solver \a s
    /// The string can be a comma separated list of tags, in which case a
    /// solver that matches all tags will be returned. The tag can also be
    /// a solver id. A concrete version can be requested using @<version>.
    /// Examples:
    ///   config("gecode@6.1.0") would request a gecode solver of version 6.1.0
    ///   config("mip,internal") would request a MIP solver that uses the internal API
    ///   config("org.minizinc.mip.osicbc@2.9/1.16 would request a specific version of OSICBC
    const SolverConfig& config(const std::string& s) const;
    /// Return list of all solver ids
    std::vector<std::string> solvers(void) const;
    /// Return JSON list of all solver configurations
    std::string solverConfigsJSON(void) const;
    /// Add a built-in solver
    static void registerBuiltinSolver(const SolverConfig& sc);
    
    /// Default solver
    const std::string& defaultSolver(void) const { return _defaultSolver; }
    /// Default solver for tag \a t
    const std::string& defaultSolver(const std::string& t) {
      static std::string noDefault;
      std::unordered_map<std::string, std::string>::const_iterator it = _tagDefault.find(t);
      return it==_tagDefault.end() ? noDefault : it->second;
    }
    /// MiniZinc library directory
    const std::string& mznlibDir(void) const { return _mznlibDir; }
  };

  /// An exception thrown when encountering an error in a solver configuration
  class ConfigException : public Exception {
  public:
    /// Construct with message \a msg
    ConfigException(const std::string& msg) : Exception(msg) {}
    /// Destructor
    ~ConfigException(void) throw() {}
    /// Return description
    virtual const char* what(void) const throw() {
      return "MiniZinc: configuration error";
    }
    
  };
}

#endif
