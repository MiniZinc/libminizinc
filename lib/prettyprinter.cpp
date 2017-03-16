/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Pierre Wilke <wilke.pierre@gmail.com>
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <iomanip>
#include <map>
#include <minizinc/prettyprinter.hh>
#include <minizinc/model.hh>
#include <minizinc/astexception.hh>
#include <minizinc/iter.hh>
#include <minizinc/hash.hh>

namespace MiniZinc {

  int precedence(const Expression* e) {
    if (const BinOp* bo = e->dyn_cast<BinOp>()) {
      switch (bo->op()) {
      case BOT_EQUIV:
        return 1200;
      case BOT_IMPL:
        return 1100;
      case BOT_RIMPL:
        return 1100;
      case BOT_OR:
        return 1000;
      case BOT_XOR:
        return 1000;
      case BOT_AND:
        return 900;
      case BOT_LE:
        return 800;
      case BOT_LQ:
        return 800;
      case BOT_GR:
        return 800;
      case BOT_GQ:
        return 800;
      case BOT_EQ:
        return 800;
      case BOT_NQ:
        return 800;
      case BOT_IN:
        return 700;
      case BOT_SUBSET:
        return 700;
      case BOT_SUPERSET:
        return 700;
      case BOT_UNION:
        return 600;
      case BOT_DIFF:
        return 600;
      case BOT_SYMDIFF:
        return 600;
      case BOT_DOTDOT:
        return 500;
      case BOT_PLUS:
        return 400;
      case BOT_MINUS:
        return 400;
      case BOT_MULT:
        return 300;
      case BOT_IDIV:
        return 300;
      case BOT_MOD:
        return 300;
      case BOT_DIV:
        return 300;
      case BOT_INTERSECT:
        return 300;
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
  
  enum Assoc {
    AS_LEFT, AS_RIGHT, AS_NONE
  };
  
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
  
  enum Parentheses {
    PN_LEFT = 1, PN_RIGHT = 2
  };

  Parentheses needParens(const BinOp* bo, const Expression* left,
                         const Expression* right) {
    int pbo = precedence(bo);
    int pl = precedence(left);
    int pr = precedence(right);
    int ret = (pbo < pl) || (pbo == pl && assoc(bo) != AS_LEFT);
    ret += 2 * ((pbo < pr) || (pbo == pr && assoc(bo) != AS_RIGHT));
    return static_cast<Parentheses>(ret);
  }
  
  std::string Printer::escapeStringLit(const ASTString& s) {
    const char* sc = s.c_str();
    std::string ret;
    for (unsigned int i=0; i<s.size(); i++) {
      switch (sc[i]) {
        case '\n':
          ret += "\\n";
          break;
        case '\t':
          ret += "\\t";
          break;
        case '"':
          ret += "\\\"";
          break;
        case '\'':
          ret += "\\\'";
          break;
        case '\\':
          ret += "\\\\";
          break;
        default:
          ret += sc[i];
      }
    }
    return ret;
  }
  
  void ppFloatVal(std::ostream& os, const FloatVal& fv, bool hexFloat) {
    std::ostringstream oss;
    if (fv.isFinite()) {
      if (hexFloat) {
        throw InternalError( "disabled due to hexfloat being not supported by g++ 4.9" );
//          std::hexfloat(oss);
        oss << fv.toDouble();
        os << oss.str();
      } else {
        oss << std::setprecision(std::numeric_limits<double>::digits10+1);
        oss << fv;
        if (oss.str().find("e") == std::string::npos && oss.str().find(".") == std::string::npos)
          oss << ".0";
        os << oss.str();
      }
    } else {
      if (fv.isPlusInfinity())
        os << "infinity";
      else
        os << "-infinity";
    }
  }
  
  class PlainPrinter {
  public:
    std::ostream& os;
    bool _flatZinc;
    PlainPrinter(std::ostream& os0, bool flatZinc) : os(os0), _flatZinc(flatZinc) {}

    void p(const Type& type, const Expression* e) {
      switch (type.ti()) {
      case Type::TI_PAR: break;
      case Type::TI_VAR: os << "var "; break;
      }
      if (type.ot()==Type::OT_OPTIONAL)
        os << "opt ";
      if (type.st()==Type::ST_SET)
        os << "set of ";
      if (e==NULL) {
        switch (type.bt()) {
        case Type::BT_INT: os << "int"; break;
        case Type::BT_BOOL: os << "bool"; break;
        case Type::BT_FLOAT: os << "float"; break;
        case Type::BT_STRING: os << "string"; break;
        case Type::BT_ANN: os << "ann"; break;
        case Type::BT_BOT: os << "bot"; break;
        case Type::BT_TOP: os << "top"; break;
        case Type::BT_UNKNOWN: os << "???"; break;
        }
      } else {
        p(e);
      }
    }
    
    void p(const Annotation& ann) {
      for (ExpressionSetIter it = ann.begin(); it != ann.end(); ++it) {
        os << ":: ";
        p(*it);
      }
    }
    
    void p(const Expression* e) {
      if (e==NULL)
        return;
      switch (e->eid()) {
      case Expression::E_INTLIT:
        os << e->cast<IntLit>()->v();
        break;
      case Expression::E_FLOATLIT:
        {
          ppFloatVal(os, e->cast<FloatLit>()->v());
        }
        break;
      case Expression::E_SETLIT:
        {
          const SetLit& sl = *e->cast<SetLit>();
          if (sl.isv()) {
            if (sl.isv()->size()==0) {
              os << (_flatZinc ? "1..0" : "{}");
            } else if (sl.isv()->size()==1) {
              os << sl.isv()->min(0) << ".." << sl.isv()->max(0);
            } else {
              if (!sl.isv()->min(0).isFinite())
                os << sl.isv()->min(0) << ".." << sl.isv()->max(0) << "++";
              os << "{";
              for (IntSetRanges isr(sl.isv()); isr();) {
                if (isr.min().isFinite() && isr.max().isFinite()) {
                  for (IntVal i=isr.min(); i<=isr.max(); i++) {
                    os << i;
                    if (i<isr.max())
                      os << ",";
                  }
                  ++isr;
                  if (isr())
                    os << ",";
                }
              }
              os << "}";
              if (!sl.isv()->max(sl.isv()->size()-1).isFinite())
                os << "++" << sl.isv()->min(sl.isv()->size()-1) << ".." << sl.isv()->max(sl.isv()->size()-1);
            }
          } else if (sl.fsv()) {
            if (sl.fsv()->size()==0) {
              os << (_flatZinc ? "1.0..0.0" : "{}");
            } else if (sl.fsv()->size()==1) {
              ppFloatVal(os, sl.fsv()->min(0));
              os << "..";
              ppFloatVal(os, sl.fsv()->max(0));
            } else {
              bool allSingleton = true;
              for (FloatSetRanges isr(sl.fsv()); isr(); ++isr) {
                if (isr.min() != isr.max()) {
                  allSingleton = false;
                  break;
                }
              }
              if (allSingleton) {
                os << "{";
                bool first = true;
                for (FloatSetRanges isr(sl.fsv()); isr(); ++isr) {
                  if (!first)
                    os << ",";
                  first = false;
                  ppFloatVal(os, isr.min());
                }
                os << "}";
              } else {
                bool first = true;
                for (FloatSetRanges isr(sl.fsv()); isr(); ++isr) {
                  if (!first)
                    os << "++";
                  first = false;
                  ppFloatVal(os, isr.min());
                  os << "..";
                  ppFloatVal(os, isr.max());
                }
              }
            }
          } else {
            os << "{";
            for (unsigned int i = 0; i < sl.v().size(); i++) {
              p(sl.v()[i]);
              if (i<sl.v().size()-1)
                os << ",";
            }
            os << "}";
          }
        }
        break;
      case Expression::E_BOOLLIT:
        os << (e->cast<BoolLit>()->v() ? "true" : "false");
        break;
      case Expression::E_STRINGLIT:
        os << "\"" << Printer::escapeStringLit(e->cast<StringLit>()->v()) << "\"";
        break;
      case Expression::E_ID:
        {
          if (e==constants().absent) {
            os << "<>";
          } else {
            const Id* id = e->cast<Id>();
            if (id->idn() == -1) {
              os << id->v();
            } else {
              os << "X_INTRODUCED_" << id->idn() << "_";
            }
          }
        }
        break;
      case Expression::E_TIID:
        os << "$" << e->cast<TIId>()->v();
        break;
      case Expression::E_ANON:
        os << "_";
        break;
      case Expression::E_ARRAYLIT:
        {
          const ArrayLit& al = *e->cast<ArrayLit>();
          int n = al.dims();
          if (n == 1 && al.min(0) == 1) {
            os << "[";
            for (unsigned int i = 0; i < al.v().size(); i++) {
              p(al.v()[i]);
              if (i<al.v().size()-1)
                os << ",";
            }
            os << "]";
          } else if (n == 2 && al.min(0) == 1 && al.min(1) == 1) {
            os << "[|";
            for (int i = 0; i < al.max(0); i++) {
              for (int j = 0; j < al.max(1); j++) {
                p(al.v()[i * al.max(1) + j]);
                if (j < al.max(1)-1)
                  os << ",";
              }
              if (i<al.max(0)-1)
                os << "|";
            }
            os << "|]";
          } else {
            os << "array" << n << "d(";
            for (int i = 0; i < al.dims(); i++) {
              os << al.min(i) << ".." << al.max(i);
              os << ",";
            }
            os << "[";
            for (unsigned int i = 0; i < al.v().size(); i++) {
              p(al.v()[i]);
              if (i<al.v().size()-1)
                os << ",";
            }
            os << "])";
          }
        }
        break;
      case Expression::E_ARRAYACCESS:
        {
          const ArrayAccess& aa = *e->cast<ArrayAccess>();
          p(aa.v());
          os << "[";
          for (unsigned int i = 0; i < aa.idx().size(); i++) {
            p(aa.idx()[i]);
            if (i<aa.idx().size()-1)
              os << ",";
          }
          os << "]";
        }
        break;
      case Expression::E_COMP:
        {
          const Comprehension& c = *e->cast<Comprehension>();
          os << (c.set() ? "{" : "[");
          p(c.e());
          os << " | ";
          for (int i=0; i<c.n_generators(); i++) {
            for (int j=0; j<c.n_decls(i); j++) {
              os << c.decl(i,j)->id()->v();
              if (j < c.n_decls(i)-1)
                os << ",";
            }
            os << " in ";
            p(c.in(i));
            if (i < c.n_generators())
              os << ", ";
          }
          if (c.where() != NULL) {
            os << " where ";
            p(c.where());
          }
          os << (c.set() ? "}" : "]");
        }
        break;
      case Expression::E_ITE:
        {
          const ITE& ite = *e->cast<ITE>();
          for (int i = 0; i < ite.size(); i++) {
            os << (i == 0 ? "if " : " elseif ");
            p(ite.e_if(i));
            os << " then ";
            p(ite.e_then(i));
          }
          os << " else ";
          p(ite.e_else());
          os << " endif";
        }
        break;
      case Expression::E_BINOP:
        {
          const BinOp& bo = *e->cast<BinOp>();
          Parentheses ps = needParens(&bo, bo.lhs(), bo.rhs());
          if (ps & PN_LEFT)
            os << "(";
          p(bo.lhs());
          if (ps & PN_LEFT)
            os << ")";
          switch (bo.op()) {
          case BOT_PLUS:
            os<<"+";
            break;
          case BOT_MINUS:
            os<<"-";
            break;
          case BOT_MULT:
            os<<"*";
            break;
          case BOT_DIV:
            os<<"/";
            break;
          case BOT_IDIV:
            os<<" div ";
            break;
          case BOT_MOD:
            os<<" mod ";
            break;
          case BOT_LE:
            os<<" < ";
            break;
          case BOT_LQ:
            os<<"<=";
            break;
          case BOT_GR:
            os<<" > ";
            break;
          case BOT_GQ:
            os<<">=";
            break;
          case BOT_EQ:
            os<<"==";
            break;
          case BOT_NQ:
            os<<"!=";
            break;
          case BOT_IN:
            os<<" in ";
            break;
          case BOT_SUBSET:
            os<<" subset ";
            break;
          case BOT_SUPERSET:
            os<<" superset ";
            break;
          case BOT_UNION:
            os<<" union ";
            break;
          case BOT_DIFF:
            os<<" diff ";
            break;
          case BOT_SYMDIFF:
            os<<" symdiff ";
            break;
          case BOT_INTERSECT:
            os<<" intersect ";
            break;
          case BOT_PLUSPLUS:
            os<<"++";
            break;
          case BOT_EQUIV:
            os<<" <-> ";
            break;
          case BOT_IMPL:
            os<<" -> ";
            break;
          case BOT_RIMPL:
            os<<" <- ";
            break;
          case BOT_OR:
            os<<" \\/ ";
            break;
          case BOT_AND:
            os<<" /\\ ";
            break;
          case BOT_XOR:
            os<<" xor ";
            break;
          case BOT_DOTDOT:
            os<<"..";
            break;
          default:
            assert(false);
            break;
          }

          if (ps & PN_RIGHT)
            os << "(";
          p(bo.rhs());
          if (ps & PN_RIGHT)
            os << ")";
        }
        break;
      case Expression::E_UNOP:
        {
          const UnOp& uo = *e->cast<UnOp>();
          switch (uo.op()) {
          case UOT_NOT:
            os << "not ";
            break;
          case UOT_PLUS:
            os << "+";
            break;
          case UOT_MINUS:
            os << "-";
            break;
          default:
            assert(false);
            break;
          }
          bool needParen = (uo.e()->isa<BinOp>() || uo.e()->isa<UnOp>() || !uo.ann().isEmpty());
          if (needParen)
            os << "(";
          p(uo.e());
          if (needParen)
            os << ")";
        }
        break;
      case Expression::E_CALL:
        {
          const Call& c = *e->cast<Call>();
          os << c.id() << "(";
          for (unsigned int i = 0; i < c.args().size(); i++) {
            p(c.args()[i]);
            if (i < c.args().size()-1)
              os << ",";
          }
          os << ")";
        }
        break;
      case Expression::E_VARDECL:
        {
          const VarDecl& vd = *e->cast<VarDecl>();
          p(vd.ti());
          if (vd.id()->idn() != -1) {
            os << ": X_INTRODUCED_" << vd.id()->idn() << "_";
          } else if (vd.id()->v().size() != 0)
            os << ": " << vd.id()->v();
          if (vd.introduced()) {
            os << " ::var_is_introduced ";
          }
          p(vd.ann());
          if (vd.e()) {
            os << " = ";
            p(vd.e());
          }
        }
        break;
      case Expression::E_LET:
        {
          const Let& l = *e->cast<Let>();
          os << "let {";

          for (unsigned int i = 0; i < l.let().size(); i++) {
            const Expression* li = l.let()[i];
            if (!li->isa<VarDecl>())
              os << "constraint ";
            p(li);
            if (i<l.let().size()-1)
              os << ", ";
          }
          os << "} in (";
          p(l.in());
          os << ")";
        }
        break;
      case Expression::E_TI:
        {
          const TypeInst& ti = *e->cast<TypeInst>();
          if (ti.isarray()) {
            os << "array [";
            for (unsigned int i = 0; i < ti.ranges().size(); i++) {
              p(Type::parint(), ti.ranges()[i]);
              if (i < ti.ranges().size()-1)
                os << ",";
            }
            os << "] of ";
          }
          p(ti.type(),ti.domain());
        }
      }
      if (!e->isa<VarDecl>()) {
        p(e->ann());
      }
    }

    void p(const Item* i) {
      if (i==NULL)
        return;
      if (i->removed())
        os << "% ";
      switch (i->iid()) {
      case Item::II_INC:
        os << "include \"" << i->cast<IncludeI>()->f() << "\"";
        break;
      case Item::II_VD:
        p(i->cast<VarDeclI>()->e());
        break;
      case Item::II_ASN:
        os << i->cast<AssignI>()->id() << " = ";
        p(i->cast<AssignI>()->e());
        break;
      case Item::II_CON:
        os << "constraint ";
        p(i->cast<ConstraintI>()->e());
        break;
      case Item::II_SOL:
        {
          const SolveI* si = i->cast<SolveI>();
          os << "solve ";
          p(si->ann());
          switch (si->st()) {
          case SolveI::ST_SAT:
            os << " satisfy";
            break;
          case SolveI::ST_MIN:
            os << " minimize ";
            p(si->e());
            break;
          case SolveI::ST_MAX:
            os << " maximize ";
            p(si->e());
            break;
          }
        }
        break;
      case Item::II_OUT:
        os << "output ";
        p(i->cast<OutputI>()->e());
        break;
      case Item::II_FUN:
        {
          const FunctionI& fi = *i->cast<FunctionI>();
          if (fi.ti()->type().isann() && fi.e() == NULL) {
            os << "annotation ";
          } else if (fi.ti()->type() == Type::parbool()) {
            os << "test ";
          } else if (fi.ti()->type() == Type::varbool()) {
            os << "predicate ";
          } else {
            os << "function ";
            p(fi.ti());
            os << " : ";
          }
          os << fi.id();
          if (fi.params().size() > 0) {
            os << "(";
            for (unsigned int i = 0; i < fi.params().size(); i++) {
              p(fi.params()[i]);
              if (i<fi.params().size()-1)
                os << ",";
            }
            os << ")";
          }
          p(fi.ann());
          if (fi.e()) {
            os << " = ";
            p(fi.e());
          }
        }
        break;
      }
      os << ";" << std::endl;
    }
  };




 

  template<class T>
  class ExpressionMapper {
  protected:
    T& _t;
  public:
    ExpressionMapper(T& t) :
      _t(t) {
    }
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
    int level;
  public:
    Document()  : level(0) {}
    virtual ~Document() {}
    int getLevel() { return level; }
    // Make this object a child of "d".
    virtual void setParent(Document* d) {
      level = d->level + 1;
    }
  };

  class BreakPoint: public Document {
  private:
    bool dontSimplify;
  public:
    BreakPoint() {
      dontSimplify = false;
    }
    BreakPoint(bool ds) {
      dontSimplify = ds;
    }
    virtual ~BreakPoint() {}
    void setDontSimplify(bool b) {
      dontSimplify = b;
    }
    bool getDontSimplify() {
      return dontSimplify;
    }
  };

  class StringDocument: public Document {
  private:
    std::string stringDocument;
  public:
    StringDocument() {}
    virtual ~StringDocument() {}

    StringDocument(std::string s)
      : stringDocument(s) {}

    std::string getString() {
      return stringDocument;
    }
    void setString(std::string s) {
      stringDocument = s;
    }
  };

  class DocumentList: public Document {
  private:
    std::vector<Document*> docs;
    std::string beginToken;
    std::string separator;
    std::string endToken;
    bool unbreakable;
    bool alignment;
  public:

    virtual ~DocumentList() {
      std::vector<Document*>::iterator it;
      for (it = docs.begin(); it != docs.end(); it++) {
        delete *it;
      }
    }
    DocumentList(std::string _beginToken = "", std::string _separator = "",
                 std::string _endToken = "", bool _alignment = true);

    void addDocumentToList(Document* d) {
      docs.push_back(d);
      d->setParent(this);
    }

    void setParent(Document* d) {
      Document::setParent(d);
      std::vector<Document*>::iterator it;
      for (it = docs.begin(); it != docs.end(); it++) {
        (*it)->setParent(this);
      }
    }

    void addStringToList(std::string s) {
      addDocumentToList(new StringDocument(s));
    }

    void addBreakPoint(bool b = false) {
      addDocumentToList(new BreakPoint(b));
    }

    std::vector<Document*> getDocs() {
      return docs;
    }

    void setList(std::vector<Document*> ld) {
      docs = ld;
    }

    std::string getBeginToken() {
      return beginToken;
    }

    std::string getEndToken() {
      return endToken;
    }

    std::string getSeparator() {
      return separator;
    }

    bool getUnbreakable() {
      return unbreakable;
    }

    void setUnbreakable(bool b) {
      unbreakable = b;
    }

    bool getAlignment() {
      return alignment;
    }

  };

  DocumentList::DocumentList(std::string _beginToken, std::string _separator, 
                             std::string _endToken, bool _alignment) {
    beginToken = _beginToken;
    separator = _separator;
    endToken = _endToken;
    alignment = _alignment;
    unbreakable = false;
  }

  class Line {
  private:
    int indentation;
    int lineLength;
    std::vector<std::string> text;

  public:
    Line()
      : indentation(0), lineLength(0), text(0) {}
    Line(const Line& l)
      : indentation(l.indentation), lineLength(l.lineLength), text(l.text) {}
    Line(const int indent)
      : indentation(indent), lineLength(0), text(0) {}
    bool operator==(const Line& l) {
      return &l == this;
    }

    void setIndentation(int i) {
      indentation = i;
    }

    int getLength() const {
      return lineLength;
    }
    int getIndentation() const {
      return indentation;
    }
    int getSpaceLeft(int maxwidth);
    void addString(const std::string& s);
    void concatenateLines(Line& l);

    void print(std::ostream& os) const {
      for (int i = 0; i < getIndentation(); i++) {
        os << " ";
      }
      std::vector<std::string>::const_iterator it;
      for (it = text.begin(); it != text.end(); it++) {
        os << (*it);
      }
      os << "\n";
    }
  };

  int
  Line::getSpaceLeft(int maxwidth) {
    return maxwidth - lineLength - indentation;
  }
  void
  Line::addString(const std::string& s) {
    lineLength += s.size();
    text.push_back(s);
  }
  void
  Line::concatenateLines(Line& l) {
    text.insert(text.end(), l.text.begin(), l.text.end());
    lineLength += l.lineLength;
  }

  class LinesToSimplify {
  private:
    std::map<int, std::vector<int> > lines;

    // (i,j) in parent <=> j can only be simplified if i is simplified
    std::vector<std::pair<int, int> > parent; 
    /*
     * if i can't simplify, remove j and his parents
     */
    //mostRecentlyAdded[level] = line of the most recently added
    std::map<int, int> mostRecentlyAdded; 
  public:
    std::vector<int>* getLinesForPriority(int p) {
      std::map<int, std::vector<int> >::iterator it;
      for (it = lines.begin(); it != lines.end(); it++) {
        if (it->first == p)
          return &(it->second);
      }
      return NULL;
    }
    void addLine(int p, int l, int par = -1) {
      if (par == -1) {
        for (int i = p - 1; i >= 0; i--) {
          std::map<int, int>::iterator it = mostRecentlyAdded.find(i);
          if (it != mostRecentlyAdded.end()) {
            par = it->second;
            break;
          }
        }
      }
      if (par != -1)
        parent.push_back(std::pair<int, int>(l, par));
      mostRecentlyAdded.insert(std::pair<int,int>(p,l));
      std::map<int, std::vector<int> >::iterator it;
      for (it = lines.begin(); it != lines.end(); it++) {
        if (it->first == p) {
          it->second.push_back(l);
          return;
        }
      }
      std::vector<int> v;
      v.push_back(l);
      lines.insert(std::pair<int, std::vector<int> >(p, v));
    }
    void decrementLine(std::vector<int>* vec, int l) {
      std::vector<int>::iterator vit;
      if (vec != NULL) {
        for (vit = vec->begin(); vit != vec->end(); vit++) {
          if (*vit >= l)
            *vit = *vit - 1;
        }
      }
      //Now the map
      std::map<int, std::vector<int> >::iterator it;
      for (it = lines.begin(); it != lines.end(); it++) {
        for (vit = it->second.begin(); vit != it->second.end(); vit++) {
          if (*vit >= l)
            *vit = *vit - 1;
        }
      }
      //And the parent table
      std::vector<std::pair<int, int> >::iterator vpit;
      for (vpit = parent.begin(); vpit != parent.end(); vpit++) {
        if (vpit->first >= l)
          vpit->first--;
        if (vpit->second >= l)
          vpit->second--;
      }
    }
    void remove(LinesToSimplify& lts){
      std::map<int, std::vector<int> >::iterator it;
      for(it = lts.lines.begin(); it != lts.lines.end(); it++){
        std::vector<int>::iterator vit;
        for(vit = it->second.begin(); vit != it->second.end(); vit++){
          remove(NULL, *vit, false);
        }
      }
    }
    void remove(std::vector<int>* v, int i, bool success = true) {
      if (v != NULL) {
        v->erase(std::remove(v->begin(), v->end(), i), v->end());
      }
      std::map<int, std::vector<int> >::iterator it;
      for (it = lines.begin(); it != lines.end(); it++) {
        std::vector<int>* v = &(it->second);
        v->erase(std::remove(v->begin(), v->end(), i), v->end());
      }
      //Call on its parent
      if (!success) {
        std::vector<std::pair<int, int> >::iterator vpit;
        for (vpit = parent.begin(); vpit != parent.end(); vpit++) {
          if (vpit->first == i && vpit->second != i
              && vpit->second != -1) {
            remove(v, vpit->second, false);
          }
        }
      }
    }
    std::vector<int>* getLinesToSimplify() {
      std::vector<int>* vec = new std::vector<int>();
      std::map<int, std::vector<int> >::iterator it;
      for (it = lines.begin(); it != lines.end(); it++) {
        std::vector<int>& svec = it->second;
        vec->insert(vec->begin(), svec.begin(), svec.end());
      }
      return vec;
    }
  };

  Document* expressionToDocument(const Expression* e);
  Document* annotationToDocument(const Annotation& ann);
  Document* tiexpressionToDocument(const Type& type, const Expression* e) {
    DocumentList* dl = new DocumentList("","","",false);
    switch (type.ti()) {
    case Type::TI_PAR: break;
    case Type::TI_VAR: dl->addStringToList("var "); break;
    }
    if (type.ot()==Type::OT_OPTIONAL)
      dl->addStringToList("opt ");
    if (type.st()==Type::ST_SET)
      dl->addStringToList("set of ");
    if (e==NULL) {
      switch (type.bt()) {
      case Type::BT_INT: dl->addStringToList("int"); break;
      case Type::BT_BOOL: dl->addStringToList("bool"); break;
      case Type::BT_FLOAT: dl->addStringToList("float"); break;
      case Type::BT_STRING: dl->addStringToList("string"); break;
      case Type::BT_ANN: dl->addStringToList("ann"); break;
      case Type::BT_BOT: dl->addStringToList("bot"); break;
      case Type::BT_TOP: dl->addStringToList("top"); break;
      case Type::BT_UNKNOWN: dl->addStringToList("???"); break;
      }
    } else {
      dl->addDocumentToList(expressionToDocument(e));
    }
    return dl;
  }

  class ExpressionDocumentMapper {
  public:
    typedef Document* ret;
    ret mapIntLit(const IntLit& il) {
      std::ostringstream oss;
      oss << il.v();
      return new StringDocument(oss.str());
    }
    ret mapFloatLit(const FloatLit& fl) {
      std::ostringstream oss;
      ppFloatVal(oss, fl.v());
      return new StringDocument(oss.str());
    }
    ret mapSetLit(const SetLit& sl) {
      DocumentList* dl;
      if (sl.isv()) {
        if (sl.isv()->size()==0) {
          dl = new DocumentList("1..0","","");
        } else if (sl.isv()->size()==1) {
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
      } else if (sl.fsv()) {
        if (sl.fsv()->size()==0) {
          dl = new DocumentList("1.0..0.0","","");
        } else if (sl.fsv()->size()==1) {
          dl = new DocumentList("", "..", "");
          {
            std::ostringstream oss;
            ppFloatVal(oss, sl.fsv()->min(0));
            dl->addDocumentToList(new StringDocument(oss.str()));
          }
          {
            std::ostringstream oss;
            ppFloatVal(oss, sl.fsv()->max(0));
            dl->addDocumentToList(new StringDocument(oss.str()));
          }
        } else {
          dl = new DocumentList("", "++", "", true);
          FloatSetRanges fsr(sl.fsv());
          for (; fsr(); ++fsr) {
            std::ostringstream oss;
            ppFloatVal(oss, fsr.min());
            oss << "..";
            ppFloatVal(oss, fsr.max());
            dl->addDocumentToList(new StringDocument(oss.str()));
          }
        }

      } else {
        dl = new DocumentList("{", ", ", "}", true);
        for (unsigned int i = 0; i < sl.v().size(); i++) {
          dl->addDocumentToList(expressionToDocument((sl.v()[i])));
        }
      }
      return dl;
    }
    ret mapBoolLit(const BoolLit& bl) {
      return new StringDocument(std::string(bl.v() ? "true" : "false"));
    }
    ret mapStringLit(const StringLit& sl) {
      std::ostringstream oss;
      oss << "\"" << Printer::escapeStringLit(sl.v()) << "\"";
      return new StringDocument(oss.str());


    }
    ret mapId(const Id& id) {
      if (&id == constants().absent)
        return new StringDocument("<>");
      if (id.idn()==-1)
        return new StringDocument(id.v().str());
      else {
        std::ostringstream oss;
        oss << "X_INTRODUCED_" << id.idn() << "_";
        return new StringDocument(oss.str());
      }
    }
    ret mapTIId(const TIId& id) {
      return new StringDocument("$"+id.v().str());
    }
    ret mapAnonVar(const AnonVar&) {
      return new StringDocument("_");
    }
    ret mapArrayLit(const ArrayLit& al) {
      /// TODO: test multi-dimensional arrays handling
      DocumentList* dl;
      int n = al.dims();
      if (n == 1 && al.min(0) == 1) {
        dl = new DocumentList("[", ", ", "]");
        for (unsigned int i = 0; i < al.v().size(); i++)
          dl->addDocumentToList(expressionToDocument(al.v()[i]));
      } else if (n == 2 && al.min(0) == 1 && al.min(1) == 1) {
        dl = new DocumentList("[| ", " | ", " |]");
        for (int i = 0; i < al.max(0); i++) {
          DocumentList* row = new DocumentList("", ", ", "");
          for (int j = 0; j < al.max(1); j++) {
            row->
              addDocumentToList(expressionToDocument(al.v()[i * al.max(1) + j]));
          }
          dl->addDocumentToList(row);
          if (i != al.max(0) - 1)
            dl->addBreakPoint(true); // dont simplify
        }
      } else {
        dl = new DocumentList("", "", "");
        std::stringstream oss;
        oss << "array" << n << "d";
        dl->addStringToList(oss.str());
        DocumentList* args = new DocumentList("(", ", ", ")");

        for (int i = 0; i < al.dims(); i++) {
          oss.str("");
          oss << al.min(i) << ".." << al.max(i);
          args->addStringToList(oss.str());
        }
        DocumentList* array = new DocumentList("[", ", ", "]");
        for (unsigned int i = 0; i < al.v().size(); i++)
          array->addDocumentToList(expressionToDocument(al.v()[i]));
        args->addDocumentToList(array);
        dl->addDocumentToList(args);
      }
      return dl;
    }
    ret mapArrayAccess(const ArrayAccess& aa) {
      DocumentList* dl = new DocumentList("", "", "");

      dl->addDocumentToList(expressionToDocument(aa.v()));
      DocumentList* args = new DocumentList("[", ", ", "]");
      for (unsigned int i = 0; i < aa.idx().size(); i++) {
        args->addDocumentToList(expressionToDocument(aa.idx()[i]));
      }
      dl->addDocumentToList(args);
      return dl;
    }
    ret mapComprehension(const Comprehension& c) {
      std::ostringstream oss;
      DocumentList* dl;
      if (c.set())
        dl = new DocumentList("{ ", " | ", " }");
      else
        dl = new DocumentList("[ ", " | ", " ]");
      dl->addDocumentToList(expressionToDocument(c.e()));
      DocumentList* head = new DocumentList("", " ", "");
      DocumentList* generators = new DocumentList("", ", ", "");
      for (int i = 0; i < c.n_generators(); i++) {
        DocumentList* gen = new DocumentList("", "", "");
        DocumentList* idents = new DocumentList("", ", ", "");
        for (int j = 0; j < c.n_decls(i); j++) {
          idents->addStringToList(c.decl(i, j)->id()->v().str());
        }
        gen->addDocumentToList(idents);
        gen->addStringToList(" in ");
        gen->addDocumentToList(expressionToDocument(c.in(i)));
        generators->addDocumentToList(gen);
      }
      head->addDocumentToList(generators);
      if (c.where() != NULL) {
        head->addStringToList("where");
        head->addDocumentToList(expressionToDocument(c.where()));
      }
      dl->addDocumentToList(head);

      return dl;
    }
    ret mapITE(const ITE& ite) {

      DocumentList* dl = new DocumentList("", "", "");
      for (int i = 0; i < ite.size(); i++) {
        std::string beg = (i == 0 ? "if " : " elseif ");
        dl->addStringToList(beg);
        dl->addDocumentToList(expressionToDocument(ite.e_if(i)));
        dl->addStringToList(" then ");

        DocumentList* ifdoc = new DocumentList("", "", "", false);
        ifdoc->addBreakPoint();
        ifdoc->addDocumentToList(expressionToDocument(ite.e_then(i)));
        dl->addDocumentToList(ifdoc);
        dl->addStringToList(" ");
      }
      dl->addBreakPoint();
      dl->addStringToList("else ");

      DocumentList* elsedoc = new DocumentList("", "", "", false);
      elsedoc->addBreakPoint();
      elsedoc->addDocumentToList(expressionToDocument(ite.e_else()));
      dl->addDocumentToList(elsedoc);
      dl->addStringToList(" ");
      dl->addBreakPoint();
      dl->addStringToList("endif");

      return dl;
    }
    ret mapBinOp(const BinOp& bo) {
      Parentheses ps = needParens(&bo, bo.lhs(), bo.rhs());
      DocumentList* opLeft;
      DocumentList* dl;
      DocumentList* opRight;
      bool linebreak = false;
      if (ps & PN_LEFT)
        opLeft = new DocumentList("(", " ", ")");
      else
        opLeft = new DocumentList("", " ", "");
      opLeft->addDocumentToList(expressionToDocument(bo.lhs()));
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

      if (ps & PN_RIGHT)
        opRight = new DocumentList("(", " ", ")");
      else
        opRight = new DocumentList("", "", "");
      opRight->addDocumentToList(expressionToDocument(bo.rhs()));
      dl->addDocumentToList(opLeft);
      if (linebreak)
        dl->addBreakPoint();
      dl->addDocumentToList(opRight);

      return dl;
    }
    ret mapUnOp(const UnOp& uo) {
      DocumentList* dl = new DocumentList("", "", "");
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
      if (needParen)
        unop = new DocumentList("(", " ", ")");
      else
        unop = new DocumentList("", " ", "");

      unop->addDocumentToList(expressionToDocument(uo.e()));
      dl->addDocumentToList(unop);
      return dl;
    }
    ret mapCall(const Call& c) {
      if (c.args().size() == 1) {
        /*
         * if we have only one argument, and this is an array comprehension,
         * we convert it into the following syntax
         * forall (f(i,j) | i in 1..10)
         * -->
         * forall (i in 1..10) (f(i,j))
         */

        const Expression* e = c.args()[0];
        if (e->isa<Comprehension>()) {
          const Comprehension* com = e->cast<Comprehension>();
          if (!com->set()) {
            DocumentList* dl = new DocumentList("", " ", "");
            dl->addStringToList(c.id().str());
            DocumentList* args = new DocumentList("", " ", "", false);
            DocumentList* generators = new DocumentList("", ", ", "");

            for (int i = 0; i < com->n_generators(); i++) {
              DocumentList* gen = new DocumentList("", "", "");
              DocumentList* idents = new DocumentList("", ", ", "");
              for (int j = 0; j<com->n_decls(i); j++) {
                idents->addStringToList(
                  com->decl(i,j)->id()->v().str());
              }
              gen->addDocumentToList(idents);
              gen->addStringToList(" in ");
              gen->addDocumentToList(expressionToDocument(com->in(i)));
              generators->addDocumentToList(gen);
            }

            args->addStringToList("(");
            args->addDocumentToList(generators);
            if (com->where() != NULL) {
              args->addStringToList("where");
              args->addDocumentToList(expressionToDocument(com->where()));
            }
            args->addStringToList(")");

            args->addStringToList("(");
            args->addBreakPoint();
            args->addDocumentToList(expressionToDocument(com->e()));

            dl->addDocumentToList(args);
            dl->addBreakPoint();
            dl->addStringToList(")");

            return dl;
          }
        }

      }
      std::string beg = c.id().str() + "(";
      DocumentList* dl = new DocumentList(beg, ", ", ")");
      for (unsigned int i = 0; i < c.args().size(); i++) {
        dl->addDocumentToList(expressionToDocument(c.args()[i]));
      }
      return dl;


    }
    ret mapVarDecl(const VarDecl& vd) {
      std::ostringstream oss;
      DocumentList* dl = new DocumentList("", "", "");
      dl->addDocumentToList(expressionToDocument(vd.ti()));
      dl->addStringToList(": ");
      if (vd.id()->idn()==-1) {
        dl->addStringToList(vd.id()->v().str());
      } else {
        std::ostringstream oss;
        oss << "X_INTRODUCED_" << vd.id()->idn() << "_";
        dl->addStringToList(oss.str());
      }
        
      if (vd.introduced()) {
        dl->addStringToList(" ::var_is_introduced ");
      }
      if (!vd.ann().isEmpty()) {
        dl->addDocumentToList(annotationToDocument(vd.ann()));
      }
      if (vd.e()) {
        dl->addStringToList(" = ");
        dl->addDocumentToList(expressionToDocument(vd.e()));
      }
      return dl;
    }
    ret mapLet(const Let& l) {
      DocumentList* letin = new DocumentList("", "", "", false);
      DocumentList* lets = new DocumentList("", " ", "", true);
      DocumentList* inexpr = new DocumentList("", "", "");
      bool ds = l.let().size() > 1;

      for (unsigned int i = 0; i < l.let().size(); i++) {
        if (i != 0)
          lets->addBreakPoint(ds);
        DocumentList* exp = new DocumentList("", " ", ",");
        const Expression* li = l.let()[i];
        if (!li->isa<VarDecl>())
          exp->addStringToList("constraint");
        exp->addDocumentToList(expressionToDocument(li));
        lets->addDocumentToList(exp);
      }

      inexpr->addDocumentToList(expressionToDocument(l.in()));
      letin->addBreakPoint(ds);
      letin->addDocumentToList(lets);

      DocumentList* letin2 = new DocumentList("", "", "", false);

      letin2->addBreakPoint();
      letin2->addDocumentToList(inexpr);

      DocumentList* dl = new DocumentList("", "", "");
      dl->addStringToList("let {");
      dl->addDocumentToList(letin);
      dl->addBreakPoint(ds);
      dl->addStringToList("} in (");
      dl->addDocumentToList(letin2);
      //dl->addBreakPoint();
      dl->addStringToList(")");
      return dl;
    }
    ret mapTypeInst(const TypeInst& ti) {
      DocumentList* dl = new DocumentList("", "", "");
      if (ti.isarray()) {
        dl->addStringToList("array [");
        DocumentList* ran = new DocumentList("", ", ", "");
        for (unsigned int i = 0; i < ti.ranges().size(); i++) {
          ran->addDocumentToList(tiexpressionToDocument(Type::parint(), ti.ranges()[i]));
        }
        dl->addDocumentToList(ran);
        dl->addStringToList("] of ");
      }
      dl->addDocumentToList(tiexpressionToDocument(ti.type(),ti.domain()));
      return dl;
    }
  };

  Document* annotationToDocument(const Annotation& ann) {
    DocumentList* dl = new DocumentList(" :: ", " :: ", "");
    for (ExpressionSetIter it = ann.begin(); it != ann.end(); ++it) {
      dl->addDocumentToList(expressionToDocument(*it));
    }
    return dl;
  }
  
  Document* expressionToDocument(const Expression* e) {
    if (e==NULL) return new StringDocument("NULL");
    ExpressionDocumentMapper esm;
    ExpressionMapper<ExpressionDocumentMapper> em(esm);
    DocumentList* dl = new DocumentList("", "", "");
    Document* s = em.map(e);
    dl->addDocumentToList(s);
    if (!e->isa<VarDecl>() && !e->ann().isEmpty()) {
      dl->addDocumentToList(annotationToDocument(e->ann()));
    }
    return dl;
  }


  class ItemDocumentMapper {
  public:
    typedef Document* ret;
    ret mapIncludeI(const IncludeI& ii) {
      std::ostringstream oss;
      oss << "include \"" << ii.f() << "\";";
      return new StringDocument(oss.str());
    }
    ret mapVarDeclI(const VarDeclI& vi) {
      DocumentList* dl = new DocumentList("", " ", ";");
      dl->addDocumentToList(expressionToDocument(vi.e()));
      return dl;
    }
    ret mapAssignI(const AssignI& ai) {
      DocumentList* dl = new DocumentList("", " = ", ";");
      dl->addStringToList(ai.id().str());
      dl->addDocumentToList(expressionToDocument(ai.e()));
      return dl;
    }
    ret mapConstraintI(const ConstraintI& ci) {
      DocumentList* dl = new DocumentList("constraint ", " ", ";");
      dl->addDocumentToList(expressionToDocument(ci.e()));
      return dl;
    }
    ret mapSolveI(const SolveI& si) {
      DocumentList* dl = new DocumentList("", "", ";");
      dl->addStringToList("solve");
      if (!si.ann().isEmpty())
        dl->addDocumentToList(annotationToDocument(si.ann()));
      switch (si.st()) {
      case SolveI::ST_SAT:
        dl->addStringToList(" satisfy");
        break;
      case SolveI::ST_MIN:
        dl->addStringToList(" minimize ");
        dl->addDocumentToList(expressionToDocument(si.e()));
        break;
      case SolveI::ST_MAX:
        dl->addStringToList(" maximize ");
        dl->addDocumentToList(expressionToDocument(si.e()));
        break;
      }
      return dl;
    }
    ret mapOutputI(const OutputI& oi) {
      DocumentList* dl = new DocumentList("output ", " ", ";");
      dl->addDocumentToList(expressionToDocument(oi.e()));
      return dl;
    }
    ret mapFunctionI(const FunctionI& fi) {
      DocumentList* dl;
      if (fi.ti()->type().isann() && fi.e() == NULL) {
        dl = new DocumentList("annotation ", " ", ";", false);
      } else if (fi.ti()->type() == Type::parbool()) {
        dl = new DocumentList("test ", "", ";", false);
      } else if (fi.ti()->type() == Type::varbool()) {
        dl = new DocumentList("predicate ", "", ";", false);
      } else {
        dl = new DocumentList("function ", "", ";", false);
        dl->addDocumentToList(expressionToDocument(fi.ti()));
        dl->addStringToList(": ");
      }
      dl->addStringToList(fi.id().str());
      if (fi.params().size() > 0) {
        DocumentList* params = new DocumentList("(", ", ", ")");
        for (unsigned int i = 0; i < fi.params().size(); i++) {
          DocumentList* par = new DocumentList("", "", "");
          par->setUnbreakable(true);
          par->addDocumentToList(expressionToDocument(fi.params()[i]));
          params->addDocumentToList(par);
        }
        dl->addDocumentToList(params);
      }
      if (!fi.ann().isEmpty()) {
        dl->addDocumentToList(annotationToDocument(fi.ann()));
      }
      if (fi.e()) {
        dl->addStringToList(" = ");
        dl->addBreakPoint();
        dl->addDocumentToList(expressionToDocument(fi.e()));
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
    PrettyPrinter(int _maxwidth = 80, int _indentationBase = 4,
                  bool sim = false, bool deepSimp = false);

    void print(Document* d);
    void print(std::ostream& os) const;

  private:
    int maxwidth;
    int indentationBase;
    int currentLine;
    int currentItem;
    std::vector<std::vector<Line> > items;
    std::vector<LinesToSimplify> linesToSimplify;
    std::vector<LinesToSimplify> linesNotToSimplify;
    bool simp;
    bool deeplySimp;

    void addItem();

    void addLine(int indentation, bool bp = false,
                 bool ds = false, int level = 0);
    static std::string printSpaces(int n);
    const std::vector<Line>& getCurrentItemLines() const;

    void printDocument(Document* d, bool alignment, int startColAlignment,
                       const std::string& before = "",
                       const std::string& after = "");
    void printDocList(DocumentList* d, int startColAlignment,
                      const std::string& before = "",
                      const std::string& after = "");
    void printStringDoc(StringDocument* d, bool alignment,
                        int startColAlignment, const std::string& before = "",
                        const std::string& after = "");
    void printString(const std::string& s, bool alignment,
                     int startColAlignment);
    bool simplify(int item, int line, std::vector<int>* vec);
    void simplifyItem(int item);
  };

  void PrettyPrinter::print(Document* d) {
    addItem();
    addLine(0);
    printDocument(d, true, 0);
    if (simp)
      simplifyItem(currentItem);
  }

  PrettyPrinter::PrettyPrinter(int _maxwidth, int _indentationBase, bool sim,
                               bool deepsim) {
    maxwidth = _maxwidth;
    indentationBase = _indentationBase;
    currentLine = -1;
    currentItem = -1;

    simp = sim;
    deeplySimp = deepsim;
  }
  const std::vector<Line>& PrettyPrinter::getCurrentItemLines() const {
    return items[currentItem];
  }

  void PrettyPrinter::addLine(int indentation, bool bp, bool simpl, int level) {
    items[currentItem].push_back(Line(indentation));
    currentLine++;
    if (bp && deeplySimp) {
      linesToSimplify[currentItem].addLine(level, currentLine);
      if (!simpl)
        linesNotToSimplify[currentItem].addLine(0, currentLine);
    }
  }
  void PrettyPrinter::addItem() {
    items.push_back(std::vector<Line>());
    linesToSimplify.push_back(LinesToSimplify());
    linesNotToSimplify.push_back(LinesToSimplify());
    currentItem++;
    currentLine = -1;
  }

  void
  PrettyPrinter::print(std::ostream& os) const {
    std::vector<Line>::const_iterator it;
    int nItems = items.size();
    for (int item = 0; item < nItems; item++) {
      for (it = items[item].begin(); it != items[item].end(); it++) {
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

  void PrettyPrinter::printDocument(Document* d, bool alignment,
                                    int alignmentCol,
                                    const std::string& before,
                                    const std::string& after) {
    if (DocumentList* dl = dynamic_cast<DocumentList*>(d)) {
      printDocList(dl, alignmentCol, before, after);
    } else if (StringDocument* sd = dynamic_cast<StringDocument*>(d)) {
      printStringDoc(sd, alignment, alignmentCol, before, after);
    } else if (BreakPoint* bp = dynamic_cast<BreakPoint*>(d)) {
      printString(before, alignment, alignmentCol);
      addLine(alignmentCol, deeplySimp, !bp->getDontSimplify(),
          d->getLevel());
      printString(after, alignment, alignmentCol);
    } else {
      throw InternalError("PrettyPrinter::print : Wrong type of document");
    }
  }

  void PrettyPrinter::printStringDoc(StringDocument* d, bool alignment,
                                     int alignmentCol,
                                     const std::string& before,
                                     const std::string& after) {
    std::string s;
    if (d != NULL)
      s = d->getString();
    s = before + s + after;
    printString(s, alignment, alignmentCol);
  }

  void PrettyPrinter::printString(const std::string& s, bool alignment,
                                  int alignmentCol) {
    Line& l = items[currentItem][currentLine];
    int size = s.size();
    if (size <= l.getSpaceLeft(maxwidth)) {
      l.addString(s);
    } else {
      int col =
          alignment && maxwidth - alignmentCol >= size ?
              alignmentCol : indentationBase;
      addLine(col);
      items[currentItem][currentLine].addString(s);
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
    int currentCol = items[currentItem][currentLine].getIndentation()
        + items[currentItem][currentLine].getLength();
    int newAlignmentCol =
        _alignment ? currentCol + beginToken.size() : alignmentCol;
    int vectorSize = ld.size();
    int lastVisibleElementIndex;
    for (int i = 0; i < vectorSize; i++) {
      if (!dynamic_cast<BreakPoint*>(ld[i]))
        lastVisibleElementIndex = i;
    }
    if (vectorSize == 0) {
      printStringDoc(NULL, true, newAlignmentCol, super_before + beginToken,
          endToken + super_after);
    }
    for (int i = 0; i < vectorSize; i++) {
      Document* subdoc = ld[i];
      bool bp = false;
      if (dynamic_cast<BreakPoint*>(subdoc)) {
        if (!_alignment)
          newAlignmentCol += indentationBase;
        bp = true;
      }
      std::string af, be;
      if (i != vectorSize - 1) {
        if (bp || lastVisibleElementIndex <= i)
          af = "";
        else
          af = separator;
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
      simplify(currentItem, currentLine, NULL);
    }

  }
  void PrettyPrinter::simplifyItem(int item) {
    linesToSimplify[item].remove(linesNotToSimplify[item]);
    std::vector<int>* vec = (linesToSimplify[item].getLinesToSimplify());
    while (!vec->empty()) {
      if (!simplify(item, (*vec)[0], vec))
        break;
    }
    delete vec;
  }

  bool PrettyPrinter::simplify(int item, int line, std::vector<int>* vec) {

    if (line == 0) {
      linesToSimplify[item].remove(vec, line, false);
      return false;
    }
    if (items[item][line].getLength()
        > items[item][line - 1].getSpaceLeft(maxwidth)) {
      linesToSimplify[item].remove(vec, line, false);
      return false;
    } else {
      linesToSimplify[item].remove(vec, line, true);
      items[item][line - 1].concatenateLines(items[item][line]);
      items[item].erase(items[item].begin() + line);

      linesToSimplify[item].decrementLine(vec, line);
      currentLine--;
    }

    return true;
  }

  Printer::Printer(std::ostream& os, int width, bool flatZinc)
  : ism(NULL), printer(NULL), _os(os), _width(width), _flatZinc(flatZinc) {}
  void
  Printer::init(void) {
    if (ism==NULL) {
      ism = new ItemDocumentMapper();
      printer =  new PrettyPrinter(_width, 4, true, true);
    }
  }
  Printer::~Printer(void) {
    delete printer;
    delete ism;
  }

  void
  Printer::p(Document* d) {
    printer->print(d);
    printer->print(_os);
    delete printer;
    printer = new PrettyPrinter(_width,4,true,true);
  }
  void
  Printer::p(const Item* i) {
    Document* d;
    switch (i->iid()) {
    case Item::II_INC:
      d = ism->mapIncludeI(*i->cast<IncludeI>());
      break;
    case Item::II_VD:
      d = ism->mapVarDeclI(*i->cast<VarDeclI>());
      break;
    case Item::II_ASN:
      d = ism->mapAssignI(*i->cast<AssignI>());
      break;
    case Item::II_CON:
      d = ism->mapConstraintI(*i->cast<ConstraintI>());
      break;
    case Item::II_SOL:
      d = ism->mapSolveI(*i->cast<SolveI>());
      break;
    case Item::II_OUT:
      d = ism->mapOutputI(*i->cast<OutputI>());
      break;
    case Item::II_FUN:
      d = ism->mapFunctionI(*i->cast<FunctionI>());
      break;
    }
    p(d);
    delete d;
  }

  void
  Printer::print(const Expression* e) {
    if (_width==0) {
      PlainPrinter p(_os,_flatZinc); p.p(e);
    } else {
      init();
      Document* d = expressionToDocument(e);
      p(d);
      delete d;
    }
  }
  void
  Printer::print(const Item* i) {
    if (_width==0) {
      PlainPrinter p(_os,_flatZinc); p.p(i);
    } else {
      init();
      p(i);
    }
  }
  void
  Printer::print(const Model* m) {
    if (_width==0) {
      PlainPrinter p(_os,_flatZinc);
      for (unsigned int i = 0; i < m->size(); i++) {
        p.p((*m)[i]);
      }
    } else {
      init();
      for (unsigned int i = 0; i < m->size(); i++) {
        p((*m)[i]);
      }
    }
  }

}

void debugprint(MiniZinc::Expression* e) {
  std::cerr << *e << "\n";
}
void debugprint(MiniZinc::Item* i) {
  std::cerr << *i;
}
void debugprint(MiniZinc::Model* m) {
  MiniZinc::Printer p(std::cerr,0); p.print(m);
}
void debugprint(const MiniZinc::Location& loc) {
  std::cerr << loc << std::endl;
}
