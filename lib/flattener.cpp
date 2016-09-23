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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <minizinc/flattener.hh>
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
  os << "Copyright (C) 2014-" << string(__DATE__).substr(7, 4)
     << "   Monash University and NICTA" << std::endl;
}

void Flattener::printHelp(ostream& os)
{
  os
  << std::endl
  << "Flattener input options:" << std::endl
  << "  --ignore-stdlib\n    Ignore the standard libraries stdlib.mzn and builtins.mzn" << std::endl
  << "  --instance-check-only\n    Check the model instance (including data) for errors, but do not\n    convert to FlatZinc." << std::endl
  << "  -e, --model-check-only\n    Check the model (without requiring data) for errors, but do not\n    convert to FlatZinc." << std::endl
  << "  --model-interface-only\n    Only extract parameters and output variables." << std::endl
  << "  --no-optimize\n    Do not optimize the FlatZinc" << std::endl
  // \n    Currently does nothing (only available for compatibility with 1.6)
  << "  -d <file>, --data <file>\n    File named <file> contains data used by the model." << std::endl
  << "  -D <data>, --cmdline-data <data>\n    Include the given data assignment in the model." << std::endl
  << "  --stdlib-dir <dir>\n    Path to MiniZinc standard library directory" << std::endl
  << "  -G --globals-dir --mzn-globals-dir <dir>\n    Search for included globals in <stdlib>/<dir>." << std::endl
  << "  - --input-from-stdin\n    Read problem from standard input" << std::endl
  << "  -I --search-dir\n    Additionally search for included files in <dir>." << std::endl
  << "  -D \"fMIPdomains=false\"\n    No domain unification for MIP" << std::endl
  << "  --only-range-domains\n    When no MIPdomains: all domains contiguous, holes replaced by inequalities" << std::endl
  << std::endl;
  os
  << "Flattener output options:" << std::endl
  << "  --no-output-ozn, -O-\n    Do not output ozn file" << std::endl
  << "  --output-base <name>\n    Base name for output files" << std::endl
  << ( fOutputByDefault ? "  -o <file>, --fzn <file>, --output-to-file <file>, --output-fzn-to-file <file>\n"
       : "  --fzn <file>, --output-fzn-to-file <file>\n" )
  << "    Filename for generated FlatZinc output" << std::endl
  << "  -O, --ozn, --output-ozn-to-file <file>\n    Filename for model output specification (-O- for none)" << std::endl
  << "  --output-to-stdout, --output-fzn-to-stdout\n    Print generated FlatZinc to standard output" << std::endl
  << "  --output-ozn-to-stdout\n    Print model output specification to standard output" << std::endl
  << "  -Werror\n    Turn warnings into errors" << std::endl
  ;
}

