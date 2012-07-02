/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Guido Tack <tack@gecode.org>
 *
 *  Copyright:
 *     Guido Tack, 2007
 *
 *  Last modified:
 *     $Date: 2011-07-26 03:37:55 +1000 (Tue, 26 Jul 2011) $ by $Author: tack $
 *     $Revision: 1178 $
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

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

#include <minizinc/model.hh>
#include <minizinc/parser.tab.hh>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cstring>

namespace MiniZinc {

  /// %State of the %MiniZinc parser
  class ParserState {
  public:
    ParserState(const ASTContext& ctx0, const std::string& f,
                const std::string& b, std::ostream& err0,
                std::vector<std::pair<std::string,Model*> >& files0,
                std::map<std::string,Model*>& seenModels0,
                MiniZinc::Model* model0,
                bool isDatafile0)
    : ctx(ctx0),
      filename(f.c_str()), buf(b.c_str()), pos(0), length(b.size()),
      lineno(1), lineStartPos(0), nTokenNextStart(1),
      files(files0), seenModels(seenModels0), model(model0),
      isDatafile(isDatafile0), hadError(false), err(err0) {}
  
    const ASTContext& ctx;
  
    const char* filename;
  
    void* yyscanner;
    const char* buf;
    unsigned int pos, length;

    int lineno;

    int lineStartPos;
    int nTokenNextStart;

    std::vector<std::pair<std::string,Model*> >& files;
    std::map<std::string,Model*>& seenModels;
    MiniZinc::Model* model;

    bool isDatafile;
    bool hadError;
    std::ostream& err;

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

  Model* parse(const ASTContext& ctx,
               const std::string& filename,
               const std::vector<std::string>& datafiles,
               const std::vector<std::string>& includePaths,
               bool ignoreStdlib,
               std::ostream& err);

}

#endif
