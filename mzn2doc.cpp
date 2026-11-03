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
#include <minizinc/utils.hh>

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace MiniZinc;

int main(int argc, char** argv) {
  std::string filename;
  std::vector<std::string> includePaths;
  bool flag_verbose = false;
  bool flag_include_stdlib = false;
  bool flag_index = true;
  bool flag_rst = false;
  int toplevel_groups = 0;
  std::string output_base;
  std::string header_file;
  std::string footer_file;

  std::string std_lib_dir;
  if (auto dir = FileUtils::get_env("MZN_STDLIB_DIR")) {
    std_lib_dir = *dir;
  }
  std::string globals_dir;

  if (argc < 2) {
    goto error;
  }

  try {
    for (int i = 1; i < argc; i++) {
      if (std::string(argv[i]) == std::string("-h") ||
          std::string(argv[i]) == std::string("--help")) {
        goto error;
      }
      if (std::string(argv[i]) == std::string("--version")) {
        std::cout << "MiniZinc documentation generator, version " << MZN_VERSION_MAJOR << "."
                  << MZN_VERSION_MINOR << "." << MZN_VERSION_PATCH << '\n';
        std::cout << "Copyright (C) 2014-2017 Monash University, NICTA, Data61\n";
        std::exit(EXIT_SUCCESS);
      }
      if (beginswith(argv[i], "-I")) {
        std::string include(argv[i]);
        if (include.length() > 2) {
          includePaths.push_back(include.substr(2) + std::string("/"));
        } else {
          i++;
          if (i == argc) {
            goto error;
          }
          includePaths.push_back(argv[i] + std::string("/"));
        }
      } else if (std::string(argv[i]) == std::string("-v") ||
                 std::string(argv[i]) == std::string("--verbose")) {
        flag_verbose = true;
      } else if (std::string(argv[i]) == "--stdlib-dir") {
        i++;
        if (i == argc) {
          goto error;
        }
        std_lib_dir = argv[i];
      } else if (beginswith(argv[i], "-G")) {
        std::string filename(argv[i]);
        if (filename.length() > 2) {
          globals_dir = filename.substr(2);
        } else {
          i++;
          if (i == argc) {
            goto error;
          }
          globals_dir = argv[i];
        }
      } else if (std::string(argv[i]) == "--toplevel-groups") {
        i++;
        if (i == argc) {
          goto error;
        }
        toplevel_groups = parse_int_option(argv[i], "--toplevel-groups");
      } else if (std::string(argv[i]) == "--html-header" ||
                 std::string(argv[i]) == "--rst-header") {
        i++;
        if (i == argc) {
          goto error;
        }
        header_file = std::string(argv[i]);
      } else if (std::string(argv[i]) == "--html-footer" ||
                 std::string(argv[i]) == "--rst-footer") {
        i++;
        if (i == argc) {
          goto error;
        }
        footer_file = std::string(argv[i]);
      } else if (std::string(argv[i]) == "--include-stdlib") {
        flag_include_stdlib = true;
      } else if (std::string(argv[i]) == "--no-index") {
        flag_index = false;
      } else if (std::string(argv[i]) == "--globals-dir" ||
                 std::string(argv[i]) == "--mzn-globals-dir") {
        i++;
        if (i == argc) {
          goto error;
        }
        globals_dir = argv[i];
      } else if (std::string(argv[i]) == "--output-base") {
        i++;
        if (i == argc) {
          goto error;
        }
        output_base = argv[i];
      } else if (std::string(argv[i]) == "--rst-output") {
        flag_rst = true;
        toplevel_groups = 0;
      } else {
        std::string input_file(argv[i]);
        if (input_file.length() <= 4) {
          std::cerr << "Error: cannot handle file " << input_file << ".\n";
          goto error;
        }
        size_t last_dot = input_file.find_last_of('.');
        std::string extension;
        if (last_dot != std::string::npos) {
          extension = input_file.substr(last_dot, std::string::npos);
        }
        if (extension == ".mzn") {
          if (filename.empty()) {
            filename = input_file;
          } else {
            std::cerr << "Error: Multiple .mzn files given.\n";
            goto error;
          }
        } else if (extension == ".dzn" || extension == ".json") {
          std::cerr << "Error: cannot generate documentation for data files.\n";
        } else {
          std::cerr << "Error: cannot handle file extension " << extension << ".\n";
          goto error;
        }
      }
    }

    if (filename.empty()) {
      std::cerr << "Error: no model file given.\n";
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
      std::string header;
      size_t header_title = std::string::npos;
      size_t title_size = std::string("@TITLE").size();
      if (!header_file.empty()) {
        std::ifstream hs(FileUtils::to_path(header_file));
        if (!hs.good()) {
          std::cerr << "Cannot open header file " << header_file << "\n";
          std::exit(EXIT_FAILURE);
        }
        std::string str((std::istreambuf_iterator<char>(hs)), std::istreambuf_iterator<char>());
        header = str;
        header_title = str.find("@TITLE");
      }
      std::string footer;
      if (!footer_file.empty()) {
        std::ifstream hs(FileUtils::to_path(footer_file));
        if (!hs.good()) {
          std::cerr << "Cannot open footer file " << footer_file << "\n";
          std::exit(EXIT_FAILURE);
        }
        std::string str((std::istreambuf_iterator<char>(hs)), std::istreambuf_iterator<char>());
        footer = str;
      }

      std::stringstream errstream;
      if (flag_verbose) {
        std::cerr << "Parsing '" << filename << "'\n";
      }
      std::vector<std::string> filenames;
      filenames.push_back(filename);
      Env env;
      if (Model* m = parse(env, filenames, {}, "", "", includePaths, false, false, false, true,
                           flag_verbose, errstream)) {
        try {
          env.model(m);
          if (flag_verbose) {
            std::cerr << "Done parsing.\n";
          }
          if (flag_verbose) {
            std::cerr << "Typechecking ...";
          }
          std::vector<TypeError> typeErrors;
          MiniZinc::typecheck(env, m, typeErrors, true, false);
          if (!typeErrors.empty()) {
            for (auto& typeError : typeErrors) {
              if (flag_verbose) {
                std::cerr << '\n';
              }
              std::cerr << typeError.loc() << ":\n";
              std::cerr << typeError.what() << ": " << typeError.msg() << '\n';
            }
            exit(EXIT_FAILURE);
          }
          if (flag_verbose) {
            std::cerr << " done\n";
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
            std::ofstream os(
                FileUtils::to_path(basedir + doc.filename() + (flag_rst ? ".rst" : ".html")));
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
            std::cerr << '\n';
          }
          std::cerr << e.loc() << ":\n";
          std::cerr << e.what() << ": " << e.msg() << '\n';
          exit(EXIT_FAILURE);
        } catch (Exception& e) {
          if (flag_verbose) {
            std::cerr << '\n';
          }
          std::cerr << e.what() << ": " << e.msg() << '\n';
          exit(EXIT_FAILURE);
        }
      } else {
        if (flag_verbose) {
          std::cerr << '\n';
        }
        std::copy(std::istreambuf_iterator<char>(errstream), std::istreambuf_iterator<char>(),
                  std::ostreambuf_iterator<char>(std::cerr));
        exit(EXIT_FAILURE);
      }
    }

    if (flag_verbose) {
      std::cerr << "Done.\n";
    }
    return 0;
  } catch (Exception& e) {
    std::cerr << e.what() << ": " << e.msg() << '\n';
    exit(EXIT_FAILURE);
  } catch (...) {
    std::cerr << "  UNHANDLED EXCEPTION.\n";
    exit(EXIT_FAILURE);
  }

error:
  std::string executable_name(argv[0]);
  executable_name = executable_name.substr(executable_name.find_last_of("/\\") + 1);
  std::cerr
      << "Usage: " << executable_name
      << " [<options>] [-I <include path>] <model>.mzn [<data>.dzn ...]\n\n"
      << "Options:\n"
      << "  --help, -h\n    Print this help message\n"
      << "  --version\n    Print version information\n"
      << "  --include-stdlib\n    Include the standard libraries in the output\n"
      << "  -v, --verbose\n    Print progress statements\n"
      << "  --stdlib-dir <dir>\n    Path to MiniZinc standard library directory\n"
      << "  -G --globals-dir --mzn-globals-dir\n    Search for included files in <stdlib>/<dir>.\n"
      << "  --single-page\n    Print entire documentation on a single HTML page.\n"
      << "  --no-index\n       Do not generate an index of all symbols.\n"
      << "  --rst-output\n       Generate ReStructuredText rather than HTML.\n"
      << "  --html-header, --html-footer, --rst-header, --rst-footer\n       Header/footer files "
         "to include in output.\n\n"
      << "Output options:\n\n"
      << "  --output-base <name>\n    Base name for output files\n";

  exit(EXIT_FAILURE);
}
