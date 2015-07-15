/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */


#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>

#include <minizinc/cli.hh>
#include <minizinc/model.hh>
#include <minizinc/parser.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/astexception.hh>

#include <minizinc/flatten.hh>
#include <minizinc/optimize.hh>
#include <minizinc/builtins.hh>
#include <minizinc/file_utils.hh>

#include <minizinc/solver_instance.hh>
#include <minizinc/solvers/fzn_solverinstance.hh>

using namespace MiniZinc;
using namespace std;

std::string stoptime(clock_t& start) {
  std::ostringstream oss;
  clock_t now = clock();
  oss << std::setprecision(0) << std::fixed << ((static_cast<double>(now-start) / CLOCKS_PER_SEC) * 1000.0) << " ms";
  start = now;
  return oss.str();
}

bool beginswith(string s, string t) {
  return s.compare(0, t.length(), t)==0;
}

int main(int argc, char** argv) {
  clock_t starttime = std::clock();
  clock_t lasttime = std::clock();
  
  CLIParser cp; CLIOptions* opts = cp.parseArgs(argc,argv);
    
  std::string filename = opts->getStringParam(constants().opts.model.str());  
  std::vector<std::string> datafiles;
  if(opts->hasParam(constants().opts.datafiles.str())) {
    datafiles = opts->getStringVectorParam(constants().opts.datafiles.str());
  }
  else if(opts->hasParam(constants().opts.datafile.str())) {
    std::string s = opts->getStringParam(constants().opts.datafile.str());
    if(s!="")
      datafiles.push_back(s);
  }
  
  string std_lib_dir;
  if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
    std_lib_dir = string(MZNSTDLIBDIR);
  }
  if(opts->hasParam(constants().opts.stdlib.str())) {
    std_lib_dir = opts->getStringParam(constants().opts.stdlib.str());
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
  
  std::string globals_dir;
  if(opts->hasParam(constants().opts.globalsDir.str())) {
    globals_dir  = opts->getStringParam(constants().opts.globalsDir.str());
  }  
  std::vector<std::string> includePaths;
  if (globals_dir!="") {
    includePaths.push_back(std_lib_dir+"/"+globals_dir+"/");
  }
  includePaths.push_back(std_lib_dir+"/std/");  
  for (unsigned int i=0; i<includePaths.size(); i++) {
    if (!FileUtils::directory_exists(includePaths[i])) {
      std::cerr << "Cannot access include directory " << includePaths[i] << "\n";
      std::exit(EXIT_FAILURE);
    }
  }
  
  std::string output_base;
  if(opts->hasParam(constants().opts.outputBase.str())) 
    output_base = opts->getStringParam(constants().opts.outputBase.str());  
  if(output_base =="") {
    output_base = filename.substr(0,filename.length()-4);
  }
  std::string output_fzn;
  if(opts->hasParam(constants().opts.fznToFile.str())) {
    output_fzn = opts->getStringParam(constants().opts.fznToFile.str());
  }
  if(output_fzn=="") {
    output_fzn = output_base+".fzn";
  }
  std::string output_ozn;
  if(opts->hasParam(constants().opts.oznToFile.str())) {
    output_ozn = opts->getStringParam(constants().opts.oznToFile.str());
  }
  if(output_ozn =="") {
    output_ozn = output_base+".ozn";
  }
  
  std::cerr << "DEBUG: Parsed input:\nmodel = " << filename << "\ndata = " << (datafiles.size() == 0 ? "" : datafiles[0]) << "\nglobals-dir = " << globals_dir << "\noutputbase = " << output_base << std::endl;
  
  bool flag_verbose = opts->getBoolParam(constants().opts.verbose.str());
  bool flag_werror = opts->getBoolParam(constants().opts.werror.str());
  bool flag_ignoreStdlib = opts->getBoolParam(constants().opts.ignoreStdlib.str());
  {
    std::stringstream errstream;
    if (flag_verbose)
      std::cerr << "Parsing '" << filename << "' ...";
    if (Model* m = parse(filename, datafiles, includePaths, flag_ignoreStdlib, false,
                         flag_verbose, errstream)) {
      try {
        if (opts->getBoolParam(constants().opts.typecheck.str())) {
          if (flag_verbose)
            std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
          if (flag_verbose)
            std::cerr << "Typechecking ...";

          Env env(m);
          vector<TypeError> typeErrors;
          MiniZinc::typecheck(env, m, typeErrors);
          if (typeErrors.size() > 0) {
            for (unsigned int i=0; i<typeErrors.size(); i++) {
              if (flag_verbose)
                std::cerr << std::endl;
              std::cerr << typeErrors[i].loc() << ":" << std::endl;
              std::cerr << typeErrors[i].what() << ": " << typeErrors[i].msg() << std::endl;
            }
            exit(EXIT_FAILURE);
          }
          MiniZinc::registerBuiltins(env, m);
          if (flag_verbose)
            std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
          
          if (!opts->getBoolParam(constants().opts.instanceCheckOnly.str())) {
            if (flag_verbose)
              std::cerr << "Flattening ...";
            
            FlatteningOptions fopts;
            try {
              flatten(env,fopts);
            } catch (LocationException& e) {
              if (flag_verbose)
                std::cerr << std::endl;
              std::cerr << e.what() << ": " << std::endl;
              env.dumpErrorStack(std::cerr);
              std::cerr << "  " << e.msg() << std::endl;
              exit(EXIT_FAILURE);
            }
            for (unsigned int i=0; i<env.warnings().size(); i++) {
              std::cerr << (flag_werror ? "Error: " : "Warning: ") << env.warnings()[i];
            }
            if (flag_werror && env.warnings().size() > 0) {
              exit(EXIT_FAILURE);
            }
            //            Model* flat = env.flat();
            if (flag_verbose)
              std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
            
            if (opts->getBoolParam(constants().opts.optimize.str())) {
              if (flag_verbose)
                std::cerr << "Optimizing ...";
              optimize(env);
              if (flag_verbose)
                std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
            }
            
            if (!opts->getBoolParam(constants().opts.newfzn.str())) {
              if (flag_verbose)
                std::cerr << "Converting to old FlatZinc ...";
              oldflatzinc(env);
              if (flag_verbose)
                std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
            } else {
              env.flat()->compact();
            }
            
            if (!opts->getBoolParam(constants().opts.noOznOutput.str())) {
              if (flag_verbose)
                std::cerr << "Printing .ozn ...";
              if (opts->getBoolParam(constants().opts.oznToStdout.str())) {
                Printer p(std::cout,0);
                p.print(env.output());
              } else {
                std::ofstream os;
                std::cerr << "DEBUG: output_ozn: " << output_ozn << std::endl;
                os.open(output_ozn.c_str(), ios::out);
                if (!os.good()) {
                  if (flag_verbose)
                    std::cerr << std::endl;
                  std::cerr << "I/O error: cannot open ozn output file. " << strerror(errno) << "." << std::endl;
                  exit(EXIT_FAILURE);
                }
                Printer p(os,0);
                p.print(env.output());
                os.close();
              }
              if (flag_verbose)
                std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
            }
            
            {
              GCLock lock;
              Options options; // TODO: update the Options from CLIOptions!
              FZNSolverInstance flatzinc(env,options);
              flatzinc.processFlatZinc();
              SolverInstance::Status status = flatzinc.solve();
              if (status==SolverInstance::SAT || status==SolverInstance::OPT) {
                env.evalOutput(std::cout);
                std::cout << "----------\n";
                if (status==SolverInstance::OPT)
                  std::cout << "==========\n";
              }
            }
            
          }
        } else { // !flag_typecheck
          Printer p(std::cout);
          p.print(m);
        }
      } catch (LocationException& e) {
        if (flag_verbose)
          std::cerr << std::endl;
        std::cerr << e.loc() << ":" << std::endl;
        std::cerr << e.what() << ": " << e.msg() << std::endl;
        exit(EXIT_FAILURE);
      } catch (Exception& e) {
        if (flag_verbose)
          std::cerr << std::endl;
        std::cerr << e.what() << ": " << e.msg() << std::endl;
        exit(EXIT_FAILURE);           
      }
      delete m;
    } else {
      if (flag_verbose)
        std::cerr << std::endl;
      std::copy(istreambuf_iterator<char>(errstream),istreambuf_iterator<char>(),ostreambuf_iterator<char>(std::cerr));
      exit(EXIT_FAILURE);
    }
  }
  
  if (flag_verbose)
    std::cerr << "Done (overall time " << stoptime(starttime) << ")." << std::endl;
  return 0;

}
