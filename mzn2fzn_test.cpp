/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef _MSC_VER 
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cerrno>

#include <minizinc/model.hh>
#include <minizinc/parser.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/astexception.hh>

#include <minizinc/flatten.hh>
#include <minizinc/optimize.hh>
#include <minizinc/builtins.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/timer.hh>

#include <minizinc/cli.hh>

using namespace MiniZinc;
using namespace std;

std::string stoptime(Timer& start) {
  std::ostringstream oss;
  oss << std::setprecision(0) << std::fixed << start.ms() << " ms";
  start.reset();
  return oss.str();
}

bool beginswith(string s, string t) {
  return s.compare(0, t.length(), t)==0;
}

int main(int argc, char** argv) {

  
  Timer starttime;
  Timer lasttime;   
  FlatteningOptions fopts;
    
  CLIParser cp; 
  CLIOptions* opts;
  try {
    opts = cp.parseArgs(argc,argv);
  } catch (Exception& e) {        
    std::cerr << e.what() << ": " << e.msg() << std::endl;
    exit(EXIT_FAILURE);           
  }
    
  std::string filename = opts->getStringParam(constants().opts.model.str());  
  std::vector<std::string> datafiles;
  if(opts->hasParam(constants().opts.datafiles.str())) {
    datafiles= opts->getStringVectorParam(constants().opts.datafiles.str()); 
  }
  string std_lib_dir =  opts->getStringParam(constants().opts.stdlib.str());    
  std::string globals_dir = opts->getStringParam(constants().opts.globalsDir.str());
  std::vector<std::string> includePaths = opts->getStringVectorParam(constants().opts.includePaths.str());  
  std::string output_base = opts->getStringParam(constants().opts.outputBase.str());  
  std::string output_fzn = opts->getStringParam(constants().opts.fznToFile.str());
  std::string output_ozn = opts->getStringParam(constants().opts.oznToFile.str());
  bool flag_verbose = opts->getBoolParam(constants().opts.verbose.str());
  bool flag_werror = opts->getBoolParam(constants().opts.werror.str());
  bool flag_ignoreStdlib = opts->getBoolParam(constants().opts.ignoreStdlib.str());
  bool flag_stdinInput = opts->getBoolParam(constants().opts.inputFromStdin.str());

  std::cerr << "DEBUG: stdlib-dir = " << std_lib_dir << "\n";
  std::cerr << "DEBUG: globals-dir = " << globals_dir << "\n";
  std::cerr << "DEBUG: output-fzn = " << output_fzn << "\n";
  
  {
    std::stringstream errstream;
    if (flag_verbose) {
      if (flag_stdinInput) {
        std::cerr << "Parsing standard input" << std::endl;
      } else {
        std::cerr << "Parsing '" << filename << "'" << std::endl;
      }
    }
    Model* m;
    if (flag_stdinInput) {
      filename = "stdin";
      std::string input = std::string(istreambuf_iterator<char>(std::cin), istreambuf_iterator<char>());
      m = parseFromString(input, filename, includePaths, flag_ignoreStdlib, false, flag_verbose, errstream);
    } else {
      m = parse(filename, datafiles, includePaths, flag_ignoreStdlib, false, flag_verbose, errstream);
    }
    
    if (m) {
      try {
        if (opts->getBoolParam(constants().opts.typecheck.str())) {
          Env env(m);
          if (flag_verbose)
            std::cerr << "Done parsing (" << stoptime(lasttime) << ")" << std::endl;
          if (flag_verbose)
            std::cerr << "Typechecking ...";
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
          MiniZinc::registerBuiltins(env,m);
          if (flag_verbose)
            std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;

          if (!opts->getBoolParam(constants().opts.instanceCheckOnly.str())) {
            if (flag_verbose)
              std::cerr << "Flattening ...";
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
            env.clearWarnings();
            Model* flat = env.flat();
            if (flag_verbose)
              std::cerr << " done (" << stoptime(lasttime) << ", max stack depth " << env.maxCallStack() << ")" << std::endl;
            
            if (opts->getBoolParam(constants().opts.optimize.str())) {
              if (flag_verbose)
                std::cerr << "Optimizing ...";
              optimize(env);
              for (unsigned int i=0; i<env.warnings().size(); i++) {
                std::cerr << (flag_werror ? "Error: " : "Warning: ") << env.warnings()[i];
              }
              if (flag_werror && env.warnings().size() > 0) {
                exit(EXIT_FAILURE);
              }
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
            
            if (opts->getBoolParam(constants().opts.statistics.str())) {
              FlatModelStatistics stats = statistics(env);
              std::cerr << "Generated FlatZinc statistics:\n";
              std::cerr << "Variables: ";
              bool had_one = false;
              if (stats.n_bool_vars) {
                had_one = true;
                std::cerr << stats.n_bool_vars << " bool";
              }
              if (stats.n_int_vars) {
                if (had_one) std::cerr << ", ";
                had_one = true;
                std::cerr << stats.n_int_vars << " int";
              }
              if (stats.n_float_vars) {
                if (had_one) std::cerr << ", ";
                had_one = true;
                std::cerr << stats.n_float_vars << " float";
              }
              if (stats.n_set_vars) {
                if (had_one) std::cerr << ", ";
                had_one = true;
                std::cerr << stats.n_set_vars << " int";
              }
              if (!had_one)
                std::cerr << "none";
              std::cerr << "\n";
              std::cerr << "Constraints: ";
              had_one = false;
              if (stats.n_bool_ct) {
                had_one = true;
                std::cerr << stats.n_bool_ct << " bool";
              }
              if (stats.n_int_ct) {
                if (had_one) std::cerr << ", ";
                had_one = true;
                std::cerr << stats.n_int_ct << " int";
              }
              if (stats.n_float_ct) {
                if (had_one) std::cerr << ", ";
                had_one = true;
                std::cerr << stats.n_float_ct << " float";
              }
              if (stats.n_set_ct) {
                if (had_one) std::cerr << ", ";
                had_one = true;
                std::cerr << stats.n_set_ct << " int";
              }
              if (!had_one)
                std::cerr << "none";
              std::cerr << "\n";
            }
            
            if (flag_verbose)
              std::cerr << "Printing FlatZinc ...";
            if (opts->getBoolParam(constants().opts.fznToStdout.str())) {
              Printer p(std::cout,0);
              p.print(flat);
            } else {
              std::ofstream os;
              os.open(output_fzn.c_str(), ios::out);
              if (!os.good()) {
                if (flag_verbose)
                  std::cerr << std::endl;
                std::cerr << "I/O error: cannot open fzn output file. " << strerror(errno) << "." << std::endl;
                exit(EXIT_FAILURE);
              }
              Printer p(os,0);
              p.print(flat);
              os.close();
            }
            if (flag_verbose)
              std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
            if (!opts->getBoolParam(constants().opts.noOznOutput.str())) {
              if (flag_verbose)
                std::cerr << "Printing .ozn ...";
              if (opts->getBoolParam(constants().opts.oznToStdout.str())) {
                Printer p(std::cout,0);
                p.print(env.output());
              } else {
                std::ofstream os;
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

  if (flag_verbose) {
    std::cerr << "Done (overall time " << stoptime(starttime) << ", ";
    size_t mem = GC::maxMem();
    if (mem < 1024)
      std::cerr << "maximum memory " << mem << " bytes";
    else if (mem < 1024*1024)
      std::cerr << "maximum memory " << mem/1024 << " Kbytes";
    else
      std::cerr << "maximum memory " << mem/(1024*1024) << " Mbytes";
    std::cerr << ")." << std::endl;
    
  }
  return 0;
}
