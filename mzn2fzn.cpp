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

#include <minizinc/solvers/gecode/gecode_pass.hh>

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
  string filename;
  vector<string> datafiles;
  vector<string> includePaths;  
  bool flag_ignoreStdlib = false;
  bool flag_typecheck = true;
  bool flag_verbose = false;
  bool flag_newfzn = false;
  bool flag_optimize = true;
  bool flag_werror = false;
  bool flag_gecode = false;
  unsigned int flag_npasses = 1;
  
  Timer starttime;
  Timer lasttime;
  
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
  bool flag_instance_check_only = false;
  FlatteningOptions fopts;
  
  if (argc < 2)
    goto error;

  GC::init();
  
  for (int i=1; i<argc; i++) {
    if (string(argv[i])==string("-h") || string(argv[i])==string("--help"))
        goto error;
    if (string(argv[i])==string("--version")) {
      std::cout << "NICTA MiniZinc to FlatZinc converter, version "
        << MZN_VERSION_MAJOR << "." << MZN_VERSION_MINOR << "." << MZN_VERSION_PATCH << std::endl;
      std::cout << "Copyright (C) 2014 Monash University and NICTA" << std::endl;
      std::exit(EXIT_SUCCESS);
    }
    if (beginswith(string(argv[i]),"-I")) {
      string include(argv[i]);
      if (include.length() > 2) {
        includePaths.push_back(include.substr(2)+string("/"));
      } else {
        i++;
        if (i==argc) {
          goto error;
        }
        includePaths.push_back(argv[i]+string("/"));
      }
    } else if (string(argv[i])==string("--ignore-stdlib")) {
      flag_ignoreStdlib = true;
    } else if (string(argv[i])==string("--no-typecheck")) {
      flag_typecheck = false;
    } else if (string(argv[i])==string("--instance-check-only")) {
      flag_instance_check_only = true;
    } else if (string(argv[i])==string("-v") || string(argv[i])==string("--verbose")) {
      flag_verbose = true;
      fopts.verbose = true;
    } else if (string(argv[i])==string("--newfzn")) {
      flag_newfzn = true;
    } else if (string(argv[i])==string("--no-optimize") || string(argv[i])==string("--no-optimise")) {
      flag_optimize = false;
    } else if (string(argv[i])==string("--no-output-ozn") ||
               string(argv[i])==string("-O-")) {
      flag_no_output_ozn = false;
    } else if (string(argv[i])=="--output-base") {
      i++;
      if (i==argc)
        goto error;
      flag_output_base = argv[i];
    } else if (string(argv[i])=="--use-gecode") {
      flag_gecode = true;
    } else if (string(argv[i])=="--npass") {
      i++;
      if (i==argc) {
        goto error;
      }
      int passes = atoi(argv[i]);
      if(passes > 0)
        flag_npasses = passes;
    } else if (beginswith(string(argv[i]),"-o")) {
        string filename(argv[i]);
        if (filename.length() > 2) {
          flag_output_fzn = filename.substr(2);
        } else {
          i++;
          if (i==argc) {
            goto error;
          }
          flag_output_fzn = argv[i];
        }
    } else if (string(argv[i])=="--output-to-file" ||
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
    } else if (beginswith(string(argv[i]),"-d")) {
      string filename(argv[i]);
      string datafile;
      if (filename.length() > 2) {
        datafile = filename.substr(2);
      } else {
        i++;
        if (i==argc) {
          goto error;
        }
        datafile = argv[i];
      }
      if (datafile.length()<=4 ||
          datafile.substr(datafile.length()-4,string::npos) != ".dzn")
        goto error;
      datafiles.push_back(datafile);
    } else if (string(argv[i])=="--data") {
      i++;
      if (i==argc) {
        goto error;
      }
      string datafile = argv[i];
      if (datafile.length()<=4 ||
          datafile.substr(datafile.length()-4,string::npos) != ".dzn")
        goto error;
      datafiles.push_back(datafile);
    } else if (string(argv[i])=="--stdlib-dir") {
      i++;
      if (i==argc)
        goto error;
      std_lib_dir = argv[i];
    } else if (beginswith(string(argv[i]),"-G")) {
      string filename(argv[i]);
      if (filename.length() > 2) {
        globals_dir = filename.substr(2);
      } else {
        i++;
        if (i==argc) {
          goto error;
        }
        globals_dir = argv[i];
      }
    } else if (beginswith(string(argv[i]),"-D")) {
      string cmddata(argv[i]);
      if (cmddata.length() > 2) {
        datafiles.push_back("cmd:/"+cmddata.substr(2));
      } else {
        i++;
        if (i==argc) {
          goto error;
        }
        datafiles.push_back("cmd:/"+string(argv[i]));
      }
    } else if (string(argv[i])=="--cmdline-data") {
      i++;
      if (i==argc) {
        goto error;
      }
      datafiles.push_back("cmd:/"+string(argv[i]));
    } else if (string(argv[i])=="--globals-dir" ||
               string(argv[i])=="--mzn-globals-dir") {
      i++;
      if (i==argc)
        goto error;
      globals_dir = argv[i];
    } else if (string(argv[i])=="--only-range-domains") {
      fopts.onlyRangeDomains = true;
    } else if (string(argv[i])=="-Werror") {
      flag_werror = true;
    } else {
      std::string input_file(argv[i]);
      if (input_file.length()<=4) {
        std::cerr << "Error: cannot handle file " << input_file << "." << std::endl;
        goto error;
      }
      std::string extension = input_file.substr(input_file.length()-4,string::npos);
      if (extension == ".mzn") {
        if (filename=="") {
          filename = input_file;
        } else {
          std::cerr << "Error: Multiple .mzn files given." << std::endl;
          goto error;
        }
      } else if (extension == ".dzn") {
        datafiles.push_back(input_file);
      } else {
        std::cerr << "Error: cannot handle file extension " << extension << "." << std::endl;
        goto error;
      }
    }
  }

  if (filename=="") {
    std::cerr << "Error: no model file given." << std::endl;
    goto error;
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
  
  if (flag_output_base == "") {
    flag_output_base = filename.substr(0,filename.length()-4);
  }
  if (flag_output_fzn == "") {
    flag_output_fzn = flag_output_base+".fzn";
  }
  if (flag_output_ozn == "") {
    flag_output_ozn = flag_output_base+".ozn";
  }

  {
    std::stringstream errstream;
    if (flag_verbose)
      std::cerr << "Parsing '" << filename << "'" << std::endl;
    if (Model* m = parse(filename, datafiles, includePaths, flag_ignoreStdlib, false,
                         flag_verbose, errstream)) {
      try {
        if (flag_typecheck) {
          if (flag_verbose)
            std::cerr << "Done parsing (" << stoptime(lasttime) << ")" << std::endl;
          if (flag_verbose)
            std::cerr << "Typechecking ...";
          vector<TypeError> typeErrors;
          MiniZinc::typecheck(m, typeErrors);
          if (typeErrors.size() > 0) {
            for (unsigned int i=0; i<typeErrors.size(); i++) {
              if (flag_verbose)
                std::cerr << std::endl;
              std::cerr << typeErrors[i].loc() << ":" << std::endl;
              std::cerr << typeErrors[i].what() << ": " << typeErrors[i].msg() << std::endl;
            }
            exit(EXIT_FAILURE);
          }
          MiniZinc::registerBuiltins(m);
          if (flag_verbose)
            std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;

          if (!flag_instance_check_only) {
            Env env(m);
            try {
              std::vector<Pass*> passes;
              Options gopts;
              FlatteningOptions pass_opts = fopts;
              for(unsigned int i=1; i<flag_npasses; i++) {
                if(flag_gecode)
                  passes.push_back(new GecodePass(pass_opts, gopts));
                else
                  passes.push_back(new CompilePass(pass_opts, "std"));
              }

              std::vector<std::string> cleanIncludePaths(includePaths.size()-1);
              for(unsigned int i=0; i<includePaths.size(); i++)
                if(i != includePaths.size()-2)
                  cleanIncludePaths.push_back(includePaths[i]);

              // Multi-pass optimisations
              if(flag_npasses > 1)
                multiPassFlatten(env, cleanIncludePaths, passes);

              // Final compilation
              if (flag_verbose)
                std::cerr << "Final Flattening ...";
              flatten(env, fopts);

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
              std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
            
            if (flag_optimize) {
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
            
            if (!flag_newfzn) {
              if (flag_verbose)
                std::cerr << "Converting to old FlatZinc ...";
              oldflatzinc(env);
              if (flag_verbose)
                std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
            } else {
              env.flat()->compact();
            }
            
            if (flag_verbose)
              std::cerr << "Printing FlatZinc ...";
            if (flag_output_fzn_stdout) {
              Printer p(std::cout,0);
              p.print(flat);
            } else {
              std::ofstream os;
              os.open(flag_output_fzn.c_str(), ios::out);
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
            if (!flag_no_output_ozn) {
              if (flag_verbose)
                std::cerr << "Printing .ozn ...";
              if (flag_output_ozn_stdout) {
                Printer p(std::cout,0);
                p.print(env.output());
              } else {
                std::ofstream os;
                os.open(flag_output_ozn.c_str(), ios::out);
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

  if (flag_verbose)
    std::cerr << "Done (overall time " << stoptime(starttime) << ")." << std::endl;
  return 0;

error:
  std::cerr << "Usage: "<< argv[0]
            << " [<options>] [-I <include path>] <model>.mzn [<data>.dzn ...]" << std::endl
            << std::endl
            << "Options:" << std::endl
            << "  --help, -h\n    Print this help message" << std::endl
            << "  --version\n    Print version information" << std::endl
            << "  --ignore-stdlib\n    Ignore the standard libraries stdlib.mzn and builtins.mzn" << std::endl
            << "  -v, --verbose\n    Print progress statements" << std::endl
            << "  --instance-check-only\n    Check the model instance (including data) for errors, but do not\n    convert to FlatZinc." << std::endl
            << "  --no-optimize\n    Do not optimize the FlatZinc\n    Currently does nothing (only available for compatibility with 1.6)" << std::endl
            << "  -d <file>, --data <file>\n    File named <file> contains data used by the model." << std::endl
            << "  -D <data>, --cmdline-data <data>\n    Include the given data in the model." << std::endl
            << "  --stdlib-dir <dir>\n    Path to MiniZinc standard library directory" << std::endl
            << "  -G --globals-dir --mzn-globals-dir\n    Search for included files in <stdlib>/<dir>." << std::endl
            << std::endl
            << "Output options:" << std::endl << std::endl
            << "  --no-output-ozn, -O-\n    Do not output ozn file" << std::endl
            << "  --output-base <name>\n    Base name for output files" << std::endl
            << "  -o <file>, --output-to-file <file>, --output-fzn-to-file <file>\n    Filename for generated FlatZinc output" << std::endl
            << "  --output-ozn-to-file <file>\n    Filename for model output specification" << std::endl
            << "  --output-to-stdout, --output-fzn-to-stdout\n    Print generated FlatZinc to standard output" << std::endl
            << "  --output-ozn-to-stdout\n    Print model output specification to standard output" << std::endl
            << "  -Werror\n    Turn warnings into errors" << std::endl
  ;

  exit(EXIT_FAILURE);
}
