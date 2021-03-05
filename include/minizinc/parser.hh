/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

// This is a workaround for a bug in flex that only shows up
// with the Microsoft C++ compiler
#if defined(_MSC_VER)
#define YY_NO_UNISTD_H
#ifdef __cplusplus
extern "C" int isatty(int);
#endif
#endif

// The Microsoft C++ compiler marks certain functions as deprecated,
// so let's take the alternative definitions
#if defined(_MSC_VER)
#define strdup _strdup
#define fileno _fileno
#endif

#if defined(_MSC_VER)
#pragma warning(disable : 4065)
#endif

namespace MiniZinc {
class ParserLocation;
}
#define YYLTYPE MiniZinc::ParserLocation
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 0

#include <minizinc/astexception.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/model.hh>
#include <minizinc/parser.tab.hh>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace MiniZinc {

struct ParseWorkItem {
  Model* m;
  IncludeI* ii;
  std::string dirName;
  std::string fileName;
  bool isSTDLib;
  bool isModelString;
  ParseWorkItem(Model* m0, IncludeI* ii0, std::string dirName0, std::string fileName0,
                bool isSTDLib0 = false, bool isModelString0 = false)
      : m(m0),
        ii(ii0),
        dirName(std::move(dirName0)),
        fileName(std::move(fileName0)),
        isSTDLib(isSTDLib0),
        isModelString(isModelString0) {}
};

/// %State of the %MiniZinc parser
class ParserState {
public:
  ParserState(const std::string& f, const std::string& b, std::ostream& err0,
              const std::vector<std::string>& includePaths0, std::vector<ParseWorkItem>& files0,
              std::map<std::string, Model*>& seenModels0, MiniZinc::Model* model0, bool isDatafile0,
              bool isFlatZinc0, bool isSTDLib0, bool parseDocComments0)
      : filename(f.c_str()),
        buf(b.c_str()),
        pos(0),
        length(static_cast<unsigned int>(b.size())),
        lineStartPos(0),
        nTokenNextStart(1),
        hadNewline(false),
        includePaths(includePaths0),
        files(files0),
        seenModels(seenModels0),
        model(model0),
        isDatafile(isDatafile0),
        isFlatZinc(isFlatZinc0),
        isSTDLib(isSTDLib0),
        parseDocComments(parseDocComments0),
        hadError(false),
        err(err0) {}

  const char* filename;

  void* yyscanner;
  const char* buf;
  unsigned int pos, length;

  int lineStartPos;
  int nTokenNextStart;
  bool hadNewline;

  const std::vector<std::string>& includePaths;
  std::vector<ParseWorkItem>& files;
  std::map<std::string, Model*>& seenModels;
  MiniZinc::Model* model;

  bool isDatafile;
  bool isFlatZinc;
  bool isSTDLib;
  bool parseDocComments;
  bool hadError;
  std::vector<SyntaxError> syntaxErrors;
  std::ostream& err;

  std::string stringBuffer;

  void printCurrentLine(int firstCol, int lastCol) {
    const char* eol_c = strchr(buf + lineStartPos, '\n');
    if (eol_c != nullptr) {
      if (eol_c == buf + lineStartPos) {
        return;
      }
      err << std::string(buf + lineStartPos, eol_c - (buf + lineStartPos));
    } else {
      err << buf + lineStartPos;
    }
    err << std::endl;
    for (int i = 0; i < firstCol - 1; i++) {
      err << " ";
    }
    for (int i = firstCol; i <= lastCol; i++) {
      err << "^";
    }
    err << std::endl;
  }

  int fillBuffer(char* lexBuf, unsigned int lexBufSize) {
    if (pos >= length) {
      return 0;
    }
    int num = std::min(length - pos, lexBufSize);
    memcpy(lexBuf, buf + pos, num);
    pos += num;
    return num;
  }

  std::string canonicalFilename(const std::string& f) const;
};

Model* parse(Env& env, const std::vector<std::string>& filename,
             const std::vector<std::string>& datafiles, const std::string& textModel,
             const std::string& textModelName, const std::vector<std::string>& includePaths,
             bool isFlatZinc, bool ignoreStdlib, bool parseDocComments, bool verbose,
             std::ostream& err);

Model* parse_from_string(Env& env, const std::string& text, const std::string& filename,
                         const std::vector<std::string>& includePaths, bool isFlatZinc,
                         bool ignoreStdlib, bool parseDocComments, bool verbose, std::ostream& err,
                         std::vector<SyntaxError>& syntaxErrors);

Model* parse_data(Env& env, Model* m, const std::vector<std::string>& datafiles,
                  const std::vector<std::string>& includePaths, bool isFlatZinc, bool ignoreStdlib,
                  bool parseDocComments, bool verbose, std::ostream& err);

}  // namespace MiniZinc
