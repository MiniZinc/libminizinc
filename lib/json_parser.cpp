/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/file_utils.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/iter.hh>
#include <minizinc/json_parser.hh>

#include <fstream>
#include <sstream>
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
  Location loc(_filename, _line, _column, _line, _column);
  return loc;
}

JSONParser::Token JSONParser::readToken(istream& is) {
  string result;
  char buf[1];
  enum { S_NOTHING, S_STRING, S_STRING_ESCAPE, S_INT, S_FLOAT } state;
  state = S_NOTHING;
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
            _column += sizeof(rest);
            if (!is.good() || std::strncmp(rest, "rue", 3) != 0) {
              throw JSONError(_env, errLocation(), "unexpected token `" + string(rest) + "'");
            }
            state = S_NOTHING;
            return Token(true);
          } break;
          case 'f': {
            char rest[4];
            is.read(rest, sizeof(rest));
            _column += sizeof(rest);
            if (!is.good() || std::strncmp(rest, "alse", 4) != 0) {
              throw JSONError(_env, errLocation(), "unexpected token `" + string(rest) + "'");
            }
            state = S_NOTHING;
            return Token(false);
          } break;
          case 'n': {
            char rest[3];
            is.read(rest, sizeof(rest));
            _column += sizeof(rest);
            if (!is.good() || std::strncmp(rest, "ull", 3) != 0) {
              throw JSONError(_env, errLocation(), "unexpected token `" + string(rest) + "'");
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
          case 'n':
            result += "\n";
            break;
          case 't':
            result += "\t";
            break;
          case '"':
            result += "\"";
            break;
          case '\\':
            result += "\\";
            break;
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
        if (e == nullptr || (i != -1 && !e->isa<Id>())) {
          throw JSONError(_env, errLocation(), "invalid enum object");
        }
        if (!c.empty()) {
          return new Call(Location().introduce(), c, {e});
        }
        if (i != -1) {
          return new Call(Location().introduce(), "to_enum", {e, IntLit::a(i)});
        }
        return e;
      default:
        throw JSONError(_env, errLocation(), "invalid enum object");
    }
  }
}

