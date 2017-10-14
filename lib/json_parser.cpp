/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/json_parser.hh>

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
    enum { S_NOTHING, S_STRING, S_INT, S_FLOAT } state;
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
              if (!is.good() || rest != string("rue"))
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
              if (!is.good() || rest != string("alse"))
                throw JSONError(env,errLocation(),"unexpected token "+string(rest));
              state = S_NOTHING;
              return Token(false);
            }
              break;
            default:
              if (buf[0]>='0' && buf[0]<='9') {
                result = buf;
                state=S_INT;
              } else {
                throw JSONError(env,errLocation(),"unexpected token "+string(buf));
              }
              break;
          }
          break;
        case S_STRING:
          if (buf[0]=='"') {
            state=S_NOTHING;
            return Token(result);
          }
          result += buf[0];
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
    throw JSONError(env,errLocation(),"unexpected token "+string(result));
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
  
  SetLit* JSONParser::parseSetLit(istream& is) {
    // precondition: found T_OBJ_OPEN
    Token setid = readToken(is);
    if (setid.t != T_STRING || setid.s != "set")
      throw JSONError(env,errLocation(),"invalid set literal");
    expectToken(is, T_COLON);
    expectToken(is, T_LIST_OPEN);
    vector<Token> elems;
    TokenT listT = T_COLON; // dummy marker
    for (Token next = readToken(is); next.t != T_LIST_CLOSE; next = readToken(is)) {
      switch (next.t) {
        case T_COMMA:
          break;
        case T_INT:
          if (listT==T_STRING)
            throw JSONError(env,errLocation(),"invalid set literal");
          if (listT!=T_FLOAT)
            listT = T_INT;
          elems.push_back(next);
          break;
        case T_FLOAT:
          if (listT==T_STRING)
            throw JSONError(env,errLocation(),"invalid set literal");
          listT = T_FLOAT;
          elems.push_back(next);
          break;
        case T_STRING:
          if (listT!=T_COLON && listT!=T_STRING)
            throw JSONError(env,errLocation(),"invalid set literal");
          listT = T_STRING;
          elems.push_back(next);
          break;
        case T_BOOL:
          if (listT==T_STRING)
            throw JSONError(env,errLocation(),"invalid set literal");
          if (listT==T_COLON)
            listT = T_BOOL;
          elems.push_back(next);
          break;
        default:
          throw JSONError(env,errLocation(),"invalid set literal");
      }
    }
    expectToken(is, T_OBJ_CLOSE);
    vector<Expression*> elems_e(elems.size());
    switch (listT) {
      case T_COLON:
        break;
      case T_BOOL:
        for (unsigned int i=0; i<elems.size(); i++) {
          elems_e[i] = new BoolLit(Location().introduce(),elems[i].b);
        }
        break;
      case T_INT:
        for (unsigned int i=0; i<elems.size(); i++) {
          elems_e[i] = IntLit::a(elems[i].i);
        }
        break;
      case T_FLOAT:
        for (unsigned int i=0; i<elems.size(); i++) {
          elems_e[i] = FloatLit::a(elems[i].d);
        }
        break;
      case T_STRING:
        for (unsigned int i=0; i<elems.size(); i++) {
          elems_e[i] = new StringLit(Location().introduce(),elems[i].s);
        }
        break;
      default:
        break;
    }
    return new SetLit(Location().introduce(), elems_e);
  }

  ArrayLit*
  JSONParser::parseArray(std::istream &is) {
    // precondition: opening parenthesis has been read
    vector<Expression*> exps;
    vector<pair<int,int> > dims;
    dims.push_back(make_pair(1, 0));
    vector<bool> hadDim;
    hadDim.push_back(false);
    Token next;
    for (;;) {
      next = readToken(is);
      if (next.t!=T_LIST_OPEN)
        break;
      dims.push_back(make_pair(1, 0));
      hadDim.push_back(false);
    }
    int curDim = dims.size()-1;
    for (;;) {
      switch (next.t) {
        case T_LIST_CLOSE:
          if (!hadDim[curDim] && dims[curDim].second>0)
            dims[curDim].second++;
          hadDim[curDim] = true;
          curDim--;
          if (curDim<0)
            goto list_done;
          break;
        case T_LIST_OPEN:
          curDim++;
          break;
        case T_COMMA:
          if (!hadDim[curDim])
            dims[curDim].second++;
          break;
        case T_INT:
          exps.push_back(IntLit::a(next.i));
          break;
        case T_FLOAT:
          exps.push_back(FloatLit::a(next.d));
          break;
        case T_STRING:
          exps.push_back(new StringLit(Location().introduce(),next.s));
          break;
        case T_BOOL:
          exps.push_back(new BoolLit(Location().introduce(),next.b));
          break;
        case T_OBJ_OPEN:
          exps.push_back(parseSetLit(is));
          break;
        default:
          throw JSONError(env,errLocation(),"cannot parse JSON file");
          break;
      }
      next = readToken(is);
    }
  list_done:
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
      case T_OBJ_OPEN:
        return parseSetLit(is);
      case T_LIST_OPEN:
        return parseArray(is);
      default:
        throw JSONError(env,errLocation(),"cannot parse JSON file");
        break;
    }
  }
  
  void
  JSONParser::parse(Model* m, std::string filename0) {
    filename = filename0;
    ifstream is;
    is.open(filename, ios::in);
    line = 0;
    column = 0;
    if (!is.good()) {
      throw JSONError(env,Location().introduce(),"cannot open file "+filename);
    }
    expectToken(is, T_OBJ_OPEN);
    for (;;) {
      string ident = expectString(is);
      expectToken(is, T_COLON);
      Expression* e = parseExp(is);
      if (ident[0]!='_') {
        AssignI* ai = new AssignI(Location().introduce(),ident,e);
        m->addItem(ai);
      }
      Token next = readToken(is);
      if (next.t==T_OBJ_CLOSE)
        break;
      if (next.t!=T_COMMA)
        throw JSONError(env,errLocation(),"cannot parse JSON file");
    }
  }
  
}
