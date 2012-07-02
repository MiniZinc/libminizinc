/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Guido Tack <tack@gecode.org>
 *
 *  Copyright:
 *     Guido Tack, 2007
 *
 *  Last modified:
 *     $Date: 2012-03-05 14:52:32 +1100 (Mon, 05 Mar 2012) $ by $Author: tack $
 *     $Revision: 1290 $
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <iostream>
#include <fstream>

#include <minizinc/model.hh>
#include <minizinc/parser.hh>

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
      Model* flat=NULL;
      // try {
        if (verbose)
          std::cerr << "parsing " << filename << std::endl;
        // TypeMap tm;
        // if (typecheck)
        //   m->typecheck(tm,true);
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
      // } catch (TypeError& e) {
      //   std::cerr << "Error: " << e.msg() << std::endl;
      //   std::cerr << "In file " << e.loc().filename << ":"
      //       << e.loc().first_line << "c"
      //       << e.loc().first_column << "-"
      //       << e.loc().last_line << "c"
      //       << e.loc().last_column
      //       << endl;
      //   exit(EXIT_FAILURE);
      // } catch (EvalError& e) {
      //   std::cerr << "Error: " << e.msg() << std::endl;
      //   std::cerr << "In file " << e.loc().filename << ":"
      //       << e.loc().first_line << "c"
      //       << e.loc().first_column << "-"
      //       << e.loc().last_line << "c"
      //       << e.loc().last_column
      //       << endl;
      //   exit(EXIT_FAILURE);
      // }
    }
  }
  return 0;

error:
  std::cerr << "Usage: "<< argv[0]
            << " [--ignore-stdlib] [-I <include path>] <model>.mzn [<data>.dzn ...]" << std::endl;
  exit(EXIT_FAILURE);
}
