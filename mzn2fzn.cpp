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

int main(int argc, char** argv) {
  int i=1;
  string filename;
  vector<string> datafiles;
  vector<string> includePaths;  
  bool flag_ignoreStdlib = false;
  bool flag_typecheck = true;
  bool flag_eval = true;
  bool flag_output = true;
  bool flag_verbose = false;
  bool flag_newfzn = false;
  bool flag_optimize = true;
  
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
    } else if (string(argv[i])==string("--no-output")) {
      flag_output = false;
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
    if (filename.length()<=4 ||
        filename.substr(filename.length()-4,string::npos) != ".dzn")
      goto error;
    datafiles.push_back(argv[i++]);
  }

  {
    if (flag_verbose)
      std::cerr << "Parsing '" << filename << "' ..." << std::endl;
    if (Model* m = parse(filename, datafiles, includePaths, flag_ignoreStdlib, 
                         std::cerr)) {
      try {
        if (flag_typecheck) {
          if (flag_verbose)
            std::cerr << "Typechecking..." << std::endl;
          MiniZinc::typecheck(m);
          MiniZinc::registerBuiltins(m);

          if (flag_verbose)
            std::cerr << "Flattening..." << std::endl;
          Env env(m);
          flatten(env);
          Model* flat = env.flat();
          
          if (flag_optimize) {
            if (flag_verbose)
              std::cerr << "Optimizing..." << std::endl;
            optimize(env);
          }

          if (flag_output) {
            if (!flag_newfzn) {
              if (flag_verbose)
                std::cerr << "Converting to old FlatZinc..." << std::endl;
              oldflatzinc(flat);
            }

            if (flag_verbose)
              std::cerr << "Printing FlatZinc..." << std::endl;
            Printer p;
            if (flag_output_fzn_stdout) {
              p.print(flat,std::cout,0);
            } else {
              std::ofstream os;
              os.open(flag_output_fzn, ios::out);
              p.print(flat,os,0);
              os.close();
            }
            if (!flag_no_output_ozn) {
              if (flag_output_ozn_stdout) {
                p.print(env.output(),std::cout,0);
              } else {
                std::ofstream os;
                os.open(flag_output_ozn, ios::out);
                p.print(env.output(),os,0);
                os.close();
              }
            }
            
          }
        } else if (flag_output) { // !flag_typecheck
          Printer p;
          p.print(m,std::cout);
        }
      } catch (LocationException& e) {
        std::cerr << e.what() << ": " << e.msg() << std::endl;
        std::cerr << e.loc() << std::endl;
        exit(EXIT_FAILURE);
      } catch (Exception& e) {
        std::cerr << e.what() << ": " << e.msg() << std::endl;
        exit(EXIT_FAILURE);
      }
      delete m;
    }
  }

  if (flag_verbose)
    std::cerr << "Done." << std::endl;
  return 0;

error:
  std::cerr << "Usage: "<< argv[0]
            << " [<options>] [-I <include path>] <model>.mzn [<data>.dzn ...]" << std::endl
            << std::endl
            << "Options:" << std::endl
            << "\t--help  -h\tPrint this help message" << std::endl
            << "\t--ignore-stdlib\tIgnore the standard libraries stdlib.mzn and builtins.mzn" << std::endl
            << "\t--newfzn\tOutput in the new FlatZinc format" << std::endl
            << "\t--verbose\tPrint progress statements" << std::endl
            << "\t--no-typecheck\tDo not typecheck (implies --no-eval)" << std::endl
            << "\t--no-eval\tDo not evaluate" << std::endl
            << "\t--no-optimize\tDo not optimize the FlatZinc (may speed up large instances)" << std::endl
            << "\t--no-output\tDo not print the output" << std::endl;

  exit(EXIT_FAILURE);
}
