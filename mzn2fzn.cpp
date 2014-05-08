/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>

#include <minizinc/model.hh>
#include <minizinc/parser.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/exception.hh>

#include <minizinc/flatten.hh>
#include <minizinc/optimize.hh>
#include <minizinc/builtins.hh>

using namespace MiniZinc;
using namespace std;

std::string stoptime(clock_t& start) {
  std::ostringstream oss;
  clock_t now = clock();
  oss << std::setprecision(0) << std::fixed << ((static_cast<double>(now-start) / CLOCKS_PER_SEC) * 1000.0) << " ms";
  start = now;
  return oss.str();
}

int main(int argc, char** argv) {
  int i=1;
  string filename;
  vector<string> datafiles;
  vector<string> includePaths;  
  bool flag_ignoreStdlib = false;
  bool flag_typecheck = true;
  bool flag_eval = true;
  bool flag_verbose = false;
  bool flag_newfzn = false;
  bool flag_optimize = true;
  
  clock_t starttime = std::clock();
  clock_t lasttime = std::clock();
  
  string std_lib_dir;
  if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
    std_lib_dir = string(MZNSTDLIBDIR);
  }
  string globals_dir;
  
  bool flag_no_output_ozn = false;
  string flag_output_base;
  string flag_output_fzn;
  string flag_output_ozn;
  bool flag_output_fzn_stdout = false;
  bool flag_output_ozn_stdout = false;
  FlatteningOptions fopts;
  
  if (argc < 2)
    goto error;

  GC::init();
  
  for (;;) {
    if (string(argv[i])==string("-h") || string(argv[i])==string("--help"))
        goto error;
    if (string(argv[i])==string("-I")) {
      i++;
      if (i==argc) {
        goto error;
      }
      includePaths.push_back(argv[i]+string("/"));
    } else if (string(argv[i])==string("--ignore-stdlib")) {
      flag_ignoreStdlib = true;
    } else if (string(argv[i])==string("--no-typecheck")) {
      flag_typecheck = false; flag_eval=false;
    } else if (string(argv[i])==string("--no-eval")) {
      flag_eval = false;
    } else if (string(argv[i])==string("--verbose")) {
      flag_verbose = true;
    } else if (string(argv[i])==string("--newfzn")) {
      flag_newfzn = true;
    } else if (string(argv[i])==string("--no-optimize")) {
      flag_optimize = false;
    } else if (string(argv[i])==string("--no-output-ozn") ||
               string(argv[i])==string("-O-")) {
      flag_no_output_ozn = false;
    } else if (string(argv[i])=="--output-base") {
      i++;
      if (i==argc)
        goto error;
      flag_output_base = argv[i];
    } else if (string(argv[i])=="-o" || string(argv[i])=="--output-to-file" ||
               string(argv[i])=="--output-fzn-to-file") {
      i++;
      if (i==argc)
        goto error;
      flag_output_fzn = argv[i];
    } else if (string(argv[i])=="--output-ozn-to-file") {
      i++;
      if (i==argc)
        goto error;
      flag_output_ozn = argv[i];
    } else if (string(argv[i])=="--output-to-stdout" ||
               string(argv[i])=="--output-fzn-to-stdout") {
      flag_output_fzn_stdout = true;
    } else if (string(argv[i])=="--output-ozn-to-stdout") {
      flag_output_ozn_stdout = true;
    } else if (string(argv[i])=="--stdlib-dir") {
      i++;
      if (i==argc)
        goto error;
      std_lib_dir = argv[i];
    } else if (string(argv[i])=="-G" ||
               string(argv[i])=="--globals-dir" ||
               string(argv[i])=="--mzn-globals-dir") {
      i++;
      if (i==argc)
        goto error;
      globals_dir = argv[i];
    } else if (string(argv[i])=="--only-range-domains") {
      fopts.onlyRangeDomains = true;
    } else {
      break;
    }
    i++;
  }

  if (std_lib_dir=="") {
    std::cerr << "Error: unknown minizinc standard library directory.\n"
              << "Specify --stdlib-dir on the command line or set the\n"
              << "MZN_STDLIB_DIR environment variable.\n";
    std::exit(EXIT_FAILURE);
  }
  
  if (globals_dir!="") {
    includePaths.push_back(std_lib_dir+"/"+globals_dir+"/");
  }
  includePaths.push_back(std_lib_dir+"/std/");
  
  if (i==argc) {
    goto error;
  }
  filename = argv[i++];
  if (filename.length()<=4 ||
      filename.substr(filename.length()-4,string::npos) != ".mzn")
    goto error;
  
  if (flag_output_base == "") {
    flag_output_base = filename.substr(0,filename.length()-4);
  }
  if (flag_output_fzn == "") {
    flag_output_fzn = flag_output_base+".fzn";
  }
  if (flag_output_ozn == "") {
    flag_output_ozn = flag_output_base+".ozn";
  }
  
  while (i<argc) {
    std::string datafile = argv[i++];
    if (datafile.length()<=4 ||
        datafile.substr(datafile.length()-4,string::npos) != ".dzn")
      goto error;
    datafiles.push_back(datafile);
  }

  {
    std::stringstream errstream;
    if (flag_verbose)
      std::cerr << "Parsing '" << filename << "' ...";
    if (Model* m = parse(filename, datafiles, includePaths, flag_ignoreStdlib, 
                         errstream)) {
      try {
        if (flag_typecheck) {
          if (flag_verbose)
            std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
          if (flag_verbose)
            std::cerr << "Typechecking ...";
          MiniZinc::typecheck(m);
          MiniZinc::registerBuiltins(m);
          if (flag_verbose)
            std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;

          if (flag_verbose)
            std::cerr << "Flattening ...";
          Env env(m);
          try {
            flatten(env,fopts);
          } catch (LocationException& e) {
            if (flag_verbose)
              std::cerr << std::endl;
            std::cerr << e.what() << ": " << e.msg() << std::endl;
            std::cerr << e.loc() << std::endl;
            env.dumpErrorStack(std::cerr);
            exit(EXIT_FAILURE);
          }
          Model* flat = env.flat();
          if (flag_verbose)
            std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
          
          if (flag_optimize) {
            if (flag_verbose)
              std::cerr << "Optimizing ...";
            optimize(env);
            if (flag_verbose)
              std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
          }

          if (!flag_newfzn) {
            if (flag_verbose)
              std::cerr << "Converting to old FlatZinc ...";
            oldflatzinc(env);
            if (flag_verbose)
              std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
          }
          
          if (flag_verbose)
            std::cerr << "Printing FlatZinc ...";
          if (flag_output_fzn_stdout) {
            Printer p(std::cout,0);
            p.print(flat);
          } else {
            std::ofstream os;
            os.open(flag_output_fzn.c_str(), ios::out);
            Printer p(os,0);
            p.print(flat);
            os.close();
          }
          if (flag_verbose)
            std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
          if (!flag_no_output_ozn) {
            if (flag_verbose)
              std::cerr << "Printing .ozn ...";
            if (flag_output_ozn_stdout) {
              Printer p(std::cout,0);
              p.print(env.output());
            } else {
              std::ofstream os;
              os.open(flag_output_ozn.c_str(), ios::out);
              Printer p(os,0);
              p.print(env.output());
              os.close();
            }
            if (flag_verbose)
              std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
          }
        } else { // !flag_typecheck
          Printer p(std::cout);
          p.print(m);
        }
      } catch (LocationException& e) {
        if (flag_verbose)
          std::cerr << std::endl;
        std::cerr << e.what() << ": " << e.msg() << std::endl;
        std::cerr << e.loc() << std::endl;
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
    }
  }

  if (flag_verbose)
    std::cerr << "Done (overall time " << stoptime(starttime) << ")." << std::endl;
  return 0;

error:
  std::cerr << "Usage: "<< argv[0]
            << " [<options>] [-I <include path>] <model>.mzn [<data>.dzn ...]" << std::endl
            << std::endl
            << "Options:" << std::endl
            << "  --help, -h\n    Print this help message" << std::endl
            << "  --ignore-stdlib\n    Ignore the standard libraries stdlib.mzn and builtins.mzn" << std::endl
            << "  --newfzn\n    Output in the new FlatZinc format" << std::endl
            << "  --verbose\n    Print progress statements" << std::endl
            << "  --no-typecheck\n    Do not typecheck (implies --no-eval)" << std::endl
            << "  --no-eval\n    Do not evaluate" << std::endl
            << "  --no-optimize\n    Do not optimize the FlatZinc (may speed up large instances)" << std::endl
            << "  --no-output-ozn, -O-\n    Do not output ozn file" << std::endl
            << "  --output-base <name>\n    Base name for output files" << std::endl
            << "  -o <file>, --output-to-file <file>, --output-fzn-to-file <file>\n    Filename for generated FlatZinc output" << std::endl
            << "  --output-ozn-to-file <file>\n    Filename for model output specification" << std::endl
            << "  --output-to-stdout, --output-fzn-to-stdout\n    Print generated FlatZinc to standard output" << std::endl
            << "  --output-ozn-to-stdout\n    Print model output specification to standard output" << std::endl
            << "  --stdlib-dir <dir>\n    Path to MiniZinc standard library directory" << std::endl
            << "  -G --globals-dir --mzn-globals-dir\n    Search for included files in <stdlib>/<dir>." << std::endl
  ;

  exit(EXIT_FAILURE);
}
