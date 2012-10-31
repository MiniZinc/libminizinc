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

#include <minizinc/eval_par.hh>
#include <minizinc/builtins.hh>

using namespace MiniZinc;
using namespace std;

int main(int argc, char** argv) {
  int i=1;
  string filename;
  vector<string> datafiles;
  vector<string> includePaths;  
  bool ignoreStdlib = false;
  bool typecheck = true;
  bool eval = true;
  bool output = true;
  bool outputFundecls = false;
  bool verbose = false;
  bool allSolutions = false;
  bool free = false;
  int nbThreads = 1;
  if (argc < 2)
    goto error;

  for (;;) {
    if (string(argv[i])==string("-I")) {
      i++;
      if (i==argc) {
        goto error;
      }
      includePaths.push_back(argv[i]+string("/"));
    } else if (string(argv[i])==string("--ignore-stdlib")) {
      ignoreStdlib = true;
    } else if (string(argv[i])==string("--no-output")) {
      output = false;
    } else if (string(argv[i])==string("--no-fundecl-output")) {
      outputFundecls = false;
    } else if (string(argv[i])==string("--no-typecheck")) {
      typecheck = false; eval=false;
    } else if (string(argv[i])==string("--no-eval")) {
      eval = false;
    } else if (string(argv[i])==string("--verbose")) {
      verbose = true;
    } else if (string(argv[i])==string("-a")) {
      allSolutions = true;
    } else if (string(argv[i])==string("-f")) {
      free = true;
    } else if (string(argv[i])==string("-p")) {
      i++;
      nbThreads = atoi(argv[i]);
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
    ASTContext ctx;

    if (Model* m = parse(ctx, filename, datafiles, includePaths, ignoreStdlib, 
                         std::cerr)) {
      try {
        if (verbose)
          std::cerr << "parsing " << filename << std::endl;
        if (typecheck) {
          MiniZinc::typecheck(ctx,m);
          MiniZinc::registerBuiltins(ctx);
          // eval_int(ctx,m);
          Printer p;
          p.print(m,std::cerr);
        }
        // if (verbose)
        //   std::cerr << "  typechecked" << std::endl;
        // flat = m->flatten(tm);
        // if (verbose)
        //   std::cerr << "  flattened" << std::endl;
        // string outfilename;
        // size_t sep = filename.rfind(".");
        // if (sep == string::npos) {
        //   outfilename = filename+".pr.mzn";
        // } else {
        //   outfilename = filename.substr(0,sep)+".pr.mzn";
        // }
        // if (output) {
        //   std::ofstream os(outfilename.c_str());
        //   if (!os.good()) {
        //     std::cerr << "Could not open file " << outfilename << " for output."
        //               << std::endl;
        //     exit(EXIT_FAILURE);
        //   }
        //   flat->print(os,outputFundecls);
        // }
      } catch (LocationException& e) {
        std::cerr << e.what() << ": " << e.msg() << std::endl;
        std::cerr << "In file " << e.loc().filename->str() << ":"
            << e.loc().first_line << "c"
            << e.loc().first_column << "-"
            << e.loc().last_line << "c"
            << e.loc().last_column
            << endl;
        exit(EXIT_FAILURE);
      } catch (Exception& e) {
        std::cerr << e.what() << ": " << e.msg() << std::endl;
        exit(EXIT_FAILURE);
      }
      delete m;
    }
  }
  return 0;

error:
  std::cerr << "Usage: "<< argv[0]
            << " [--ignore-stdlib] [-I <include path>] <model>.mzn [<data>.dzn ...]" << std::endl;
  exit(EXIT_FAILURE);
}
