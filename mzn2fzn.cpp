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
  if (argc < 2)
    goto error;

  GC::init();

  for (;;) {
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
    } else {
      break;
    }
    i++;
  }

  if (i==argc) {
    goto error;
  }
  filename = argv[i++];
  
  while (i<argc)
    datafiles.push_back(argv[i++]);

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
          Model* flat = flatten(m);

          if (flag_optimize) {
            if (flag_verbose)
              std::cerr << "Optimizing..." << std::endl;
            optimize(flat);
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
            p.print(flat,std::cout);
          }
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
            << "\t--ignore-stdlib\tIgnore the standard libraries stdlib.mzn and builtins.mzn" << std::endl
            << "\t--newfzn\tOutput in the new FlatZinc format" << std::endl
            << "\t--verbose\tPrint progress statements" << std::endl
            << "\t--no-typecheck\tDo not typecheck (implies --no-eval)" << std::endl
            << "\t--no-eval\tDo not evaluate" << std::endl
            << "\t--no-optimize\tDo not optimize the FlatZinc (may speed up large instances)" << std::endl
            << "\t--no-output\tDo not print the output" << std::endl;

  exit(EXIT_FAILURE);
}
