/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
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

#include <minizinc/model.hh>
#include <minizinc/parser.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/astexception.hh>
#include <minizinc/hash.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/builtins.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/timer.hh>

using namespace MiniZinc;
using namespace std;

std::string stoptime(Timer& timer) {
  std::ostringstream oss;
  oss << std::setprecision(0) << std::fixed << timer.ms() << " ms";
  return oss.str();
}

int main(int argc, char** argv) {
  Timer starttime;
  string filename;
  
  string flag_output_file;

  bool flag_output_comments = true;
  bool flag_output_flush = true;
  bool flag_output_time = false;
  string solfile;
  int flag_ignore_lines = 0;
  istream& solstream = cin;

  string solution_separator = "----------";
  string solution_comma     = "";
  string unsatisfiable_msg  = "=====UNSATISFIABLE=====";
  string unbounded_msg      = "=====UNBOUNDED=====";
  string unknown_msg        = "=====UNKNOWN=====";
  string search_complete_msg= "==========";

  vector<string> includePaths;
  string std_lib_dir;
  if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
    std_lib_dir = string(MZNSTDLIBDIR);
  }
  
  if (argc < 2)
    goto error;

  for (int i=1; i<argc; i++) {
    if (string(argv[i])==string("--version")) {
      std::cout << "NICTA MiniZinc solution printing tool, version "
      << MZN_VERSION_MAJOR << "." << MZN_VERSION_MINOR << "." << MZN_VERSION_PATCH << std::endl;
      std::cout << "Copyright (C) 2014, 2015 Monash University and NICTA" << std::endl;
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
    } else if (string(argv[i])=="--no-flush-output") {
      flag_output_flush = false;
    } else if (string(argv[i])=="--no-output-comments") {
      flag_output_comments = false;
    } else if (string(argv[i])=="-i" ||
               string(argv[i])=="--ignore-lines" ||
               string(argv[i])=="--ignore-leading-lines") {
      ++i;
      if (i==argc)
        goto error;
      flag_ignore_lines = atoi(argv[i]);
    } else if (string(argv[i])=="--soln-sep" ||
               string(argv[i])=="--soln-separator" ||
               string(argv[i])=="--solution-separator") {
      ++i;
      if (i==argc)
        goto error;
      solution_separator = string(argv[i]);
    } else if (string(argv[i])=="--soln-comma" ||
               string(argv[i])=="--solution-comma") {
      ++i;
      if (i==argc)
        goto error;
      solution_comma = string(argv[i]);
    } else if (string(argv[i])=="--unsat-msg" ||
               string(argv[i])=="--unsatisfiable-msg") {
      ++i;
      if (i==argc)
        goto error;
      unsatisfiable_msg = string(argv[i]);
    } else if (string(argv[i])=="--unbounded-msg") {
      ++i;
      if (i==argc)
        goto error;
      unbounded_msg = string(argv[i]);
    } else if (string(argv[i])=="--unknown-msg") {
      ++i;
      if (i==argc)
        goto error;
      unknown_msg = string(argv[i]);
    } else if (string(argv[i])=="--search-complete-msg") {
      ++i;
      if (i==argc)
        goto error;
      search_complete_msg = string(argv[i]);
    } else if (string(argv[i])=="--output-time") {
      flag_output_time = true;
    } else {
      filename = argv[i++];
      if (filename.length()<=4 ||
          filename.substr(filename.length()-4,string::npos) != ".ozn") {
        std::cerr << "Invalid .ozn file " << filename << "." << std::endl;
        goto error;
      }
    }
  }

  if (std_lib_dir=="") {
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
  
  includePaths.push_back(std_lib_dir+"/std/");
  
  {
    if (Model* outputm = parse(filename, std::vector<std::string>(), includePaths, false, false, false,
                               std::cerr)) {
      try {
        std::vector<TypeError> typeErrors;
        Env env(outputm);
        MiniZinc::typecheck(env,outputm,typeErrors);
        MiniZinc::registerBuiltins(env,outputm);
        
        typedef pair<VarDecl*,Expression*> DE;
        ASTStringMap<DE>::t declmap;
        Expression* outputExpr = NULL;
        for (unsigned int i=0; i<outputm->size(); i++) {
          if (VarDeclI* vdi = (*outputm)[i]->dyn_cast<VarDeclI>()) {
            declmap.insert(pair<ASTString,DE>(vdi->e()->id()->v(),DE(vdi->e(),vdi->e()->e())));
          } else if (OutputI* oi = (*outputm)[i]->dyn_cast<OutputI>()) {
            outputExpr = oi->e();
          }
        }

        //ostream& fout(flag_output_file.empty() ? std::cout : new fstream(flag_output_file));
        fstream file_ostream;
        if (!flag_output_file.empty())
            file_ostream.open(flag_output_file.c_str(), std::fstream::out);
        ostream& fout = flag_output_file.empty() ? std::cout : file_ostream;

        int solutions_found = 0;

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
              ++solutions_found;
              if (flag_output_time)
                fout << "% time elapsed: " << stoptime(starttime) << "\n";
              // Put the "solution comma" before the solution itself,
              // but only for solutions after the first one.
              if (solutions_found > 1 && !solution_comma.empty())
                fout << solution_comma << std::endl;
              if (outputExpr != NULL) {
                for (unsigned int i=0; i<outputm->size(); i++) {
                  if (VarDeclI* vdi = (*outputm)[i]->dyn_cast<VarDeclI>()) {
                    ASTStringMap<DE>::t::iterator it = declmap.find(vdi->e()->id()->v());
                    vdi->e()->e(it->second.second);
                    vdi->e()->evaluated(false);
                  }
                }
                Model* sm = parseFromString(solution, "solution.szn", includePaths, true, false, false, cerr);
                for (unsigned int i=0; i<sm->size(); i++) {
                  if (AssignI* ai = (*sm)[i]->dyn_cast<AssignI>()) {
                    ASTStringMap<DE>::t::iterator it = declmap.find(ai->id());
                    if (it==declmap.end()) {
                      cerr << "Error: unexpected identifier " << ai->id() << " in output\n";
                      exit(EXIT_FAILURE);
                    }
                    ai->e()->type(it->second.first->type());
                    ai->decl(it->second.first);
                    typecheck(env,outputm, ai);
                    if (Call* c = ai->e()->dyn_cast<Call>()) {
                      // This is an arrayXd call, make sure we get the right builtin
                      assert(c->args()[c->args().size()-1]->isa<ArrayLit>());
                      for (unsigned int i=0; i<c->args().size(); i++)
                        c->args()[i]->type(Type::parsetint());
                      c->args()[c->args().size()-1]->type(it->second.first->type());
                      c->decl(outputm->matchFn(env.envi(),c));
                    }
                    it->second.first->e(ai->e());
                  }
                }
                delete sm;
                
                GCLock lock;
                ArrayLit* al = eval_array_lit(env.envi(),outputExpr);
                std::string os;
                for (unsigned int i=0; i<al->v().size(); i++) {
                  std::string s = eval_string(env.envi(),al->v()[i]);
                  if (!s.empty()) {
                    os = s;
                    fout << os;
                    if (flag_output_flush)
                      fout.flush();
                  }
                }
                if (!os.empty() && os[os.size()-1] != '\n')
                  fout << std::endl;
                  if (flag_output_flush)
                    fout.flush();
              }
              fout << comments;
              fout << solution_separator << std::endl;
              if (flag_output_flush)
                fout.flush();
              solution = "";
              comments = "";
            } else if (line=="==========") {
              if (flag_output_time)
                fout << "% time elapsed: " << stoptime(starttime) << "\n";
              fout << search_complete_msg << std::endl;
              if (flag_output_flush)
                fout.flush();
            } else if(line=="=====UNSATISFIABLE=====") {
              if (flag_output_time)
                fout << "% time elapsed: " << stoptime(starttime) << "\n";
              fout << unsatisfiable_msg << std::endl;
              if (flag_output_flush)
                fout.flush();
            } else if(line=="=====UNBOUNDED=====") {
              if (flag_output_time)
                fout << "% time elapsed: " << stoptime(starttime) << "\n";
              fout << unbounded_msg << std::endl;
              if (flag_output_flush)
                fout.flush();
            } else if(line=="=====UNKNOWN=====") {
              if (flag_output_time)
                fout << "% time elapsed: " << stoptime(starttime) << "\n";
              fout << unknown_msg << std::endl;
              if (flag_output_flush)
                fout.flush();
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
        fout << comments;
        if (flag_output_flush)
          fout.flush();
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
            << "  --output-time\n    Print timing information in the FlatZinc solution stream." << std::endl
            << "  --no-flush-output\n    Don't flush output stream after every line." << std::endl
            << "  -i <n>, --ignore-lines <n>, --ignore-leading-lines <n>\n    Ignore the first <n> lines in the FlatZinc solution stream." << std::endl
            << "  --soln-sep <s>, --soln-separator <s>, --solution-separator <s>\n    Specify the string printed after each solution.\n    The default is to use the same as FlatZinc,\n    \"----------\"." << std::endl
            << "  --soln-comma <s>, --solution-comma <s>\n    Specify the string used to separate solutions.\n    The default is the empty string." << std::endl
            << "  --unsat-msg <msg>, --unsatisfiable-msg <msg>\n    Specify the message to print if the model instance is\n    unsatisfiable.\n    The default is to print \"=====UNSATISFIABLE=====\"." << std::endl
            << "  --unbounded-msg <msg>\n    Specify the message to print if the objective of the\n    model instance is unbounded.\n    The default is to print \"=====UNBOUNDED=====\"." << std::endl
            << "  --unknown-msg <msg>\n    Specify the message to print if search terminates without\n    the entire search space having been explored and no\n    solution has been found.\n    The default is to print \"=====UNKNOWN=====\"." << std::endl
            << "  --search-complete-msg <msg>\n    Specify the message to print if search terminates having\n    explored the entire search space.\n    The default is to print \"==========\"." << std::endl
  ;

  exit(EXIT_FAILURE);
}
