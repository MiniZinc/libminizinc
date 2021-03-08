/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Pierre Wilke <wilke.pierre@gmail.com>
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/astexception.hh>
#include <minizinc/hash.hh>
#include <minizinc/iter.hh>
#include <minizinc/model.hh>
#include <minizinc/prettyprinter.hh>

#include <iomanip>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace MiniZinc {

int precedence(const Expression* e) {
  if (const auto* bo = e->dynamicCast<BinOp>()) {
    switch (bo->op()) {
      case BOT_EQUIV:
        return 1200;
      case BOT_IMPL:
      case BOT_RIMPL:
        return 1100;
      case BOT_OR:
      case BOT_XOR:
        return 1000;
      case BOT_AND:
        return 900;
      case BOT_LE:
      case BOT_LQ:
      case BOT_GR:
      case BOT_GQ:
      case BOT_EQ:
      case BOT_NQ:
        return 800;
      case BOT_IN:
      case BOT_SUBSET:
      case BOT_SUPERSET:
        return 700;
      case BOT_UNION:
      case BOT_DIFF:
      case BOT_SYMDIFF:
        return 600;
      case BOT_DOTDOT:
        return 500;
      case BOT_PLUS:
      case BOT_MINUS:
        return 400;
      case BOT_MULT:
      case BOT_IDIV:
      case BOT_MOD:
      case BOT_DIV:
      case BOT_INTERSECT:
        return 300;
      case BOT_POW:
      case BOT_PLUSPLUS:
        return 200;
      default:
        assert(false);
        return -1;
    }

  } else if (e->isa<Let>()) {
    return 1300;

  } else {
    return 0;
  }
}

enum Assoc { AS_LEFT, AS_RIGHT, AS_NONE };

Assoc assoc(const BinOp* bo) {
  switch (bo->op()) {
    case BOT_LE:
    case BOT_LQ:
    case BOT_GR:
    case BOT_GQ:
    case BOT_NQ:
    case BOT_EQ:
    case BOT_IN:
    case BOT_SUBSET:
    case BOT_SUPERSET:
    case BOT_DOTDOT:
      return AS_NONE;
    case BOT_PLUSPLUS:
      return AS_RIGHT;
    default:
      return AS_LEFT;
  }
}

enum Parentheses { PN_LEFT = 1, PN_RIGHT = 2 };

Parentheses need_parentheses(const BinOp* bo, const Expression* left, const Expression* right) {
  int pbo = precedence(bo);
  int pl = precedence(left);
  int pr = precedence(right);
  int ret = static_cast<int>((pbo < pl) || (pbo == pl && assoc(bo) != AS_LEFT));
  ret += 2 * static_cast<int>((pbo < pr) || (pbo == pr && assoc(bo) != AS_RIGHT));
  return static_cast<Parentheses>(ret);
}

void pp_floatval(std::ostream& os, const FloatVal& fv, bool hexFloat) {
  std::ostringstream oss;
  if (fv.isFinite()) {
    if (hexFloat) {
      throw InternalError("disabled due to hexfloat being not supported by g++ 4.9");
      //          std::hexfloat(oss);
      oss << fv.toDouble();
      os << oss.str();
    }
    oss << std::setprecision(std::numeric_limits<double>::digits10 + 1);
    oss << fv;
    if (oss.str().find('e') == std::string::npos && oss.str().find('.') == std::string::npos) {
      oss << ".0";
    }
    os << oss.str();

  } else {
    if (fv.isPlusInfinity()) {
      os << "infinity";
    } else {
      os << "-infinity";
    }
  }
}

class PlainPrinter {
private:
  bool _flatZinc;
  EnvI* _env;
  std::ostream& _os;

public:
  PlainPrinter(std::ostream& os, bool flatZinc, EnvI* env)
      : _env(env), _os(os), _flatZinc(flatZinc) {}

  void p(const Type& type, const Expression* e) {
    switch (type.ti()) {
      case Type::TI_PAR:
        break;
      case Type::TI_VAR:
        _os << "var ";
        break;
    }
    if (type.ot() == Type::OT_OPTIONAL) {
      _os << "opt ";
    }
    if (type.st() == Type::ST_SET) {
      _os << "set of ";
    }
    if (e == nullptr) {
      switch (type.bt()) {
        case Type::BT_INT:
          _os << "int";
          break;
        case Type::BT_BOOL:
          _os << "bool";
          break;
        case Type::BT_FLOAT:
          _os << "float";
          break;
        case Type::BT_STRING:
          _os << "string";
          break;
        case Type::BT_ANN:
          _os << "ann";
          break;
        case Type::BT_BOT:
          _os << "bot";
          break;
        case Type::BT_TOP:
          _os << "top";
          break;
        case Type::BT_UNKNOWN:
          _os << "???";
          break;
      }
    } else {
      p(e);
    }
  }

  void p(const Annotation& ann) {
    for (ExpressionSetIter it = ann.begin(); it != ann.end(); ++it) {
      _os << ":: ";
      p(*it);
    }
  }

