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
    /// default boolean value
    bool _bdef;
    /// default string value
    std::string _sdef;
    /// the string under which the option value will be stored in the CLIOptions map
    const std::string _optMapString;
    
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
    /**
       * Constructor for a Boolean command line option whose value will later be stored 
       * in CLIOptions. The number of arguments (on the command line) to the option are 
       * given through the function pointer type: this constructor is for Boolean options 
       * that take no arguments on the command line.
       * 
       * @param name the command line string for the option, e.g. "--verbose"         
       * @param def default value for the option (defined by optMapString!)
       * @param optMapString the string that (will) map to the option value in CLIOptions, e.g. "verbose"
       * @param f the function to be executed when this option is given in the command line
       */  
    CLIOption(const std::string& name, bool def, const std::string& optMapString, CLIOption::func_no_args f) : 
      _name(name), _bdef(def), _optMapString(optMapString) 
      { func.no_args = f; _nbArgs = 0; _beginsWith = false; }   
    
    /**
       * Constructor for a Boolean command line option that whose value does not need 
       * to be stored in CLIOptions. The Boolean option has not command line parameter.
       * 
       * @param name the command line string for the option, e.g. --version  
       * @param def default value for the option (if there is none, set to anything)
       * @param f the function to be executed when this option is given in the command line
       */  
    CLIOption(const std::string& name, bool def, CLIOption::func_no_args f) : 
    _name(name), _bdef(def) { func.no_args = f; _nbArgs = 0; _beginsWith = false; }
        
    /** 
       * Constructor for a String command line option whose value will later be stored 
       * in CLIOptions. The number of arguments (on the command line) to the option are 
       * given through the function pointer type: this constructor is for String options 
       * that take one argument on the command line.
       * 
       * @param name the command line string for the option, e.g. --stdlib /home/user/mylib/
       * @param beginsWith true, if the option's argument can be concatenated with the option name, e.g. -I/home/user/mznlib
       * @param optMapString the string that (will) map to the option value in CLIOptions
       * @param f the function to be executed when this option is given in the command line
       */  
    // TODO: add string option constructor that takes an initialisation function as argument (for setting stdlib etc)
    CLIOption(const std::string& name, bool beginsWith, const std::string& optMapString, CLIOption::func_str_arg f) : 
    _name(name), _beginsWith(beginsWith), _optMapString(optMapString) { func.str_arg = f; _nbArgs = 1; }
    CLIOption(const std::string& name, bool beginsWith, const std::string& optMapString, CLIOption::func_int_arg f) : 
    _name(name), _beginsWith(beginsWith), _optMapString(optMapString) { func.int_arg = f; _nbArgs = 1; }
    
    
    bool takesArgs(void) { return _nbArgs > 0; }
    const int getNbArgs(void) const { return _nbArgs; }
    /// returns the string under which the value of the option will be stored in CLIOptions
    const std::string& getOptMapString(void) { return _optMapString; }
    
    bool getBoolDefaultValue(void) const { return _bdef; }
    std::string getStringDefaultValue(void) const { return _sdef; }
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
    CLIOptions* parseArgs(int argc, char** argv);
    
  protected:
    /// creates the default MiniZinc CLI options and enters them into the _known_options map
    void generateDefaultCLIOptions(void);
    /// returns true if the given option \a opt (such as "-h") is part of the known options
    bool knowsOption(const std::string& opt) const;
    /// returns pointer to the CLIOption object that represents CLI option \a name
    CLIOption* getCLIOption(const std::string& name) const;
    /// default functionality that will be executed when a CLI error occurred
    void error(void);
    /// set the options that were not assigned by the command line to their default values
    void setDefaultOptionValues(CLIOptions* opt);
    
    void applyOption(CLIOptions* opts, char** argv, int& argc, int& idx, const std::string& arg);
  };
}

#endif
