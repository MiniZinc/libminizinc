/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <string>
#include <vector>
#include <minizinc/exception.hh>
#include <unordered_map>

namespace MiniZinc {

  /**
   * \brief Configuration data for individual MiniZinc solvers
   */
  class SolverConfig {
  public:
    /// Extra command line flags supported by solver
    struct ExtraFlag {
      std::string flag;
      std::string description;
      std::string flag_type;
      std::string default_value;
      ExtraFlag(const std::string& f, const std::string& d, const std::string& t="bool", const std::string& v="false")
      : flag(f), description(d), flag_type(t), default_value(v) {}
    };
  protected:
    /// The configuration file for this solver (or empty string for built-in solvers)
    std::string _configFile;
    /// The unique identifier for the solver
    std::string _id;
    /// Name of the solver (used for output)
    std::string _name;
    /// The path to the executable
    std::string _executable;
    /// The path to the executable, after resolving
    std::string _executable_resolved;
    /// The path to the solver's MiniZinc library
    std::string _mznlib;
    /// The path to the solver's MiniZinc library, after resolving
    std::string _mznlib_resolved;
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
    /// Whether solver supports NL input
    bool _supportsNL=false;
    /// Whether solver requires solutions2out processing
    bool _needsSolns2Out=true;
    /// Whether solver is a GUI application
    bool _isGUIApplication=false;
    /// Whether solver needs path to minizinc executable (passed as --minizinc-exe)
    bool _needsMznExecutable=false;
    /// Whether solver needs path to MiniZinc standard library (passed as --stdlib-dir)
    bool _needsStdlibDir=false;
    /// Whether solver needs path to symbol table (paths file) (passed as --paths)
    bool _needsPathsFile=false;
    /// Supported standard command line flags
    std::vector<std::string> _stdFlags;
    /// Supported extra command line flags (flag and description)
    std::vector<ExtraFlag> _extraFlags;
    /// Required command line flags
    std::vector<std::string> _requiredFlags;
    /// Default command line flags (imported from global or user configuration)
    std::vector<std::string> _defaultFlags;
    /// Tags
    std::vector<std::string> _tags;
  public:
    /// Load solver configuration from \a filename
    static SolverConfig load(std::string filename);
    /// Default constructor
    SolverConfig() {}
    /// Constructor
    SolverConfig(const std::string& id, const std::string& version)
    : _id(id), _version(version) {}
    /// Return identifier
    std::string id(void) const { return _id; }
    /// Return version string
    std::string version(void) const { return _version; }

    /// Return configuration file name
    std::string configFile(void) const { return _configFile; }
    /// Set configuration file name
    void configFile(const std::string& s) { _configFile = s; }

    /// Return name
    std::string name(void) const { return _name; }
    // Set name
    void name(const std::string& s) { _name = s; }
    
    /// Return executable path
    std::string executable(void) const { return _executable; }
    /// Set executable path
    void executable(const std::string& s) { _executable = s; }

    /// Return resolved executable path
    std::string executable_resolved(void) const { return _executable_resolved; }

    /// Return MiniZinc library path
    std::string mznlib(void) const { return _mznlib; }
    /// Set MiniZinc library path
    void mznlib(const std::string& s) { _mznlib = s; }

    /// Return resolved MiniZinc library path
    std::string mznlib_resolved(void) const { return _mznlib_resolved; }

    /// Return required MiniZinc library version
    int mznlibVersion(void) const { return _mznlibVersion; }
    /// Set required MiniZinc library version
    void mznlibVersion(int i) { _mznlibVersion = i; }
    
    /// Whether solver supports MiniZinc input
    bool supportsMzn(void) const { return _supportsMzn; }
    /// Set whether solver supports MiniZinc input
    void supportsMzn(bool b) { _supportsMzn = b; }
    
    /// Whether solver supports FlatZinc input
    bool supportsFzn(void) const { return _supportsFzn; }
    /// Set whether solver supports FlatZinc input
    void supportsFzn(bool b) { _supportsFzn = b; }

    /// Whether solver supports NL input
    bool supportsNL(void) const { return _supportsNL; }
    /// Set whether solver supports NL input
    void supportsNL(bool b) { _supportsNL = b; }

    /// Whether solver requires solutions2out processing
    bool needsSolns2Out(void) const { return _needsSolns2Out; }
    /// Set whether solver requires solutions2out processing
    void needsSolns2Out(bool b) { _needsSolns2Out = b; }

    /// Whether solver is a GUI application
    bool isGUIApplication(void) const { return _isGUIApplication; }
    /// Set whether solver is a GUI application
    void isGUIApplication(bool b) { _isGUIApplication = b; }
    
