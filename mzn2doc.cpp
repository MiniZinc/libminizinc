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

#include <minizinc/astexception.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/htmlprinter.hh>
#include <minizinc/model.hh>
#include <minizinc/parser.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/solver_config.hh>
#include <minizinc/typecheck.hh>

#include <fstream>
#include <iomanip>
#include <iostream>

using namespace MiniZinc;
using namespace std;

bool beginswith(const string& s, const string& t) { return s.compare(0, t.length(), t) == 0; }

int main(int argc, char** argv) {
  string filename;
  vector<string> includePaths;
  bool flag_verbose = false;
  bool flag_include_stdlib = false;
  bool flag_index = true;
  bool flag_rst = false;
  int toplevel_groups = 0;
  string output_base;
  string header_file;
  string footer_file;

  string std_lib_dir;
  if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
    std_lib_dir = string(MZNSTDLIBDIR);
  }
  string globals_dir;

  if (argc < 2) {
    goto error;
  }

  try {
    for (int i = 1; i < argc; i++) {
      if (string(argv[i]) == string("-h") || string(argv[i]) == string("--help")) {
        goto error;
      }
      if (string(argv[i]) == string("--version")) {
        std::cout << "MiniZinc documentation generator, version " << MZN_VERSION_MAJOR << "."
                  << MZN_VERSION_MINOR << "." << MZN_VERSION_PATCH << std::endl;
        std::cout << "Copyright (C) 2014-2017 Monash University, NICTA, Data61" << std::endl;
        std::exit(EXIT_SUCCESS);
      }
      if (beginswith(string(argv[i]), "-I")) {
        string include(argv[i]);
        if (include.length() > 2) {
          includePaths.push_back(include.substr(2) + string("/"));
        } else {
          i++;
          if (i == argc) {
            goto error;
          }
          includePaths.push_back(argv[i] + string("/"));
        }
      } else if (string(argv[i]) == string("-v") || string(argv[i]) == string("--verbose")) {
        flag_verbose = true;
      } else if (string(argv[i]) == "--stdlib-dir") {
        i++;
        if (i == argc) {
          goto error;
        }
        std_lib_dir = argv[i];
      } else if (beginswith(string(argv[i]), "-G")) {
        string filename(argv[i]);
        if (filename.length() > 2) {
          globals_dir = filename.substr(2);
        } else {
          i++;
          if (i == argc) {
            goto error;
          }
          globals_dir = argv[i];
        }
      } else if (string(argv[i]) == "--toplevel-groups") {
        i++;
        if (i == argc) {
          goto error;
        }
        toplevel_groups = atoi(argv[i]);
      } else if (string(argv[i]) == "--html-header" || string(argv[i]) == "--rst-header") {
        i++;
        if (i == argc) {
          goto error;
        }
        header_file = string(argv[i]);
      } else if (string(argv[i]) == "--html-footer" || string(argv[i]) == "--rst-footer") {
        i++;
        if (i == argc) {
          goto error;
        }
        footer_file = string(argv[i]);
      } else if (string(argv[i]) == "--include-stdlib") {
        flag_include_stdlib = true;
      } else if (string(argv[i]) == "--no-index") {
        flag_index = false;
      } else if (string(argv[i]) == "--globals-dir" || string(argv[i]) == "--mzn-globals-dir") {
        i++;
        if (i == argc) {
          goto error;
        }
        globals_dir = argv[i];
      } else if (string(argv[i]) == "--output-base") {
        i++;
        if (i == argc) {
          goto error;
        }
        output_base = argv[i];
      } else if (string(argv[i]) == "--rst-output") {
        flag_rst = true;
        toplevel_groups = 0;
      } else {
        std::string input_file(argv[i]);
        if (input_file.length() <= 4) {
          std::cerr << "Error: cannot handle file " << input_file << "." << std::endl;
          goto error;
        }
        size_t last_dot = input_file.find_last_of('.');
        std::string extension;
        if (last_dot != string::npos) {
          extension = input_file.substr(last_dot, string::npos);
        }
        if (extension == ".mzn") {
          if (filename.empty()) {
            filename = input_file;
          } else {
            std::cerr << "Error: Multiple .mzn files given." << std::endl;
            goto error;
          }
        } else if (extension == ".dzn" || extension == ".json") {
          std::cerr << "Error: cannot generate documentation for data files." << std::endl;
        } else {
          std::cerr << "Error: cannot handle file extension " << extension << "." << std::endl;
          goto error;
        }
      }
    }

    if (filename.empty()) {
      std::cerr << "Error: no model file given." << std::endl;
      goto error;
    }

    if (std_lib_dir.empty()) {
      SolverConfigs solver_configs(std::cerr);
      std_lib_dir = solver_configs.mznlibDir();
    }

    if (std_lib_dir.empty()) {
      std::cerr << "Error: unknown minizinc standard library directory.\n"
                << "Specify --stdlib-dir on the command line or set the\n"
                << "MZN_STDLIB_DIR environment variable.\n";
      std::exit(EXIT_FAILURE);
    }

    if (!globals_dir.empty()) {
      includePaths.push_back(std_lib_dir + "/" + globals_dir + "/");
    }
    includePaths.push_back(std_lib_dir + "/std/");

    for (auto& includePath : includePaths) {
      if (!FileUtils::directory_exists(includePath)) {
        std::cerr << "Cannot access include directory " << includePath << "\n";
        std::exit(EXIT_FAILURE);
      }
    }

    if (output_base.empty()) {
      output_base = filename.substr(0, filename.length() - 4);
    }

    {
      string header;
      size_t header_title = std::string::npos;
      size_t title_size = std::string("@TITLE").size();
      if (!header_file.empty()) {
        std::ifstream hs(FILE_PATH(header_file));
        if (!hs.good()) {
          std::cerr << "Cannot open header file " << header_file << "\n";
          std::exit(EXIT_FAILURE);
        }
        std::string str((std::istreambuf_iterator<char>(hs)), std::istreambuf_iterator<char>());
        header = str;
        header_title = str.find("@TITLE");
      }
      string footer;
      if (!footer_file.empty()) {
        std::ifstream hs(FILE_PATH(footer_file));
        if (!hs.good()) {
          std::cerr << "Cannot open footer file " << footer_file << "\n";
          std::exit(EXIT_FAILURE);
        }
        std::string str((std::istreambuf_iterator<char>(hs)), std::istreambuf_iterator<char>());
        footer = str;
      }

      std::stringstream errstream;
      if (flag_verbose) {
        std::cerr << "Parsing '" << filename << "'" << std::endl;
      }
      std::vector<std::string> filenames;
      filenames.push_back(filename);
      Env env;
      if (Model* m = parse(env, filenames, vector<string>(), "", "", includePaths, false, false,
                           true, flag_verbose, errstream)) {
        try {
          env.model(m);
          if (flag_verbose) {
            std::cerr << "Done parsing." << std::endl;
          }
          if (flag_verbose) {
            std::cerr << "Typechecking ...";
          }
          vector<TypeError> typeErrors;
          MiniZinc::typecheck(env, m, typeErrors, true, false);
          if (!typeErrors.empty()) {
            for (auto& typeError : typeErrors) {
              if (flag_verbose) {
                std::cerr << std::endl;
              }
              std::cerr << typeError.loc() << ":" << std::endl;
              std::cerr << typeError.what() << ": " << typeError.msg() << std::endl;
            }
            exit(EXIT_FAILURE);
          }
          if (flag_verbose) {
            std::cerr << " done" << std::endl;
          }
          std::string basename = output_base;
          std::string basedir;
          size_t lastSlash = output_base.find_last_of('/');
          if (lastSlash != std::string::npos) {
            basedir = basename.substr(0, lastSlash) + "/";
            basename = basename.substr(lastSlash + 1, std::string::npos);
          }
          std::vector<HtmlDocument> docs;
          if (flag_rst) {
            docs = RSTPrinter::printRST(env.envi(), m, basename, toplevel_groups,
                                        flag_include_stdlib, flag_index);
          } else {
            docs = HtmlPrinter::printHtml(env.envi(), m, basename, toplevel_groups,
                                          flag_include_stdlib, flag_index);
          }

          for (auto& doc : docs) {
            std::ofstream os(FILE_PATH(basedir + doc.filename() + (flag_rst ? ".rst" : ".html")));
            std::string header_replace = header;
            if (header_title != std::string::npos) {
              header_replace = header_replace.replace(header_title, title_size, doc.title());
            }
            os << header_replace;
            os << doc.document();
            os << footer;
            os.close();
          }
        } catch (LocationException& e) {
          if (flag_verbose) {
            std::cerr << std::endl;
          }
          std::cerr << e.loc() << ":" << std::endl;
          std::cerr << e.what() << ": " << e.msg() << std::endl;
          exit(EXIT_FAILURE);
        } catch (Exception& e) {
          if (flag_verbose) {
            std::cerr << std::endl;
          }
          std::cerr << e.what() << ": " << e.msg() << std::endl;
          exit(EXIT_FAILURE);
        }
      } else {
        if (flag_verbose) {
          std::cerr << std::endl;
        }
        std::copy(istreambuf_iterator<char>(errstream), istreambuf_iterator<char>(),
                  ostreambuf_iterator<char>(std::cerr));
        exit(EXIT_FAILURE);
      }
    }

    if (flag_verbose) {
      std::cerr << "Done." << std::endl;
    }
    return 0;
  } catch (...) {
    std::cerr << "  UNHANDLED EXCEPTION." << std::endl;
    exit(EXIT_FAILURE);
  }

