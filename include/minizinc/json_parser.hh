/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_JSON_PARSER_HH__
#define __MINIZINC_JSON_PARSER_HH__

#include <vector>
#include <fstream>
#include <string>
#include <minizinc/model.hh>
#include <minizinc/astexception.hh>

namespace MiniZinc {
  
  class JSONError : public LocationException {
  public:
    JSONError(EnvI& env, const Location& loc, const std::string& msg)
    : LocationException(env,loc,msg) {}
    virtual const char* what(void) const throw() {
      return "MiniZinc: JSON parsing error";
    }
    
  };

  class JSONParser {
  protected:
    enum TokenT { T_LIST_OPEN, T_LIST_CLOSE, T_OBJ_OPEN, T_OBJ_CLOSE, T_COMMA, T_COLON,
      T_STRING, T_INT, T_FLOAT, T_BOOL, T_EOF } t;

    class Token;
    EnvI& env;
    int line;
    int column;
    std::string filename;
    Location errLocation(void) const;
    Token readToken(std::istream& is);
    void expectToken(std::istream& is, TokenT t);
    std::string expectString(std::istream& is);
    Expression* parseExp(std::istream& is);
    ArrayLit* parseArray(std::istream& is);
    
    SetLit* parseSetLit(std::istream& is);
    
    void parse(Model*m, std::istream& is);
  public:
    JSONParser(EnvI& env0) : env(env0) {}
    /// Parses \a filename as MiniZinc data and creates assign items in \a m
    void parse(Model* m, const std::string& filename);
    /// Parses \a data as JSON-encoded MiniZinc data and creates assign items in \a m
    void parseFromString(Model* m, const std::string& data);
    /// Check if file \a filename may contain JSON-encoded MiniZinc data
    static bool fileIsJSON(const std::string& filename);
    /// Check if string \a data may contain JSON-encoded MiniZinc data
    static bool stringIsJSON(const std::string& data);
  };
  
}

#endif
