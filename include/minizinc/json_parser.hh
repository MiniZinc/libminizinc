/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/astexception.hh>
#include <minizinc/model.hh>

#include <fstream>
#include <string>
#include <vector>

namespace MiniZinc {

class JSONError : public LocationException {
public:
  JSONError(EnvI& env, const Location& loc, const std::string& msg)
      : LocationException(env, loc, msg) {}
  const char* what() const throw() override { return "MiniZinc: JSON parsing error"; }
};

class JSONParser {
protected:
  enum TokenT {
    T_LIST_OPEN,
    T_LIST_CLOSE,
    T_OBJ_OPEN,
    T_OBJ_CLOSE,
    T_COMMA,
    T_COLON,
    T_STRING,
    T_INT,
    T_FLOAT,
    T_BOOL,
    T_NULL,
    T_EOF
  } _t;

  class Token;
  EnvI& _env;
  int _line;
  int _column;
  std::string _filename;
  Location errLocation() const;
  Token readToken(std::istream& is);
  void expectToken(std::istream& is, TokenT t);
  std::string expectString(std::istream& is);
  void expectEof(std::istream& is);
  Token parseEnumString(std::istream& is);
  Expression* parseExp(std::istream& is, bool parseObjects = true);
  ArrayLit* parseArray(std::istream& is);
  Expression* parseObject(std::istream& is);

  void parseModel(Model* m, std::istream& is, bool isData);
  static Expression* coerceArray(TypeInst* intendedTI, Expression* array);

public:
  JSONParser(EnvI& env) : _env(env) {}
  /// Parses \a filename as MiniZinc data and creates assign items in \a m
  void parse(Model* m, const std::string& filename, bool isData = true);
  /// Parses \a data as JSON-encoded MiniZinc data and creates assign items in \a m
  void parseFromString(Model* m, const std::string& data, bool isData = true);
  /// Check if file \a filename may contain JSON-encoded MiniZinc data
  static bool fileIsJSON(const std::string& filename);
  /// Check if string \a data may contain JSON-encoded MiniZinc data
  static bool stringIsJSON(const std::string& data);
};

}  // namespace MiniZinc
