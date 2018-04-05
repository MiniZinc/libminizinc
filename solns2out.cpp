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
 
// #include <iostream>
// #include <fstream>
// #include <iomanip>
// 
// #include <minizinc/model.hh>
// #include <minizinc/parser.hh>
// #include <minizinc/prettyprinter.hh>
// #include <minizinc/typecheck.hh>
// #include <minizinc/astexception.hh>
// #include <minizinc/hash.hh>
// #include <minizinc/eval_par.hh>
// #include <minizinc/builtins.hh>
// #include <minizinc/file_utils.hh>
// #include <minizinc/timer.hh>

#include <minizinc/solns2out.hh>
#include <minizinc/solver_config.hh>

using namespace MiniZinc;
using namespace std;

namespace MiniZinc {
  class Solns2OutFull : public Solns2Out {
    const int argc;
    const char** argv;
    string std_lib_dir;
    istream& solstream = cin;
    SolverConfigs solver_configs;
  public:
    string filename;
    Solns2OutFull( const int ac, const char** av )
      : Solns2Out(cout, cerr), argc(ac), argv(av) { }
    void printVersion(ostream& os) {
      os << "MiniZinc solution printing tool, version "
         << MZN_VERSION_MAJOR << "." << MZN_VERSION_MINOR << "." << MZN_VERSION_PATCH << std::endl;
      os << "Copyright (C) 2014-" << string(__DATE__).substr(7, 4)
         << "   Monash University, NICTA, Data61" << std::endl;
    }
    void printHelp(ostream& os) {
      std::string executable_name(argv[0]);
      executable_name = executable_name.substr(executable_name.find_last_of("/\\") + 1);
      os << "Usage: " << executable_name
            << " [<options>] <model>.ozn" << std::endl
            << std::endl
            << "General options:" << std::endl
            << "  --help, -h\n    Print this help message." << std::endl
            << "  --version\n    Print version information." << std::endl
            << "  --stdlib-dir <dir>\n    Path to MiniZinc standard library directory." << std::endl
            << std::endl;        
      Solns2Out::printHelp(os);
    }
    bool processOptions()
    {
      int i=1;
      if (argc < 2)
        return false;
      string avi;
      for (i=1; i<argc; ++i) {
        avi = argv[i];
        CLOParser cop( i, argc, argv );
        if (string(argv[i])=="-h" || string(argv[i])=="--help") {
          printHelp(cout);
          std::exit(EXIT_SUCCESS);
        }
        if (string(argv[i])=="--version") {
          printVersion(cout);
          std::exit(EXIT_SUCCESS);
        } else if (cop.getOption("--stdlib-dir", &std_lib_dir)) {
        } else if ( Solns2Out::processOption(i, argc, argv)) {
        } else {
          filename = argv[i];
          if (filename.length()<=4 ||
              filename.substr(filename.length()-4,string::npos) != ".ozn") {
            std::cerr << "Invalid .ozn file " << filename << "." << std::endl;
            goto NotFound;
          }
        }
    Found: { }
      }
      return true;
    NotFound:
      cerr << "solns2out: unrecognized option `" << avi << "'" << endl;
      return false;
    }
    void run() {
      initFromOzn( filename );
      while ( solstream.good() ) {
        string line;
        getline( solstream, line );
        line += '\n';                // need eols as in t=raw stream
        feedRawDataChunk( line.c_str() );
      }
    }
  private:
    bool initFromOzn( string& fo ) {
      if ( !parseOzn(fo) )
        return false;
      init();
      return true;
    }
    bool parseOzn(string& fileOzn)
    {
      std::vector<string> filenames( 1, fileOzn );
      // If set before:
      
      if (std_lib_dir.empty()) {
        std_lib_dir = solver_configs.mznlibDir();
      }

      if (std_lib_dir.empty()) {
        std::cerr << "Error: solns2out: unknown minizinc standard library directory.\n"
          << "Specify --stdlib-dir on the command line or set the\n"
          << "MZN_STDLIB_DIR environment variable.\n";
        std::exit(EXIT_FAILURE);
      }

      includePaths.push_back(std_lib_dir+"/std/");

      for (unsigned int i=0; i<includePaths.size(); i++) {
        if (!FileUtils::directory_exists(includePaths[i])) {
          std::cerr << "solns2out: cannot access include directory " << includePaths[i] << "\n";
          std::exit(EXIT_FAILURE);
        }
      }

      {
        pEnv = new Env();
        if ((pOutput = parse(*pEnv, filenames, std::vector<std::string>(), includePaths, false, false, false,
                                   std::cerr))) {
          std::vector<TypeError> typeErrors;
          pEnv->model(pOutput);
          MZN_ASSERT_HARD_MSG( pEnv, "solns2out: could not allocate Env" );
          pEnv_guard.reset( pEnv );
          MiniZinc::typecheck(*pEnv,pOutput,typeErrors,false,false);
          MiniZinc::registerBuiltins(*pEnv,pOutput);
          pEnv->envi().swap_output();
          return true;
        }
      }
      
      return false;
    }
  };
}

int main(int argc, const char** argv) {
  
  Solns2OutFull s2out( argc, argv );
  
  if (!s2out.processOptions()) {
    s2out.printHelp( cout );
    exit( EXIT_FAILURE );
  }
  
  try {
    s2out.run();
  } catch (LocationException& e) {
    std::cerr << e.what() << ": " << e.msg() << std::endl;
    std::cerr << e.loc() << std::endl;
    exit(EXIT_FAILURE);
  } catch (Exception& e) {
    std::cerr << e.what() << ": " << e.msg() << std::endl;
    exit(EXIT_FAILURE);
  }
  
  return 0;
}
