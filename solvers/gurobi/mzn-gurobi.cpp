// * -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

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
#include <ctime>
#include <iomanip>

#include <minizinc/model.hh>
#include <minizinc/parser.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/astexception.hh>

#include <minizinc/flatten.hh>
#include <minizinc/flatten_internal.hh>  // temp., TODO
#include <minizinc/optimize.hh>
#include <minizinc/builtins.hh>
#include <minizinc/file_utils.hh>

#include <minizinc/solver_instance.hh>
#include <minizinc/solvers/gurobi_solverinstance.hh>

using namespace MiniZinc;
using namespace std;

std::string stoptime(clock_t& start) {
  std::ostringstream oss;
  clock_t now = clock();
  oss << std::setprecision(0) << std::fixed << ((static_cast<double>(now-start) / CLOCKS_PER_SEC) * 1000.0) << " ms";
  start = now;
  return oss.str();
}

bool beginswith(string s, string t) {
  return s.compare(0, t.length(), t)==0;
}

int main(int argc, char** argv) {
  string filename;
  vector<string> datafiles;
  vector<string> includePaths;  
  bool is_flatzinc = false;

  bool flag_ignoreStdlib = false;
  bool flag_typecheck = true;
  bool flag_verbose = false;
  bool flag_newfzn = false;
  bool flag_optimize = true;
  bool flag_werror = false;
  bool flag_only_range_domains = false;
  bool flag_all_solutions = false;

  /// PARAMS
  int nThreads=-1;
  string sExportModel;
  double nTimeout=-1;
  double nWorkMemLimit=-1;


  clock_t starttime = std::clock();
  clock_t starttime01 = starttime;
  clock_t lasttime = std::clock();

  string std_lib_dir;
  if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
    std_lib_dir = string(MZNSTDLIBDIR);
  }
  string globals_dir = "linear";

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
    } else if (string(argv[i]) == "-a" ) {
      flag_all_solutions = true;
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
      flag_only_range_domains = true;
    } else if (string(argv[i])=="-Werror") {
      flag_werror = true;
    } else if (string(argv[i])=="--writeModel") {
      i++;
      if (i==argc) {
        goto error;
      }
      sExportModel = argv[i];
    } else if (beginswith(string(argv[i]),"-p")) {
      string nP(argv[i]);
      if (nP.length() > 2) {
        nP.erase(0, 2);
      } else {
        i++;
        if (i==argc) {
          goto error;
        }
        nP = argv[i];
      }
      istringstream iss(nP);
      iss >> nThreads;
      if (!iss && !iss.eof()) {
        cerr << "\nBad value for -p: " << nP << endl;
        goto error;
      }
    } else if (beginswith(string(argv[i]),"--timeout")) {
      string nP(argv[i]);
      if (nP.length() > 9) {
        nP.erase(0, 9);
      } else {
        i++;
        if (i==argc) {
          goto error;
        }
        nP = argv[i];
      }
      istringstream iss(nP);
      iss >> nTimeout;
      if (!iss && !iss.eof()) {
        cerr << "\nBad value for --timeout: " << nP << endl;
        goto error;
      }
    } else if (beginswith(string(argv[i]),"--workmem")) {
      string nP(argv[i]);
      if (nP.length() > 9) {
        nP.erase(0, 9);
      } else {
        i++;
        if (i==argc) {
          goto error;
        }
        nP = argv[i];
      }
      istringstream iss(nP);
      iss >> nWorkMemLimit;
      if (!iss && !iss.eof()) {
        cerr << "\nBad value for --workmem: " << nP << endl;
        goto error;
      }
    } else {
      std::string input_file(argv[i]);
      if (input_file.length()<=4) {
        std::cerr << "Error: cannot handle file " << input_file << "." << std::endl;
        goto error;
      }
      std::string extension = input_file.substr(input_file.length()-4,string::npos);
      if (extension == ".mzn" || extension == ".fzn") {
        is_flatzinc = extension == ".fzn";
        if (filename=="") {
          filename = input_file;
        } else {
          std::cerr << "Error: Multiple .mzn or .fzn files given." << std::endl;
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

  flag_only_range_domains = flag_only_range_domains || globals_dir == "linear";

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
  //   if (flag_output_fzn == "") {
  //     flag_output_fzn = flag_output_base+".fzn";
  //   }
  //   if (flag_output_ozn == "") {
  //     flag_output_ozn = flag_output_base+".ozn";
  //   }

  {
    std::stringstream errstream;
    bool parseDocComments = false;
    if (flag_verbose)
      std::cerr << "Parsing '" << filename << "' ...";
    if (Model* m = parse(filename, datafiles, includePaths, flag_ignoreStdlib, 
          parseDocComments, flag_verbose, errstream)) {
      try {
        if (flag_typecheck) {
          if (flag_verbose)
            std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
          if (flag_verbose)
            std::cerr << "Typechecking ...";
          Env env(m);
          vector<TypeError> typeErrors;
          MiniZinc::typecheck(env, m, typeErrors, false);
          if (typeErrors.size() > 0) {
            for (unsigned int i=0; i<typeErrors.size(); i++) {
              if (flag_verbose)
                std::cerr << std::endl;
              std::cerr << typeErrors[i].loc() << ":" << std::endl;
              std::cerr << typeErrors[i].what() << ": " << typeErrors[i].msg() << std::endl;
            }
            exit(EXIT_FAILURE);
          }
          MiniZinc::registerBuiltins(env, m);
          if (flag_verbose)
            std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;

          if (!flag_instance_check_only) {
            if (is_flatzinc) {
              GCLock lock;
              env.swap();
              populateOutput(env);
            } else {
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
              //            Model* flat = env.flat();
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
              } else {
                env.flat()->compact();
              }

              if (flag_output_fzn_stdout) {
                if (flag_verbose)
                  std::cerr << "Printing FlatZinc to stdout\n";
                Printer p(std::cout,0);
                p.print(env.flat());
              } else if(flag_output_fzn != "") {
                if (flag_verbose)
                  std::cerr << "Printing FlatZinc to " << flag_output_fzn << "...";
                std::ofstream os;
                os.open(flag_output_fzn.c_str(), ios::out);
                Printer p(os,0);
                p.print(env.flat());
                os.close();
                if (flag_verbose)
                  std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
              }
            }

            /// To cout:
            //std::cout << "\n   -------------------  FLATTENING COMPLETE  --------------------------------" << std::endl;
            //std::cout << "% Flattening time  : " << double(lasttime-starttime01)/CLOCKS_PER_SEC << " sec\n" << std::endl;

            /// To cout:
            //             std::cout << "\n\n\n   -------------------  DUMPING env  --------------------------------" << std::endl;
            //             env.envi().dump();

            {
              GCLock lock;
              Options options;

              options.setBoolParam  ("all_solutions",    flag_all_solutions);
              options.setStringParam("export_model",     sExportModel);
              options.setBoolParam  ("verbose",          flag_verbose);
              options.setIntParam   ("parallel_threads", nThreads);
              options.setFloatParam ("timelimit",        nTimeout);

              GurobiSolverInstance gurobi(env,options);
              gurobi.processFlatZinc();
              SolverInstance::Status status = gurobi.solve();
              if (status==SolverInstance::SAT || status==SolverInstance::OPT) {
                gurobi.printSolution();
                if (status==SolverInstance::OPT)
                  std::cout << "==========" << std::endl;
              }
            }

            if(is_flatzinc) {
              env.swap();
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
  << "MIP solver options:" << std::endl
  // -s                  print statistics
  //            << "  --readParam <file>  read Gurobi parameters from file
  //               << "--writeParam <file> write Gurobi parameters to file
  //               << "--tuneParam         instruct Gurobi to tune parameters instead of solving
  << "--writeModel <file> write model to <file> (.lp, .mps)" << std::endl
  << "--solutionCallback  print intermediate solutions  NOT IMPL" << std::endl
  << "-p <N>              use N threads" << std::endl
  << "--nomippresolve     disable MIP presolving   NOT IMPL" << std::endl
  << "--timeout <N>       stop search after N seconds" << std::endl
  << "--workmem <N>       maximal amount of RAM used, MB" << std::endl
  << "--readParam <file>  read Gurobi parameters from file   NOT IMPL" << std::endl
  << "--writeParam <file> write Gurobi parameters to file   NOT IMPL" << std::endl
  << "--tuneParam         instruct Gurobi to tune parameters instead of solving   NOT IMPL" << std::endl
  << "--solutionCallback  print intermediate solutions   NOT IMPL" << std::endl

  << std::endl
  << "Output options:" << std::endl
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
