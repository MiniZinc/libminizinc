/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/json_parser.hh>

#include <minizinc/iter.hh>
#include <fstream>
#include <sstream>

using namespace std;

namespace MiniZinc {
  
  class JSONParser::Token {
  public:
    TokenT t;
  protected:
    Token(TokenT t0) : t(t0) {}
  public:
    Token(void) : t(T_EOF) {}
    std::string s;
    int i;
    double d;
    bool b;
    Token(std::string s0) : t(T_STRING), s(s0) {}
    Token(int i0) : t(T_INT), i(i0), d(i0) {}
    Token(double d0) : t(T_FLOAT), d(d0) {}
    Token(bool b0) : t(T_BOOL), i(b0), d(b0), b(b0) {}
    static Token listOpen() { return Token(T_LIST_OPEN); }
    static Token listClose() { return Token(T_LIST_CLOSE); }
    static Token objOpen() { return Token(T_OBJ_OPEN); }
    static Token objClose() { return Token(T_OBJ_CLOSE); }
    static Token comma() { return Token(T_COMMA); }
    static Token colon() { return Token(T_COLON); }
    static Token eof() { return Token(T_EOF); }
    static Token null() { return Token(T_NULL); }
    string toString(void) {
      switch (t) {
        case T_LIST_OPEN: return "[";
        case T_LIST_CLOSE: return "]";
        case T_OBJ_OPEN: return "{";
        case T_OBJ_CLOSE: return "}";
        case T_COMMA: return ",";
        case T_COLON: return ":";
        case T_STRING: return "\""+s+"\"";
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
    }
  };
  
  Location
  JSONParser::errLocation(void) const {
    Location loc(filename,line,column,line,column);
    return loc;
  }
  
  JSONParser::Token
  JSONParser::readToken(istream& is) {
    string result;
    char buf[1];
    enum { S_NOTHING, S_STRING, S_STRING_ESCAPE, S_INT, S_FLOAT } state;
    state = S_NOTHING;
    while (is.good()) {
      is.read(buf, sizeof(buf));
      column += sizeof(buf);
      if (is.eof())
        return Token::eof();
      if (!is.good())
        throw JSONError(env,errLocation(),"tokenization failed");
      switch (state) {
        case S_NOTHING:
          switch (buf[0]) {
            case '\n':
              line++;
              column = 0;
              // fall through
            case ' ':
            case '\t':
            case '\r':
              break;
            case '[': return Token::listOpen();
            case ']': return Token::listClose();
            case '{': return Token::objOpen();
            case '}': return Token::objClose();
            case ',': return Token::comma();
            case ':': return Token::colon();
            case '"':
              result="";
              state=S_STRING;
              break;
            case 't':
            {
              char rest[3];
              is.read(rest,sizeof(rest));
              column += sizeof(rest);
              if (!is.good() || std::strncmp(rest, "rue", 3) != 0)
                throw JSONError(env,errLocation(),"unexpected token `"+string(rest)+"'");
              state = S_NOTHING;
              return Token(true);
            }
              break;
            case 'f':
            {
              char rest[4];
              is.read(rest,sizeof(rest));
              column += sizeof(rest);
              if (!is.good() || std::strncmp(rest, "alse", 4) != 0)
                throw JSONError(env,errLocation(),"unexpected token `"+string(rest)+"'");
              state = S_NOTHING;
              return Token(false);
            }
              break;
            case 'n':
            {
              char rest[3];
              is.read(rest,sizeof(rest));
              column += sizeof(rest);
              if (!is.good() || std::strncmp(rest, "ull", 3) != 0)
                throw JSONError(env,errLocation(),"unexpected token `"+string(rest)+"'");
              state = S_NOTHING;
              return Token::null();
            }
              break;
            default:
              if ( (buf[0]>='0' && buf[0]<='9') || (buf[0]=='-') ) {
                result = buf[0];
                state=S_INT;
              } else {
                throw JSONError(env,errLocation(),"unexpected token `"+string(1,buf[0])+"'");
              }
              break;
          }
          break;
        case S_STRING_ESCAPE:
          switch (buf[0]) {
            case 'n': result += "\n"; break;
            case 't': result += "\t"; break;
            case '"': result += "\""; break;
            case '\\': result += "\\"; break;
            default: result += "\\"; result += buf[0]; break;
          }
          state = S_STRING;
          break;
        case S_STRING:
          if (buf[0]=='"') {
            state=S_NOTHING;
            return Token(result);
          }
          if (buf[0]=='\\') {
            state=S_STRING_ESCAPE;
          } else {
            result += buf[0];
          }
          break;
        case S_INT:
          if (buf[0]=='.') {
            result += buf[0];
            state=S_FLOAT;
          } else if (buf[0]>='0' && buf[0]<='9') {
            result += buf[0];
          } else {
            is.unget();
            std::istringstream iss(result);
            int v;
            iss >> v;
            state=S_NOTHING;
            return Token(v);
          }
          break;
        case S_FLOAT:
          if (buf[0]>='0' && buf[0]<='9') {
            result += buf[0];
          } else {
            is.unget();
            std::istringstream iss(result);
            double v;
            iss >> v;
            state=S_NOTHING;
            return Token(v);
          }
          break;
      }
    }
    throw JSONError(env,errLocation(),"unexpected token `"+string(result)+"'");
  }
  
