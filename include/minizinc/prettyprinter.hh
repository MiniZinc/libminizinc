/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Pierre Wilke <wilke.pierre@gmail.com>
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/ast.hh>

#include <iostream>

namespace MiniZinc {

class Document;
class ItemDocumentMapper;
class PrettyPrinter;

class Printer {
private:
  EnvI* _env;
  ItemDocumentMapper* _ism;
  PrettyPrinter* _printer;
  std::ostream& _os;
  int _width;
  bool _flatZinc;

  void init();
  void p(Document* d);
  void p(const Item* i);

public:
  Printer(std::ostream& os, int width = 80, bool flatZinc = true, EnvI* env = nullptr);
  ~Printer();

  void trace(const Expression* e);
  void print(const Expression* e);
  void print(const Item* i);
  void print(const Model* m);

  static std::string escapeStringLit(std::string_view s) {
    std::ostringstream ret;
    for (char c : s) {
      switch (c) {
        case '\n':
          ret << "\\n";
          break;
        case '\t':
          ret << "\\t";
          break;
        case '"':
          ret << "\\\"";
          break;
        case '\\':
          ret << "\\\\";
          break;
        default:
          ret << c;
      }
    }
    return ret.str();
  }

  static std::string quoteId(std::string_view s) {
    constexpr std::string_view idChars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
    constexpr std::string_view idBegin = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    if (s.empty() || s[0] == '\'') {
      // Empty or already quoted
      return std::string(s);
    }
    constexpr std::string_view reserved[] = {
        "ann",       "annotation", "any",      "array",    "bool",     "case",      "constraint",
        "default",   "diff",       "div",      "else",     "elseif",   "endif",     "enum",
        "false",     "float",      "function", "if",       "in",       "include",   "int",
        "intersect", "let",        "list",     "maximize", "minimize", "mod",       "not",
        "of",        "op",         "opt",      "output",   "par",      "predicate", "record",
        "satisfy",   "set",        "solve",    "string",   "subset",   "superset",  "symdiff",
        "test",      "then",       "true",     "tuple",    "type",     "union",     "var",
        "where",     "xor"};
    bool is_reserved = std::find(std::begin(reserved), std::end(reserved), s) != std::end(reserved);
    size_t offset = s[0] == '_' ? 1 : 0;
    if (is_reserved || (offset < s.size() && idBegin.find(s[offset]) == std::string_view::npos) ||
        s.find_first_not_of(idChars, offset + 1) != std::string_view::npos) {
      return "'" + std::string(s) + "'";
    }
    return std::string(s);
  }
};

class FznJSONPrinter {
private:
  std::ostream& _os;
  EnvI& _env;

  void printBasicElement(std::ostream& os, Expression* e);
  void printAnnotations(std::ostream& os, const Annotation& ann);

public:
  FznJSONPrinter(std::ostream& os, EnvI& env) : _os(os), _env(env) {}

  void print(Model* m);
};

/// Output operator for expressions
template <class Char, class Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os,
                                             const Expression& e) {
  std::basic_ostringstream<Char, Traits> s;
  s.copyfmt(os);
  s.width(0);
  Printer p(s, 0);
  p.print(&e);
  return os << s.str();
}

/// Output operator for items
template <class Char, class Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, const Item& i) {
  std::basic_ostringstream<Char, Traits> s;
  s.copyfmt(os);
  s.width(0);
  Printer p(s);
  p.print(&i);
  return os << s.str();
}

void pp_floatval(std::ostream& os, const FloatVal& fv, bool hexFloat = false);

std::string show_enum_type(EnvI& env, Expression* e, Type t, bool dzn, bool json);

std::string show_with_type(EnvI& env, Expression* exp, Type t, bool showDzn);

}  // namespace MiniZinc

void debugprint(const MiniZinc::Expression* e);
void debugprint(const MiniZinc::Expression* e, MiniZinc::EnvI& env);
void debugprint(const MiniZinc::Item* i);
void debugprint(const MiniZinc::Item* i, MiniZinc::EnvI& env);
void debugprint(const MiniZinc::Model* m);
void debugprint(const MiniZinc::Model* m, MiniZinc::EnvI& env);
void debugprint(const MiniZinc::Location& l);
void debugprint(const MiniZinc::Location& l, const MiniZinc::EnvI& env);
void debugprint(const MiniZinc::Type& t);
void debugprint(const MiniZinc::Type& t, const MiniZinc::EnvI& env);
void debugprint(const MiniZinc::IntSetVal* isv);
void debugprint(const MiniZinc::FloatSetVal* fsv);

void debugprint(const std::vector<MiniZinc::Expression*>& x);
void debugprint(const std::vector<MiniZinc::Expression*>& x, MiniZinc::EnvI& env);
void debugprint(const std::vector<MiniZinc::VarDecl*>& x);
void debugprint(const std::vector<MiniZinc::VarDecl*>& x, MiniZinc::EnvI& env);
void debugprint(const std::vector<MiniZinc::Ref<MiniZinc::Expression>>& x);
void debugprint(const std::vector<MiniZinc::Ref<MiniZinc::Expression>>& x, MiniZinc::EnvI& env);
void debugprint(const std::vector<MiniZinc::Item*>& x);
void debugprint(const std::vector<MiniZinc::Item*>& x, MiniZinc::EnvI& env);
void debugprint(const std::vector<MiniZinc::Type>& x);
void debugprint(const std::vector<MiniZinc::Type>& x, MiniZinc::EnvI& env);