  void p(const Expression* e) {
    if (e == nullptr) {
      return;
    }
    switch (e->eid()) {
      case Expression::E_INTLIT:
        _os << e->cast<IntLit>()->v();
        break;
      case Expression::E_FLOATLIT: {
        pp_floatval(_os, e->cast<FloatLit>()->v());
      } break;
      case Expression::E_SETLIT: {
        const SetLit& sl = *e->cast<SetLit>();
        if (sl.isv() != nullptr) {
          if (sl.type().bt() == Type::BT_BOOL) {
            if (sl.isv()->size() == 0) {
              _os << (_flatZinc ? "true..false" : "{}");
            } else {
              _os << "{";
              if (sl.isv()->min() == 0) {
                if (sl.isv()->max() == 0) {
                  _os << "false";
                } else {
                  _os << "false,true";
                }
              } else {
                _os << "true";
              }
              _os << "}";
            }
          } else {
            if (sl.isv()->size() == 0) {
              _os << (_flatZinc ? "1..0" : "{}");
            } else if (sl.isv()->size() == 1) {
              _os << sl.isv()->min(0) << ".." << sl.isv()->max(0);
            } else {
              if (!sl.isv()->min(0).isFinite()) {
                _os << sl.isv()->min(0) << ".." << sl.isv()->max(0) << " union ";
              }
              _os << "{";
              bool first = true;
              for (IntSetRanges isr(sl.isv()); isr(); ++isr) {
                if (isr.min().isFinite() && isr.max().isFinite()) {
                  for (IntVal i = isr.min(); i <= isr.max(); i++) {
                    if (!first) {
                      _os << ",";
                    }
                    first = false;
                    _os << i;
                  }
                }
              }
              _os << "}";
              if (!sl.isv()->max(sl.isv()->size() - 1).isFinite()) {
                _os << " union " << sl.isv()->min(sl.isv()->size() - 1) << ".."
                    << sl.isv()->max(sl.isv()->size() - 1);
              }
            }
          }
        } else if (sl.fsv() != nullptr) {
          if (sl.fsv()->size() == 0) {
            _os << (_flatZinc ? "1.0..0.0" : "{}");
          } else if (sl.fsv()->size() == 1) {
            pp_floatval(_os, sl.fsv()->min(0));
            _os << "..";
            pp_floatval(_os, sl.fsv()->max(0));
          } else {
            bool allSingleton = true;
            for (FloatSetRanges isr(sl.fsv()); isr(); ++isr) {
              if (isr.min() != isr.max()) {
                allSingleton = false;
                break;
              }
            }
            if (allSingleton) {
              _os << "{";
              bool first = true;
              for (FloatSetRanges isr(sl.fsv()); isr(); ++isr) {
                if (!first) {
                  _os << ",";
                }
                first = false;
                pp_floatval(_os, isr.min());
              }
              _os << "}";
            } else {
              bool first = true;
              for (FloatSetRanges isr(sl.fsv()); isr(); ++isr) {
                if (!first) {
                  _os << " union ";
                }
                first = false;
                pp_floatval(_os, isr.min());
                _os << "..";
                pp_floatval(_os, isr.max());
              }
            }
          }
        } else {
          _os << "{";
          for (unsigned int i = 0; i < sl.v().size(); i++) {
            p(sl.v()[i]);
            if (i < sl.v().size() - 1) {
              _os << ",";
            }
          }
          _os << "}";
        }
      } break;
      case Expression::E_BOOLLIT:
        _os << (e->cast<BoolLit>()->v() ? "true" : "false");
        break;
      case Expression::E_STRINGLIT:
        _os << "\"" << Printer::escapeStringLit(e->cast<StringLit>()->v()) << "\"";
        break;
      case Expression::E_ID: {
        if (e == constants().absent) {
          _os << "<>";
        } else {
          const Id* id = e->cast<Id>();
          if (id->decl() != nullptr) {
            id = id->decl()->id();
          }
          if (id->idn() == -1) {
            _os << id->v();
          } else {
            _os << "X_INTRODUCED_" << id->idn() << "_";
          }
        }
      } break;
      case Expression::E_TIID:
        _os << "$" << e->cast<TIId>()->v();
        break;
      case Expression::E_ANON:
        _os << "_";
        break;
      case Expression::E_ARRAYLIT: {
        const ArrayLit& al = *e->cast<ArrayLit>();
        unsigned int n = al.dims();
        if (n == 1 && al.min(0) == 1) {
          _os << "[";
          for (unsigned int i = 0; i < al.size(); i++) {
            p(al[i]);
            if (i < al.size() - 1) {
              _os << ",";
            }
          }
          _os << "]";
        } else if (n == 2 && al.min(0) == 1 && al.min(1) == 1 && al.max(1) != 0) {
          _os << "[|";
          for (int i = 0; i < al.max(0); i++) {
            for (int j = 0; j < al.max(1); j++) {
              p(al[i * al.max(1) + j]);
              if (j < al.max(1) - 1) {
                _os << ",";
              }
            }
            if (i < al.max(0) - 1) {
              _os << "|";
            }
          }
          _os << "|]";
        } else {
          _os << "array" << n << "d(";
          for (int i = 0; i < al.dims(); i++) {
            _os << al.min(i) << ".." << al.max(i);
            _os << ",";
          }
          _os << "[";
          for (unsigned int i = 0; i < al.size(); i++) {
            p(al[i]);
            if (i < al.size() - 1) {
              _os << ",";
            }
          }
          _os << "])";
        }
      } break;
      case Expression::E_ARRAYACCESS: {
        const ArrayAccess& aa = *e->cast<ArrayAccess>();
        p(aa.v());
        _os << "[";
        for (unsigned int i = 0; i < aa.idx().size(); i++) {
          p(aa.idx()[i]);
          if (i < aa.idx().size() - 1) {
            _os << ",";
          }
        }
        _os << "]";
      } break;
      case Expression::E_COMP: {
        const Comprehension& c = *e->cast<Comprehension>();
        _os << (c.set() ? "{" : "[");
        p(c.e());
        _os << " | ";
        for (int i = 0; i < c.numberOfGenerators(); i++) {
          for (int j = 0; j < c.numberOfDecls(i); j++) {
            auto* ident = c.decl(i, j)->id();
            if (ident->idn() == -1) {
              _os << ident->v();
            } else {
              _os << "X_INTRODUCED_" << ident->idn() << "_";
            }
            if (j < c.numberOfDecls(i) - 1) {
              _os << ",";
            }
          }
          if (c.in(i) == nullptr) {
            _os << " = ";
            p(c.where(i));
          } else {
            _os << " in ";
            p(c.in(i));
            if (c.where(i) != nullptr) {
              _os << " where ";
              p(c.where(i));
            }
          }
          if (i < c.numberOfGenerators()) {
            _os << ", ";
          }
        }
        _os << (c.set() ? "}" : "]");
      } break;
      case Expression::E_ITE: {
        const ITE& ite = *e->cast<ITE>();
        for (int i = 0; i < ite.size(); i++) {
          _os << (i == 0 ? "if " : " elseif ");
          p(ite.ifExpr(i));
          _os << " then ";
          p(ite.thenExpr(i));
        }
        if (ite.elseExpr() != nullptr) {
          _os << " else ";
          p(ite.elseExpr());
        }
        _os << " endif";
      } break;
      case Expression::E_BINOP: {
        const BinOp& bo = *e->cast<BinOp>();
        Parentheses ps = need_parentheses(&bo, bo.lhs(), bo.rhs());
        if ((ps & PN_LEFT) != 0) {
          _os << "(";
        }
        p(bo.lhs());
        if ((ps & PN_LEFT) != 0) {
          _os << ")";
        }
        switch (bo.op()) {
          case BOT_PLUS:
            _os << "+";
            break;
          case BOT_MINUS:
            _os << "-";
            break;
          case BOT_MULT:
            _os << "*";
            break;
          case BOT_POW:
            _os << "^";
            break;
          case BOT_DIV:
            _os << "/";
            break;
          case BOT_IDIV:
            _os << " div ";
            break;
          case BOT_MOD:
            _os << " mod ";
            break;
          case BOT_LE:
            _os << " < ";
            break;
          case BOT_LQ:
            _os << "<=";
            break;
          case BOT_GR:
            _os << " > ";
            break;
          case BOT_GQ:
            _os << ">=";
            break;
          case BOT_EQ:
            _os << "==";
            break;
          case BOT_NQ:
            _os << "!=";
            break;
          case BOT_IN:
            _os << " in ";
            break;
          case BOT_SUBSET:
            _os << " subset ";
            break;
          case BOT_SUPERSET:
            _os << " superset ";
            break;
          case BOT_UNION:
            _os << " union ";
            break;
          case BOT_DIFF:
            _os << " diff ";
            break;
          case BOT_SYMDIFF:
            _os << " symdiff ";
            break;
          case BOT_INTERSECT:
            _os << " intersect ";
            break;
          case BOT_PLUSPLUS:
            _os << "++";
            break;
          case BOT_EQUIV:
            _os << " <-> ";
            break;
          case BOT_IMPL:
            _os << " -> ";
            break;
          case BOT_RIMPL:
            _os << " <- ";
            break;
          case BOT_OR:
            _os << " \\/ ";
            break;
          case BOT_AND:
            _os << " /\\ ";
            break;
          case BOT_XOR:
            _os << " xor ";
            break;
          case BOT_DOTDOT:
            _os << "..";
            break;
          default:
            assert(false);
            break;
        }

        if ((ps & PN_RIGHT) != 0) {
          _os << "(";
        }
        p(bo.rhs());
        if ((ps & PN_RIGHT) != 0) {
          _os << ")";
        }
      } break;
      case Expression::E_UNOP: {
        const UnOp& uo = *e->cast<UnOp>();
        switch (uo.op()) {
          case UOT_NOT:
            _os << "not ";
            break;
          case UOT_PLUS:
            _os << "+";
            break;
          case UOT_MINUS:
            _os << "-";
            break;
          default:
            assert(false);
            break;
        }
        bool needParen = (uo.e()->isa<BinOp>() || uo.e()->isa<UnOp>() || !uo.ann().isEmpty());
        if (needParen) {
          _os << "(";
        }
        p(uo.e());
        if (needParen) {
          _os << ")";
        }
      } break;
      case Expression::E_CALL: {
        const Call& c = *e->cast<Call>();
        _os << c.id() << "(";
        for (unsigned int i = 0; i < c.argCount(); i++) {
          p(c.arg(i));
          if (i < c.argCount() - 1) {
            _os << ",";
          }
        }
        _os << ")";
      } break;
      case Expression::E_VARDECL: {
        const VarDecl& vd = *e->cast<VarDecl>();
        p(vd.ti());
        if (!vd.ti()->isEnum() && (vd.id()->idn() != -1 || vd.id()->v().size() > 0)) {
          _os << ":";
        }
        if (vd.id()->idn() != -1) {
          _os << " X_INTRODUCED_" << vd.id()->idn() << "_";
        } else if (vd.id()->v().size() != 0) {
          _os << " " << vd.id()->v();
        }
        if (vd.introduced()) {
          _os << " ::var_is_introduced ";
        }
        p(vd.ann());
        if (vd.e() != nullptr) {
          _os << " = ";
          p(vd.e());
        }
      } break;
      case Expression::E_LET: {
        const Let& l = *e->cast<Let>();
        _os << "let {";

        for (unsigned int i = 0; i < l.let().size(); i++) {
          const Expression* li = l.let()[i];
          if (!li->isa<VarDecl>()) {
            _os << "constraint ";
          }
          p(li);
          if (i < l.let().size() - 1) {
            _os << ", ";
          }
        }
        _os << "} in (";
        p(l.in());
        _os << ")";
      } break;
      case Expression::E_TI: {
        const TypeInst& ti = *e->cast<TypeInst>();
        if (ti.isEnum()) {
          _os << "enum";
        } else if (_env != nullptr) {
          _os << ti.type().toString(*_env);
        } else {
          if (ti.isarray()) {
            _os << "array [";
            for (unsigned int i = 0; i < ti.ranges().size(); i++) {
              p(Type::parint(), ti.ranges()[i]);
              if (i < ti.ranges().size() - 1) {
                _os << ",";
              }
            }
            _os << "] of ";
          }
          p(ti.type(), ti.domain());
        }
      }
    }
    if (!e->isa<VarDecl>()) {
      p(e->ann());
    }
  }

