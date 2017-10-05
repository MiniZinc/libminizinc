/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_PARSER_HH__
#define __MINIZINC_PARSER_HH__

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
#pragma warning(disable:4065)
#endif

#include <minizinc/model.hh>
#include <minizinc/parser.tab.hh>
#include <minizinc/astexception.hh>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cstring>

namespace MiniZinc {

  struct ParseWorkItem {
    Model* m;
    std::string dirName;
    std::string fileName;
    ParseWorkItem(Model* m0, const std::string& dirName0, const std::string& fileName0)
    : m(m0), dirName(dirName0), fileName(fileName0) {}
  };
  

  /// %State of the %MiniZinc parser
  class ParserState {
  public:
    ParserState(const std::string& f,
                const std::string& b, std::ostream& err0,
                std::vector<ParseWorkItem>& files0,
                std::map<std::string,Model*>& seenModels0,
                MiniZinc::Model* model0,
                bool isDatafile0, bool isFlatZinc0, bool parseDocComments0)
    : filename(f.c_str()), buf(b.c_str()), pos(0), length(b.size()),
      lineno(1), lineStartPos(0), nTokenNextStart(1),
      files(files0), seenModels(seenModels0), model(model0),
      isDatafile(isDatafile0), isFlatZinc(isFlatZinc0), parseDocComments(parseDocComments0),
      hadError(false), err(err0) {}
  
    const char* filename;
  
    void* yyscanner;
    const char* buf;
    unsigned int pos, length;

    int lineno;

    int lineStartPos;
    int nTokenNextStart;

    std::vector<ParseWorkItem>& files;
    std::map<std::string,Model*>& seenModels;
    MiniZinc::Model* model;

    bool isDatafile;
    bool isFlatZinc;
    bool parseDocComments;
    bool hadError;
    std::vector<SyntaxError> syntaxErrors;
    std::ostream& err;
    
    std::string stringBuffer;

    void printCurrentLine(void) {
      const char* eol_c = strchr(buf+lineStartPos,'\n');
      if (eol_c) {
        err << std::string(buf+lineStartPos,eol_c-(buf+lineStartPos));
      } else {
        err << buf+lineStartPos;
      }
      err << std::endl;
    }
  
    int fillBuffer(char* lexBuf, unsigned int lexBufSize) {
      if (pos >= length)
        return 0;
      int num = std::min(length - pos, lexBufSize);
      memcpy(lexBuf,buf+pos,num);
      pos += num;
      return num;    
    }

  };

  Model* parse(Env& env,
               const std::vector<std::string>& filename,
               const std::vector<std::string>& datafiles,
               const std::vector<std::string>& includePaths,
               bool ignoreStdlib, bool parseDocComments, bool verbose,
               std::ostream& err);

  Model* parseFromString(const std::string& model,
                         const std::string& filename,
                         const std::vector<std::string>& includePaths,
                         bool ignoreStdlib, bool parseDocComments, bool verbose,
                         std::ostream& err,
                         std::vector<SyntaxError>& syntaxErrors);

  Model* parseData(Env& env,
                   Model* m,
                   const std::vector<std::string>& datafiles,
                   const std::vector<std::string>& includePaths,
                   bool ignoreStdlib, bool parseDocComments, bool verbose,
                   std::ostream& err);

}

#endif
