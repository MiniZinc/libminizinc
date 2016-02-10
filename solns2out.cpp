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

#include <minizinc/solns2out.h>

using namespace MiniZinc;
using namespace std;

namespace MiniZinc {
  class Solns2OutFull : public Solns2Out {
    const int argc;
    const char* const* argv;
    string std_lib_dir;
    istream& solstream = cin;
  public:
    string filename;
    Solns2OutFull( const int ac, const char* const* av )
      : argc(ac), argv(av) { }
    void printVersion(ostream& os) {
      os << "NICTA MiniZinc solution printing tool, version "
         << MZN_VERSION_MAJOR << "." << MZN_VERSION_MINOR << "." << MZN_VERSION_PATCH << std::endl;
      os << "Copyright (C) 2014-" << string(__DATE__).substr(7, 4)
         << "   Monash University and NICTA" << std::endl;
    }
    void printHelp(ostream& os) {
      os << "Usage: " << argv[0]
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
      for (i=1; i<argc; ++i) {
        CLOParser cop( i, argc, argv );
        if (string(argv[i])=="-h" || string(argv[i])=="--help") {
          printHelp(cout);
          std::exit(EXIT_SUCCESS);
        }
        if (string(argv[i])=="--version") {
          printVersion(cout);
          std::exit(EXIT_SUCCESS);
        } else if ( cop.getOption( "--stdlib-dir", &std_lib_dir ) ) {
        } else {
          filename = argv[i++];
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
      cerr << "  Unrecognized option: '" << argv[i] << "'" << endl;
      return false;
    }
    void run() {
      initFromOzn( filename );
      while ( solstream.good() ) {
        string line;
        getline( solstream, line );
        feedRawDataChunk( line.c_str() );
      }
    }
  private:
    bool initFromOzn( string& fo ) {
      init();
      return parseOzn(fo);
    }
    bool parseOzn(string& fileOzn)
    {
      std::vector<string> filenames( 1, fileOzn );
      // If set before:
      
      if (std_lib_dir.empty())
        if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
          std_lib_dir = string(MZNSTDLIBDIR);
        }

      if (std_lib_dir.empty()) {
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

      if (std_lib_dir.empty()) {
        std::cerr << "Error: Solns2Out: unknown minizinc standard library directory.\n"
          << "Specify --stdlib-dir on the command line or set the\n"
          << "MZN_STDLIB_DIR environment variable.\n";
        std::exit(EXIT_FAILURE);
      }

      includePaths.push_back(std_lib_dir+"/std/");

      for (unsigned int i=0; i<includePaths.size(); i++) {
        if (!FileUtils::directory_exists(includePaths[i])) {
          std::cerr << "Cannot access include directory " << includePaths[i] << "\n";
          std::exit(EXIT_FAILURE);
        }
      }

      {
        if (pOutput = parse(filenames, std::vector<std::string>(), includePaths, false, false, false,
                                  std::cerr)) {
          std::vector<TypeError> typeErrors;
          pEnv = new Env(pOutput);
          MZN_ASSERT_HARD_MSG( pEnv, "Solns2Out: could not allocate Env" );
          pEnv_guard.reset( pEnv );
          MiniZinc::typecheck(*pEnv,pOutput,typeErrors);
          MiniZinc::registerBuiltins(*pEnv,pOutput);
          pEnv->envi().swap_output();
          return true;
        }
      }
      
      return false;
    }
  };
}

int main(int argc, char** argv) {
  
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