  void JSONParser::expectToken(istream& is, JSONParser::TokenT t) {
    Token rt = readToken(is);
    if (rt.t != t) {
      throw JSONError(env,errLocation(),"unexpected token");
    }
  }
  
  string JSONParser::expectString(istream& is) {
    Token rt = readToken(is);
    if (rt.t != T_STRING) {
      throw JSONError(env,errLocation(),"unexpected token, expected string");
    }
    return rt.s;
  }
  
  JSONParser::Token JSONParser::parseEnumString(istream& is) {
    Token next = readToken(is);
    if (next.t != T_STRING) {
      throw JSONError(env,errLocation(),"invalid enum object");
    }
    if (next.s.empty()) {
      throw JSONError(env,errLocation(),"invalid enum identifier");
    }
    size_t nonIdChar = next.s.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_");
    size_t nonIdBegin = next.s.find_first_of("0123456789_");
    if (nonIdChar!=std::string::npos || nonIdBegin==0) {
      next.s = "'"+next.s+"'";
    }
    return next;
  }
  
  Expression* JSONParser::parseObject(istream& is) {
    // precondition: found T_OBJ_OPEN
    Token objid = readToken(is);
    if (objid.t != T_STRING)
      throw JSONError(env,errLocation(),"invalid object");
    expectToken(is, T_COLON);
    if (objid.s == "set") {
      expectToken(is, T_LIST_OPEN);
      vector<Token> elems;
      TokenT listT = T_COLON; // dummy marker
      for (Token next = readToken(is); next.t != T_LIST_CLOSE; next = readToken(is)) {
        switch (next.t) {
          case T_COMMA:
            break;
          case T_INT:
            if (listT==T_STRING || listT==T_OBJ_OPEN)
              throw JSONError(env,errLocation(),"invalid set literal");
            if (listT!=T_FLOAT)
              listT = T_INT;
            elems.push_back(next);
            elems.push_back(next);
            break;
          case T_FLOAT:
            if (listT==T_STRING || listT==T_OBJ_OPEN)
              throw JSONError(env,errLocation(),"invalid set literal");
            listT = T_FLOAT;
            elems.push_back(next);
            elems.push_back(next);
            break;
          case T_STRING:
            if (listT!=T_COLON && listT!=T_STRING)
              throw JSONError(env,errLocation(),"invalid set literal");
            listT = T_STRING;
            elems.push_back(next);
            break;
          case T_BOOL:
            if (listT==T_STRING || listT==T_OBJ_OPEN)
              throw JSONError(env,errLocation(),"invalid set literal");
            if (listT==T_COLON)
              listT = T_BOOL;
            elems.push_back(next);
            break;
          case T_OBJ_OPEN:
          {
            if (listT!=T_COLON && listT!=T_OBJ_OPEN)
              throw JSONError(env,errLocation(),"invalid set literal");
            listT = T_OBJ_OPEN;
            Token enumid = readToken(is);
            if (enumid.t != T_STRING || enumid.s != "e")
              throw JSONError(env,errLocation(),"invalid enum object");
            expectToken(is, T_COLON);
            Token next = parseEnumString(is);
            expectToken(is, T_OBJ_CLOSE);
            elems.push_back(next);
            break;
          }
          case T_LIST_OPEN:
            if (listT != T_COLON && listT != T_INT && listT != T_FLOAT)
              throw JSONError(env, errLocation(), "invalid set literal");

            next = readToken(is);
            if (next.t == T_INT) {
              if (listT != T_FLOAT)
                listT = T_INT;
            } else if (next.t == T_FLOAT) {
              listT = T_FLOAT;
            } else {
              throw JSONError(env, errLocation(), "invalid set literal");
            }
            elems.push_back(next);

            expectToken(is, T_COMMA);

            next = readToken(is);
            if (next.t == T_INT) {
              if (listT != T_FLOAT)
                listT = T_INT;
            } else if (next.t == T_FLOAT) {
              listT = T_FLOAT;
            } else {
              throw JSONError(env, errLocation(), "invalid set literal");
            }
            elems.push_back(next);

            expectToken(is, T_LIST_CLOSE);
            break;
          default:
            throw JSONError(env,errLocation(),"invalid set literal");
        }
      }
      expectToken(is, T_OBJ_CLOSE);

      if (listT == T_INT) {
        int n = elems.size() / 2;
        auto res = IntSetVal::a();
        for (unsigned int i = 0; i < n; i++) {
          IntVal m(elems[2 * i].i);
          IntVal n(elems[2 * i + 1].i);
          auto isv = IntSetVal::a(m, n);
          IntSetRanges isr(isv);
          IntSetRanges r(res);
          Ranges::Union<IntVal, IntSetRanges, IntSetRanges> u(isr, r);
          res = IntSetVal::ai(u);
        }
        return new SetLit(Location().introduce(), res);
      } else if (listT == T_FLOAT) {
        int n = elems.size() / 2;
        auto res = FloatSetVal::a();
        for (unsigned int i = 0; i < n; i ++) {
          FloatVal m(elems[2 * i].d);
          FloatVal n(elems[2 * i + 1].d);
          auto fsv = FloatSetVal::a(m, n);
          FloatSetRanges fsr(fsv);
          FloatSetRanges r(res);
          Ranges::Union<FloatVal, FloatSetRanges, FloatSetRanges > u(fsr, r);
          res = FloatSetVal::ai(u);
        }
        return new SetLit(Location().introduce(), res);
      }

      vector<Expression*> elems_e(elems.size());
      switch (listT) {
        case T_COLON:
          break;
        case T_BOOL:
          for (unsigned int i=0; i<elems.size(); i++) {
            elems_e[i] = new BoolLit(Location().introduce(),elems[i].b);
          }
          break;
        case T_STRING:
          for (unsigned int i=0; i<elems.size(); i++) {
            elems_e[i] = new StringLit(Location().introduce(),elems[i].s);
          }
          break;
        case T_OBJ_OPEN:
          for (unsigned int i=0; i<elems.size(); i++) {
            elems_e[i] = new Id(Location().introduce(),ASTString(elems[i].s),NULL);
          }
          break;
        default:
          break;
      }
      return new SetLit(Location().introduce(), elems_e);
    } else if (objid.s == "e") {
      Token next = parseEnumString(is);
      expectToken(is, T_OBJ_CLOSE);
      return new Id(Location().introduce(),ASTString(next.s),NULL);
    } else {
      throw JSONError(env,errLocation(),"invalid object");
    }
  }

