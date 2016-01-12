/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was ! distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* A basic mzn2fzn wrapper, can be used as a plugin
 */

#include <minizinc/flattener.h>
#include <fstream>

using namespace std;
using namespace MiniZinc;

#ifndef __NO_EXPORT_FLATTENER__  // define this to avoid exporting this class here
Flattener* MiniZinc::getGlobalFlattener(bool fOutputByDefault) {
  return new Flattener(fOutputByDefault);
}
void MiniZinc::cleanupGlobalFlattener(Flattener* pFlt) {
  if (pFlt)
    delete pFlt;
}
#endif  // __NO_EXPORT_FLATTENER__

void Flattener::printVersion(ostream& os)
{
  os << "NICTA MiniZinc to FlatZinc converter, version "
     << MZN_VERSION_MAJOR << "." << MZN_VERSION_MINOR << "." << MZN_VERSION_PATCH << std::endl;
  os << "Copyright (C) " __DATE__ "  " __TIME__ "   Monash University and NICTA" << std::endl;
}

void Flattener::printHelp(ostream& os)
{
  os
  << std::endl
  << "Flattener input options:" << std::endl
  << "  --ignore-stdlib\n    Ignore the standard libraries stdlib.mzn and builtins.mzn" << std::endl
  << "  --instance-check-only\n    Check the model instance (including data) for errors, but do !\n    convert to FlatZinc." << std::endl
  << "  --no-optimize\n    Do not optimize the FlatZinc" << std::endl
  // \n    Currently does nothing (only available for compatibility with 1.6)
  << "  -d <file>, --data <file>\n    File named <file> contains data used by the model." << std::endl
  << "  -D <data>, --cmdline-data <data>\n    Include the given data assignment in the model." << std::endl
  << "  --stdlib-dir <dir>\n    Path to MiniZinc standard library directory" << std::endl
  << "  -G --globals-dir --mzn-globals-dir\n    Search for included files in <stdlib>/<dir>." << std::endl
  << "  -D \"fMIPdomains=false\"\n    No domain unification for MIP" << std::endl
  << "  --only-range-domains\n    When no MIPdomains: all domains contiguous, holes replaced by inequalities" << std::endl
  << std::endl;
  os
  << "Flattener output options:" << std::endl
  << "  --no-output-ozn, -O-\n    Do not output ozn file" << std::endl
  << "  --output-base <name>\n    Base name for output files" << std::endl
  << "  -o <file>, --output-to-file <file>, --output-fzn-to-file <file>\n    Filename for generated FlatZinc output" << std::endl
  << "  --output-ozn-to-file <file>\n    Filename for model output specification" << std::endl
  << "  --output-to-stdout, --output-fzn-to-stdout\n    Print generated FlatZinc to standard output" << std::endl
  << "  --output-ozn-to-stdout\n    Print model output specification to standard output" << std::endl
  << "  -Werror\n    Turn warnings into errors" << std::endl
  ;
}

bool Flattener::processOption(int& i, int argc, const char** argv)
{
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
    flag_no_output_ozn = true;
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
    } else if (string(argv[i])=="-" || string(argv[i])=="--input-from-stdin") {
      if (datafiles.size() > 0 || filenames.size() > 0)
        goto error;
      flag_stdinInput = true;
  } else if (beginswith(string(argv[i]),"-d")) {
    if (flag_stdinInput)
      goto error;
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
    if (flag_stdinInput)
      goto error;
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
    if (flag_stdinInput)
      goto error;
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
    if (flag_stdinInput)
      goto error;
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
  } else if (string(argv[i])=="--no-MIPdomains") {   // internal
    flag_noMIPdomains = true;
  } else if (string(argv[i])=="-Werror") {
    flag_werror = true;
  } else {
    if (flag_stdinInput)
      goto error;
    std::string input_file(argv[i]);
    if (input_file.length()<=4) {
//       std::cerr << "Error: cannot handle file " << input_file << "." << std::endl;
      goto error;
    }
    std::string extension = input_file.substr(input_file.length()-4,string::npos);
    if (extension == ".mzn" || extension ==  ".mzc" || extension == ".fzn") {
      if ( extension == ".fzn" ) {
        is_flatzinc = true;
        if ( fOutputByDefault )        // mzn2fzn mode
          goto error;
      }
//       if (filenames.empty()) {
        filenames.push_back(input_file);
//       } else {
//         std::cerr << "Error: Multiple .mzn or .fzn files given." << std::endl;
//         goto error;
//       }
    } else if (extension == ".dzn") {
      datafiles.push_back(input_file);
    } else {
      if ( fOutputByDefault )
        std::cerr << "Error: cannot handle file extension " << extension << "." << std::endl;
      goto error;
    }
  }
  return true;