  void p(const Item* i) {
    if (i == nullptr) {
      return;
    }
    if (i->removed()) {
      _os << "% ";
    }
    switch (i->iid()) {
      case Item::II_INC:
        _os << "include \"" << Printer::escapeStringLit(i->cast<IncludeI>()->f()) << "\"";
        break;
      case Item::II_VD:
        p(i->cast<VarDeclI>()->e());
        break;
      case Item::II_ASN:
        _os << i->cast<AssignI>()->id() << " = ";
        p(i->cast<AssignI>()->e());
        break;
      case Item::II_CON:
        _os << "constraint ";
        p(i->cast<ConstraintI>()->e());
        break;
      case Item::II_SOL: {
        const auto* si = i->cast<SolveI>();
        _os << "solve ";
        p(si->ann());
        switch (si->st()) {
          case SolveI::ST_SAT:
            _os << " satisfy";
            break;
          case SolveI::ST_MIN:
            _os << " minimize ";
            p(si->e());
            break;
          case SolveI::ST_MAX:
            _os << " maximize ";
            p(si->e());
            break;
        }
      } break;
      case Item::II_OUT:
        _os << "output ";
        p(i->cast<OutputI>()->e());
        break;
      case Item::II_FUN: {
        const FunctionI& fi = *i->cast<FunctionI>();
        if (fi.ti()->type().isAnn() && fi.e() == nullptr) {
          _os << "annotation ";
        } else if (fi.ti()->type() == Type::parbool()) {
          _os << "test ";
        } else if (fi.ti()->type() == Type::varbool()) {
          _os << "predicate ";
        } else {
          _os << "function ";
          p(fi.ti());
          _os << " : ";
        }
        _os << fi.id();
        if (fi.params().size() > 0) {
          _os << "(";
          for (unsigned int j = 0; j < fi.params().size(); j++) {
            p(fi.params()[j]);
            if (j < fi.params().size() - 1) {
              _os << ",";
            }
          }
          _os << ")";
        }
        p(fi.ann());
        if (fi.e() != nullptr) {
          _os << " = ";
          p(fi.e());
        }
      } break;
    }
    _os << ";" << std::endl;
  }
};

template <class T>
class ExpressionMapper {
protected:
  T& _t;

public:
  ExpressionMapper(T& t) : _t(t) {}
  typename T::ret map(const Expression* e) {
    switch (e->eid()) {
      case Expression::E_INTLIT:
        return _t.mapIntLit(*e->cast<IntLit>());
      case Expression::E_FLOATLIT:
        return _t.mapFloatLit(*e->cast<FloatLit>());
      case Expression::E_SETLIT:
        return _t.mapSetLit(*e->cast<SetLit>());
      case Expression::E_BOOLLIT:
        return _t.mapBoolLit(*e->cast<BoolLit>());
      case Expression::E_STRINGLIT:
        return _t.mapStringLit(*e->cast<StringLit>());
      case Expression::E_ID:
        return _t.mapId(*e->cast<Id>());
      case Expression::E_ANON:
        return _t.mapAnonVar(*e->cast<AnonVar>());
      case Expression::E_ARRAYLIT:
        return _t.mapArrayLit(*e->cast<ArrayLit>());
      case Expression::E_ARRAYACCESS:
        return _t.mapArrayAccess(*e->cast<ArrayAccess>());
      case Expression::E_COMP:
        return _t.mapComprehension(*e->cast<Comprehension>());
      case Expression::E_ITE:
        return _t.mapITE(*e->cast<ITE>());
      case Expression::E_BINOP:
        return _t.mapBinOp(*e->cast<BinOp>());
      case Expression::E_UNOP:
        return _t.mapUnOp(*e->cast<UnOp>());
      case Expression::E_CALL:
        return _t.mapCall(*e->cast<Call>());
      case Expression::E_VARDECL:
        return _t.mapVarDecl(*e->cast<VarDecl>());
      case Expression::E_LET:
        return _t.mapLet(*e->cast<Let>());
      case Expression::E_TI:
        return _t.mapTypeInst(*e->cast<TypeInst>());
      case Expression::E_TIID:
        return _t.mapTIId(*e->cast<TIId>());
      default:
        assert(false);
        return typename T::ret();
        break;
    }
  }
};

class Document {
private:
  int _level;

public:
  Document() : _level(0) {}
  virtual ~Document() {}
  int getLevel() const { return _level; }
  // Make this object a child of "d".
  virtual void setParent(Document* d) { _level = d->_level + 1; }
};

class BreakPoint : public Document {
private:
  bool _dontSimplify;

public:
  BreakPoint() { _dontSimplify = false; }
  BreakPoint(bool ds) { _dontSimplify = ds; }
  ~BreakPoint() override {}
  void setDontSimplify(bool b) { _dontSimplify = b; }
  bool getDontSimplify() const { return _dontSimplify; }
};

class StringDocument : public Document {
private:
  std::string _stringDocument;

public:
  StringDocument() {}
  ~StringDocument() override {}

  StringDocument(std::string s) : _stringDocument(std::move(s)) {}

  std::string getString() { return _stringDocument; }
  void setString(std::string s) { _stringDocument = std::move(s); }
};

class DocumentList : public Document {
private:
  std::vector<Document*> _docs;
  std::string _beginToken;
  std::string _separator;
  std::string _endToken;
  bool _unbreakable;
  bool _alignment;

public:
  ~DocumentList() override {
    std::vector<Document*>::iterator it;
    for (it = _docs.begin(); it != _docs.end(); it++) {
      delete *it;
    }
  }
  DocumentList(std::string beginToken = "", std::string separator = "", std::string endToken = "",
               bool alignment = true);

  void addDocumentToList(Document* d) {
    _docs.push_back(d);
    d->setParent(this);
  }

  void setParent(Document* d) override {
    Document::setParent(d);
    std::vector<Document*>::iterator it;
    for (it = _docs.begin(); it != _docs.end(); it++) {
      (*it)->setParent(this);
    }
  }

  void addStringToList(std::string s) { addDocumentToList(new StringDocument(std::move(s))); }

  void addBreakPoint(bool b = false) { addDocumentToList(new BreakPoint(b)); }

