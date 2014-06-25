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
    if (string(argv[i])==string("--version")) {
      std::cout << "NICTA MiniZinc solution printing tool, version "
      << MZN_VERSION_MAJOR << "." << MZN_VERSION_MINOR << "." << MZN_VERSION_PATCH << std::endl;
      std::cout << "Copyright (C) 2014 Monash University and NICTA" << std::endl;
      std::exit(EXIT_SUCCESS);
    }
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
  
  {
    if (Model* outputm = parse(filename, std::vector<std::string>(), includePaths, false,
                               std::cerr)) {
      try {
        std::vector<TypeError> typeErrors;
        MiniZinc::typecheck(outputm,typeErrors);
        MiniZinc::registerBuiltins(outputm);

        typedef pair<VarDecl*,Expression*> DE;
        ASTStringMap<DE>::t declmap;
        Expression* outputExpr = NULL;
        for (int i=0; i<outputm->size(); i++) {
          if (VarDeclI* vdi = (*outputm)[i]->dyn_cast<VarDeclI>()) {
            declmap.insert(pair<ASTString,DE>(vdi->e()->id()->v(),DE(vdi->e(),vdi->e()->e())));
          } else if (OutputI* oi = (*outputm)[i]->dyn_cast<OutputI>()) {
            outputExpr = oi->e();
          }
        }
        
        string solution;
        string comments;
        for (;;) {
          if (solstream.good()) {
            string line;
            getline(solstream, line);
            if (flag_ignore_lines > 0) {
              flag_ignore_lines--;
              continue;
            }
            if (line=="----------") {
              if (outputExpr != NULL) {
                for (int i=0; i<outputm->size(); i++) {
                  if (VarDeclI* vdi = (*outputm)[i]->dyn_cast<VarDeclI>()) {
                    ASTStringMap<DE>::t::iterator it = declmap.find(vdi->e()->id()->v());
                    vdi->e()->e(it->second.second);
                    vdi->e()->evaluated(false);
                  }
                }
                Model* sm = parseFromString(solution, "solution.szn", includePaths, true, cerr);
                for (int i=0; i<sm->size(); i++) {
                  if (AssignI* ai = (*sm)[i]->dyn_cast<AssignI>()) {
                    ASTStringMap<DE>::t::iterator it = declmap.find(ai->id());
                    if (it==declmap.end()) {
                      cerr << "Error: unexpected identifier " << ai->id() << " in output\n";
                      exit(EXIT_FAILURE);
                    }
                    ai->e()->type(it->second.first->type());
                    ai->decl(it->second.first);
                    typecheck(outputm, ai);
                    if (Call* c = ai->e()->dyn_cast<Call>()) {
                      // This is an arrayXd call, make sure we get the right builtin
                      assert(c->args()[c->args().size()-1]->isa<ArrayLit>());
                      for (int i=0; i<c->args().size(); i++)
                        c->args()[i]->type(Type::parsetint());
                      c->args()[c->args().size()-1]->type(it->second.first->type());
                      c->decl(outputm->matchFn(c));
                    }
                    it->second.first->e(ai->e());
                  }
                }

                GCLock lock;
                ArrayLit* al = eval_array_lit(outputExpr);
                std::string os;
                for (int i=0; i<al->v().size(); i++) {
                  std::string s = eval_string(al->v()[i]);
                  if (!s.empty()) {
                    os = s;
                    std::cout << os;
                  }
                }
                if (os.empty() || os[os.size()-1] != '\n')
                  std::cout << std::endl;
              }
              solution = "";
              comments = "";
              cout << comments;
              cout << line << std::endl;
            } else if (line=="==========" ||
                       line=="=====UNSATISFIABLE=====" ||
                       line=="=====UNBOUNDED=====" ||
                       line=="=====UNKNOWN=====") {
              cout << line << std::endl;
            } else {
              solution += line+"\n";
              size_t comment_pos = line.find('%');
              if (comment_pos != string::npos) {
                comments += line.substr(comment_pos);
                comments += "\n";
              }
            }
          } else {
            break;
          }
        }
        cout << comments;
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
            << "  --help, -h\n    Print this help message." << std::endl
            << "  --version\n    Print version information." << std::endl
            << "  -o <file>, --output-to-file <file>\n    Filename for generated output." << std::endl
            << "  --stdlib-dir <dir>\n    Path to MiniZinc standard library directory." << std::endl
            << "  --no-output-comments\n    Do not print comments in the FlatZinc solution stream." << std::endl
            << "  -i <n>, --ignore-lines <n>, --ignore-leading-lines <n>\n    Ignore the first <n> lines in the FlatZinc solution stream." << std::endl
  ;

  exit(EXIT_FAILURE);
}
