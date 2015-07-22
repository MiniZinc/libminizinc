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
    // NOTE: setting string vector parameters is very expensive since they are converted into ArrayLits/ASTExprVecs and back
    void setStringVectorParam(const std::string& name, KeepAlive e);
    void setStringVectorParam(const std::string& name, const std::vector<std::string>& e);
    std::vector<std::string> getStringVectorParam(const std::string& name) const;
    std::vector<std::string> getStringVectorParam(const std::string& name, std::vector<std::string>& def) const; 
  };
  
  class CLIOption;
  
  /// parser for command line arguments for MiniZinc
  class CLIParser {
  public:
    typedef UNORDERED_NAMESPACE::unordered_map<std::string, CLIOption*> opt_map;
  protected:    
    /// the options that are recognized by MiniZinc
    opt_map _known_options;
    
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
    /// applies the option \a arg given the argument vector and the next working index \a idx
    void applyOption(CLIOptions* opts, char** argv, int& argc, int& idx, const std::string& arg);
  };  
  
  /// class for each CLI option that MiniZinc recognizes
  class CLIOption {
  protected:
    /// option names as given in the command line, e.g. \"--help\"
    std::vector<std::string> _names;
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
    /// the description of the option in the help message
    std::string _description;
    
    // function pointers to function to be executed for this option    
    typedef void (*func_no_args) (CLIOptions* opt);
    typedef void (*func_str_arg) (CLIOptions* opt, std::string& s);
    typedef void (*func_int_arg) (CLIOptions* opt, int v);
    //typedef void (*func_str_args) (CLIOptions* opt, std::vector<std::string> args);  
    typedef void (*func_known_opts) (CLIOptions* opt, CLIParser::opt_map known_opts, std::string cmd);
    
    // function pointer to functions that set default String option value
    typedef void (*func_init) (CLIOptions* opt, CLIOption* o);
    
  public:
    struct {
      func_int_arg int_arg;
      func_no_args no_args;
      func_str_arg str_arg;
      func_known_opts opts_arg;
      //func_str_args str_args;
    } func;
    
    // functions to initialize the String option
    func_init _init;    
    
  public:
    /**
       * Constructor for a Boolean command line option whose value will later be stored 
       * in CLIOptions. The number of arguments (on the command line) to the option are 
       * given through the function pointer type: this constructor is for Boolean options 
       * that take no arguments on the command line.
       * 
       * @param name the command line strings for the option, e.g. "--verbose"         
       * @param def default value for the option (defined by optMapString!)
       * @param optMapString the string that (will) map to the option value in CLIOptions, e.g. "verbose"
       * @param f the function to be executed when this option is given in the command line
       */  
    CLIOption(std::vector<std::string> names, bool def, const std::string& optMapString, std::string description, CLIOption::func_no_args f) : 
      _names(names), _bdef(def), _optMapString(optMapString), _init(NULL),
      _nbArgs(0), _beginsWith(false), _description(description)
      { func.int_arg = NULL; func.no_args = f; func.str_arg = NULL; func.opts_arg = NULL; }   
    
    /**
       * Constructor for a Boolean command line option that whose value does not need 
       * to be stored in CLIOptions. The Boolean option has not command line parameter.
       * 
       * @param name the command line string for the option, e.g. --version  
       * @param def default value for the option (if there is none, set to anything)       
       * @param f the function to be executed when this option is given in the command line
       */  
    CLIOption(std::vector<std::string> names , bool def, std::string description, CLIOption::func_no_args f) : 
      _names(names), _bdef(def), _nbArgs(0), _beginsWith(false), _init(NULL), _description(description)
       { func.no_args = f; func.int_arg = NULL; func.str_arg = NULL; func.opts_arg = NULL; }
       
    /**
       * Constructor for a Boolean command line option that whose value does not need 
       * to be stored in CLIOptions but that needs to know all known options to be executed. 
       * The Boolean option has no command line parameter.
       * 
       * @param name the command line string for the option, e.g. --help      
       * @param f the function to be executed when this option is given in the command line
       */  
    CLIOption(std::vector<std::string> names, std::string description, CLIOption::func_known_opts f) : 
      _names(names), _bdef(false), _nbArgs(0), _beginsWith(false), _init(NULL), _description(description)
       { func.no_args = NULL; func.int_arg = NULL; func.str_arg = NULL; func.opts_arg = f; }       
        
    /** 
       * Constructor for a String command line option whose value will later be stored 
       * in CLIOptions. The number of arguments (on the command line) to the option are 
       * given through the function pointer type: this constructor is for String options 
       * that take one argument on the command line and that needs to be initialized in
       * case the option is not given in the command line.
       * 
       * @param name the command line string for the option, e.g. --stdlib /home/user/mylib/
       * @param beginsWith true, if the option's argument can be concatenated with the option name, e.g. -I/home/user/mznlib
       * @param optMapString the string that (will) map to the option value in CLIOptions
       * @param f the function to be executed when this option is given in the command line
       * @param init the function to be executed when this option is NOT given in the command line, to set the defaul string value
       */      
    CLIOption(std::vector<std::string> names, bool beginsWith, std::string description, const std::string& optMapString,
              CLIOption::func_str_arg f, CLIOption::func_init init) : 
     _names(names), _beginsWith(beginsWith), _optMapString(optMapString), _init(init), _description(description)
      { func.str_arg = f; func.no_args = NULL; func.int_arg = NULL;  _nbArgs = 1; func.opts_arg = NULL;}

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
    CLIOption(std::vector<std::string> names, bool beginsWith, const std::string& optMapString, std::string description, CLIOption::func_str_arg f) : 
     _names(names), _beginsWith(beginsWith), _optMapString(optMapString), _init(NULL), _nbArgs(1), _description(description)
      { func.str_arg = f; _nbArgs = 1; func.no_args = NULL; func.str_arg = NULL; func.opts_arg = NULL; }      
      
    // The int constructor is currently not used (there are no int options I know of)
    //CLIOption(const std::string& name, bool beginsWith, const std::string& optMapString, CLIOption::func_int_arg f) : 
    //_name(name), _beginsWith(beginsWith), _optMapString(optMapString), _init(NULL) 
    // { func.int_arg = f; func.no_args = NULL; func.str_arg = NULL; _nbArgs = 1; }
    
    
    bool takesArgs(void) { return _nbArgs > 0; }
    const int getNbArgs(void) const { return _nbArgs; }
    /// returns the string under which the value of the option will be stored in CLIOptions
    const std::string& getOptMapString(void) { return _optMapString; }    
    bool getBoolDefaultValue(void) const { return _bdef; }
    std::string getStringDefaultValue(void) const { return _sdef; } 
    std::string getDescription(void) const { return _description; }
    /// returns the list of strings that can be used for this option in the command line
    std::vector<std::string> getCommandLineNames(void) { return _names; }
    
    void setDefaultString(std::string& s) { _sdef = s; }
  };

}

#endif