  std::vector<Document*> getDocs() { return _docs; }

  void setList(std::vector<Document*> ld) { _docs = std::move(ld); }

  std::string getBeginToken() { return _beginToken; }

  std::string getEndToken() { return _endToken; }

  std::string getSeparator() { return _separator; }

  bool getUnbreakable() const { return _unbreakable; }

  void setUnbreakable(bool b) { _unbreakable = b; }

  bool getAlignment() const { return _alignment; }
};

DocumentList::DocumentList(std::string beginToken, std::string separator, std::string endToken,
                           bool alignment) {
  _beginToken = std::move(beginToken);
  _separator = std::move(separator);
  _endToken = std::move(endToken);
  _alignment = alignment;
  _unbreakable = false;
}

class Line {
private:
  int _indentation;
  int _lineLength;
  std::vector<std::string> _text;

public:
  Line() : _indentation(0), _lineLength(0), _text(0) {}
  Line(const Line&) = default;
  Line(const int indent) : _indentation(indent), _lineLength(0), _text(0) {}
  Line& operator=(const Line&) = default;
  bool operator==(const Line& l) { return &l == this; }

  void setIndentation(int i) { _indentation = i; }

  int getLength() const { return _lineLength; }
  int getIndentation() const { return _indentation; }
  int getSpaceLeft(int maxwidth) const;
  void addString(const std::string& s);
  void concatenateLines(Line& l);

  void print(std::ostream& os) const {
    for (int i = 0; i < getIndentation(); i++) {
      os << " ";
    }
    std::vector<std::string>::const_iterator it;
    for (it = _text.begin(); it != _text.end(); it++) {
      os << (*it);
    }
    os << "\n";
  }
};

int Line::getSpaceLeft(int maxwidth) const { return maxwidth - _lineLength - _indentation; }
void Line::addString(const std::string& s) {
  _lineLength += static_cast<int>(s.size());
  _text.push_back(s);
}
void Line::concatenateLines(Line& l) {
  _text.insert(_text.end(), l._text.begin(), l._text.end());
  _lineLength += l._lineLength;
}

class LinesToSimplify {
private:
  std::map<int, std::vector<int> > _lines;

  // (i,j) in parent <=> j can only be simplified if i is simplified
  std::vector<std::pair<int, int> > _parent;
  /*
   * if i can't simplify, remove j and his parents
   */
  // mostRecentlyAdded[level] = line of the most recently added
  std::map<int, int> _mostRecentlyAdded;

public:
  std::vector<int>* getLinesForPriority(int p) {
    std::map<int, std::vector<int> >::iterator it;
    for (it = _lines.begin(); it != _lines.end(); it++) {
      if (it->first == p) {
        return &(it->second);
      }
    }
    return nullptr;
  }
  void addLine(int p, int l, int par = -1) {
    if (par == -1) {
      for (int i = p - 1; i >= 0; i--) {
        auto it = _mostRecentlyAdded.find(i);
        if (it != _mostRecentlyAdded.end()) {
          par = it->second;
          break;
        }
      }
    }
    if (par != -1) {
      _parent.emplace_back(l, par);
    }
    _mostRecentlyAdded.insert(std::pair<int, int>(p, l));
    std::map<int, std::vector<int> >::iterator it;
    for (it = _lines.begin(); it != _lines.end(); it++) {
      if (it->first == p) {
        it->second.push_back(l);
        return;
      }
    }
    std::vector<int> v;
    v.push_back(l);
    _lines.insert(std::pair<int, std::vector<int> >(p, v));
  }
  void decrementLine(std::vector<int>* vec, int l) {
    std::vector<int>::iterator vit;
    if (vec != nullptr) {
      for (vit = vec->begin(); vit != vec->end(); vit++) {
        if (*vit >= l) {
          *vit = *vit - 1;
        }
      }
    }
    // Now the map
    std::map<int, std::vector<int> >::iterator it;
    for (it = _lines.begin(); it != _lines.end(); it++) {
      for (vit = it->second.begin(); vit != it->second.end(); vit++) {
        if (*vit >= l) {
          *vit = *vit - 1;
        }
      }
    }
    // And the parent table
    std::vector<std::pair<int, int> >::iterator vpit;
    for (vpit = _parent.begin(); vpit != _parent.end(); vpit++) {
      if (vpit->first >= l) {
        vpit->first--;
      }
      if (vpit->second >= l) {
        vpit->second--;
      }
    }
  }
  void remove(LinesToSimplify& lts) {
    std::map<int, std::vector<int> >::iterator it;
    for (it = lts._lines.begin(); it != lts._lines.end(); it++) {
      std::vector<int>::iterator vit;
      for (vit = it->second.begin(); vit != it->second.end(); vit++) {
        remove(nullptr, *vit, false);
      }
    }
  }
  void remove(std::vector<int>* v, int i, bool success = true) {
    if (v != nullptr) {
      v->erase(std::remove(v->begin(), v->end(), i), v->end());
    }
    for (auto& line : _lines) {
      std::vector<int>& l = line.second;
      l.erase(std::remove(l.begin(), l.end(), i), l.end());
    }
    // Call on its parent
    if (!success) {
      std::vector<std::pair<int, int> >::iterator vpit;
      for (vpit = _parent.begin(); vpit != _parent.end(); vpit++) {
        if (vpit->first == i && vpit->second != i && vpit->second != -1) {
          remove(v, vpit->second, false);
        }
      }
    }
  }
  std::vector<int>* getLinesToSimplify() {
    auto* vec = new std::vector<int>();
    std::map<int, std::vector<int> >::iterator it;
    for (it = _lines.begin(); it != _lines.end(); it++) {
      std::vector<int>& svec = it->second;
      vec->insert(vec->begin(), svec.begin(), svec.end());
    }
    return vec;
  }
};

Document* expression_to_document(const Expression* e);
Document* annotation_to_document(const Annotation& ann);
Document* tiexpression_to_document(const Type& type, const Expression* e) {
  auto* dl = new DocumentList("", "", "", false);
  switch (type.ti()) {
    case Type::TI_PAR:
      break;
    case Type::TI_VAR:
      dl->addStringToList("var ");
      break;
  }
  if (type.ot() == Type::OT_OPTIONAL) {
    dl->addStringToList("opt ");
  }
  if (type.st() == Type::ST_SET) {
    dl->addStringToList("set of ");
  }
  if (e == nullptr) {
    switch (type.bt()) {
      case Type::BT_INT:
        dl->addStringToList("int");
        break;
      case Type::BT_BOOL:
        dl->addStringToList("bool");
        break;
      case Type::BT_FLOAT:
        dl->addStringToList("float");
        break;
      case Type::BT_STRING:
        dl->addStringToList("string");
        break;
      case Type::BT_ANN:
        dl->addStringToList("ann");
        break;
      case Type::BT_BOT:
        dl->addStringToList("bot");
        break;
      case Type::BT_TOP:
        dl->addStringToList("top");
        break;
      case Type::BT_UNKNOWN:
        dl->addStringToList("???");
        break;
    }
  } else {
    dl->addDocumentToList(expression_to_document(e));
  }
  return dl;
}

