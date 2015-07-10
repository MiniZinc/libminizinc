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
  
  CLIParser::CLIParser(void) {
    generateDefaultCLIOptions();
  }
  
  void CLIParser::generateDefaultCLIOptions(void) {
   // initialize the standard options      
  _known_options[constants().cli.cmdlineData_short_str.str()] = new CLIOption(constants().cli.cmdlineData_short_str.str(),
                                                                              1, /*nbArgs*/ true /* begins with */ );  
  _known_options[constants().cli.cmdlineData_str.str()] = new CLIOption(constants().cli.cmdlineData_str.str(),
                                                                              1, /*nbArgs*/ false /* begins with */ );    
  _known_options[constants().cli.datafile_short_str.str()] = new CLIOption(constants().cli.datafile_short_str.str(),
                                                                              1, /*nbArgs*/ true /* begins with */ );
  _known_options[constants().cli.datafile_str.str()] = new CLIOption(constants().cli.datafile_str.str(),
                                                                              1, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.globalsDir_alt_str.str()] = new CLIOption(constants().cli.globalsDir_alt_str.str(),
                                                                              1, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.globalsDir_short_str.str()] = new CLIOption(constants().cli.globalsDir_short_str.str(),
                                                                              1, /*nbArgs*/ true /* begins with */ );
  _known_options[constants().cli.globalsDir_str.str()] = new CLIOption(constants().cli.globalsDir_str.str(),
                                                                              1, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.help_short_str.str()] = new CLIOption(constants().cli.help_short_str.str(),
                                                                              0, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.help_str.str()] = new CLIOption(constants().cli.help_str.str(),
                                                                              0, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.ignoreStdlib_str.str()] = new CLIOption(constants().cli.ignoreStdlib_str.str(),
                                                                              0, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.include_str.str()] = new CLIOption(constants().cli.include_str.str(),
                                                                              1, /*nbArgs*/ true /* begins with */ ); 
  _known_options[constants().cli.instanceCheckOnly_str.str()] = new CLIOption(constants().cli.instanceCheckOnly_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ ); 
  _known_options[constants().cli.newfzn_str.str()] = new CLIOption(constants().cli.newfzn_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ ); 
  _known_options[constants().cli.no_optimize_alt_str.str()] = new CLIOption(constants().cli.no_optimize_alt_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ ); 
  _known_options[constants().cli.no_optimize_str.str()] = new CLIOption(constants().cli.no_optimize_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.no_outputOzn_short_str.str()] = new CLIOption(constants().cli.no_outputOzn_short_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.no_outputOzn_str.str()] = new CLIOption(constants().cli.no_outputOzn_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.no_typecheck_str.str()] = new CLIOption(constants().cli.no_typecheck_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.onlyRangeDomains_str.str()] = new CLIOption(constants().cli.onlyRangeDomains_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.outputBase_str.str()] = new CLIOption(constants().cli.outputBase_str.str(),
                                                                            1, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.outputFznToFile_alt_str.str()] = new CLIOption(constants().cli.outputFznToFile_alt_str.str(),
                                                                            1, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.outputFznToFile_str.str()] = new CLIOption(constants().cli.outputFznToFile_str.str(),
                                                                            1, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.outputFznToFile_short_str.str()] = new CLIOption(constants().cli.outputFznToFile_short_str.str(),
                                                                            1, /*nbArgs*/ true /* begins with */ );
  _known_options[constants().cli.outputFznToStdout_alt_str.str()] = new CLIOption(constants().cli.outputFznToStdout_alt_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.outputFznToStdout_str.str()] = new CLIOption(constants().cli.outputFznToStdout_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.outputOznToFile_str.str()] = new CLIOption(constants().cli.outputOznToFile_str.str(),
                                                                            1, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.outputOznToStdout_str.str()] = new CLIOption(constants().cli.outputOznToStdout_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.statistics_short_str.str()] = new CLIOption(constants().cli.statistics_short_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.statistics_str.str()] = new CLIOption(constants().cli.statistics_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.stdlib_str.str()] = new CLIOption(constants().cli.stdlib_str.str(),
                                                                            1, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.verbose_short_str.str()] = new CLIOption(constants().cli.verbose_short_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.verbose_str.str()] = new CLIOption(constants().cli.verbose_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.version_str.str()] = new CLIOption(constants().cli.version_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.version_str.str()] = new CLIOption(constants().cli.version_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ );
  _known_options[constants().cli.werror_str.str()] = new CLIOption(constants().cli.werror_str.str(),
                                                                            0, /*nbArgs*/ false /* begins with */ );
  }
  
  CLIOptions* CLIParser::parseCLI(int argc, char** argv) {
    CLIOptions* opts = new CLIOptions();
    int cnt = 0;
    while(cnt < argc) {     
      const std::string arg = std::string(argv[cnt]);
      cnt++;
      if(knowsOption(arg)) {        
        CLIOption* o = getCLIOption(arg);
        if(o->takesArgs()) {
          std::vector<std::string> args;
          for(int i=0; i<o->getNbArgs(); i++) {
            if(cnt >= argc) {
              std::cerr << "Missing argument for option: " << arg << std::endl;
              error();
            }
            args.push_back(std::string(argv[cnt]));
            cnt++;
          }
          o->func.str_args(opts,args); // execute the function for option o
        }
        else {
          o->func.no_args(opts);
        }        
      }
      else {
        // TODO: store the option anyway and give a warning
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