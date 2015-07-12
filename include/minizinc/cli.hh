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
    //// default value
    KeepAlive _def;
    
    // function pointers to function to be executed for this option    
    typedef void (*func_no_args) (CLIOptions* opt);
    typedef void (*func_str_arg) (CLIOptions* opt, std::string& s);
    typedef void (*func_int_arg) (CLIOptions* opt, int v);
    typedef void (*func_str_args) (CLIOptions* opt, std::vector<std::string> args);    
    
  public:
    struct {
      func_int_arg int_arg;
      func_no_args no_args;
      func_str_arg str_arg;
      func_str_args str_args;
    } func;
    
  public:
    // TODO: extend constructor with function pointers
    CLIOption(const std::string& name, int nbArgs, bool beginsWith, KeepAlive& def) : 
    _name(name), _nbArgs(nbArgs), _beginsWith(beginsWith), _def(def) {}
    
    CLIOption(const std::string& name, int nbArgs, bool beginsWith) : 
    _name(name), _nbArgs(nbArgs), _beginsWith(beginsWith) {} // TODO: remove constructor when all function pointers are added
    CLIOption(const std::string& name, int nbArgs, bool beginsWith, CLIOption::func_no_args f) : 
    _name(name), _nbArgs(nbArgs), _beginsWith(beginsWith) { func.no_args = f; }    
    CLIOption(const std::string& name, int nbArgs, bool beginsWith, CLIOption::func_str_arg f) : 
    _name(name), _nbArgs(nbArgs), _beginsWith(beginsWith) { func.str_arg = f; }
    CLIOption(const std::string& name, int nbArgs, bool beginsWith, CLIOption::func_int_arg f) : 
    _name(name), _nbArgs(nbArgs), _beginsWith(beginsWith) { func.int_arg = f; }
    
    
    bool takesArgs(void) { return _nbArgs > 0; }
    const int getNbArgs(void) const { return _nbArgs; }
  };

  /// parser for command line arguments for MiniZinc
  class CLIParser {
  protected:
    /// the options that are recognized by MiniZinc
    UNORDERED_NAMESPACE::unordered_map<std::string, CLIOption*> _known_options;
    
  public:
    /// initiates the default MiniZinc CLI options
    CLIParser(void); 
    /// parses the command line arguments and stores them in CLIOptions that is returned
    CLIOptions* parseCLI(int argc, char** argv);
    
  protected:
    /// creates the default MiniZinc CLI options and enters them into the _known_options map
    void generateDefaultCLIOptions(void);
    /// returns true if the given option \a opt (such as "-h") is part of the known options
    bool knowsOption(const std::string& opt) const;
    /// returns pointer to the CLIOption object that represents CLI option \a name
    CLIOption* getCLIOption(const std::string& name) const;
    /// default functionality that will be executed when a CLI error occurred
    void error(void);
  };
}

#endif