error:
  return false;
}

Flattener::Flattener(bool fOutputByDef_)
  : fOutputByDefault(fOutputByDef_)
{
  if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
    std_lib_dir = string(MZNSTDLIBDIR);
  }
}

Flattener::~Flattener()
{
  if (pEnv.get())       // ??? TODO
    if(is_flatzinc) {
      pEnv->swap();
    }
}


void Flattener::flatten()
{
  starttime01 = std::clock();
  lasttime = starttime01;
  
  if (flag_verbose)
    printVersion(cerr);

  // controlled from redefs and command line:
//   if (beginswith(globals_dir, "linear")) {
//     flag_only_range_domains = true;
//     if (flag_verbose)
//       cerr << "Assuming a linear programming-based solver (only_range_domains)." << endl;
//   }

  if ( filenames.empty() && !flag_stdinInput ) {
    throw runtime_error( "Error: no model file given." );
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
    if (flag_stdinInput) {
      flag_output_base = "mznout";
    } else {
      flag_output_base = filenames[0].substr(0,filenames[0].length()-4);
    }
  }
  
  if (flag_output_fzn == filenames[0]) {
    cerr << "  WARNING: fzn filename matches input file, ignoring." << endl;
    flag_output_fzn = "";
  }
  if (flag_output_ozn == filenames[0]) {
    cerr << "  WARNING: ozn filename matches input file, ignoring." << endl;
    flag_output_ozn = "";
  }
  
  if (fOutputByDefault) {
    if (flag_output_fzn == "") {
      flag_output_fzn = flag_output_base+".fzn";
    }
    if (flag_output_ozn == "" && ! flag_no_output_ozn) {
      flag_output_ozn = flag_output_base+".ozn";
    }
  }

  {
    std::stringstream errstream;
    try {
      Model* m;
      if (flag_stdinInput) {
        if (flag_verbose)
          std::cerr << "Parsing standard input ..." << endl;
        std::string input = std::string(istreambuf_iterator<char>(std::cin), istreambuf_iterator<char>());
        m = parseFromString(input, "stdin", includePaths, flag_ignoreStdlib, false, flag_verbose, errstream);
      } else {
        if (flag_verbose)
          std::cerr << "Parsing '" << filenames[0] << "' ...";
        m = parse(filenames, datafiles, includePaths, flag_ignoreStdlib, false, flag_verbose, errstream);
      }
      if (m) {
        pModel.reset(m);
        if (flag_typecheck) {
          if (flag_verbose)
            std::cerr << " done parsing (" << stoptime(lasttime) << ")" << std::endl;
          if (flag_verbose)
            std::cerr << "Typechecking ...";
          pEnv.reset(new Env(m));
          Env& env = *getEnv();
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
                fopts.onlyRangeDomains = flag_only_range_domains;
                ::flatten(env,fopts);
              } catch (LocationException& e) {
                if (flag_verbose)
                  std::cerr << std::endl;
                std::cerr << e.what() << ": " << std::endl;
                env.dumpErrorStack(std::cerr);
                std::cerr << "  " << e.msg() << std::endl;
                exit(EXIT_FAILURE);
              }
              for (unsigned int i=0; i<env.warnings().size(); i++) {
                std::cerr << (flag_werror ? "\n  ERROR: " : "\n  WARNING: ") << env.warnings()[i];
              }
              if (flag_werror && env.warnings().size() > 0) {
                exit(EXIT_FAILURE);
              }
              env.clearWarnings();
              //            Model* flat = env.flat();
              if (flag_verbose)
                std::cerr << " done (" << stoptime(lasttime)
                << "), max stack depth " << env.maxCallStack() << std::endl;

              if ( ! flag_noMIPdomains ) {
                if (flag_verbose)
                  std::cerr << "MIP domains ...";
                MIPdomains(env, flag_statistics);
                if (flag_verbose)
                  std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
              }

              if (flag_optimize) {
                if (flag_verbose)
                  std::cerr << "Optimizing ...";
                optimize(env);
                for (unsigned int i=0; i<env.warnings().size(); i++) {
                  std::cerr << (flag_werror ? "\n  ERROR: " : "\n  WARNING: ") << env.warnings()[i];
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
                env.output()->compact();
              }

              if (flag_statistics) {
                FlatModelStatistics stats = statistics(env);
                std::cerr << "Generated FlatZinc statistics:\n";
                std::cerr << "Variables: ";
                bool had_one = false;
                if (stats.n_bool_vars) {
                  had_one = true;
                  std::cerr << stats.n_bool_vars << " bool";
                }
                if (stats.n_int_vars) {
                  if (had_one) std::cerr << ", ";
                  had_one = true;
                  std::cerr << stats.n_int_vars << " int";
                }
                if (stats.n_float_vars) {
                  if (had_one) std::cerr << ", ";
                  had_one = true;
                  std::cerr << stats.n_float_vars << " float";
                }
                if (stats.n_set_vars) {
                  if (had_one) std::cerr << ", ";
                  had_one = true;
                  std::cerr << stats.n_set_vars << " int";
                }
                if (!had_one)
                  std::cerr << "none";
                std::cerr << "\n";
                std::cerr << "Constraints: ";
                had_one = false;
                if (stats.n_bool_ct) {
                  had_one = true;
                  std::cerr << stats.n_bool_ct << " bool";
                }
                if (stats.n_int_ct) {
                  if (had_one) std::cerr << ", ";
                  had_one = true;
                  std::cerr << stats.n_int_ct << " int";
                }
                if (stats.n_float_ct) {
                  if (had_one) std::cerr << ", ";
                  had_one = true;
                  std::cerr << stats.n_float_ct << " float";
                }
                if (stats.n_set_ct) {
                  if (had_one) std::cerr << ", ";
                  had_one = true;
                  std::cerr << stats.n_set_ct << " int";
                }
                if (!had_one)
                  std::cerr << "none";
                std::cerr << "\n";
              }
              
              if (flag_output_fzn_stdout) {
                if (flag_verbose)
                  std::cerr << "Printing FlatZinc to stdout ..." << std::endl;
                Printer p(std::cout,0);
                p.print(env.flat());
                if (flag_verbose)
                  std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
              } else if(flag_output_fzn != "") {
                if (flag_verbose)
                  std::cerr << "Printing FlatZinc to '"
                  << flag_output_fzn << "' ..." << std::flush;
                std::ofstream os;
                os.open(flag_output_fzn.c_str(), ios::out);
                checkIOStatus (os.good(), " I/O error: cannot open fzn output file. ");
                Printer p(os,0);
                p.print(env.flat());
                checkIOStatus (os.good(), " I/O error: cannot write fzn output file. ");
                os.close();
                if (flag_verbose)
                  std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
              }
              if (!flag_no_output_ozn) {
                if (flag_output_ozn_stdout) {
                  if (flag_verbose)
                    std::cerr << "Printing .ozn to stdout ..." << std::endl;
                  Printer p(std::cout,0);
                  p.print(env.output());
                  if (flag_verbose)
                    std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
                } else if (flag_output_ozn != "") {
                  if (flag_verbose)
                    std::cerr << "Printing .ozn to '"
                    << flag_output_ozn << "' ..." << std::flush;
                  std::ofstream os;
                  os.open(flag_output_ozn.c_str(), std::ios::out);
                  checkIOStatus (os.good(), " I/O error: cannot open ozn output file. ");
                  Printer p(os,0);
                  p.print(env.output());
                  checkIOStatus (os.good(), " I/O error: cannot write ozn output file. ");
                  os.close();
                  if (flag_verbose)
                    std::cerr << " done (" << stoptime(lasttime) << ")" << std::endl;
                }
              }
            }

            /// To cout:
            //             std::cout << "\n\n\n   -------------------  DUMPING env  --------------------------------" << std::endl;
            //             env.envi().dump();
/// Putting this to destructor?  TODO
//             if(is_flatzinc) {
//               env.swap();
//             }
          }
        } else { // !flag_typecheck
          Printer p(std::cout);
          p.print(m);
        }
//         delete m;
      } else {
        if (flag_verbose)
          std::cerr << std::endl;
        std::copy(istreambuf_iterator<char>(errstream),istreambuf_iterator<char>(),ostreambuf_iterator<char>(std::cerr));
        exit(EXIT_FAILURE);
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
  }
  
  if (getEnv()->flat()->failed()) {
    status = SolverInstance::UNSAT;
  }
  
//   if (flag_verbose)
  if (flag_verbose) {
//     std::cerr << "Done (overall time " << stoptime(starttime) << ", ";
//      std::cerr << " done (" << stoptime(lasttime) << "), flattening finished. ";
    size_t mem = GC::maxMem();
    if (mem < 1024)
      std::cerr << "Maximum memory " << mem << " bytes";
    else if (mem < 1024*1024)
      std::cerr << "Maximum memory " << mem/1024 << " Kbytes";
    else
      std::cerr << "Maximum memory " << mem/(1024*1024) << " Mbytes";
    std::cerr << "." << std::endl;    
  }
}

void Flattener::printStatistics(ostream&)
{
}