class ExpressionDocumentMapper {
public:
  typedef Document* ret;
  static ret mapIntLit(const IntLit& il) {
    std::ostringstream oss;
    oss << il.v();
    return new StringDocument(oss.str());
  }
  static ret mapFloatLit(const FloatLit& fl) {
    std::ostringstream oss;
    pp_floatval(oss, fl.v());
    return new StringDocument(oss.str());
  }
  static ret mapSetLit(const SetLit& sl) {
    DocumentList* dl;
    if (sl.isv() != nullptr) {
      if (sl.type().bt() == Type::BT_BOOL) {
        if (sl.isv()->size() == 0) {
          dl = new DocumentList("true..false", "", "");
        } else {
          if (sl.isv()->min() == 0) {
            if (sl.isv()->max() == 0) {
              dl = new DocumentList("{false}", "", "");
            } else {
              dl = new DocumentList("{false,true}", "", "");
            }
          } else {
            dl = new DocumentList("{true}", "", "");
          }
        }
      } else {
        if (sl.isv()->size() == 0) {
          dl = new DocumentList("1..0", "", "");
        } else if (sl.isv()->size() == 1) {
          dl = new DocumentList("", "..", "");
          {
            std::ostringstream oss;
            oss << sl.isv()->min(0);
            dl->addDocumentToList(new StringDocument(oss.str()));
          }
          {
            std::ostringstream oss;
            oss << sl.isv()->max(0);
            dl->addDocumentToList(new StringDocument(oss.str()));
          }
        } else {
          dl = new DocumentList("{", ", ", "}", true);
          IntSetRanges isr(sl.isv());
          for (Ranges::ToValues<IntSetRanges> isv(isr); isv(); ++isv) {
            std::ostringstream oss;
            oss << isv.val();
            dl->addDocumentToList(new StringDocument(oss.str()));
          }
        }
      }
    } else if (sl.fsv() != nullptr) {
      if (sl.fsv()->size() == 0) {
        dl = new DocumentList("1.0..0.0", "", "");
      } else if (sl.fsv()->size() == 1) {
        dl = new DocumentList("", "..", "");
        {
          std::ostringstream oss;
          pp_floatval(oss, sl.fsv()->min(0));
          dl->addDocumentToList(new StringDocument(oss.str()));
        }
        {
          std::ostringstream oss;
          pp_floatval(oss, sl.fsv()->max(0));
          dl->addDocumentToList(new StringDocument(oss.str()));
        }
      } else {
        dl = new DocumentList("", " union ", "", true);
        FloatSetRanges fsr(sl.fsv());
        for (; fsr(); ++fsr) {
          std::ostringstream oss;
          pp_floatval(oss, fsr.min());
          oss << "..";
          pp_floatval(oss, fsr.max());
          dl->addDocumentToList(new StringDocument(oss.str()));
        }
      }

    } else {
      dl = new DocumentList("{", ", ", "}", true);
      for (unsigned int i = 0; i < sl.v().size(); i++) {
        dl->addDocumentToList(expression_to_document((sl.v()[i])));
      }
    }
    return dl;
  }
  static ret mapBoolLit(const BoolLit& bl) {
    return new StringDocument(std::string(bl.v() ? "true" : "false"));
  }
  static ret mapStringLit(const StringLit& sl) {
    std::ostringstream oss;
    oss << "\"" << Printer::escapeStringLit(sl.v()) << "\"";
    return new StringDocument(oss.str());
  }
  static ret mapId(const Id& id) {
    if (&id == constants().absent) {
      return new StringDocument("<>");
    }
    if (id.idn() == -1) {
      return new StringDocument(std::string(id.v().c_str(), id.v().size()));
    }
    std::ostringstream oss;
    oss << "X_INTRODUCED_" << id.idn() << "_";
    return new StringDocument(oss.str());
  }
  static ret mapTIId(const TIId& id) {
    std::ostringstream ss;
    ss << "$" << id.v();
    return new StringDocument(ss.str());
  }
  static ret mapAnonVar(const AnonVar& /*v*/) { return new StringDocument("_"); }
  static ret mapArrayLit(const ArrayLit& al) {
    /// TODO: test multi-dimensional arrays handling
    DocumentList* dl;
    unsigned int n = al.dims();
    if (n == 1 && al.min(0) == 1) {
      dl = new DocumentList("[", ", ", "]");
      for (unsigned int i = 0; i < al.size(); i++) {
        dl->addDocumentToList(expression_to_document(al[i]));
      }
    } else if (n == 2 && al.min(0) == 1 && al.min(1) == 1) {
      dl = new DocumentList("[| ", " | ", " |]");
      for (int i = 0; i < al.max(0); i++) {
        auto* row = new DocumentList("", ", ", "");
        for (int j = 0; j < al.max(1); j++) {
          row->addDocumentToList(expression_to_document(al[i * al.max(1) + j]));
        }
        dl->addDocumentToList(row);
        if (i != al.max(0) - 1) {
          dl->addBreakPoint(true);  // dont simplify
        }
      }
    } else {
      dl = new DocumentList("", "", "");
      std::stringstream oss;
      oss << "array" << n << "d";
      dl->addStringToList(oss.str());
      auto* args = new DocumentList("(", ", ", ")");

      for (int i = 0; i < al.dims(); i++) {
        oss.str("");
        oss << al.min(i) << ".." << al.max(i);
        args->addStringToList(oss.str());
      }
      auto* array = new DocumentList("[", ", ", "]");
      for (unsigned int i = 0; i < al.size(); i++) {
        array->addDocumentToList(expression_to_document(al[i]));
      }
      args->addDocumentToList(array);
      dl->addDocumentToList(args);
    }
    return dl;
  }
  static ret mapArrayAccess(const ArrayAccess& aa) {
    auto* dl = new DocumentList("", "", "");

    dl->addDocumentToList(expression_to_document(aa.v()));
    auto* args = new DocumentList("[", ", ", "]");
    for (unsigned int i = 0; i < aa.idx().size(); i++) {
      args->addDocumentToList(expression_to_document(aa.idx()[i]));
    }
    dl->addDocumentToList(args);
    return dl;
  }
  static ret mapComprehension(const Comprehension& c) {
    std::ostringstream oss;
    DocumentList* dl;
    if (c.set()) {
      dl = new DocumentList("{ ", " | ", " }");
    } else {
      dl = new DocumentList("[ ", " | ", " ]");
    }
    dl->addDocumentToList(expression_to_document(c.e()));
    auto* head = new DocumentList("", " ", "");
    auto* generators = new DocumentList("", ", ", "");
    for (int i = 0; i < c.numberOfGenerators(); i++) {
      auto* gen = new DocumentList("", "", "");
      auto* idents = new DocumentList("", ", ", "");
      for (int j = 0; j < c.numberOfDecls(i); j++) {
        std::ostringstream ss;
        Id* ident = c.decl(i, j)->id();
        if (ident->idn() == -1) {
          ss << ident->v();
        } else {
          ss << "X_INTRODUCED_" << ident->idn() << "_";
        }
        idents->addStringToList(ss.str());
      }
      gen->addDocumentToList(idents);
      if (c.in(i) == nullptr) {
        gen->addStringToList(" = ");
        gen->addDocumentToList(expression_to_document(c.where(i)));
      } else {
        gen->addStringToList(" in ");
        gen->addDocumentToList(expression_to_document(c.in(i)));
        if (c.where(i) != nullptr) {
          gen->addStringToList(" where ");
          gen->addDocumentToList(expression_to_document(c.where(i)));
        }
      }
      generators->addDocumentToList(gen);
    }
    head->addDocumentToList(generators);
    dl->addDocumentToList(head);

    return dl;
  }
  static ret mapITE(const ITE& ite) {
    auto* dl = new DocumentList("", "", "");
    for (int i = 0; i < ite.size(); i++) {
      std::string beg = (i == 0 ? "if " : " elseif ");
      dl->addStringToList(beg);
      dl->addDocumentToList(expression_to_document(ite.ifExpr(i)));
      dl->addStringToList(" then ");

      auto* ifdoc = new DocumentList("", "", "", false);
      ifdoc->addBreakPoint();
      ifdoc->addDocumentToList(expression_to_document(ite.thenExpr(i)));
      dl->addDocumentToList(ifdoc);
      dl->addStringToList(" ");
    }
    dl->addBreakPoint();
    dl->addStringToList("else ");

    auto* elsedoc = new DocumentList("", "", "", false);
    elsedoc->addBreakPoint();
    elsedoc->addDocumentToList(expression_to_document(ite.elseExpr()));
    dl->addDocumentToList(elsedoc);
    dl->addStringToList(" ");
    dl->addBreakPoint();
    dl->addStringToList("endif");

    return dl;
  }
  static ret mapBinOp(const BinOp& bo) {
    Parentheses ps = need_parentheses(&bo, bo.lhs(), bo.rhs());
    DocumentList* opLeft;
    DocumentList* dl;
    DocumentList* opRight;
    bool linebreak = false;
    if ((ps & PN_LEFT) != 0) {
      opLeft = new DocumentList("(", " ", ")");
    } else {
      opLeft = new DocumentList("", " ", "");
    }
    opLeft->addDocumentToList(expression_to_document(bo.lhs()));
    std::string op;
    switch (bo.op()) {
      case BOT_PLUS:
        op = "+";
        break;
      case BOT_MINUS:
        op = "-";
        break;
      case BOT_MULT:
        op = "*";
        break;
      case BOT_POW:
        op = "^";
        break;
      case BOT_DIV:
        op = "/";
        break;
      case BOT_IDIV:
        op = " div ";
        break;
      case BOT_MOD:
        op = " mod ";
        break;
      case BOT_LE:
        op = " < ";
        break;
      case BOT_LQ:
        op = "<=";
        break;
      case BOT_GR:
        op = " > ";
        break;
      case BOT_GQ:
        op = ">=";
        break;
      case BOT_EQ:
        op = "==";
        break;
      case BOT_NQ:
        op = "!=";
        break;
      case BOT_IN:
        op = " in ";
        break;
      case BOT_SUBSET:
        op = " subset ";
        break;
      case BOT_SUPERSET:
        op = " superset ";
        break;
      case BOT_UNION:
        op = " union ";
        break;
      case BOT_DIFF:
        op = " diff ";
        break;
      case BOT_SYMDIFF:
        op = " symdiff ";
        break;
      case BOT_INTERSECT:
        op = " intersect ";
        break;
      case BOT_PLUSPLUS:
        op = "++";
        linebreak = true;
        break;
      case BOT_EQUIV:
        op = " <-> ";
        break;
      case BOT_IMPL:
        op = " -> ";
        break;
      case BOT_RIMPL:
        op = " <- ";
        break;
      case BOT_OR:
        op = " \\/ ";
        linebreak = true;
        break;
      case BOT_AND:
        op = " /\\ ";
        linebreak = true;
        break;
      case BOT_XOR:
        op = " xor ";
        break;
      case BOT_DOTDOT:
        op = "..";
        break;
      default:
        assert(false);
        break;
    }
    dl = new DocumentList("", op, "");

    if ((ps & PN_RIGHT) != 0) {
      opRight = new DocumentList("(", " ", ")");
    } else {
      opRight = new DocumentList("", "", "");
    }
    opRight->addDocumentToList(expression_to_document(bo.rhs()));
    dl->addDocumentToList(opLeft);
    if (linebreak) {
      dl->addBreakPoint();
    }
    dl->addDocumentToList(opRight);

    return dl;
  }
  static ret mapUnOp(const UnOp& uo) {
    auto* dl = new DocumentList("", "", "");
    std::string op;
    switch (uo.op()) {
      case UOT_NOT:
        op = "not ";
        break;
      case UOT_PLUS:
        op = "+";
        break;
      case UOT_MINUS:
        op = "-";
        break;
      default:
        assert(false);
        break;
    }
    dl->addStringToList(op);
    DocumentList* unop;
    bool needParen = (uo.e()->isa<BinOp>() || uo.e()->isa<UnOp>());
    if (needParen) {
      unop = new DocumentList("(", " ", ")");
    } else {
      unop = new DocumentList("", " ", "");
    }

    unop->addDocumentToList(expression_to_document(uo.e()));
    dl->addDocumentToList(unop);
    return dl;
  }
  static ret mapCall(const Call& c) {
    if (c.argCount() == 1) {
      /*
       * if we have only one argument, and this is an array comprehension,
       * we convert it into the following syntax
       * forall (f(i,j) | i in 1..10)
       * -->
       * forall (i in 1..10) (f(i,j))
       */

      const Expression* e = c.arg(0);
      if (e->isa<Comprehension>()) {
        const auto* com = e->cast<Comprehension>();
        if (!com->set()) {
          auto* dl = new DocumentList("", " ", "");
          dl->addStringToList(std::string(c.id().c_str(), c.id().size()));
          auto* args = new DocumentList("", " ", "", false);
          auto* generators = new DocumentList("", ", ", "");

          for (int i = 0; i < com->numberOfGenerators(); i++) {
            auto* gen = new DocumentList("", "", "");
            auto* idents = new DocumentList("", ", ", "");
            for (int j = 0; j < com->numberOfDecls(i); j++) {
              idents->addStringToList(std::string(com->decl(i, j)->id()->v().c_str(),
                                                  com->decl(i, j)->id()->v().size()));
            }
            gen->addDocumentToList(idents);
            if (com->in(i) == nullptr) {
              gen->addStringToList(" = ");
              gen->addDocumentToList(expression_to_document(com->where(i)));
            } else {
              gen->addStringToList(" in ");
              gen->addDocumentToList(expression_to_document(com->in(i)));
              if (com->where(i) != nullptr) {
                gen->addStringToList(" where ");
                gen->addDocumentToList(expression_to_document(com->where(i)));
              }
            }
            generators->addDocumentToList(gen);
          }

          args->addStringToList("(");
          args->addDocumentToList(generators);
          args->addStringToList(")");

          args->addStringToList("(");
          args->addBreakPoint();
          args->addDocumentToList(expression_to_document(com->e()));

          dl->addDocumentToList(args);
          dl->addBreakPoint();
          dl->addStringToList(")");

          return dl;
        }
      }
    }
    std::ostringstream beg;
    beg << c.id() << "(";
    auto* dl = new DocumentList(beg.str(), ", ", ")");
    for (unsigned int i = 0; i < c.argCount(); i++) {
      dl->addDocumentToList(expression_to_document(c.arg(i)));
    }
    return dl;
  }
  static ret mapVarDecl(const VarDecl& vd) {
    std::ostringstream oss;
    auto* dl = new DocumentList("", "", "");
    dl->addDocumentToList(expression_to_document(vd.ti()));
    if (vd.id()->idn() == -1) {
      if (vd.id()->v().size() != 0) {
        oss << ": " << vd.id()->v().c_str();
      }
    } else {
      oss << ": X_INTRODUCED_" << vd.id()->idn() << "_";
    }
    dl->addStringToList(oss.str());

    if (vd.introduced()) {
      dl->addStringToList(" ::var_is_introduced ");
    }
    if (!vd.ann().isEmpty()) {
      dl->addDocumentToList(annotation_to_document(vd.ann()));
    }
    if (vd.e() != nullptr) {
      dl->addStringToList(" = ");
      dl->addDocumentToList(expression_to_document(vd.e()));
    }
    return dl;
  }
  static ret mapLet(const Let& l) {
    auto* letin = new DocumentList("", "", "", false);
    auto* lets = new DocumentList("", " ", "", true);
    auto* inexpr = new DocumentList("", "", "");
    bool ds = l.let().size() > 1;

    for (unsigned int i = 0; i < l.let().size(); i++) {
      if (i != 0) {
        lets->addBreakPoint(ds);
      }
      auto* exp = new DocumentList("", " ", ",");
      const Expression* li = l.let()[i];
      if (!li->isa<VarDecl>()) {
        exp->addStringToList("constraint");
      }
      exp->addDocumentToList(expression_to_document(li));
      lets->addDocumentToList(exp);
    }

    inexpr->addDocumentToList(expression_to_document(l.in()));
    letin->addBreakPoint(ds);
    letin->addDocumentToList(lets);

    auto* letin2 = new DocumentList("", "", "", false);

    letin2->addBreakPoint();
    letin2->addDocumentToList(inexpr);

    auto* dl = new DocumentList("", "", "");
    dl->addStringToList("let {");
    dl->addDocumentToList(letin);
    dl->addBreakPoint(ds);
    dl->addStringToList("} in (");
    dl->addDocumentToList(letin2);
    // dl->addBreakPoint();
    dl->addStringToList(")");
    return dl;
  }
  static ret mapTypeInst(const TypeInst& ti) {
    auto* dl = new DocumentList("", "", "");
    if (ti.isarray()) {
      dl->addStringToList("array [");
      auto* ran = new DocumentList("", ", ", "");
      for (unsigned int i = 0; i < ti.ranges().size(); i++) {
        ran->addDocumentToList(tiexpression_to_document(Type::parint(), ti.ranges()[i]));
      }
      dl->addDocumentToList(ran);
      dl->addStringToList("] of ");
    }
    dl->addDocumentToList(tiexpression_to_document(ti.type(), ti.domain()));
    return dl;
  }
};