bool Flattener::processOption(int& i, const int argc, const char** argv)
{
  CLOParser cop( i, argc, argv );
  string buffer;
  
  if ( cop.getOption( "-I --search-dir", &buffer ) ) {
    includePaths.push_back(buffer+string("/"));
  } else if ( cop.getOption( "--ignore-stdlib" ) ) {
    flag_ignoreStdlib = true;
  } else if ( cop.getOption( "--no-typecheck") ) {
    flag_typecheck = false;
  } else if ( cop.getOption( "--instance-check-only") ) {
    flag_instance_check_only = true;
  } else if ( cop.getOption( "-e --model-check-only") ) {
    flag_model_check_only = true;
  } else if ( cop.getOption( "--model-interface-only") ) {
    flag_model_interface_only = true;
  } else if ( cop.getOption( "-v --verbose") ) {
    flag_verbose = true;
  } else if (string(argv[i])==string("--newfzn")) {
    flag_newfzn = true;
  } else if ( cop.getOption( "--no-optimize --no-optimise") ) {
    flag_optimize = false;
  } else if ( cop.getOption( "--no-output-ozn -O-") ) {
    flag_no_output_ozn = true;
  } else if ( cop.getOption( "--output-base", &flag_output_base ) ) {
  } else if ( cop.getOption(
    fOutputByDefault ?
      "-o --fzn --output-to-file --output-fzn-to-file"
      : "--fzn --output-fzn-to-file", &flag_output_fzn) ) {
  } else if ( cop.getOption( "-O --ozn --output-ozn-to-file", &flag_output_ozn) ) {
  } else if ( cop.getOption( "--output-to-stdout --output-fzn-to-stdout" ) ) {
    flag_output_fzn_stdout = true;
  } else if ( cop.getOption( "--output-ozn-to-stdout" ) ) {
    flag_output_ozn_stdout = true;
  } else if ( cop.getOption( "- --input-from-stdin" ) ) {
      if (datafiles.size() > 0 || filenames.size() > 0)
        goto error;
      flag_stdinInput = true;
  } else if ( cop.getOption( "-d --data", &buffer ) ) {
    if (flag_stdinInput)
      goto error;
    if ( buffer.length()<=4 ||
         buffer.substr(buffer.length()-4,string::npos) != ".dzn")
      goto error;
    datafiles.push_back(buffer);
  } else if ( cop.getOption( "--stdlib-dir", &std_lib_dir ) ) {
  } else if ( cop.getOption( "-G --globals-dir --mzn-globals-dir", &globals_dir ) ) {
  } else if ( cop.getOption( "-D --cmdline-data", &buffer)) {
    if (flag_stdinInput)
      goto error;
    datafiles.push_back("cmd:/"+buffer);
  } else if ( cop.getOption( "--only-range-domains" ) ) {
    flag_only_range_domains = true;
  } else if ( cop.getOption( "--no-MIPdomains" ) ) {   // internal
    flag_noMIPdomains = true;
  } else if ( cop.getOption( "-Werror" ) ) {
    flag_werror = true;
  } else {
    if (flag_stdinInput)
      goto error;
    std::string input_file(argv[i]);
    if (input_file.length()<=4) {
//       std::cerr << "Error: cannot handle file " << input_file << "." << std::endl;
      goto error;
    }
    size_t last_dot = input_file.find_last_of('.');
    if (last_dot == string::npos) {
      goto error;
    }
    std::string extension = input_file.substr(last_dot,string::npos);
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
    } else if (extension == ".dzn" || extension == ".json") {
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
  
  if ( filenames.end() !=
      find( filenames.begin(), filenames.end(), flag_output_fzn ) ||
       datafiles.end() !=
      find( datafiles.begin(), datafiles.end(), flag_output_fzn ) ) {
    cerr << "  WARNING: fzn filename '" << flag_output_fzn
      << "' matches an input file, ignoring." << endl;
    flag_output_fzn = "";
  }
  if ( filenames.end() !=
      find( filenames.begin(), filenames.end(), flag_output_ozn ) ||
       datafiles.end() !=
      find( datafiles.begin(), datafiles.end(), flag_output_ozn ) ) {
    cerr << "  WARNING: ozn filename '" << flag_output_ozn
      << "' matches an input file, ignoring." << endl;
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
      pEnv.reset(new Env());
      Env& env = *getEnv();
      if (flag_stdinInput) {
        if (flag_verbose)
          std::cerr << "Parsing standard input ..." << endl;
        std::string input = std::string(istreambuf_iterator<char>(std::cin), istreambuf_iterator<char>());
        std::vector<SyntaxError> se;
        m = parseFromString(input, "stdin", includePaths, flag_ignoreStdlib, false, flag_verbose, errstream, se);
      } else {
        if (flag_verbose)
          std::cerr << "Parsing '" << filenames[0] << "' ...";
        m = parse(env, filenames, datafiles, includePaths, flag_ignoreStdlib, false, flag_verbose, errstream);
      }
      if (m) {
        env.model(m);
//         pModel.reset(m);   // seems to be unnec
        if (flag_typecheck) {
          if (flag_verbose)
            std::cerr << " done parsing (" << stoptime(lasttime) << ")" << std::endl;
          if (flag_verbose)
            std::cerr << "Typechecking ...";
          vector<TypeError> typeErrors;
          MiniZinc::typecheck(env, m, typeErrors, flag_model_check_only || flag_model_interface_only);
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

          if (flag_model_interface_only) {
            MiniZinc::output_model_interface(env, m, std::cout);
          }
          
          if (!flag_instance_check_only && !flag_model_check_only && !flag_model_interface_only) {
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
            }

            if (flag_statistics) {
              FlatModelStatistics stats = statistics(env);
              std::cerr << "Generated FlatZinc statistics:\n";
              std::cerr << "Variables: ";
              HadOne ho;
              std::cerr << ho(stats.n_bool_vars, " bool");
              std::cerr << ho(stats.n_int_vars, " int");
              std::cerr << ho(stats.n_float_vars, " float");
              std::cerr << ho(stats.n_set_vars, " set");
              if (!ho)
                std::cerr << "none";
              std::cerr << "\n";
              ho.reset();
              std::cerr << "Constraints: ";
              std::cerr << ho(stats.n_bool_ct, " bool");
              std::cerr << ho(stats.n_int_ct, " int");
              std::cerr << ho(stats.n_float_ct, " float");
              std::cerr << ho(stats.n_set_ct, " set");
              if (!ho)
                std::cerr << "none";
              std::cerr << "\n";
              /// Objective+bounds / SAT
              SolveI* solveItem = env.flat()->solveItem();
              if (solveItem->st() != SolveI::SolveType::ST_SAT) {
                if (solveItem->st() == SolveI::SolveType::ST_MAX) {
                  cerr << "    This is a maximization problem." << endl;
                } else {
                  cerr << "    This is a minimization problem." << endl;
                }
//                 cerr << "    Bounds for the objective function: "
//                   << dObjVarLB << ", " << dObjVarUB << endl;
              } else {
                cerr << "    This is a satisfiability problem." << endl;
              }
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
            /// To cout:
            //             std::cout << "\n\n\n   -------------------  DUMPING env  --------------------------------" << std::endl;
            //             env.envi().dump();
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
//       throw;
    } catch (Exception& e) {
      if (flag_verbose)
        std::cerr << std::endl;
      std::cerr << e.what() << ": " << e.msg() << std::endl;
      exit(EXIT_FAILURE);
//       throw;
    }
  }
  
  if (getEnv()->envi().failed()) {
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