  ArrayLit*
  JSONParser::parseArray(std::istream &is) {
    // precondition: opening parenthesis has been read
    vector<Expression*> exps;
    vector<pair<int,int> > dims;
    dims.emplace_back(1, 0);
    vector<bool> hadDim;
    hadDim.push_back(false);
    Token next;
    for (;;) {
      next = readToken(is);
      if (next.t!=T_LIST_OPEN)
        break;
      dims.emplace_back(1, 0);
      hadDim.push_back(false);
    }
    int curDim = static_cast<int>(dims.size())-1;
    for (;;) {
      switch (next.t) {
        case T_LIST_CLOSE:
          hadDim[curDim] = true;
          curDim--;
          if (curDim<0) {
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
        case T_STRING:
          if (!hadDim[curDim]) {
            dims[curDim].second++;
          }
          exps.push_back(new StringLit(Location().introduce(),next.s));
          break;
        case T_BOOL:
          if (!hadDim[curDim]) {
            dims[curDim].second++;
          }
          exps.push_back(new BoolLit(Location().introduce(),next.b));
          break;
        case T_NULL:
          if (!hadDim[curDim]) {
            dims[curDim].second++;
          }
          exps.push_back(constants().absent);
          break;
        case T_OBJ_OPEN:
          if (!hadDim[curDim]) {
            dims[curDim].second++;
          }
          exps.push_back(parseObject(is));
          break;
        default:
          throw JSONError(env,errLocation(),"cannot parse JSON file");
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
      throw JSONError(env,errLocation(),"mismatch in array dimensions");
      /// TODO: check each individual sub-array
    }
    return new ArrayLit(Location().introduce(),exps,dims);
  }
  
  Expression*
  JSONParser::parseExp(std::istream &is) {
    Token next = readToken(is);
    switch (next.t) {
      case T_INT:
        return IntLit::a(next.i);
        break;
      case T_FLOAT:
        return FloatLit::a(next.d);
      case T_STRING:
        return new StringLit(Location().introduce(),next.s);
      case T_BOOL:
        return new BoolLit(Location().introduce(),next.b);
      case T_NULL:
        return constants().absent;
      case T_OBJ_OPEN:
        return parseObject(is);
      case T_LIST_OPEN:
        return parseArray(is);
      default:
        throw JSONError(env,errLocation(),"cannot parse JSON file");
        break;
    }
  }
  
  void
  JSONParser::parse(Model* m, std::istream& is, bool ignoreUnknown) {
    line = 0;
    column = 0;
    expectToken(is, T_OBJ_OPEN);
    unordered_set<string> knownIds;
    if (ignoreUnknown) {
      // Collect known VarDecl ids from model and includes
      class VarDeclVisitor : public ItemVisitor {
      private:
        unordered_set<string>& knownIds;
      public:
        VarDeclVisitor(unordered_set<string>& _knownIds) : knownIds(_knownIds) {}
        void vVarDeclI(VarDeclI* vdi) {
          knownIds.insert(vdi->e()->id()->str().str());
        }
      } _varDecls(knownIds);
      iterItems(_varDecls, m);
    }
    for (;;) {
      string ident = expectString(is);
      expectToken(is, T_COLON);
      Expression* e = parseExp(is);
      if (ident[0]!='_' && (!ignoreUnknown || knownIds.count(ident) == 1)) {
        AssignI* ai = new AssignI(Location().introduce(), ident, e);
        m->addItem(ai);
      }
      Token next = readToken(is);
      if (next.t==T_OBJ_CLOSE)
        break;
      if (next.t!=T_COMMA)
        throw JSONError(env,errLocation(),"cannot parse JSON file");
    }
  }

  void
  JSONParser::parse(Model* m, const std::string& filename0, bool ignoreUnknown) {
    filename = filename0;
    ifstream is;
    is.open(filename, ios::in);
    if (!is.good()) {
      throw JSONError(env,Location().introduce(),"cannot open file "+filename);
    }
    parse(m, is, ignoreUnknown);
  }
  
  void
  JSONParser::parseFromString(Model* m, const std::string& data, bool ignoreUnknown) {
    istringstream iss(data);
    line = 0;
    column = 0;
    parse(m, iss, ignoreUnknown);
  }
  
  namespace {
    bool isJSON(std::istream& is) {
      while (is.good()) {
        char c = is.get();
        if (c=='{')
          return true;
        if (c!=' ' && c!='\n' && c!='\t' && c!='\r')
          return false;
      }
      return false;
    }
  }
  
  bool JSONParser::stringIsJSON(const std::string& data) {
    std::istringstream iss(data);
    return isJSON(iss);
  }

  bool JSONParser::fileIsJSON(const std::string& filename) {
    ifstream is;
    is.open(filename, ios::in);
    return isJSON(is);
  }

}