Document* annotation_to_document(const Annotation& ann) {
  auto* dl = new DocumentList(" :: ", " :: ", "");
  for (ExpressionSetIter it = ann.begin(); it != ann.end(); ++it) {
    dl->addDocumentToList(expression_to_document(*it));
  }
  return dl;
}

Document* expression_to_document(const Expression* e) {
  if (e == nullptr) {
    return new StringDocument("NULL");
  }
  ExpressionDocumentMapper esm;
  ExpressionMapper<ExpressionDocumentMapper> em(esm);
  auto* dl = new DocumentList("", "", "");
  Document* s = em.map(e);
  dl->addDocumentToList(s);
  if (!e->isa<VarDecl>() && !e->ann().isEmpty()) {
    dl->addDocumentToList(annotation_to_document(e->ann()));
  }
  return dl;
}

class ItemDocumentMapper {
public:
  typedef Document* ret;
  static ret mapIncludeI(const IncludeI& ii) {
    std::ostringstream oss;
    oss << "include \"" << Printer::escapeStringLit(ii.f()) << "\";";
    return new StringDocument(oss.str());
  }
  static ret mapVarDeclI(const VarDeclI& vi) {
    auto* dl = new DocumentList("", " ", ";");
    dl->addDocumentToList(expression_to_document(vi.e()));
    return dl;
  }
  static ret mapAssignI(const AssignI& ai) {
    auto* dl = new DocumentList("", " = ", ";");
    dl->addStringToList(std::string(ai.id().c_str(), ai.id().size()));
    dl->addDocumentToList(expression_to_document(ai.e()));
    return dl;
  }
  static ret mapConstraintI(const ConstraintI& ci) {
    auto* dl = new DocumentList("constraint ", " ", ";");
    dl->addDocumentToList(expression_to_document(ci.e()));
    return dl;
  }
  static ret mapSolveI(const SolveI& si) {
    auto* dl = new DocumentList("", "", ";");
    dl->addStringToList("solve");
    if (!si.ann().isEmpty()) {
      dl->addDocumentToList(annotation_to_document(si.ann()));
    }
    switch (si.st()) {
      case SolveI::ST_SAT:
        dl->addStringToList(" satisfy");
        break;
      case SolveI::ST_MIN:
        dl->addStringToList(" minimize ");
        dl->addDocumentToList(expression_to_document(si.e()));
        break;
      case SolveI::ST_MAX:
        dl->addStringToList(" maximize ");
        dl->addDocumentToList(expression_to_document(si.e()));
        break;
    }
    return dl;
  }
  static ret mapOutputI(const OutputI& oi) {
    auto* dl = new DocumentList("output ", " ", ";");
    dl->addDocumentToList(expression_to_document(oi.e()));
    return dl;
  }
  static ret mapFunctionI(const FunctionI& fi) {
    DocumentList* dl;
    if (fi.ti()->type().isAnn() && fi.e() == nullptr) {
      dl = new DocumentList("annotation ", " ", ";", false);
    } else if (fi.ti()->type() == Type::parbool()) {
      dl = new DocumentList("test ", "", ";", false);
    } else if (fi.ti()->type() == Type::varbool()) {
      dl = new DocumentList("predicate ", "", ";", false);
    } else {
      dl = new DocumentList("function ", "", ";", false);
      dl->addDocumentToList(expression_to_document(fi.ti()));
      dl->addStringToList(": ");
    }
    dl->addStringToList(std::string(fi.id().c_str(), fi.id().size()));
    if (fi.params().size() > 0) {
      auto* params = new DocumentList("(", ", ", ")");
      for (unsigned int i = 0; i < fi.params().size(); i++) {
        auto* par = new DocumentList("", "", "");
        par->setUnbreakable(true);
        par->addDocumentToList(expression_to_document(fi.params()[i]));
        params->addDocumentToList(par);
      }
      dl->addDocumentToList(params);
    }
    if (!fi.ann().isEmpty()) {
      dl->addDocumentToList(annotation_to_document(fi.ann()));
    }
    if (fi.e() != nullptr) {
      dl->addStringToList(" = ");
      dl->addBreakPoint();
      dl->addDocumentToList(expression_to_document(fi.e()));
    }

    return dl;
  }
};

