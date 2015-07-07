/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:    
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_CLI_HH__
#define __MINIZINC_CLI_HH__

#include <minizinc/options.hh>

namespace MiniZinc {
 
  // Options for the command line interface
  class CLIOptions : public Options {
  public:
    void setStringParam(const std::string& name, KeepAlive e);
    void setStringParam(const std::string& name, std::string& e);
    std::string getStringParam(const std::string& name) const;
    std::string getStringParam(const std::string& name, std::string& def) const; 
    
    // NOTE: setting string vector parameters is very expensive since they are converted into ArrayLits/ASTExprVecs and back
    void setStringVectorParam(const std::string& name, KeepAlive e);
    void setStringVectorParam(const std::string& name, const std::vector<std::string>& e);
    std::vector<std::string> getStringVectorParam(const std::string& name) const;
    std::vector<std::string> getStringVectorParam(const std::string& name, std::vector<std::string>& def) const; 
  };
  
  /// class for each CLI option that MiniZinc recognizes
  class CLIOption {
  protected:
    /// option name as given in the command line, e.g. \"--help\"
    const std::string _name;
    /// the number of arguments of the option
    int _nbArgs;
    /// the arg string can begin with the name, e.g. -I/home/user/mydir/
    bool _beginsWith;
    /// the value the option has been set to via the command line
    KeepAlive _value;
    //// default value
    KeepAlive _def;
  public:
    CLIOption(const std::string& name, int nbArgs, bool beginsWith, KeepAlive def) : 
    _name(name), _nbArgs(nbArgs), _beginsWith(beginsWith), _def(def) {}
    
    bool setValue(std::string v);
    bool setValue(int v);
    bool setValue(bool v);
    bool setValue(float f);
    bool setValue(std::vector<std::string> v);
  };

  /// parser for command line arguments for MiniZinc
  class CLIParser {
  protected:
    /// the options that are recognized by MiniZinc
    UNORDERED_NAMESPACE::unordered_map<std::string, CLIOption&> _known_options;
    
  public:
    /// initiates the default MiniZinc CLI options
    CLIParser(void); 
    /// parses the command line arguments and stores them in CLIOptions that is returned
    CLIOptions& parseCLI(int argc, char** argv);
  };
}

#endif
