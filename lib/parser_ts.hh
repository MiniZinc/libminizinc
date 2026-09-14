/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Shared helpers for the MiniZinc and DataZinc lowerers. */

#ifndef __MINIZINC_PARSER_TS_HH__
#define __MINIZINC_PARSER_TS_HH__

#include <minizinc/parser.hh>
#include <minizinc/values.hh>

#include <algorithm>
#include <iterator>
#include <string>

#include <tree_feller.h>

namespace MiniZinc {

/// Functions emitted by `--output-mode dzn` that data files may call.
const char* const DATA_FUNCTIONS[] = {"anon_enum", "anon_enum_set", "array1d", "array2d", "array3d",
                                      "array4d",   "array5d",       "array6d", "to_enum"};

/// Operators that may use call syntax, such as `'+'(a, b)`.
struct QuotedOp {
  const char* text;
  int bot;  ///< -1 means `not`
};

const QuotedOp QUOTED_OPS[] = {
    {"<->", BOT_EQUIV},
    {"->", BOT_IMPL},
    {"<-", BOT_RIMPL},
    {"\\/", BOT_OR},
    {"/\\", BOT_AND},
    {"xor", BOT_XOR},
    {"<", BOT_LE},
    {"<=", BOT_LQ},
    {">", BOT_GR},
    {">=", BOT_GQ},
    {"=", BOT_EQ},
    {"==", BOT_EQ},
    {"!=", BOT_NQ},
    {"in", BOT_IN},
    {"subset", BOT_SUBSET},
    {"superset", BOT_SUPERSET},
    {"union", BOT_UNION},
    {"diff", BOT_DIFF},
    {"symdiff", BOT_SYMDIFF},
    {"+", BOT_PLUS},
    {"-", BOT_MINUS},
    {"*", BOT_MULT},
    {"/", BOT_DIV},
    {"div", BOT_IDIV},
    {"mod", BOT_MOD},
    {"^", BOT_POW},
    {"intersect", BOT_INTERSECT},
    {"++", BOT_PLUSPLUS},
    {"..", BOT_DOTDOT},
    {"not", -1},
};

/// Return the canonical quoted operator name, or null for an identifier.
inline const char* quoted_op_name(const std::string& content) {
  static const struct {
    const char* text;
    const char* name;
  } NAMES[] = {
      {"<->", "'<->'"},
      {"->", "'->'"},
      {"<-", "'<-'"},
      {"\\/", "'\\/'"},
      {"xor", "'xor'"},
      {"/\\", "'/\\'"},
      {"<", "'<'"},
      {">", "'>'"},
      {"<=", "'<='"},
      {">=", "'>='"},
      {"=", "'='"},
      {"==", "'='"},
      {"!=", "'!='"},
      {"in", "'in'"},
      {"subset", "'subset'"},
      {"superset", "'superset'"},
      {"union", "'union'"},
      {"diff", "'diff'"},
      {"symdiff", "'symdiff'"},
      {"..", "'..'"},
      {"<..", "'<..'"},
      {"..<", "'..<'"},
      {"<..<", "'<..<'"},
      {"+", "'+'"},
      {"-", "'-'"},
      {"*", "'*'"},
      {"^", "'^'"},
      {"/", "'/'"},
      {"div", "'div'"},
      {"mod", "'mod'"},
      {"intersect", "'intersect'"},
      {"not", "'not'"},
      {"++", "'++'"},
  };
  for (const auto& entry : NAMES) {
    if (content == entry.text) {
      return entry.name;
    }
  }
  return nullptr;
}

/// Match the reserved names rejected as calls by the reference lexer.
inline bool is_reserved_call_name(const std::string& s) {
  static const char* const RESERVED[] = {
      "ann", "any", "array",  "bool", "enum",   "float", "int",  "list",
      "opt", "par", "record", "set",  "string", "tuple", "type", "var",
  };
  auto it = std::lower_bound(
      std::begin(RESERVED), std::end(RESERVED), s,
      [](const char* name, const std::string& value) { return value.compare(name) > 0; });
  return it != std::end(RESERVED) && s == *it;
}

inline const QuotedOp* quoted_op(const std::string& text) {
  for (const auto& e : QUOTED_OPS) {
    if (text == e.text) {
      return &e;
    }
  }
  return nullptr;
}

/// Match lexer.lxx integer conversion, returning false on overflow.
inline bool decimal_to_intval(const char* b, const char* e, IntVal& out) {
  IntVal x = 0;
  try {
    for (const char* p = b; p != e; p++) {
      x = (x * 10) + (*p - '0');
    }
  } catch (ArithmeticError&) {
    return false;
  }
  out = x;
  return true;
}

inline bool based_to_intval(const char* b, const char* e, int base, IntVal& out) {
  IntVal x = 0;
  try {
    for (const char* p = b; p != e; p++) {
      char c = *p;
      int d;
      if (c >= '0' && c <= '9') {
        d = c - '0';
      } else if (c >= 'a' && c <= 'f') {
        d = c - 'a' + 10;
      } else {
        d = c - 'A' + 10;
      }
      if (d >= base) {
        return false;
      }
      x = (x * base) + d;
    }
  } catch (ArithmeticError&) {
    return false;
  }
  out = x;
  return true;
}

/// Convert a byte offset to a source location.
inline ParserLocation nul_location(const ParserState& pp, unsigned int offset) {
  unsigned int line = 1 + pp.lineOffset;
  unsigned int lineStart = 0;
  for (unsigned int i = 0; i < offset; i++) {
    if (pp.buf[i] == '\n') {
      line++;
      lineStart = i + 1;
    }
  }
  unsigned int col = 0;
  for (unsigned int i = lineStart; i < offset; i++) {
    if ((static_cast<unsigned char>(pp.buf[i]) & 0xc0) != 0x80) {
      col++;
    }
  }
  return {ASTString(pp.filename), line, col + 1, line, col + 1};
}

/// Parse `pp.buf` with the DataZinc grammar, writing items to \a items.
/// Returns false and sets \a error if the grammar rejects the file.
bool parse_datazinc(ParserState& pp, Model* items, TFError& error);

}  // namespace MiniZinc

#endif