class PrettyPrinter {
public:
  /*
   * \brief Constructor for class Pretty Printer
   * \param maxwidth (default 80) : number of rows
   * \param indentationBase : spaces that represent the atomic number of spaces
   * \param sim : whether we want to simplify the result
   * \param deepSimp : whether we want to simplify at each breakpoint or not
   */
  PrettyPrinter(int _maxwidth = 80, int _indentationBase = 4, bool sim = false,
                bool deepSimp = false);

  void print(Document* d);
  void print(std::ostream& os) const;

private:
  int _maxwidth;
  int _indentationBase;
  int _currentLine;
  int _currentItem;
  std::vector<std::vector<Line> > _items;
  std::vector<LinesToSimplify> _linesToSimplify;
  std::vector<LinesToSimplify> _linesNotToSimplify;
  bool _simp;
  bool _deeplySimp;

  void addItem();

  void addLine(int indentation, bool bp = false, bool simpl = false, int level = 0);
  static std::string printSpaces(int n);
  const std::vector<Line>& getCurrentItemLines() const;

  void printDocument(Document* d, bool alignment, int alignmentCol, const std::string& before = "",
                     const std::string& after = "");
  void printDocList(DocumentList* d, int alignmentCol, const std::string& before = "",
                    const std::string& after = "");
  void printStringDoc(StringDocument* d, bool alignment, int alignmentCol,
                      const std::string& before = "", const std::string& after = "");
  void printString(const std::string& s, bool alignment, int alignmentCol);
  bool simplify(int item, int line, std::vector<int>* vec);
  void simplifyItem(int item);
};

void PrettyPrinter::print(Document* d) {
  addItem();
  addLine(0);
  printDocument(d, true, 0);
  if (_simp) {
    simplifyItem(_currentItem);
  }
}

PrettyPrinter::PrettyPrinter(int maxwidth, int indentationBase, bool sim, bool deepsim) {
  _maxwidth = maxwidth;
  _indentationBase = indentationBase;
  _currentLine = -1;
  _currentItem = -1;

  _simp = sim;
  _deeplySimp = deepsim;
}
const std::vector<Line>& PrettyPrinter::getCurrentItemLines() const { return _items[_currentItem]; }

void PrettyPrinter::addLine(int indentation, bool bp, bool simpl, int level) {
  _items[_currentItem].push_back(Line(indentation));
  _currentLine++;
  if (bp && _deeplySimp) {
    _linesToSimplify[_currentItem].addLine(level, _currentLine);
    if (!simpl) {
      _linesNotToSimplify[_currentItem].addLine(0, _currentLine);
    }
  }
}
void PrettyPrinter::addItem() {
  _items.emplace_back();
  _linesToSimplify.emplace_back();
  _linesNotToSimplify.emplace_back();
  _currentItem++;
  _currentLine = -1;
}