Expression* JSONParser::parseObject(istream& is, bool possibleString) {
  // precondition: found T_OBJ_OPEN
  Token objid = readToken(is);
  if (objid.t != T_STRING) {
    throw JSONError(_env, errLocation(), "invalid object");
  }
  expectToken(is, T_COLON);
  if (objid.s == "set") {
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
          if (possibleString) {
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
  if (objid.s == "e" || objid.s == "c" || objid.s == "i") {
    return parseEnumObject(is, objid.s);
  }
  throw JSONError(_env, errLocation(), "invalid object");
}

ArrayLit* JSONParser::parseArray(std::istream& is, bool possibleString) {
  // precondition: opening parenthesis has been read
  vector<Expression*> exps;
  vector<pair<int, int>> dims;
  dims.emplace_back(1, 0);
  vector<bool> hadDim;
  hadDim.push_back(false);
  Token next;
  for (;;) {
    next = readToken(is);
    if (next.t != T_LIST_OPEN) {
      break;
    }
    dims.emplace_back(1, 0);
    hadDim.push_back(false);
  }
  int curDim = static_cast<int>(dims.size()) - 1;
  for (;;) {
    switch (next.t) {
      case T_LIST_CLOSE:
        hadDim[curDim] = true;
        curDim--;
        if (curDim < 0) {
          goto list_done;
        } else if (!hadDim[curDim]) {
          dims[curDim].second++;
        }
        break;
      case T_LIST_OPEN:
        curDim++;
        break;
      case T_COMMA:
        break;
      case T_INT:
        if (!hadDim[curDim]) {
          dims[curDim].second++;
        }
        exps.push_back(IntLit::a(next.i));
        break;
      case T_FLOAT:
        if (!hadDim[curDim]) {
          dims[curDim].second++;
        }
        exps.push_back(FloatLit::a(next.d));
        break;
      case T_STRING: {
        if (!hadDim[curDim]) {
          dims[curDim].second++;
        }
        if (possibleString) {
          exps.push_back(new StringLit(Location().introduce(), next.s));
        } else {
          exps.push_back(new Id(Location().introduce(), ASTString(next.s), nullptr));
        }
        break;
      }
      case T_BOOL:
        if (!hadDim[curDim]) {
          dims[curDim].second++;
        }
        exps.push_back(new BoolLit(Location().introduce(), next.b));
        break;
      case T_NULL:
        if (!hadDim[curDim]) {
          dims[curDim].second++;
        }
        exps.push_back(_env.constants.absent);
        break;
      case T_OBJ_OPEN:
        if (!hadDim[curDim]) {
          dims[curDim].second++;
        }
        exps.push_back(parseObject(is));
        break;
      default:
        throw JSONError(_env, errLocation(), "cannot parse JSON file");
        break;
    }
    next = readToken(is);
  }
list_done:
  unsigned int expectedSize = 1;
  for (auto& d : dims) {
    expectedSize *= d.second;
  }
  if (exps.size() != expectedSize) {
    std::stringstream ss;
    ss << "mismatch in array dimensions: the array contains " << exps.size() << " elements, but "
       << expectedSize << " elements were expected according to the declared index set(s).";
    throw JSONError(_env, errLocation(), ss.str());
    /// TODO: check each individual sub-array
  }
  return new ArrayLit(Location().introduce(), exps, dims);
}

Expression* JSONParser::parseExp(std::istream& is, bool parseObjects, bool possibleString) {
  Token next = readToken(is);
  switch (next.t) {
    case T_INT:
      return IntLit::a(next.i);
      break;
    case T_FLOAT:
      return FloatLit::a(next.d);
    case T_STRING:
      if (!possibleString) {
        return new Id(Location().introduce(), ASTString(next.s), nullptr);
      }
      return new StringLit(Location().introduce(), next.s);
    case T_BOOL:
      return new BoolLit(Location().introduce(), next.b);
    case T_NULL:
      return _env.constants.absent;
    case T_OBJ_OPEN:
      return parseObjects ? parseObject(is, possibleString) : nullptr;
    case T_LIST_OPEN:
      return parseArray(is, possibleString);
    default:
      throw JSONError(_env, errLocation(), "cannot parse JSON file");
      break;
  }
}

Expression* JSONParser::coerceArray(TypeInst* ti, ArrayLit* al) {
  assert(al != nullptr);
  const Location& loc = al->loc();

  if (al->empty()) {
    return al;  // Nothing to coerce
  }
  if (al->dims() != 1 && al->dims() != ti->ranges().size()) {
    return al;  // Incompatible: TypeError will be thrown on original array
  }

  int missing_index = -1;
  for (int i = 0; i < ti->ranges().size(); ++i) {
    TypeInst* nti = ti->ranges()[i];
    if (nti->domain() == nullptr) {
      if (missing_index != -1) {
        return al;  // More than one index set is missing. Cannot compute correct index sets.
      }
      missing_index = i;
    }
  }

  std::vector<Expression*> args(ti->ranges().size() + 1);
  Expression* missing_max = missing_index >= 0 ? IntLit::a(al->size()) : nullptr;
  for (int i = 0; i < ti->ranges().size(); ++i) {
    if (i != missing_index) {
      assert(ti->ranges()[i]->domain() != nullptr);
      args[i] = ti->ranges()[i]->domain();
      if (missing_index >= 0) {
        missing_max = new BinOp(loc.introduce(), missing_max, BOT_IDIV,
                                new Call(Location().introduce(), "card", {args[i]}));
      }
    }
  }
  if (missing_index >= 0) {
    args[missing_index] = new BinOp(loc.introduce(), IntLit::a(1), BOT_DOTDOT, missing_max);
  }
  args[args.size() - 1] = al;

  std::string name = "array" + std::to_string(ti->ranges().size()) + "d";
  Call* c = new Call(al->loc().introduce(), name, args);
  if (al->dims() != 1) {
    c->addAnnotation(Constants::constants().ann.array_check_form);
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
    expectToken(is, T_COLON);
    auto it = knownIds.find(ident);
    bool possibleString = it == knownIds.end() ||
                          (!it->second->isEnum() && it->second->type().bt() != Type::BT_UNKNOWN);
    Expression* e = parseExp(is, isData, possibleString);
    if (ident[0] != '_' && (!isData || it != knownIds.end())) {
      if (e == nullptr) {
        // This is a nested object
        auto* subModel = new Model;
        parseModel(subModel, is, isData);
        auto* ii = new IncludeI(Location().introduce(), ident);
        ii->m(subModel, true);
        m->addItem(ii);
      } else {
        auto* al = e->dynamicCast<ArrayLit>();
        if (it != knownIds.end() && al != nullptr) {
          if (it->second->isarray()) {
            // Add correct index sets if they are non-standard
            e = coerceArray(it->second, al);
          } else if (it->second->type().isSet()) {
            // Convert array to a set
            e = new SetLit(Location().introduce(), al->getVec());
          }
        }
        auto* ai = new AssignI(e->loc().introduce(), ident, e);
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
