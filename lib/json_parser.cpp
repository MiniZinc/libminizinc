/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/astmap.hh>
#include <minizinc/aststring.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/iter.hh>
#include <minizinc/json_parser.hh>

#include <fstream>
#include <sstream>
#include <string>
#include <utility>

using namespace std;

namespace MiniZinc {

class JSONParser::Token {
public:
  TokenT t;

protected:
  Token(TokenT t0) : t(t0) {}

public:
  Token() : t(T_EOF) {}
  std::string s;
  int i;
  double d;
  bool b;
  Token(std::string s0) : t(T_STRING), s(std::move(s0)) {}
  Token(int i0) : t(T_INT), i(i0), d(i0) {}
  Token(double d0) : t(T_FLOAT), d(d0) {}
  Token(bool b0) : t(T_BOOL), i(static_cast<int>(b0)), d(static_cast<double>(b0)), b(b0) {}
  static Token listOpen() { return Token(T_LIST_OPEN); }
  static Token listClose() { return Token(T_LIST_CLOSE); }
  static Token objOpen() { return Token(T_OBJ_OPEN); }
  static Token objClose() { return Token(T_OBJ_CLOSE); }
  static Token comma() { return Token(T_COMMA); }
  static Token colon() { return Token(T_COLON); }
  static Token eof() { return Token(T_EOF); }
  static Token null() { return Token(T_NULL); }
  string toString() const {
    switch (t) {
      case T_LIST_OPEN:
        return "[";
      case T_LIST_CLOSE:
        return "]";
      case T_OBJ_OPEN:
        return "{";
      case T_OBJ_CLOSE:
        return "}";
      case T_COMMA:
        return ",";
      case T_COLON:
        return ":";
      case T_STRING:
        return "\"" + s + "\"";
      case T_INT: {
        std::stringstream ss;
        ss << i;
        return ss.str();
      }
      case T_FLOAT: {
        std::stringstream ss;
        ss << d;
        return ss.str();
      }
      case T_BOOL:
        return b ? "true" : "false";
      case T_NULL:
        return "null";
      case T_EOF:
        return "eof";
    }
    return "UNKNOWN";
  }
};

Location JSONParser::errLocation() const {
  Location loc(ASTString(_filename), _line, _column, _line, _column);
  return loc;
}

JSONParser::Token JSONParser::readToken(istream& is) {
  string result;
  char buf[1];
  enum { S_NOTHING, S_STRING, S_STRING_ESCAPE, S_INT, S_FLOAT, S_FLOAT_EXPONENT } state;
  state = S_NOTHING;

  auto read_exponent_start = [&]() {
    char next[1];
    is.read(next, sizeof(next));
    _column += sizeof(next);
    if (is.eof()) {
      throw JSONError(_env, errLocation(), "unexpected end of file");
    }
    if (is.good() && next[0] == '-') {
      result += next[0];
      is.read(next, sizeof(next));
      _column += sizeof(next);
      if (is.eof()) {
        throw JSONError(_env, errLocation(), "unexpected end of file");
      }
    }
    if (!is.good() || !(next[0] >= '0' && next[0] <= '9')) {
      throw JSONError(_env, errLocation(), "unexpected token `" + string(next, is.gcount()) + "'");
    }
    result += next[0];
  };

  while (is.good()) {
    is.read(buf, sizeof(buf));
    _column += sizeof(buf);
    if (is.eof()) {
      return Token::eof();
    }
    if (!is.good()) {
      throw JSONError(_env, errLocation(), "tokenization failed");
    }
    switch (state) {
      case S_NOTHING:
        switch (buf[0]) {
          case '\n':
            _line++;
            _column = 0;
            // fall through
          case ' ':
          case '\t':
          case '\r':
            break;
          case '[':
            return Token::listOpen();
          case ']':
            return Token::listClose();
          case '{':
            return Token::objOpen();
          case '}':
            return Token::objClose();
          case ',':
            return Token::comma();
          case ':':
            return Token::colon();
          case '"':
            result = "";
            state = S_STRING;
            break;
          case 't': {
            char rest[3];
            is.read(rest, sizeof(rest));
            _column += static_cast<int>(is.gcount());
            if (!is.good() || std::strncmp(rest, "rue", 3) != 0) {
              throw JSONError(_env, errLocation(),
                              "unexpected token `" + string(rest, is.gcount()) + "'");
            }
            state = S_NOTHING;
            return Token(true);
          } break;
          case 'f': {
            char rest[4];
            is.read(rest, sizeof(rest));
            _column += static_cast<int>(is.gcount());
            if (!is.good() || std::strncmp(rest, "alse", 4) != 0) {
              throw JSONError(_env, errLocation(),
                              "unexpected token `" + string(rest, is.gcount()) + "'");
            }
            state = S_NOTHING;
            return Token(false);
          } break;
          case 'n': {
            char rest[3];
            is.read(rest, sizeof(rest));
            _column += static_cast<int>(is.gcount());
            if (!is.good() || std::strncmp(rest, "ull", 3) != 0) {
              throw JSONError(_env, errLocation(),
                              "unexpected token `" + string(rest, is.gcount()) + "'");
            }
            state = S_NOTHING;
            return Token::null();
          } break;
          default:
            if ((buf[0] >= '0' && buf[0] <= '9') || (buf[0] == '-')) {
              result = buf[0];
              state = S_INT;
            } else {
              throw JSONError(_env, errLocation(), "unexpected token `" + string(1, buf[0]) + "'");
            }
            break;
        }
        break;
      case S_STRING_ESCAPE:
        switch (buf[0]) {
          case '"':
            result += "\"";
            break;
          case '\\':
            result += "\\";
            break;
          case '/':
            result += "/";
            break;
          case 'n':
            result += "\n";
            break;
          case 't':
            result += "\t";
            break;
          case 'u': {
            char rest[4];
            is.read(rest, sizeof(rest));
            _column += sizeof(rest);
            if (is.eof()) {
              throw JSONError(_env, errLocation(), "unexpected end of file");
            }
            if (!is.good()) {
              throw JSONError(_env, errLocation(),
                              "unexpected token `" + string(rest, is.gcount()) + "'");
            }
            std::stringstream ss;
            ss << std::hex << std::string(rest, is.gcount());
            unsigned long codepoint;
            ss >> codepoint;
            if (ss.fail()) {
              throw JSONError(_env, errLocation(),
                              "unexpected token `" + string(rest, is.gcount()) + "'");
            }
            if (codepoint >= 0xD800 && codepoint <= 0xDBFF) {
              // high surrogate, should follow with low surrogate
              char rest[6];
              is.read(rest, sizeof(rest));
              _column += sizeof(rest);
              if (is.eof()) {
                throw JSONError(_env, errLocation(), "unexpected end of file");
              }
              if (!is.good() || std::strncmp(rest, "\\u", 2) != 0) {
                throw JSONError(_env, errLocation(),
                                "unexpected token `" + string(rest, is.gcount()) + "'");
              }
              std::stringstream ss;
              ss << std::hex << std::string(&rest[2], 4);
              unsigned int lowSurrogate;
              ss >> lowSurrogate;
              if (ss.fail() || lowSurrogate < 0xDC00 || lowSurrogate > 0xDFFF) {
                throw JSONError(_env, errLocation(),
                                "unexpected token `" + string(rest, is.gcount()) + "'");
              }
              codepoint = (((codepoint - 0xD800) << 10) | (lowSurrogate - 0xDC00)) | 0x10000;
            }

            char utf8[4];
            size_t bytes = 4;
            if (codepoint <= 0x7F) {
              bytes = 1;
            } else if (codepoint <= 0x7FF) {
              bytes = 2;
            } else if (codepoint <= 0xFFFF) {
              bytes = 3;
            }
            for (size_t i = bytes - 1; i > 0; i--) {
              utf8[i] = static_cast<char>(0x80 | (codepoint & 0x3F));
              codepoint = codepoint >> 6;
            }
            if (bytes > 1) {
              codepoint = (0xF0 << (4 - bytes)) | codepoint;
            }
            utf8[0] = static_cast<char>(codepoint);
            result += std::string(utf8, bytes);
            break;
          }
          default:
            result += "\\";
            result += buf[0];
            break;
        }
        state = S_STRING;
        break;
      case S_STRING:
        if (buf[0] == '"') {
          state = S_NOTHING;
          return Token(result);
        }
        if (buf[0] == '\\') {
          state = S_STRING_ESCAPE;
        } else {
          result += buf[0];
        }
        break;
      case S_INT:
        if (buf[0] == '.') {
          result += buf[0];
          state = S_FLOAT;
        } else if (buf[0] == 'e' || buf[0] == 'E') {
          result += buf[0];
          read_exponent_start();
          state = S_FLOAT_EXPONENT;
        } else if (buf[0] >= '0' && buf[0] <= '9') {
          result += buf[0];
        } else {
          is.unget();
          std::istringstream iss(result);
          int v;
          iss >> v;
          state = S_NOTHING;
          return Token(v);
        }
        break;
      case S_FLOAT:
        if (buf[0] == 'e' || buf[0] == 'E') {
          result += buf[0];
          read_exponent_start();
          state = S_FLOAT_EXPONENT;
        } else if (buf[0] >= '0' && buf[0] <= '9') {
          result += buf[0];
        } else {
          is.unget();
          std::istringstream iss(result);
          double v;
          iss >> v;
          state = S_NOTHING;
          return Token(v);
        }
        break;
      case S_FLOAT_EXPONENT:
        if (buf[0] >= '0' && buf[0] <= '9') {
          result += buf[0];
        } else {
          is.unget();
          std::istringstream iss(result);
          double v;
          iss >> v;
          state = S_NOTHING;
          return Token(v);
        }
        break;
    }
  }
  if (result.empty()) {
    // EOF
    return Token();
  }
  throw JSONError(_env, errLocation(), "unexpected token `" + string(result) + "'");
}

void JSONParser::expectToken(istream& is, JSONParser::TokenT t) {
  Token rt = readToken(is);
  if (rt.t != t) {
    throw JSONError(_env, errLocation(), "unexpected token");
  }
}

string JSONParser::expectString(istream& is) {
  Token rt = readToken(is);
  if (rt.t != T_STRING) {
    throw JSONError(_env, errLocation(), "unexpected token, expected string");
  }
  return rt.s;
}

int JSONParser::expectInt(istream& is) {
  Token rt = readToken(is);
  if (rt.t != T_INT) {
    throw JSONError(_env, errLocation(), "unexpected token, expected int");
  }
  return rt.i;
}

void JSONParser::expectEof(istream& is) {
  Token rt = readToken(is);
  if (rt.t != T_EOF) {
    throw JSONError(_env, errLocation(), "unexpected token, expected end of file");
  }
}

Expression* JSONParser::parseEnum(std::istream& is) {
  Token next = readToken(is);
  switch (next.t) {
    case T_STRING:
      // Enum identifier
      return new Id(Location().introduce(), next.s, nullptr);
    case T_INT:
      // Integer member of contructor enum
      return IntLit::a(next.i);
    case T_OBJ_OPEN: {
      // Enum object or enum constructor
      auto k = expectString(is);
      expectToken(is, T_COLON);
      return parseEnumObject(is, k);
    }
    default:
      throw JSONError(_env, errLocation(), "invalid enum object");
  }
}

Expression* JSONParser::parseEnumObject(std::istream& is, const std::string& seen) {
  // precondition: already parsed '{ "e" :' or '{ "c" :' or '{ "i":'
  //               seen = "e" or "c" or "i"
  auto key = seen;
  Expression* e;
  std::string c;
  int i = -1;

  for (;;) {
    if (key == "e") {
      e = parseEnum(is);
    } else if (key == "c" && i == -1) {
      c = expectString(is);
    } else if (key == "i" && c.empty()) {
      i = expectInt(is);
    } else {
      throw JSONError(_env, errLocation(), "invalid enum object");
    }

    auto next = readToken(is);
    switch (next.t) {
      case T_COMMA:
        key = expectString(is);
        expectToken(is, T_COLON);
        break;
      case T_OBJ_CLOSE:
        if (e == nullptr ||
            (!c.empty() && !Expression::isa<Id>(e) && !Expression::isa<IntLit>(e) &&
             !Expression::isa<Call>(e)) ||
            (i != -1 && !Expression::isa<Id>(e))) {
          throw JSONError(_env, errLocation(), "invalid enum object");
        }
        if (!c.empty()) {
          return Call::a(Location().introduce(), c, {e});
        }
        if (i != -1) {
          return Call::a(Location().introduce(), "to_enum", {e, IntLit::a(i)});
        }
        return e;
      default:
        throw JSONError(_env, errLocation(), "invalid enum object");
    }
  }
}

Expression* JSONParser::parseSet(istream& is, TypeInst* ti) {
  expectToken(is, T_LIST_OPEN);
  vector<Expression*> exprs;
  vector<pair<Token, Token>> ranges;
  TokenT listT = T_COLON;  // dummy marker
  for (Token next = readToken(is); next.t != T_LIST_CLOSE; next = readToken(is)) {
    switch (next.t) {
      case T_COMMA:
        break;
      case T_INT:
        if (listT == T_STRING || listT == T_OBJ_OPEN) {
          throw JSONError(_env, errLocation(), "invalid set literal");
        }
        if (listT != T_FLOAT) {
          listT = T_INT;
        }
        ranges.emplace_back(next, next);
        break;
      case T_FLOAT:
        if (listT == T_STRING || listT == T_OBJ_OPEN) {
          throw JSONError(_env, errLocation(), "invalid set literal");
        }
        listT = T_FLOAT;
        ranges.emplace_back(next, next);
        break;
      case T_STRING:
        if (listT != T_COLON && listT != T_STRING) {
          throw JSONError(_env, errLocation(), "invalid set literal");
        }
        listT = T_STRING;
        if (ti == nullptr || (!ti->isEnum() && ti->type().bt() != Type::BT_UNKNOWN)) {
          exprs.push_back(new StringLit(Location().introduce(), next.s));
        } else {
          exprs.push_back(new Id(Location().introduce(), next.s, nullptr));
        }
        break;
      case T_BOOL:
        if (listT == T_STRING || listT == T_OBJ_OPEN) {
          throw JSONError(_env, errLocation(), "invalid set literal");
        }
        if (listT == T_COLON) {
          listT = T_BOOL;
        }
        exprs.push_back(_env.constants.boollit(next.b));
        break;
      case T_OBJ_OPEN: {
        if (listT != T_COLON && listT != T_OBJ_OPEN) {
          throw JSONError(_env, errLocation(), "invalid set literal");
        }
        listT = T_OBJ_OPEN;
        Token t = readToken(is);
        expectToken(is, T_COLON);
        exprs.push_back(parseEnumObject(is, t.s));
        break;
      }
      case T_LIST_OPEN: {
        if (listT != T_COLON && listT != T_INT && listT != T_FLOAT) {
          throw JSONError(_env, errLocation(), "invalid set literal");
        }

        Token range_min = readToken(is);
        if (range_min.t == T_INT) {
          if (listT != T_FLOAT) {
            listT = T_INT;
          }
        } else if (range_min.t == T_FLOAT) {
          listT = T_FLOAT;
        } else {
          throw JSONError(_env, errLocation(), "invalid set literal");
        }

        expectToken(is, T_COMMA);

        Token range_max = readToken(is);
        if (range_max.t == T_INT) {
          if (listT != T_FLOAT) {
            listT = T_INT;
          }
        } else if (range_max.t == T_FLOAT) {
          listT = T_FLOAT;
        } else {
          throw JSONError(_env, errLocation(), "invalid set literal");
        }
        ranges.emplace_back(range_min, range_max);

        expectToken(is, T_LIST_CLOSE);
        break;
      }
      default:
        throw JSONError(_env, errLocation(), "invalid set literal");
    }
  }
  expectToken(is, T_OBJ_CLOSE);

  if (listT == T_INT) {
    auto* res = IntSetVal::a();
    for (const auto& range : ranges) {
      auto* isv = IntSetVal::a(range.first.i, range.second.i);
      IntSetRanges isr(isv);
      IntSetRanges r(res);
      Ranges::Union<IntVal, IntSetRanges, IntSetRanges> u(isr, r);
      res = IntSetVal::ai(u);
    }
    return new SetLit(Location().introduce(), res);
  }
  if (listT == T_FLOAT) {
    auto* res = FloatSetVal::a();
    for (const auto& range : ranges) {
      auto* fsv = FloatSetVal::a(range.first.d, range.second.d);
      FloatSetRanges fsr(fsv);
      FloatSetRanges r(res);
      Ranges::Union<FloatVal, FloatSetRanges, FloatSetRanges> u(fsr, r);
      res = FloatSetVal::ai(u);
    }
    return new SetLit(Location().introduce(), res);
  }

  return new SetLit(Location().introduce(), exprs);
}

Expression* JSONParser::parseObject(istream& is, TypeInst* ti) {
  // precondition: found T_OBJ_OPEN
  std::vector<Expression*> fields;

  ASTStringMap<TypeInst*> fieldTIs;
  ASTStringSet optFields;
  if (ti != nullptr && ti->type().bt() == Type::BT_RECORD) {
    auto* dom = Expression::cast<ArrayLit>(ti->domain());
    for (unsigned int i = 0; i < dom->size(); ++i) {
      auto* fieldDef = Expression::cast<VarDecl>((*dom)[i]);
      fieldTIs.emplace(fieldDef->id()->str(), fieldDef->ti());
      if (fieldDef->ti()->type().isOpt()) {
        optFields.insert(fieldDef->id()->str());
      }
    }
  };

  Token next;
  do {
    next = readToken(is);
    if (next.t != T_STRING) {
      throw JSONError(_env, errLocation(), "invalid object");
    }
    ASTString key(next.s);
    expectToken(is, T_COLON);
    if (key == "set") {
      if (!fields.empty()) {
        throw JSONError(_env, errLocation(), "invalid set literal");
      }
      return parseSet(is, ti);
    }
    if (ti != nullptr &&
        (ti->isEnum() || ti->type().bt() == Type::BT_INT || ti->type().bt() == Type::BT_UNKNOWN) &&
        (key == "e" || key == "i" || key == "c")) {
      if (!fields.empty()) {
        throw JSONError(_env, errLocation(), "invalid enum object");
      }
      return parseEnumObject(is, std::string(key.c_str(), key.size()));
    }

    auto it = fieldTIs.find(key);
    Expression* e = parseExp(is, true, it != fieldTIs.end() ? it->second : nullptr);

    fields.push_back(
        new VarDecl(Location().introduce(), new TypeInst(Location().introduce(), Type()), key, e));
    optFields.erase(key);
    next = readToken(is);
  } while (next.t == T_COMMA);
  if (next.t != T_OBJ_CLOSE) {
    throw JSONError(_env, errLocation(), "invalid object");
  }

  // Add <> literal for known optional fields
  for (const auto& key : optFields) {
    fields.push_back(new VarDecl(Location().introduce(),
                                 new TypeInst(Location().introduce(), Type()), key,
                                 _env.constants.absent));
  }

  auto* record = ArrayLit::constructTuple(Location().introduce(), fields);
  record->type(Type::record());
  return record;
}

Expression* JSONParser::parseArray(std::istream& is, TypeInst* ti, size_t range_index) {
  // precondition: opening parenthesis has been read
  vector<Expression*> exps;
  Token next = readToken(is);

  while (next.t != T_LIST_CLOSE) {
    switch (next.t) {
      case T_LIST_OPEN: {
        // Create element TI once
        exps.push_back(parseArray(is, ti, ti == nullptr ? 0 : range_index + 1));
        break;
      }
      case T_COMMA:
        break;
      case T_INT:
        exps.push_back(IntLit::a(next.i));
        break;
      case T_FLOAT:
        exps.push_back(FloatLit::a(next.d));
        break;
      case T_STRING: {
        if (ti == nullptr || (!ti->isEnum() && ti->type().bt() != Type::BT_UNKNOWN)) {
          exps.push_back(new StringLit(Location().introduce(), next.s));
        } else {
          exps.push_back(new Id(Location().introduce(), ASTString(next.s), nullptr));
        }
        break;
      }
      case T_BOOL:
        exps.push_back(new BoolLit(Location().introduce(), next.b));
        break;
      case T_NULL:
        exps.push_back(_env.constants.absent);
        break;
      case T_OBJ_OPEN: {
        TypeInst* elTI = ti;
        if (ti != nullptr && ti->type().bt() == Type::BT_TUPLE) {
          // If parsing a tuple, then retrieve field TI from domain
          auto* dom = Expression::cast<ArrayLit>(ti->domain());
          if (exps.size() < dom->size()) {
            elTI = Expression::cast<TypeInst>((*dom)[static_cast<unsigned int>(exps.size())]);
          }
        }
        exps.push_back(parseObject(is, elTI));
        break;
      }
      default:
        throw JSONError(_env, errLocation(), "cannot parse JSON file");
        break;
    }
    next = readToken(is);
  }
  if (ti != nullptr) {
    if (range_index >= ti->ranges().size()) {
      // Converting the element type of the ti
      if (ti->type().isSet()) {
        // Convert array to a set
        return new SetLit(Location().introduce(), exps);
      }
      if (ti->type().bt() == Type::BT_TUPLE) {
        // Add correct index sets if they are non-standard
        TypeInst* tupTI = ti;
        if (!ti->ranges().empty()) {
          tupTI = Expression::cast<TypeInst>(copy(_env, ti));
          tupTI->type(ti->type().elemType(_env));
          tupTI->setRanges({});
        }
        return coerceArray(tupTI, new ArrayLit(Location().introduce(), exps));
      }
    }
    if (ti->isarray() && range_index == 0) {
      // Add correct index sets if they are non-standard
      return coerceArray(ti, new ArrayLit(Location().introduce(), exps));
    }
  }
  return new ArrayLit(Location().introduce(), exps);
}

Expression* JSONParser::parseExp(std::istream& is, bool parseObjects, TypeInst* ti) {
  Token next = readToken(is);
  switch (next.t) {
    case T_INT:
      return IntLit::a(next.i);
      break;
    case T_FLOAT:
      return FloatLit::a(next.d);
    case T_STRING:
      if (ti == nullptr || (!ti->isEnum() && ti->type().bt() != Type::BT_UNKNOWN)) {
        return new StringLit(Location().introduce(), next.s);
      }
      return new Id(Location().introduce(), ASTString(next.s), nullptr);
    case T_BOOL:
      return new BoolLit(Location().introduce(), next.b);
    case T_NULL:
      return _env.constants.absent;
    case T_OBJ_OPEN:
      return parseObjects ? parseObject(is, ti) : nullptr;
    case T_LIST_OPEN:
      return parseArray(is, ti);
    default:
      throw JSONError(_env, errLocation(), "cannot parse JSON file");
      break;
  }
}

Expression* JSONParser::coerceArray(TypeInst* ti, ArrayLit* al) {
  assert(al != nullptr);
  const Location& loc = Expression::loc(al);

  if (al->empty()) {
    return al;  // Nothing to coerce
  }

  // Add dimensions for array parsed by JSON
  if (ti->type().dim() > 1 && Expression::isa<ArrayLit>((*al)[0])) {
    std::vector<Expression*> elements;
    std::vector<std::pair<size_t, ArrayLit*>> it({{0, al}});
    vector<pair<int, int>> dims;
    dims.emplace_back(1, al->size());
    while (!it.empty()) {
      if (it.size() == ti->type().dim()) {
        for (size_t i = 0; i < it.back().second->size(); ++i) {
          elements.push_back((*it.back().second)[static_cast<unsigned int>(i)]);
        }
        it.pop_back();
      } else {
        if (it.back().first < it.back().second->size()) {
          Expression* expr = (*it.back().second)[static_cast<unsigned int>(it.back().first)];
          it.back().first++;
          if (!Expression::isa<ArrayLit>(expr)) {
            throw JSONError(_env, Expression::loc(expr),
                            "Expected JSON array with " + std::to_string(ti->type().dim()) +
                                " dimensions, but an expression in dimension " +
                                std::to_string(it.size()) + " is not an array literal.");
          }
          auto* nal = Expression::cast<ArrayLit>(expr);
          it.emplace_back(0, nal);
          if (dims.size() < it.size()) {
            dims.emplace_back(1, nal->size());
          } else {
            if (nal->size() != dims[it.size() - 1].second) {
              throw JSONError(_env, Expression::loc(expr),
                              "Mismatch in array dimensions in JSON array literal: dimension " +
                                  std::to_string(it.size()) + " was found with both length " +
                                  std::to_string(dims[it.size() - 1].second) + " and length " +
                                  std::to_string(nal->size()) + ".");
            }
          }
        } else {
          it.pop_back();
        }
      }
    }
    al = new ArrayLit(Expression::loc(al), elements, dims);
  }

  // Convert tuples
  if (ti->type().bt() == Type::BT_TUPLE) {
    if (ti->type().dim() == 0) {
      assert(!ti->isarray());
      al = ArrayLit::constructTuple(Expression::loc(al), al);
    } else {
      auto* types = Expression::cast<ArrayLit>(ti->domain());
      for (unsigned int i = 0; i < al->size(); ++i) {
        if (Expression::isa<ArrayLit>((*al)[i])) {
          auto* tup = ArrayLit::constructTuple(Expression::loc((*al)[i]),
                                               Expression::cast<ArrayLit>((*al)[i]));
          al->set(i, tup);

          if (tup->size() != types->size()) {
            continue;  // Error will be raised by typechecker
          }
          for (unsigned int j = 0; j < tup->size(); ++j) {
            if (Expression::isa<ArrayLit>((*tup)[j])) {
              tup->set(j, coerceArray(Expression::cast<TypeInst>((*types)[j]),
                                      Expression::cast<ArrayLit>((*tup)[j])));
            }
          }
        }
      }
    }
  }

  // Check if just a tuple or no explicit ranges are given for the indices
  if (ti->type().dim() == 0 || ti->ranges().size() != ti->type().dim()) {
    return al;
  }

  // Check if any indexes are missing
  int missing_index = -1;
  bool needs_call = false;
  for (unsigned int i = 0; i < ti->ranges().size(); ++i) {
    TypeInst* nti = ti->ranges()[i];
    if (nti->domain() == nullptr || Expression::isa<AnonVar>(nti->domain())) {
      if (missing_index != -1) {
        return al;  // More than one index set is missing. Cannot compute correct index sets.
      }
      missing_index = static_cast<int>(i);
      needs_call = true;
    } else {
      needs_call = true;
    }
  }

  // Construct index set arguments for an "arrayXd" call.
  std::vector<Expression*> args(ti->ranges().size() + 1);
  Expression* missing_max = missing_index >= 0 ? IntLit::a(al->size()) : nullptr;
  for (unsigned int i = 0; i < ti->ranges().size(); ++i) {
    if (i != missing_index) {
      assert(ti->ranges()[i]->domain() != nullptr);
      args[i] = ti->ranges()[i]->domain();
      if (missing_index >= 0) {
        missing_max = new BinOp(loc.introduce(), missing_max, BOT_IDIV,
                                Call::a(Location().introduce(), "card", {args[i]}));
      }
    }
  }
  if (missing_index >= 0) {
    args[missing_index] = new BinOp(loc.introduce(), IntLit::a(1), BOT_DOTDOT, missing_max);
  }
  args[args.size() - 1] = al;

  std::string name = "array" + std::to_string(ti->ranges().size()) + "d";
  Call* c = Call::a(Expression::loc(al).introduce(), name, args);
  if (al->dims() != 1) {
    Expression::addAnnotation(c, Constants::constants().ann.array_check_form);
  }
  return c;
}

void JSONParser::parseModel(Model* m, std::istream& is, bool isData) {
  // precondition: found T_OBJ_OPEN
  ASTStringMap<TypeInst*> knownIds;
  if (isData) {
    // Collect known VarDecl ids from model and includes
    class VarDeclVisitor : public ItemVisitor {
    private:
      ASTStringMap<TypeInst*>& _knownIds;

    public:
      VarDeclVisitor(ASTStringMap<TypeInst*>& knownIds) : _knownIds(knownIds) {}
      void vVarDeclI(VarDeclI* vdi) {
        VarDecl* vd = vdi->e();
        _knownIds.emplace(vd->id()->str(), vd->ti());
      }
    } _varDecls(knownIds);
    iter_items(_varDecls, m);
  }
  for (;;) {
    string ident = expectString(is);
    ASTString ast_ident(ident);
    expectToken(is, T_COLON);
    auto it = knownIds.find(ast_ident);
    Expression* e = parseExp(is, isData, it != knownIds.end() ? it->second : nullptr);

    if (ident[0] != '_' && (!isData || it != knownIds.end())) {
      if (e == nullptr) {
        // This is a nested object
        auto* subModel = new Model;
        parseModel(subModel, is, isData);
        auto* ii = new IncludeI(Location().introduce(), ast_ident);
        ii->m(subModel, true);
        m->addItem(ii);
      } else {
        auto* ai = new AssignI(Expression::loc(e).introduce(), ast_ident, e);
        m->addItem(ai);
      }
    }

    Token next = readToken(is);
    if (next.t == T_OBJ_CLOSE) {
      break;
    }
    if (next.t != T_COMMA) {
      throw JSONError(_env, errLocation(), "cannot parse JSON file");
    }
  }
}

void JSONParser::parse(Model* m, const std::string& filename0, bool isData) {
  _filename = filename0;
  ifstream is(FILE_PATH(_filename), ios::in);
  if (!is.good()) {
    throw JSONError(_env, Location().introduce(), "cannot open file " + _filename);
  }
  _line = 0;
  _column = 0;
  expectToken(is, T_OBJ_OPEN);
  parseModel(m, is, isData);
  expectEof(is);
}

void JSONParser::parseFromString(Model* m, const std::string& data, bool isData) {
  istringstream iss(data);
  _line = 0;
  _column = 0;
  expectToken(iss, T_OBJ_OPEN);
  parseModel(m, iss, isData);
  expectEof(iss);
}

namespace {
bool is_json(std::istream& is) {
  while (is.good()) {
    char c;
    is.get(c);
    if (c == '{') {
      return true;
    }
    if (c != ' ' && c != '\n' && c != '\t' && c != '\r') {
      return false;
    }
  }
  return false;
}
}  // namespace

bool JSONParser::stringIsJSON(const std::string& data) {
  std::istringstream iss(data);
  return is_json(iss);
}

bool JSONParser::fileIsJSON(const std::string& filename) {
  ifstream is(FILE_PATH(filename), ios::in);
  return is_json(is);
}

}  // namespace MiniZinc