void PrettyPrinter::print(std::ostream& os) const {
  std::vector<Line>::const_iterator it;
  int nItems = static_cast<int>(_items.size());
  for (int item = 0; item < nItems; item++) {
    for (it = _items[item].begin(); it != _items[item].end(); it++) {
      it->print(os);
    }
    // os << std::endl;
  }
}
std::string PrettyPrinter::printSpaces(int n) {
  std::string result;
  for (int i = 0; i < n; i++) {
    result += " ";
  }
  return result;
}

void PrettyPrinter::printDocument(Document* d, bool alignment, int alignmentCol,
                                  const std::string& before, const std::string& after) {
  if (auto* dl = dynamic_cast<DocumentList*>(d)) {
    printDocList(dl, alignmentCol, before, after);
  } else if (auto* sd = dynamic_cast<StringDocument*>(d)) {
    printStringDoc(sd, alignment, alignmentCol, before, after);
  } else if (auto* bp = dynamic_cast<BreakPoint*>(d)) {
    printString(before, alignment, alignmentCol);
    addLine(alignmentCol, _deeplySimp, !bp->getDontSimplify(), d->getLevel());
    printString(after, alignment, alignmentCol);
  } else {
    throw InternalError("PrettyPrinter::print : Wrong type of document");
  }
}

void PrettyPrinter::printStringDoc(StringDocument* d, bool alignment, int alignmentCol,
                                   const std::string& before, const std::string& after) {
  std::string s;
  if (d != nullptr) {
    s = d->getString();
  }
  s = before + s + after;
  printString(s, alignment, alignmentCol);
}

void PrettyPrinter::printString(const std::string& s, bool alignment, int alignmentCol) {
  Line& l = _items[_currentItem][_currentLine];
  int size = static_cast<int>(s.size());
  if (size <= l.getSpaceLeft(_maxwidth)) {
    l.addString(s);
  } else {
    int col = alignment && _maxwidth - alignmentCol >= size ? alignmentCol : _indentationBase;
    addLine(col);
    _items[_currentItem][_currentLine].addString(s);
  }
}

void PrettyPrinter::printDocList(DocumentList* d, int alignmentCol, const std::string& super_before,
                                 const std::string& super_after) {
  std::vector<Document*> ld = d->getDocs();
  std::string beginToken = d->getBeginToken();
  std::string separator = d->getSeparator();
  std::string endToken = d->getEndToken();
  bool _alignment = d->getAlignment();
  if (d->getUnbreakable()) {
    addLine(alignmentCol);
  }
  int currentCol = _items[_currentItem][_currentLine].getIndentation() +
                   _items[_currentItem][_currentLine].getLength();
  int newAlignmentCol =
      _alignment ? currentCol + static_cast<int>(beginToken.size()) : alignmentCol;
  int vectorSize = static_cast<int>(ld.size());
  int lastVisibleElementIndex;
  for (int i = 0; i < vectorSize; i++) {
    if (dynamic_cast<BreakPoint*>(ld[i]) == nullptr) {
      lastVisibleElementIndex = i;
    }
  }
  if (vectorSize == 0) {
    printStringDoc(nullptr, true, newAlignmentCol, super_before + beginToken,
                   endToken + super_after);
  }
  for (int i = 0; i < vectorSize; i++) {
    Document* subdoc = ld[i];
    bool bp = false;
    if (dynamic_cast<BreakPoint*>(subdoc) != nullptr) {
      if (!_alignment) {
        newAlignmentCol += _indentationBase;
      }
      bp = true;
    }
    std::string af;
    std::string be;
    if (i != vectorSize - 1) {
      if (bp || lastVisibleElementIndex <= i) {
        af = "";
      } else {
        af = separator;
      }
    } else {
      af = endToken + super_after;
    }
    if (i == 0) {
      be = super_before + beginToken;
    } else {
      be = "";
    }
    printDocument(subdoc, _alignment, newAlignmentCol, be, af);
  }
  if (d->getUnbreakable()) {
    simplify(_currentItem, _currentLine, nullptr);
  }
}
void PrettyPrinter::simplifyItem(int item) {
  _linesToSimplify[item].remove(_linesNotToSimplify[item]);
  std::vector<int>* vec = (_linesToSimplify[item].getLinesToSimplify());
  while (!vec->empty()) {
    if (!simplify(item, (*vec)[0], vec)) {
      break;
    }
  }
  delete vec;
}

bool PrettyPrinter::simplify(int item, int line, std::vector<int>* vec) {
  if (line == 0) {
    _linesToSimplify[item].remove(vec, line, false);
    return false;
  }
  if (_items[item][line].getLength() > _items[item][line - 1].getSpaceLeft(_maxwidth)) {
    _linesToSimplify[item].remove(vec, line, false);
    return false;
  }
  _linesToSimplify[item].remove(vec, line, true);
  _items[item][line - 1].concatenateLines(_items[item][line]);
  _items[item].erase(_items[item].begin() + line);

  _linesToSimplify[item].decrementLine(vec, line);
  _currentLine--;

  return true;
}

Printer::Printer(std::ostream& os, int width, bool flatZinc, EnvI* env)
    : _env(env), _ism(nullptr), _printer(nullptr), _os(os), _width(width), _flatZinc(flatZinc) {}
void Printer::init() {
  if (_ism == nullptr) {
    _ism = new ItemDocumentMapper();
    _printer = new PrettyPrinter(_width, 4, true, true);
  }
}
Printer::~Printer() {
  delete _printer;
  delete _ism;
}

void Printer::p(Document* d) {
  _printer->print(d);
  _printer->print(_os);
  delete _printer;
  _printer = new PrettyPrinter(_width, 4, true, true);
}
void Printer::p(const Item* i) {
  Document* d;
  switch (i->iid()) {
    case Item::II_INC:
      d = ItemDocumentMapper::mapIncludeI(*i->cast<IncludeI>());
      break;
    case Item::II_VD:
      d = ItemDocumentMapper::mapVarDeclI(*i->cast<VarDeclI>());
      break;
    case Item::II_ASN:
      d = ItemDocumentMapper::mapAssignI(*i->cast<AssignI>());
      break;
    case Item::II_CON:
      d = ItemDocumentMapper::mapConstraintI(*i->cast<ConstraintI>());
      break;
    case Item::II_SOL:
      d = ItemDocumentMapper::mapSolveI(*i->cast<SolveI>());
      break;
    case Item::II_OUT:
      d = ItemDocumentMapper::mapOutputI(*i->cast<OutputI>());
      break;
    case Item::II_FUN:
      d = ItemDocumentMapper::mapFunctionI(*i->cast<FunctionI>());
      break;
  }
  p(d);
  delete d;
}

void Printer::print(const Expression* e) {
  if (_width == 0) {
    PlainPrinter p(_os, _flatZinc, _env);
    p.p(e);
  } else {
    init();
    Document* d = expression_to_document(e);
    p(d);
    delete d;
  }
}
void Printer::print(const Item* i) {
  if (_width == 0) {
    PlainPrinter p(_os, _flatZinc, _env);
    p.p(i);
  } else {
    init();
    p(i);
  }
}
void Printer::print(const Model* m) {
  if (_width == 0) {
    PlainPrinter p(_os, _flatZinc, _env);
    for (auto* i : *m) {
      p.p(i);
    }
  } else {
    init();
    for (auto* i : *m) {
      p(i);
    }
  }
}

}  // namespace MiniZinc

void debugprint(MiniZinc::Expression* e) { std::cerr << *e << "\n"; }
void debugprint(MiniZinc::Item* i) { std::cerr << *i; }
void debugprint(MiniZinc::Model* m) {
  MiniZinc::Printer p(std::cerr, 0);
  p.print(m);
}
void debugprint(const MiniZinc::Location& loc) { std::cerr << loc << std::endl; }
