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
#include <minizinc/hash.hh>
#include <minizinc/eval_par.hh>

#include <minizinc/builtins.hh>

using namespace MiniZinc;
using namespace std;

int main(int argc, char** argv) {
  int i=1;
  string filename;
  
  string flag_output_file;

  bool flag_output_comments = true;
  string solfile;
  int flag_ignore_lines = 0;
  istream& solstream = cin;

  vector<string> includePaths;
  string std_lib_dir;
  if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
    std_lib_dir = string(MZNSTDLIBDIR);
  }
  
  if (argc < 2)
    goto error;

  GC::init();
  
  for (;;) {
    if (string(argv[i])==string("-h") || string(argv[i])==string("--help")) {
        goto error;
    } else if (string(argv[i])=="-o" || string(argv[i])=="--output-to-file") {
      i++;
      if (i==argc)
        goto error;
      flag_output_file = argv[i];
    } else if (string(argv[i])=="--stdlib-dir") {
      i++;
      if (i==argc)
        goto error;
      std_lib_dir = argv[i];
    } else if (string(argv[i])=="--no-output-comments") {
      flag_output_comments = false;
    } else if (string(argv[i])=="-i" ||
               string(argv[i])=="--ignore-lines" ||
               string(argv[i])=="--ignore-leading-lines") {
      ++i;
      if (i==argc)
        goto error;
      flag_ignore_lines = atoi(argv[i]);
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
  
  includePaths.push_back(std_lib_dir+"/std/");

  if (i==argc) {
    goto error;
  }
  filename = argv[i++];
  if (filename.length()<=4 ||
      filename.substr(filename.length()-4,string::npos) != ".ozn")
    goto error;
//  if (i==argc) {
//    goto error;
//  }
  
//  solfile = argv[i++];
//  solstream = ifstream(solfile);
  
  {
    if (Model* outputm = parse(filename, std::vector<std::string>(), includePaths, false,
                               std::cerr)) {
      try {
        MiniZinc::typecheck(outputm);
        MiniZinc::registerBuiltins(outputm);

        ASTStringMap<VarDecl*>::t declmap;
        Expression* outputExpr = NULL;
        for (int i=0; i<outputm->size(); i++) {
          if (VarDeclI* vdi = (*outputm)[i]->dyn_cast<VarDeclI>()) {
            declmap.insert(pair<ASTString,VarDecl*>(vdi->e()->id()->v(),vdi->e()));
          } else if (OutputI* oi = (*outputm)[i]->dyn_cast<OutputI>()) {
            outputExpr = oi->e();
          }
        }
        
        string solution;
        for (;;) {
          if (solstream.good()) {
            string line;
            getline(solstream, line);
            if (line=="----------") {
              if (outputExpr != NULL) {
                Model* sm = parseFromString(solution, "solution.szn", includePaths, true, cerr);
                for (int i=0; i<sm->size(); i++) {
                  if (AssignI* ai = (*sm)[i]->dyn_cast<AssignI>()) {
                    ASTStringMap<VarDecl*>::t::iterator it = declmap.find(ai->id());
                    if (it==declmap.end()) {
                      cerr << "Error: unexpected identifier " << ai->id() << " in output\n";
                      exit(EXIT_FAILURE);
                    }
                    ai->e()->type(it->second->type());
                    if (Call* c = ai->e()->dyn_cast<Call>()) {
                      // This is an arrayXd call, make sure we get the right builtin
                      assert(c->args()[c->args().size()-1]->isa<ArrayLit>());
                      for (int i=0; i<c->args().size(); i++)
                        c->args()[i]->type(Type::parsetint());
                      c->args()[c->args().size()-1]->type(it->second->type());
                      c->decl(outputm->matchFn(c));
                    }
                    it->second->e(ai->e());
                  }
                }
                GCLock lock;
                ArrayLit* al = eval_array_lit(outputExpr);
                for (int i=0; i<al->v().size(); i++)
                  std::cout << eval_string(al->v()[i]);
              }
              solution = "";
              cout << line << std::endl;
            } else if (line=="==========" ||
                       line=="=====UNSATISFIABLE=====" ||
                       line=="=====UNBOUNDED=====" ||
                       line=="=====UNKNOWN=====") {
              cout << line << std::endl;
            } else {
              solution += line+"\n";
            }
          } else {
            break;
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
      delete outputm;
    }
  }

  return 0;

error:
  std::cerr << "Usage: "<< argv[0]
            << " [<options>] <model>.ozn" << std::endl
            << std::endl
            << "Options:" << std::endl
            << "  --help, -h\n    Print this help message" << std::endl
            << "  -o <file>, --output-to-file <file>\n    Filename for generated output" << std::endl
  ;

  exit(EXIT_FAILURE);
}
