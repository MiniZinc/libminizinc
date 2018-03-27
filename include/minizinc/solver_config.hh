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
    /// The short name by which the solver can be identified
    std::string _id;
    /// The path to the executable
    std::string _executable;
    /// The path to the solver's MiniZinc library
    std::string _mznlib;
    /// Version string
    std::string _version;
    /// Short description
    std::string _description;
    /// Contact email
    std::string _contact;
    /// URL for more information
    std::string _website;
  public:
    /// Load solver configuration from \a filename
    static SolverConfig load(std::string filename);
    /// Return identifier
    std::string id(void) const { return _id; }
    /// Return executable path
    std::string executable(void) const { return _executable; }
    /// Return MiniZinc library path
    std::string mznlib(void) const { return _mznlib; }
    /// Return version string
    std::string version(void) const { return _version; }
    /// Return short description
    std::string description(void) const { return _description; }
    /// Return contact email
    std::string contact(void) const { return _contact; }
    /// Return web site URL
    std::string website(void) const { return _website; }
  };
  
  /// A container for solver configurations
  class SolverConfigs {
  protected:
    typedef UNORDERED_NAMESPACE::unordered_map<std::string,std::vector<SolverConfig> > SolverMap;
    /// The solvers (mapping id to configuration)
    SolverMap _solvers;
  public:
    /** \brief Constructor loading configurations from \a solverpath
     *
     * Configuration files must be called config.msc and the path
     * uses platform specific separators (: on Unix-like systems, ; on Windows).
     */
    SolverConfigs(const std::string& solverpath);
    const SolverConfig& config(std::string solver_id, std::string version=std::string()) const;
    std::vector<std::string> solvers(void) const;
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