    /// Whether solver needs path to minizinc executable (passed as --minizinc-exe)
    bool needsMznExecutable(void) const { return _needsMznExecutable; }
    /// Set whether solver needs path to minizinc executable
    void needsMznExecutable(bool b) { _needsMznExecutable = b; }

    /// Whether solver needs path to MiniZinc standard library (passed as --stdlib-dir)
    bool needsStdlibDir(void) const { return _needsStdlibDir; }
    /// Set whether solver needs path to MiniZinc standard library
    void needsStdlibDir(bool b) { _needsStdlibDir = b; }
    
    /// Whether solver needs path to symbol table (paths file) (passed as --paths)
    bool needsPathsFile(void) const { return _needsPathsFile; }
    /// Set whether solver needs path to symbol table (paths file)
    void needsPathsFile(bool b) { _needsPathsFile = b; }

    /// Return short description
    std::string description(void) const { return _description; }
    /// Set short description
    void description(const std::string& s) { _description = s; }
    
    /// Return contact email
    std::string contact(void) const { return _contact; }
    /// Set contact email
    void contact(const std::string& s) { _contact = s; }
    
    /// Return web site URL
    std::string website(void) const { return _website; }
    /// Set web site URL
    void website(const std::string& s) { _website = s; }
    
    /// Return supported standard command line flags
    const std::vector<std::string>& stdFlags(void) const { return _stdFlags; }
    /// Set supported standard command line flags
    void stdFlags(const std::vector<std::string>& f) { _stdFlags = f; }
    
    /// Return supported extra command line flags
    const std::vector<ExtraFlag>& extraFlags(void) const { return _extraFlags; }
    /// Set supported extra command line flags
    void extraFlags(const std::vector<ExtraFlag>& f) { _extraFlags = f; }

    /// Return supported required command line flags
    const std::vector<std::string>& requiredFlags(void) const { return _requiredFlags; }
    /// Set supported required command line flags
    void requiredFlags(const std::vector<std::string>& f) { _requiredFlags = f; }

    /// Return default command line flags
    const std::vector<std::string>& defaultFlags(void) const { return _defaultFlags; }
    /// Set default command line flags
    void defaultFlags(const std::vector<std::string>& f) { _defaultFlags = f; }

    /// Return tags
    const std::vector<std::string>& tags(void) const { return _tags; }
    /// Set tags
    void tags(const std::vector<std::string>& t) { _tags = t; }
    
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
    typedef std::unordered_map<std::string,std::vector<int> > TagMap;
    /// Mapping tags to vectors of solvers (indexed into _solvers)
    TagMap _tags;
    /// The default solver
    std::string _defaultSolver;
    /// The MiniZinc library directory
    std::string _mznlibDir;
    /// The solver configurations path
    std::vector<std::string> _solver_path;
    typedef std::unordered_map<std::string,std::string> DefaultMap;
    /// Mapping from tag to default solver for that tag
    DefaultMap _tagDefault;
    typedef std::unordered_map<std::string,std::vector<std::string> > SolverDefaultMap;
    /// Mapping from solver id to default options for that solver
    SolverDefaultMap _solverDefaultOptions;
    /// Add new solver configuration \a sc
    void addConfig(const SolverConfig& sc);
  public:
    /** \brief Constructor loading configurations from \a solverpath
     *
     * Configuration files must be called config.msc and the path
     * uses platform specific separators (: on Unix-like systems, ; on Windows).
     */
    SolverConfigs(std::ostream& log);
    /// Return configuration for solver \a s
    /// The string can be a comma separated list of tags, in which case a
    /// solver that matches all tags will be returned. The tag can also be
    /// a solver id. A concrete version can be requested using @<version>.
    /// Examples:
    ///   config("gecode@6.1.0") would request a gecode solver of version 6.1.0
    ///   config("mip,internal") would request a MIP solver that uses the internal API
    ///   config("org.minizinc.mip.coin-bc@2.9/1.16 would request a specific version of OSICBC
    const SolverConfig& config(const std::string& s);
    /// Return list of all solver ids
    std::vector<std::string> solvers(void) const;
    /// Return search path for solver configs
    std::vector<std::string> solverConfigsPath(void) const;
    /// Return JSON list of all solver configurations
    std::string solverConfigsJSON(void) const;
    /// Add a built-in solver
    static void registerBuiltinSolver(const SolverConfig& sc);
    
    /// Default solver
    const std::string& defaultSolver(void) const { return _defaultSolver; }
    /// Default solver for tag \a t
    const std::string& defaultSolver(const std::string& t) {
      static std::string noDefault;
      auto it = _tagDefault.find(t);
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
