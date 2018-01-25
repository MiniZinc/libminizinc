/* -*- mode: C++; c-basic-offOptions::set: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors: 
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/cli.hh>
#include <minizinc/file_utils.hh>

#include <set>

namespace MiniZinc {
  
  void CLIOptions::setStringVectorParam(const std::string& name, KeepAlive ka) {
   Expression* e = ka();
    if(ArrayLit* al = e->dyn_cast<ArrayLit>()) {
      if(al->size() > 0 && (*al)[0]->type().ispar() && (*al)[0]->type().isstring()) {
        _options[name] = e;
        return;
      }
    }
    std::stringstream ss;
    ss << "For option: " << name << " expected Par String vector, received " << e->type().nonEnumToString() << std::endl;
    throw InternalError(ss.str());
  }
  
  void CLIOptions::setStringVectorParam(const std::string& name, const std::vector<std::string>& v) {
    GCLock lock; 
    std::vector<Expression*> vs;
    for(unsigned int i=0; i<v.size(); i++)       
      vs.push_back(new StringLit(Location(),v[i]));
    ArrayLit* al = new ArrayLit(Location(), vs);
    KeepAlive ka(al);
    
    setStringVectorParam(name,ka);
  }
  
  std::vector<std::string> CLIOptions::getStringVectorParam(const std::string& name) const {
    if(hasParam(name)) {
      if(ArrayLit* al = getParam(name)->dyn_cast<ArrayLit>()) {    
        if(al->size() > 0 && (*al)[0]->type().isstring()) {
          std::vector<std::string> v;
          for(unsigned int i=0; i<al->size(); i++)
            v.push_back((*al)[i]->cast<StringLit>()->v().str());
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
        if(al->size() > 0 && (*al)[0]->type().isstring()) {
          std::vector<std::string> v;
          for(unsigned int i=0; i<al->size(); i++)
            v.push_back((*al)[i]->cast<StringLit>()->v().str());
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
  void cli_help(CLIOptions* opt, CLIParser::opt_map knownOptions, std::string command, std::vector<std::string> categories) {
    std::cerr << "Usage: "<< command;            
    for(unsigned int i=0; i<categories.size(); i++) {
      std::cerr << "\n\n" << categories[i] << ":\n";
      /*struct InsensitiveCompare { 
        bool operator() (const CLIOption& ca, const CLIOption& cb) const {
          std::string a = ca.getCommandLineNames()[0];
          std::string b = cb.getCommandLineNames()[0];
          return std::strcmp(a.c_str(), b.c_str()) < 0;
        }
      };*/
      std::set<CLIOption*> options;      
      // TODO: display options alphabetically
      for(CLIParser::opt_map::const_iterator it = knownOptions.begin(); it!=knownOptions.end(); ++it) {
        CLIOption* o = it->second;
        if(o->getCategory() != categories[i])
          continue; // skip this option
        if(options.find(o) != options.end())
          continue; // we've already printed the option
        options.insert(o);
        std::vector<std::string> cmd_opts = o->getCommandLineNames();      
        std::cerr << "  " << std::endl;
        for(unsigned int i=0; i<cmd_opts.size(); i++) {
          std::cerr << cmd_opts[i];
          for(unsigned int j=0; j<o->getNbArgs(); j++) {
            if(o->getNbArgs() == 1)
              std::cerr << " <arg>"; // TODO: allow options to specify names of its arguments
            else std::cerr << " <arg" << (j+1) << ">" << (o->getNbArgs() == j-1 ? "" : " ") ;
          }
          if(i<cmd_opts.size()-1) 
            std::cerr << ", ";
        }
        std::cerr << "\n    " << o->getDescription(); // TODO: include line breaks into description if too long
      }   
    }
    std::cerr << std::endl;
    exit(EXIT_FAILURE);
  }
  void cli_ignoreStdlib(CLIOptions* opt) {
    opt->setBoolParam(constants().opts.ignoreStdlib.str(), true);
  }
  void cli_include(CLIOptions* opt, std::string& s) {
    opt->setStringParam(constants().opts.includeDir.str(),s);
  }
  void cli_inputFromStdin(CLIOptions* opt) {
    opt->setBoolParam(constants().opts.inputFromStdin.str(),true);
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
    std::cout << "MiniZinc to FlatZinc converter, version "
      << MZN_VERSION_MAJOR << "." << MZN_VERSION_MINOR << "." << MZN_VERSION_PATCH << std::endl;
    std::cout << "Copyright (C) 2014-2017 Monash University, NICTA, Data61" << std::endl;
    std::exit(EXIT_SUCCESS);
  }
  void cli_werror(CLIOptions* opt) {
    opt->setBoolParam(constants().opts.werror.str(),true);
  }
  
  CLIParser::CLIParser(void) {   
    _cli_categories.push_back(constants().cli_cat.general.str());
    _cli_categories.push_back(constants().cli_cat.io.str()); 
    _cli_categories.push_back(constants().cli_cat.translation.str());
    _cmd_params = "[<options>] [-I <include path>] <model>.mzn [<data>.dzn ...]";
    generateDefaultCLIOptions();
  }
  
  void CLIParser::generateDefaultCLIOptions(void) {   
    // initialize the standard MiniZinc options
    std::vector<std::string> n_cmdl; 
    n_cmdl.push_back(constants().cli.cmdlineData_short_str.str());
    n_cmdl.push_back(constants().cli.cmdlineData_str.str());
    CLIOption* o_cmdl = new CLIOption(n_cmdl, true /* begins with */, 
                                      constants().opts.cmdlineData.str(), 
                                      "Include the given data in the model.",
                                      constants().cli_cat.general.str(),
                                      cli_cmdlineData ); 
    _known_options[constants().cli.cmdlineData_short_str.str()] = o_cmdl;
    _known_options[constants().cli.cmdlineData_str.str()] = o_cmdl;
  
    std::vector<std::string> n_datafile;
    n_datafile.push_back(constants().cli.datafile_short_str.str());
    n_datafile.push_back(constants().cli.datafile_str.str());
    CLIOption* o_data = new CLIOption(n_datafile, true /* begins with */, 
                                      constants().opts.datafile.str(), 
                                      "Include the given data in the model.", 
                                      constants().cli_cat.general.str(), cli_datafile );
    _known_options[constants().cli.datafile_short_str.str()] = o_data;
    _known_options[constants().cli.datafile_str.str()] = o_data;
    
    std::vector<std::string> n_globals;
    n_globals.push_back(constants().cli.globalsDir_alt_str.str());
    n_globals.push_back(constants().cli.globalsDir_short_str.str());
    n_globals.push_back(constants().cli.globalsDir_str.str());
    CLIOption* o_globals = new CLIOption(n_globals, true /* begins with */, 
                                         constants().opts.globalsDir.str(), 
                                         "Search for included files in <stdlib>/<dir>.", 
                                         constants().cli_cat.general.str(), cli_globals_dir );
    _known_options[constants().cli.globalsDir_alt_str.str()] = o_globals;
    _known_options[constants().cli.globalsDir_short_str.str()] = o_globals;
    _known_options[constants().cli.globalsDir_str.str()] = o_globals;
  
    std::vector<std::string> n_help;
    n_help.push_back(constants().cli.help_short_str.str());
    n_help.push_back(constants().cli.help_str.str());
    CLIOption* o_help = new CLIOption(n_help, "Print this help message.", constants().cli_cat.general.str(), cli_help);
    _known_options[constants().cli.help_short_str.str()] = o_help;
    _known_options[constants().cli.help_str.str()] = o_help;
  
    std::vector<std::string> n_ignoreStdl; n_ignoreStdl.push_back(constants().cli.ignoreStdlib_str.str());
    _known_options[constants().cli.ignoreStdlib_str.str()] = new CLIOption(n_ignoreStdl, false /* default */, constants().opts.ignoreStdlib.str(), 
                                                                           "Ignore the standard libraries stdlib.mzn and builtins.mzn", 
                                                                           constants().cli_cat.general.str(), cli_ignoreStdlib );
  
    std::vector<std::string> n_include; n_include.push_back(constants().cli.include_str.str());
    _known_options[constants().cli.include_str.str()] = new CLIOption(n_include, true /* begins with */, 
                                                                    constants().opts.includeDir.str(), "Specify include path", 
                                                                    constants().cli_cat.general.str(),  cli_include ); 
    
    std::vector<std::string> n_inputStdin; n_inputStdin.push_back(constants().cli.inputFromStdin_str.str());
    _known_options[constants().cli.inputFromStdin_str.str()] = new CLIOption(n_inputStdin, false /* default value */,
                                                                             constants().opts.inputFromStdin.str(), 
                                                                             "Problem input will be given via stdin.", constants().cli_cat.io.str(), 
                                                                             cli_inputFromStdin);
    
    std::vector<std::string> n_instanceCk; n_instanceCk.push_back(constants().cli.instanceCheckOnly_str.str());
    _known_options[constants().cli.instanceCheckOnly_str.str()] = new CLIOption(n_instanceCk, false /* default */, constants().opts.instanceCheckOnly.str(), 
                                                                                "Check the model instance (including data) for errors, but do not convert to FlatZinc.", 
                                                                                constants().cli_cat.translation.str(), cli_instanceCheckOnly ); 
    
    std::vector<std::string> n_newfzn; n_newfzn.push_back(constants().cli.newfzn_str.str());
    _known_options[constants().cli.newfzn_str.str()] = new CLIOption(n_newfzn, false /* default */, constants().opts.newfzn.str(), 
                                                                     "Convert to new FlatZinc format", constants().cli_cat.translation.str(), cli_newfzn ); 
    
    std::vector<std::string> n_opt; 
    n_opt.push_back(constants().cli.no_optimize_alt_str.str());
    n_opt.push_back(constants().cli.no_optimize_str.str());
    CLIOption* o_opt = new CLIOption(n_opt, true /* default for optimize! */, 
                                     constants().opts.optimize.str(), "Do not optimize the FlatZinc.", constants().cli_cat.translation.str(), cli_no_optimize );     
    _known_options[constants().cli.no_optimize_alt_str.str()] = o_opt;
    _known_options[constants().cli.no_optimize_str.str()] = o_opt;
    
    std::vector<std::string> n_no_oznOut; 
    n_no_oznOut.push_back(constants().cli.no_outputOzn_short_str.str());
    n_no_oznOut.push_back(constants().cli.no_outputOzn_str.str());
    CLIOption* o_no_oznOut = new CLIOption(n_no_oznOut, false /* default */, constants().opts.noOznOutput.str(), 
                                           "Do not output the model output specification (ozn file)", constants().cli_cat.io.str(), cli_no_outputOzn );
    _known_options[constants().cli.no_outputOzn_short_str.str()]  = o_no_oznOut;
    _known_options[constants().cli.no_outputOzn_str.str()] = o_no_oznOut;
    
    std::vector<std::string> n_typeChk; n_typeChk.push_back(constants().cli.no_typecheck_str.str());
    _known_options[constants().cli.no_typecheck_str.str()] = new CLIOption(n_typeChk, true /* default for typecheck!! */, constants().opts.typecheck.str(), 
                                                                         "Only parse and print model without typechecking", constants().cli_cat.translation.str(), cli_no_typecheck );
    
    std::vector<std::string> n_rangeD; n_rangeD.push_back(constants().cli.rangeDomainsOnly_str.str());
    _known_options[constants().cli.rangeDomainsOnly_str.str()] = new CLIOption(n_rangeD, false /* default */, constants().opts.rangeDomainsOnly.str(), 
                                                                               "", constants().cli_cat.translation.str(), cli_rangeDomainsOnly ); //TODO: give description
    
    std::vector<std::string> n_output; n_output.push_back(constants().cli.outputBase_str.str());
    _known_options[constants().cli.outputBase_str.str()] = new CLIOption(n_output, false /* begins with */, constants().opts.outputBase.str(), 
                                                                         "Base name for output files.", constants().cli_cat.io.str(), cli_outputBase );
    
    std::vector<std::string> n_fznToFile; 
    n_fznToFile.push_back(constants().cli.outputFznToFile_alt_str.str());
    n_fznToFile.push_back(constants().cli.outputFznToFile_str.str());
    n_fznToFile.push_back(constants().cli.outputFznToFile_short_str.str());
    CLIOption* o_fzn2file = new CLIOption(n_fznToFile, false /* begins with */, constants().opts.fznToFile.str(), 
                                          "Filename for generated FlatZinc output.", constants().cli_cat.io.str(), cli_outputFznToFile );    
    _known_options[constants().cli.outputFznToFile_alt_str.str()] = o_fzn2file;
    _known_options[constants().cli.outputFznToFile_str.str()] = o_fzn2file;
    _known_options[constants().cli.outputFznToFile_short_str.str()] = o_fzn2file;
    
    std::vector<std::string> n_fzn2Stdout;
    n_fzn2Stdout.push_back(constants().cli.outputFznToStdout_alt_str.str());
    n_fzn2Stdout.push_back(constants().cli.outputFznToStdout_str.str());
    CLIOption* o_fzn2Stdout = new CLIOption(n_fzn2Stdout, false /* default */, constants().opts.fznToStdout.str(), 
                                            "Print generated FlatZinc to standard output", constants().cli_cat.io.str(), cli_outputFznToStdout );    
    _known_options[constants().cli.outputFznToStdout_alt_str.str()] = o_fzn2Stdout;
    _known_options[constants().cli.outputFznToStdout_str.str()] = o_fzn2Stdout;
    
    std::vector<std::string> n_ozn2file; n_ozn2file.push_back(constants().cli.outputOznToFile_str.str());
    _known_options[constants().cli.outputOznToFile_str.str()] = new CLIOption(n_ozn2file, false /* begins with */, constants().opts.oznToFile.str(), 
                                                                              "Filename for model output specification (ozn file)", constants().cli_cat.io.str(), cli_oznToFile );
    
    std::vector<std::string> n_ozn2Stdout; n_ozn2Stdout.push_back(constants().cli.outputOznToStdout_str.str());
    _known_options[constants().cli.outputOznToStdout_str.str()] = new CLIOption(n_ozn2Stdout, false /* default */, constants().opts.oznToStdout.str(), 
                                                                              "Print model output specification to standard output", constants().cli_cat.io.str(), cli_oznToStdout);
    
    std::vector<std::string> n_stats; 
    n_stats.push_back(constants().cli.statistics_short_str.str());
    n_stats.push_back(constants().cli.statistics_str.str());
    CLIOption* o_stats = new CLIOption(n_stats, false /* default */, constants().opts.statistics.str(), 
                                       "Print statistics", constants().cli_cat.general.str(), cli_statistics );      
    _known_options[constants().cli.statistics_short_str.str()] = o_stats;
    _known_options[constants().cli.statistics_str.str()] = o_stats;
  
    std::vector<std::string> n_stdlib; n_stdlib.push_back(constants().cli.stdlib_str.str());
    _known_options[constants().cli.stdlib_str.str()] = new CLIOption(n_stdlib, false /* begins with */ , constants().opts.stdlib.str(), 
                                                                   "Path to MiniZinc standard library directory", constants().cli_cat.general.str(), cli_stdlib);
  
    std::vector<std::string> n_verbose;
    n_verbose.push_back(constants().cli.verbose_short_str.str());
    n_verbose.push_back(constants().cli.verbose_str.str());
    CLIOption* o_verb = new CLIOption(n_verbose, false /* default */, constants().opts.verbose.str(), 
                                      "Print progress statements", constants().cli_cat.general.str(), cli_verbose );
    _known_options[constants().cli.verbose_short_str.str()] = o_verb;
    _known_options[constants().cli.verbose_str.str()] = o_verb;
  
    std::vector<std::string> n_version; n_version.push_back(constants().cli.version_str.str());
    _known_options[constants().cli.version_str.str()] = new CLIOption(n_version, false /* default */, "Print version information", constants().cli_cat.general.str(), cli_version );
    
    std::vector<std::string> n_werror; n_werror.push_back(constants().cli.werror_str.str());
    _known_options[constants().cli.werror_str.str()] = new CLIOption(n_werror, false /* default */, constants().opts.werror.str(), 
                                                                     "Turn warnings into errors", constants().cli_cat.general.str(), cli_werror );
  }
  
  CLIOptions* CLIParser::parseArgs(int argc, char** argv) {
    CLIOptions* opts = new CLIOptions();    
    int idx = 1; // omit the command
    std::string model;
    std::vector<std::string> datafiles;
    while(idx < argc) {     
      const std::string arg = std::string(argv[idx]);
      idx++;     
      if(knowsOption(arg)) {        
        applyOption(opts,argv,argc,idx,arg);
      }      
      else { 
        if(arg.length() > 4) {
          size_t last_dot = arg.find_last_of('.');
          std::string extension = arg.substr(last_dot,std::string::npos);
          if (extension == ".mzn") {
            if(model != "") {
              std::cerr << "Error: Multiple .mzn files given." << std::endl;
              error();
            }
            model = arg;
            continue;
          }
          else if(extension == ".dzn" || extension==".json") {
            datafiles.push_back(arg);                 
            continue;
          }
        }       
        // TODO: store the option anyway and give a warning that it is not known
        std::cerr << "Warning: Ignoring unknown option: " << arg << std::endl;       
      }
    }
    if(model==""){
      std::cerr << "Error: No model file given." << std::endl;
      error();
    }
    opts->setStringParam(constants().opts.model.str(),model);
    if(datafiles.size() == 1)
      opts->setStringParam(constants().opts.datafile.str(),datafiles[0]);      
    else if (datafiles.size() > 1)
      opts->setStringVectorParam(constants().opts.datafiles.str(),datafiles);    
    // set default options in case they are not set
    setDefaultOptionValues(opts);
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
    // TODO: print proper error message, including help
    exit(EXIT_FAILURE);
  }
  
  void CLIParser::setDefaultOptionValues(CLIOptions* opts) {
    for(UNORDERED_NAMESPACE::unordered_map<std::string, CLIOption* >::const_iterator it =_known_options.begin(); it!=_known_options.end(); ++it) {
      if(!opts->hasParam(it->second->getOptMapString())) {          
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
        /*else {
          // NOTE: there currently are NO string vector options
        } */
      }
    }
    std::string filename = opts->getStringParam(constants().opts.model.str());       
    std::string output_base = opts->getStringParam(constants().opts.outputBase.str());  
    if(output_base =="") {
      if (opts->getBoolParam(constants().opts.inputFromStdin.str())) 
        output_base = "mznout";
      else
        output_base = filename.substr(0,filename.length()-4);
      opts->setStringParam(constants().opts.outputBase.str(), output_base);
    }
    std::string std_lib_dir = opts->getStringParam(constants().opts.stdlib.str());
    if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
      std_lib_dir = std::string(MZNSTDLIBDIR);
    }    
    if(std_lib_dir == "") {
      std::string mypath = FileUtils::progpath();
      if (!mypath.empty()) {
        if (FileUtils::file_exists(mypath+"/share/minizinc/std/builtins.mzn")) {
          std_lib_dir = mypath+"/share/minizinc";
        } else if (FileUtils::file_exists(mypath+"/../share/minizinc/std/builtins.mzn")) {
          std_lib_dir = mypath+"/../share/minizinc";
        } else if (FileUtils::file_exists(mypath+"/../../share/minizinc/std/builtins.mzn")) {
          std_lib_dir = mypath+"/../../share/minizinc";
        }
      }
    }
    if (std_lib_dir=="") {
      std::cerr << "Error: unknown minizinc standard library directory.\n"
      << "Specify --stdlib-dir on the command line or set the\n"
      << "MZN_STDLIB_DIR environment variable.\n";
      std::exit(EXIT_FAILURE);
    }
    opts->setStringParam(constants().opts.stdlib.str(),std_lib_dir);
    std::string output_fzn = opts->getStringParam(constants().opts.fznToFile.str());
    if(output_fzn=="") {
      output_fzn = output_base+".fzn";
      opts->setStringParam(constants().opts.fznToFile.str(),output_fzn);
    }
    std::string output_ozn = opts->getStringParam(constants().opts.oznToFile.str());
    if(output_ozn =="") {
      output_ozn = output_base+".ozn";
      opts->setStringParam(constants().opts.oznToFile.str(),output_ozn);
    }  
    std::vector<std::string> datafiles;
    if(opts->hasParam(constants().opts.datafiles.str())) 
      datafiles = opts->getStringVectorParam(constants().opts.datafiles.str());
    else if(opts->hasParam(constants().opts.datafile.str())) {
      std::string s = opts->getStringParam(constants().opts.datafile.str());
      if(s!="")
        datafiles.push_back(s);
    }
    if(datafiles.size() > 0)
      opts->setStringVectorParam(constants().opts.datafiles.str(), datafiles);
    std::vector<std::string> includePaths;
    std::string globals_dir = opts->getStringParam(constants().opts.globalsDir.str());    
    if (globals_dir!="") {
      if(globals_dir.back() != '/') {
        globals_dir = globals_dir+"/";
        opts->setStringParam(constants().opts.globalsDir.str(),globals_dir);
      }
      includePaths.push_back(globals_dir);
    }
    includePaths.push_back(std_lib_dir+"/std/");  
    for (unsigned int i=0; i<includePaths.size(); i++) {
      if (!FileUtils::directory_exists(includePaths[i])) {
        std::cerr << "Cannot access include directory " << includePaths[i] << "\n";
        std::exit(EXIT_FAILURE);
      }
    }
    opts->setStringVectorParam(constants().opts.includePaths.str(),includePaths);
  }
  
  void CLIParser::applyOption(CLIOptions* opts, char** argv, int& argc, int& idx, const std::string& arg) {
    CLIOption* o = getCLIOption(arg);
    int nbArgs = o->getNbArgs();
    if(nbArgs == 0) {
      if(o->func.opts_arg == NULL)
        o->func.no_args(opts); // execute the function for option o
      else {
        std::string cmd = std::string(argv[0])+" "+_cmd_params;
        o->func.opts_arg(opts,_known_options,cmd,_cli_categories); // help function
      }
    }      
    else if(nbArgs == 1) {
      if(idx >= argc) {
        std::cerr << "Error: Missing argument for option: " << arg << std::endl;
        error();
      }
      std::string s = std::string(argv[idx]);
      if(o->func.str_arg == NULL) {
        std::stringstream ssm; 
        ssm << "Function for CLIOption \"" << arg << "\" is NULL. Expected function with one string argument.";
        throw InternalError(ssm.str());
      }
      o->func.str_arg(opts,s);
      idx++;
    }
   /* else { // NOTE: more than 1 argument: not in use at the moment
      std::vector<std::string> args;
      for(int i=0; i<o->getNbArgs(); i++) {
        if(idx >= argc) {
          std::cerr << "Error: Missing argument for option: " << arg << std::endl;
          error();
        }
        args.push_back(std::string(argv[idx]));
        idx++;
      }
      o->func.str_args(opts,args); // execute the function for option o
    } */
  }
  
  CLISParser::CLISParser(void)  {   
    _cli_categories.push_back(constants().cli_cat.solver.str());
    generateDefaultSolverOptions();    
  }
  
  // The functions to be executed for the solver options
  void cli_allSols(CLIOptions* opt) {
    opt->setBoolParam(constants().opts.solver.allSols.str(),true);
  }
  
  void cli_fzn_solver(CLIOptions* opt, std::string& s) {
    opt->setStringParam(constants().opts.solver.fzn_solver.str(),s);
  }
  
  void CLISParser::generateDefaultSolverOptions(void) {
    std::vector<std::string> n_allSols; 
    n_allSols.push_back(constants().cli.solver.all_sols_str.str());    
    CLIOption* o_allSols = new CLIOption(n_allSols, false /* default value */, 
                                      constants().opts.solver.allSols.str(), 
                                      "Find all solutions.",
                                      constants().cli_cat.solver.str(),
                                      cli_allSols ); 
    _known_options[constants().cli.solver.all_sols_str.str()] = o_allSols;
    
    std::vector<std::string> n_fzn_solver;
    n_fzn_solver.push_back(constants().cli.solver.fzn_solver_str.str());
    CLIOption* o_fznSolver = new CLIOption(n_fzn_solver, false /* begins with*/,
                                           constants().opts.solver.fzn_solver.str(),
                                           "The (path to the) fzn-solver for solving the problem",
                                           constants().cli_cat.solver.str(), cli_fzn_solver);
    _known_options[constants().cli.solver.fzn_solver_str.str()] = o_fznSolver;
  }
  
}