error:
  std::string executable_name(argv[0]);
  executable_name = executable_name.substr(executable_name.find_last_of("/\\") + 1);
  std::cerr
      << "Usage: " << executable_name
      << " [<options>] [-I <include path>] <model>.mzn [<data>.dzn ...]" << std::endl
      << std::endl
      << "Options:" << std::endl
      << "  --help, -h\n    Print this help message" << std::endl
      << "  --version\n    Print version information" << std::endl
      << "  --include-stdlib\n    Include the standard libraries in the output" << std::endl
      << "  -v, --verbose\n    Print progress statements" << std::endl
      << "  --stdlib-dir <dir>\n    Path to MiniZinc standard library directory" << std::endl
      << "  -G --globals-dir --mzn-globals-dir\n    Search for included files in <stdlib>/<dir>."
      << std::endl
      << "  --single-page\n    Print entire documentation on a single HTML page." << std::endl
      << "  --no-index\n       Do not generate an index of all symbols." << std::endl
      << "  --rst-output\n       Generate ReStructuredText rather than HTML." << std::endl
      << "  --html-header, --html-footer, --rst-header, --rst-footer\n       Header/footer files "
         "to include in output."
      << std::endl
      << std::endl
      << "Output options:" << std::endl
      << std::endl
      << "  --output-base <name>\n    Base name for output files" << std::endl;

  exit(EXIT_FAILURE);
}
