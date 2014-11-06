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
#include <minizinc/htmlprinter.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/astexception.hh>

#include <minizinc/file_utils.hh>

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
  vector<string> includePaths;
  bool flag_ignoreStdlib = false;
  bool flag_verbose = false;
  bool flag_single_page = false;
  string flag_output_base;
  
  string std_lib_dir;
  if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
    std_lib_dir = string(MZNSTDLIBDIR);
  }
  string globals_dir;
  
  if (argc < 2)
    goto error;

  GC::init();
  
  for (int i=1; i<argc; i++) {
    if (string(argv[i])==string("-h") || string(argv[i])==string("--help"))
        goto error;
    if (string(argv[i])==string("--version")) {
      std::cout << "NICTA MiniZinc documentation generator, version "
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
    } else if (string(argv[i])==string("-v") || string(argv[i])==string("--verbose")) {
      flag_verbose = true;
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
    } else if (string(argv[i])=="--single-page") {
      flag_single_page = true;
    } else if (string(argv[i])=="--globals-dir" ||
               string(argv[i])=="--mzn-globals-dir") {
      i++;
      if (i==argc)
        goto error;
      globals_dir = argv[i];
    } else if (string(argv[i])=="--output-base") {
      i++;
      if (i==argc)
        goto error;
      flag_output_base = argv[i];
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
        std::cerr << "Error: cannot generate documentation for data files." << std::endl;
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

  {
    std::stringstream errstream;
    if (flag_verbose)
      std::cerr << "Parsing '" << filename << "' ...";
    if (Model* m = parse(filename, vector<string>(), includePaths, flag_ignoreStdlib,
                         errstream)) {
      try {
        if (flag_verbose)
          std::cerr << " done" << std::endl;
        if (flag_verbose)
          std::cerr << "Typechecking ...";
        vector<TypeError> typeErrors;
        MiniZinc::typecheck(m, typeErrors, true);
        if (typeErrors.size() > 0) {
          for (unsigned int i=0; i<typeErrors.size(); i++) {
            if (flag_verbose)
              std::cerr << std::endl;
            std::cerr << typeErrors[i].loc() << ":" << std::endl;
            std::cerr << typeErrors[i].what() << ": " << typeErrors[i].msg() << std::endl;
          }
          exit(EXIT_FAILURE);
        }
        if (flag_verbose)
          std::cerr << " done" << std::endl;
        if (flag_single_page) {
          HtmlDocument doc = HtmlPrinter::printHtmlSinglePage(m);
          std::ofstream os(flag_output_base+".html");
          HtmlPrinter::htmlHeader(os, "");
          os << doc.document();
          HtmlPrinter::htmlFooter(os);
          os.close();
        } else {
          std::vector<HtmlDocument> docs = HtmlPrinter::printHtml(m);
          for (unsigned int i=0; i<docs.size(); i++) {
            std::ofstream os(flag_output_base+"_"+docs[i].filename()+".html");
            HtmlPrinter::htmlHeader(os, docs[i].filename());
            os << docs[i].document();
            HtmlPrinter::htmlFooter(os);
            os.close();
          }
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
    std::cerr << "Done." << std::endl;
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
            << "  --stdlib-dir <dir>\n    Path to MiniZinc standard library directory" << std::endl
            << "  -G --globals-dir --mzn-globals-dir\n    Search for included files in <stdlib>/<dir>." << std::endl
            << "  --single-page\n    Print entire documentation on a single HTML page." << std::endl
            << std::endl
            << "Output options:" << std::endl << std::endl
            << "  --output-base <name>\n    Base name for output files" << std::endl
  ;

  exit(EXIT_FAILURE);
}
