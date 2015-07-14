/* -*- mode: C++; c-basic-offOptions::set: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors: 
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/cli.hh>
//#include <minizinc/stl_map_set.hh>

namespace MiniZinc {
  
  void CLIOptions::setStringParam(const std::string& name, KeepAlive ka) {
    Expression* e = ka();
    if(e && e->type().ispar() && e->type().isstring()) {
      _options[name] = e;
    } else {
      std::stringstream ss;
      ss << "For option: " << name << " expected Par String, received " << e->type().toString() << std::endl;
      throw InternalError(ss.str());
    }
  }
  
  void CLIOptions::setStringParam(const std::string& name, std::string& s) {
    GCLock lock;
    StringLit* sl = new StringLit(Location(), s);
    KeepAlive ka(sl);
    
    setStringParam(name, ka);
  }
  
  std::string CLIOptions::getStringParam(const std::string& name) const {
    if(hasParam(name)) {
      if(StringLit* sl = getParam(name)->dyn_cast<StringLit>()) {
        return sl->v().str();
      }
    }    
    std::stringstream ss;
    ss << "Option: \"" << name << "\" does not exist or is not Par String" << std::endl;
    throw InternalError(ss.str());    
  }
  
  std::string CLIOptions::getStringParam(const std::string& name, std::string& def) const {
    if(hasParam(name)) {
      if(StringLit* sl = getParam(name)->dyn_cast<StringLit>()) {
        return sl->v().str();
      }
    } 
    return def;
  }
  
  void CLIOptions::setStringVectorParam(const std::string& name, KeepAlive ka) {
   Expression* e = ka();
    if(ArrayLit* al = e->dyn_cast<ArrayLit>()) {
      ASTExprVec<Expression> vec = al->v();
      if(vec.size() > 0 && vec[0]->type().ispar() && vec[0]->type().isstring()) {
        _options[name] = e;
        return;
      }
    }
    std::stringstream ss;
    ss << "For option: " << name << " expected Par String vector, received " << e->type().toString() << std::endl;
    throw InternalError(ss.str());
  }
  
  void CLIOptions::setStringVectorParam(const std::string& name, const std::vector<std::string>& v) {
    GCLock lock; 
    std::vector<Expression*> vs;
    for(unsigned int i=0; i<v.size(); i++)       
      vs.push_back(new StringLit(Location(),v[i]));
    ASTExprVec<Expression> vec(vs);
    ArrayLit* al = new ArrayLit(Location(), vec);
    KeepAlive ka(al);
    
    setStringVectorParam(name,ka);
  }
  
  std::vector<std::string> CLIOptions::getStringVectorParam(const std::string& name) const {
    if(hasParam(name)) {
      if(ArrayLit* al = getParam(name)->dyn_cast<ArrayLit>()) {    
        ASTExprVec<Expression> vec = al->v();
        if(vec.size() > 0 && vec[0]->type().isstring()) {
          std::vector<std::string> v;
          for(unsigned int i=0; i<vec.size(); i++)
            v.push_back(vec[i]->cast<StringLit>()->v().str());
          return v;
        }
      }
    }
    std::stringstream ss;
    ss << "Option: \"" << name << "\" does not exist or is not a String Vector" << std::endl;
    throw InternalError(ss.str());  
  }
  
  std::vector<std::string> CLIOptions::getStringVectorParam(const std::string& name, std::vector<std::string>& def) const {
    if(hasParam(name)) {
      if(ArrayLit* al = getParam(name)->dyn_cast<ArrayLit>()) {    
        ASTExprVec<Expression> vec = al->v();
        if(vec.size() > 0 && vec[0]->type().isstring()) {
          std::vector<std::string> v;
          for(unsigned int i=0; i<vec.size(); i++)
            v.push_back(vec[i]->cast<StringLit>()->v().str());
          return v;
        }
      }
    }
    return def;
  } 
  
  // functions for each CLI option
  void cli_cmdlineData(CLIOptions* opt, std::string& s) {
    opt->setStringParam(constants().opts.cmdlineData.str(),s);
  }
  void cli_datafile(CLIOptions* opt, std::string& s) {
    opt->setStringParam(constants().opts.datafile.str(),s);
  }
  void cli_globals_dir(CLIOptions* opt, std::string& s) {
    opt->setStringParam(constants().opts.globalsDir.str(),s);
  }
  void cli_help(CLIOptions* opt) {
    // TODO: print description of each CLIOption (TODO: add description to each option)
    std::cerr << "HELP is in construction.\n" ;
    exit(EXIT_FAILURE);
  }
  void cli_ignoreStdlib(CLIOptions* opt) {
    opt->setBoolParam(constants().opts.ignoreStdlib.str(), true);
  }
  void cli_include(CLIOptions* opt, std::string& s) {
    opt->setStringParam(constants().opts.includeDir.str(),s);
  }
  void cli_instanceCheckOnly(CLIOptions* opt) {
    opt->setBoolParam(constants().opts.instanceCheckOnly.str(),true);
  }
  void cli_newfzn(CLIOptions* opt) { // TODO: is the default FALSE??
    opt->setBoolParam(constants().opts.newfzn.str(),true);
  }
  void cli_no_optimize(CLIOptions* opt) {
    opt->setBoolParam(constants().opts.optimize.str(),false);
  }
  void cli_no_outputOzn(CLIOptions* opt) {
    opt->setBoolParam(constants().opts.noOznOutput.str(),true);
  }
  void cli_no_typecheck(CLIOptions* opt) {
    opt->setBoolParam(constants().opts.typecheck.str(),false);
  }
  void cli_rangeDomainsOnly(CLIOptions* opt) {
    opt->setBoolParam(constants().opts.rangeDomainsOnly.str(),true);
  }
  void cli_outputBase(CLIOptions* opt, std::string& s) {
    opt->setStringParam(constants().opts.outputBase.str(),s);
  }
  void cli_outputFznToFile(CLIOptions* opt, std::string& s) {
    opt->setStringParam(constants().opts.fznToFile.str(),s);
  }
  void cli_outputFznToStdout(CLIOptions* opt) {
    opt->setBoolParam(constants().opts.fznToStdout.str(),true);
  }
  void cli_oznToFile(CLIOptions* opt, std::string& s) {
    opt->setStringParam(constants().opts.oznToFile.str(),s);
  }
  void cli_oznToStdout(CLIOptions* opt) {
    opt->setBoolParam(constants().opts.oznToStdout.str(),true);
  }
  void cli_statistics(CLIOptions* opt) {
    opt->setBoolParam(constants().opts.statistics.str(),true);
  }
  void cli_stdlib(CLIOptions* opt, std::string& s) {
    opt->setStringParam(constants().opts.stdlib.str(),s);
  }
  void cli_verbose(CLIOptions* opt) {
    opt->setBoolParam(constants().opts.verbose.str(),true);
  }
  void cli_version(CLIOptions* opt) {
    std::cout << "NICTA MiniZinc to FlatZinc converter, version "
      << MZN_VERSION_MAJOR << "." << MZN_VERSION_MINOR << "." << MZN_VERSION_PATCH << std::endl;
    std::cout << "Copyright (C) 2014, 2015 Monash University and NICTA" << std::endl;
    std::exit(EXIT_SUCCESS);
  }
  void cli_werror(CLIOptions* opt) {
    opt->setBoolParam(constants().opts.werror.str(),true);
  }
  
  CLIParser::CLIParser(void) {
    generateDefaultCLIOptions();
  }
  
  void CLIParser::generateDefaultCLIOptions(void) {
   // initialize the standard MiniZinc options      
  _known_options[constants().cli.cmdlineData_short_str.str()] = new CLIOption(constants().cli.cmdlineData_short_str.str(),
                                                                              1, /*nbArgs*/ true /* begins with */, cli_cmdlineData );  
  _known_options[constants().cli.cmdlineData_str.str()] = new CLIOption(constants().cli.cmdlineData_str.str(),
                                                                              1, /*nbArgs*/ false /* begins with */, cli_cmdlineData );    
  _known_options[constants().cli.datafile_short_str.str()] = new CLIOption(constants().cli.datafile_short_str.str(),
                                                                              1, /*nbArgs*/ true /* begins with */, cli_datafile );
  _known_options[constants().cli.datafile_str.str()] = new CLIOption(constants().cli.datafile_str.str(),
                                                                              1, /*nbArgs*/ false /* begins with */, cli_datafile );
  _known_options[constants().cli.globalsDir_alt_str.str()] = new CLIOption(constants().cli.globalsDir_alt_str.str(),
                                                                              1, /*nbArgs*/ false /* begins with */, cli_globals_dir );
  _known_options[constants().cli.globalsDir_short_str.str()] = new CLIOption(constants().cli.globalsDir_short_str.str(),
                                                                              1, /*nbArgs*/ true /* begins with */, cli_globals_dir );
  _known_options[constants().cli.globalsDir_str.str()] = new CLIOption(constants().cli.globalsDir_str.str(),
                                                                              1, /*nbArgs*/ false /* begins with */, cli_globals_dir );
  _known_options[constants().cli.help_short_str.str()] = new CLIOption(constants().cli.help_short_str.str(),
                                                                              0, /*nbArgs*/ false /* begins with */, false /* default */, cli_help );
  _known_options[constants().cli.help_str.str()] = new CLIOption(constants().cli.help_str.str(),
                                                                              0, /*nbArgs*/ false /* begins with */, false /* default */, cli_help );
  _known_options[constants().cli.ignoreStdlib_str.str()] = new CLIOption(constants().cli.ignoreStdlib_str.str(),
                                                                              0, /*nbArgs*/ false /* begins with */, false /* default */, cli_ignoreStdlib );
  _known_options[constants().cli.include_str.str()] = new CLIOption(constants().cli.include_str.str(),
                                                                              1, /*nbArgs*/ true /* begins with */, cli_include ); 
  _known_options[constants().cli.instanceCheckOnly_str.str()] = new CLIOption(constants().cli.instanceCheckOnly_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */, false /* default */, cli_instanceCheckOnly ); 
  _known_options[constants().cli.newfzn_str.str()] = new CLIOption(constants().cli.newfzn_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */, false /* default */, cli_newfzn ); 
  _known_options[constants().cli.no_optimize_alt_str.str()] = new CLIOption(constants().cli.no_optimize_alt_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */, false /* default */, cli_no_optimize ); 
  _known_options[constants().cli.no_optimize_str.str()] = new CLIOption(constants().cli.no_optimize_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */, false /* default */, cli_no_optimize );
  _known_options[constants().cli.no_outputOzn_short_str.str()] = new CLIOption(constants().cli.no_outputOzn_short_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */, false /* default */, cli_no_outputOzn );
  _known_options[constants().cli.no_outputOzn_str.str()] = new CLIOption(constants().cli.no_outputOzn_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */, false /* default */, cli_no_outputOzn );
  _known_options[constants().cli.no_typecheck_str.str()] = new CLIOption(constants().cli.no_typecheck_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */, false /* default */, cli_no_typecheck );
  _known_options[constants().cli.rangeDomainsOnly_str.str()] = new CLIOption(constants().cli.rangeDomainsOnly_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */, false /* default */, cli_rangeDomainsOnly );
  _known_options[constants().cli.outputBase_str.str()] = new CLIOption(constants().cli.outputBase_str.str(),
                                                                            1, /*nbArgs*/ false /* begins with */, cli_outputBase );
  _known_options[constants().cli.outputFznToFile_alt_str.str()] = new CLIOption(constants().cli.outputFznToFile_alt_str.str(),
                                                                            1, /*nbArgs*/ false /* begins with */, cli_outputFznToFile );
  _known_options[constants().cli.outputFznToFile_str.str()] = new CLIOption(constants().cli.outputFznToFile_str.str(),
                                                                            1, /*nbArgs*/ false /* begins with */, cli_outputFznToFile );
  _known_options[constants().cli.outputFznToFile_short_str.str()] = new CLIOption(constants().cli.outputFznToFile_short_str.str(),
                                                                            1, /*nbArgs*/ true /* begins with */, cli_outputFznToFile );
  _known_options[constants().cli.outputFznToStdout_alt_str.str()] = new CLIOption(constants().cli.outputFznToStdout_alt_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */, false /* default */, cli_outputFznToStdout );
  _known_options[constants().cli.outputFznToStdout_str.str()] = new CLIOption(constants().cli.outputFznToStdout_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */, false /* default */, cli_outputFznToStdout );
  _known_options[constants().cli.outputOznToFile_str.str()] = new CLIOption(constants().cli.outputOznToFile_str.str(),
                                                                            1, /*nbArgs*/ false /* begins with */,cli_oznToFile );
  _known_options[constants().cli.outputOznToStdout_str.str()] = new CLIOption(constants().cli.outputOznToStdout_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ , false /* default */, cli_oznToStdout);
  _known_options[constants().cli.statistics_short_str.str()] = new CLIOption(constants().cli.statistics_short_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */, false /* default */, cli_statistics );
  _known_options[constants().cli.statistics_str.str()] = new CLIOption(constants().cli.statistics_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */, false /* default */, cli_statistics );
  _known_options[constants().cli.stdlib_str.str()] = new CLIOption(constants().cli.stdlib_str.str(),
                                                                            1, /*nbArgs*/ false /* begins with */ , cli_stdlib);
  _known_options[constants().cli.verbose_short_str.str()] = new CLIOption(constants().cli.verbose_short_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */, false /* default */, cli_verbose );
  _known_options[constants().cli.verbose_str.str()] = new CLIOption(constants().cli.verbose_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */, false /* default */, cli_verbose );
  _known_options[constants().cli.version_str.str()] = new CLIOption(constants().cli.version_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */, false /* default */, cli_version );
  _known_options[constants().cli.version_str.str()] = new CLIOption(constants().cli.version_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */, false /* default */, cli_version );
  _known_options[constants().cli.werror_str.str()] = new CLIOption(constants().cli.werror_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */, false /* default */, cli_werror );
  }
  
  CLIOptions* CLIParser::parseCLI(int argc, char** argv) {
    CLIOptions* opts = new CLIOptions();
    int idx = 0;
    while(idx < argc) {     
      const std::string arg = std::string(argv[idx]);
      idx++;
      if(knowsOption(arg)) {        
        CLIOption* o = getCLIOption(arg);
        int nbArgs = o->getNbArgs();
        if(nbArgs == 0) {
          o->func.no_args(opts);
        }      
        else if(nbArgs == 1) {
          if(idx >= argc) {
            std::cerr << "Missing argument for option: " << arg << std::endl;
            error();
          }
          std::string s = std::string(argv[idx]);
          o->func.str_arg(opts,s); // TODO: check for int-argument option (though there are none in the current options)
          idx++;
        }
        else { // more than 1 argument
          std::vector<std::string> args;
          for(int i=0; i<o->getNbArgs(); i++) {
            if(idx >= argc) {
              std::cerr << "Missing argument for option: " << arg << std::endl;
              error();
            }
            args.push_back(std::string(argv[idx]));
            idx++;
          }
          o->func.str_args(opts,args); // execute the function for option o
        }
      }
      // the given option is not known
      else { 
        // TODO: store the option anyway and give a warning that it is not known
      }
    }
    // set default options in case they are not set
    for(UNORDERED_NAMESPACE::unordered_map<std::string, CLIOption* >::const_iterator it =_known_options.begin(); it!=_known_options.end(); ++it) {
       if(opts->hasParam(it->second->getOptMapString())) {
         // do nothing
       }
       else {
         CLIOption* o = it->second;
         int nbArgs = o->getNbArgs();
         if(nbArgs == 0) {
           opts->setBoolParam(o->getOptMapString(), o->getBoolDefaultValue()); 
         }
         else if(nbArgs == 1) {
           // TODO: check for integer values (though there are none in the current options)
           std::string def = o->getStringDefaultValue();
           opts->setStringParam(o->getOptMapString(), def); 
         }
         else {
           // TODO
         }
       }
    }
    return opts;
  }
  
  bool CLIParser::knowsOption(const std::string& name) const {
    return _known_options.find(name) != _known_options.end();
  }
  
  CLIOption* CLIParser::getCLIOption(const std::string& name) const {
    UNORDERED_NAMESPACE::unordered_map<std::string, CLIOption* >::const_iterator it = _known_options.find(name);
    if(it == _known_options.end()) {
      std::stringstream ss;
      ss << "Could not find CLI option: \"" << name << "\"." << std::endl;
      throw InternalError(ss.str());
    }
    return (it->second);
  }
  
  void CLIParser::error(void) {
    exit(EXIT_FAILURE);
  }
  
}