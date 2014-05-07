/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/hash.hh>
#include <minizinc/exception.hh>
#include <minizinc/iter.hh>
#include <minizinc/model.hh>

#include <minizinc/prettyprinter.hh>

namespace MiniZinc {

  Location::Location(void)
  : first_line(0),
    first_column(0),
    last_line(0),
    last_column(0) {}

  std::string
  Location::toString(void) const {
    std::ostringstream oss;
    oss << filename << ":" << first_line << "." << first_column;
    return oss.str();
  }

  void
  Location::mark(void) const {
    filename.mark();
  }

  void
  Expression::addAnnotation(Expression* ann) {
    _ann.add(ann);
  }
  void
  Expression::addAnnotations(std::vector<Expression*> ann) {
    for (unsigned int i=0; i<ann.size(); i++)
      _ann.add(ann[i]);
  }

#define pushstack(e) do { if (e!=NULL) { stack.push_back(e); }} while(0)
#define pushall(v) do { v.mark(); for (unsigned int i=0; i<v.size(); i++) if (v[i]!=NULL) { stack.push_back(v[i]); }} while(0)
#define pushann(a) do { for (ExpressionSetIter it = a.begin(); it != a.end(); ++it) { pushstack(*it); }} while(0)
  void
  Expression::mark(Expression* e) {
    if (e==NULL) return;
    std::vector<const Expression*> stack;
    stack.push_back(e);
    while (!stack.empty()) {
      const Expression* cur = stack.back(); stack.pop_back();
      if (cur->_gc_mark==0) {
        cur->_gc_mark = 1;
        cur->loc().mark();
        pushann(cur->ann());
        switch (cur->eid()) {
        case Expression::E_INTLIT:
        case Expression::E_FLOATLIT:
        case Expression::E_BOOLLIT:
        case Expression::E_ANON:
          break;
        case Expression::E_SETLIT:
          if (cur->cast<SetLit>()->isv())
            cur->cast<SetLit>()->isv()->mark();
          else
            pushall(cur->cast<SetLit>()->v());
          break;
        case Expression::E_STRINGLIT:
          cur->cast<StringLit>()->v().mark();
          break;
        case Expression::E_ID:
          if (cur->cast<Id>()->idn()==-1)
            cur->cast<Id>()->v().mark();
          pushstack(cur->cast<Id>()->decl());
          break;
        case Expression::E_ARRAYLIT:
          pushall(cur->cast<ArrayLit>()->v());
          cur->cast<ArrayLit>()->_dims.mark();
          break;
        case Expression::E_ARRAYACCESS:
          pushstack(cur->cast<ArrayAccess>()->v());
          pushall(cur->cast<ArrayAccess>()->idx());
          break;
        case Expression::E_COMP:
          pushstack(cur->cast<Comprehension>()->_e);
          pushstack(cur->cast<Comprehension>()->_where);
          pushall(cur->cast<Comprehension>()->_g);
          cur->cast<Comprehension>()->_g_idx.mark();
          break;
        case Expression::E_ITE:
          pushstack(cur->cast<ITE>()->e_else());
          pushall(cur->cast<ITE>()->_e_if_then);
          break;
        case Expression::E_BINOP:
          pushstack(cur->cast<BinOp>()->lhs());
          pushstack(cur->cast<BinOp>()->rhs());
          break;
        case Expression::E_UNOP:
          pushstack(cur->cast<UnOp>()->e());
          break;
        case Expression::E_CALL:
          cur->cast<Call>()->id().mark();
          pushall(cur->cast<Call>()->_args);
          if (FunctionI* fi = cur->cast<Call>()->_decl) {
            fi->id().mark();
            pushstack(fi->ti());
            pushann(fi->ann());
            pushstack(fi->e());
            pushall(fi->params());
          }
          break;
        case Expression::E_VARDECL:
          pushstack(cur->cast<VarDecl>()->ti());
          pushstack(cur->cast<VarDecl>()->e());
          pushstack(cur->cast<VarDecl>()->id());
          break;
        case Expression::E_LET:
          pushall(cur->cast<Let>()->let());
          pushstack(cur->cast<Let>()->in());
          break;
        case Expression::E_TI:
          pushstack(cur->cast<TypeInst>()->domain());
          pushall(cur->cast<TypeInst>()->ranges());
          break;
        case Expression::E_TIID:
          cur->cast<TIId>()->v().mark();
          break;
        }
      }
    }
  }
#undef pushstack
#undef pushall

  void
  IntLit::rehash(void) {
    init_hash();
    std::hash<IntVal> h;
    cmb_hash(h(_v));
  }

  void
  FloatLit::rehash(void) {
    init_hash();
    std::hash<FloatVal> h;
    cmb_hash(h(_v));
  }

  void
  SetLit::rehash(void) {
    init_hash();
    if (isv()) {
      std::hash<IntVal> h;
      for (IntSetRanges r0(isv()); r0(); ++r0) {
        cmb_hash(h(r0.min()));
        cmb_hash(h(r0.max()));
      }
    } else {
      for (unsigned int i=v().size(); i--;)
        cmb_hash(Expression::hash(_v[i]));
    }
  }

  void
  BoolLit::rehash(void) {
    init_hash();
    std::hash<bool> h;
    cmb_hash(h(_v));
  }

  void
  StringLit::rehash(void) {
    init_hash();
    cmb_hash(_v.hash());
  }

  void
  Id::rehash(void) {
    init_hash();
    std::hash<long long int> h;
    if (idn()==-1)
      cmb_hash(v().hash());
    else
      cmb_hash(h(idn()));
  }

  ASTString
  Id::str() const {
    if (idn()==-1)
      return v();
    std::ostringstream oss;
    oss << "X_INTRODUCED" << idn();
    return oss.str();
  }
  
  void
  TIId::rehash(void) {
    init_hash();
    cmb_hash(_v.hash());
  }

  void
  AnonVar::rehash(void) {
    init_hash();
  }

  void
  ArrayLit::rehash(void) {
    init_hash();
    std::hash<int> h;
    for (unsigned int i=0; i<_dims.size(); i+=2) {
      cmb_hash(h(_dims[i]));
      cmb_hash(h(_dims[i+1]));
    }
    for (unsigned int i=_v.size(); i--;)
      cmb_hash(Expression::hash(_v[i]));
  }
  int
  ArrayLit::dims(void) const {
    return _dims.size()/2;
  }
  int
  ArrayLit::min(int i) const {
    return _dims[2*i];
  }
  int
  ArrayLit::max(int i) const {
    return _dims[2*i+1];
  }
  int
  ArrayLit::length(void) const {
    if(dims() == 0) return 0;
    int l = max(0) - min(0) + 1;
    for(int i=1; i<dims(); i++)
      l *= (max(i) - min(i) + 1);
    return l;
  }

  void
  ArrayAccess::rehash(void) {
    init_hash();
    cmb_hash(Expression::hash(_v));
    std::hash<unsigned int> h;
    cmb_hash(h(_idx.size()));
    for (unsigned int i=_idx.size(); i--;)
      cmb_hash(Expression::hash(_idx[i]));
  }

  Generator::Generator(const std::vector<ASTString>& v,
                       Expression* in) {
    std::vector<VarDecl*> vd;
    for (unsigned int i=0; i<v.size(); i++)
      vd.push_back(new VarDecl(in->loc(),
        new TypeInst(in->loc(),Type::parint()),v[i],
        new IntLit(in->loc(),0)));
    _v = vd;
    _in = in;
  }
  Generator::Generator(const std::vector<std::string>& v,
                       Expression* in) {
    std::vector<VarDecl*> vd;
    for (unsigned int i=0; i<v.size(); i++)
      vd.push_back(new VarDecl(in->loc(),
        new TypeInst(in->loc(),Type::parint()),ASTString(v[i]),
        new IntLit(in->loc(),0)));
    _v = vd;
    _in = in;
  }
  Generator::Generator(const std::vector<VarDecl*>& v,
                       Expression* in) {
    _v = v;
    _in = in;
  }

  bool
  Comprehension::set(void) const {
    return _flag_1;
  }
  void
  Comprehension::rehash(void) {
    init_hash();
    std::hash<unsigned int> h;
    cmb_hash(h(set()));
    cmb_hash(Expression::hash(_e));
    cmb_hash(Expression::hash(_where));
    cmb_hash(h(_g_idx.size()));
    for (unsigned int i=_g_idx.size(); i--;) {
      cmb_hash(h(_g_idx[i]));
    }
    cmb_hash(h(_g.size()));
    for (unsigned int i=_g.size(); i--;) {
      cmb_hash(Expression::hash(_g[i]));
    }
  }

  int
  Comprehension::n_generators(void) const {
    return _g_idx.size()-1;
  }
  Expression*
  Comprehension::in(int i) {
    return _g[_g_idx[i]];
  }
  const Expression*
  Comprehension::in(int i) const {
    return _g[_g_idx[i]];
  }

  int
  Comprehension::n_decls(int i) const {
    return _g_idx[i+1]-_g_idx[i]-1;
  }
  VarDecl*
  Comprehension::decl(int gen, int i) {
    return _g[_g_idx[gen]+1+i]->cast<VarDecl>();
  }
  const VarDecl*
  Comprehension::decl(int gen, int i) const {
    return _g[_g_idx[gen]+1+i]->cast<VarDecl>();
  }

  void
  ITE::rehash(void) {
    init_hash();
    std::hash<unsigned int> h;
    cmb_hash(h(_e_if_then.size()));
    for (unsigned int i=_e_if_then.size(); i--; ) {
      cmb_hash(Expression::hash(_e_if_then[i]));
    }
    cmb_hash(Expression::hash(e_else()));
  }

  BinOpType
  BinOp::op(void) const {
    return static_cast<BinOpType>(_sec_id);
  }
  void
  BinOp::rehash(void) {
    init_hash();
    std::hash<int> h;
    cmb_hash(h(static_cast<int>(op())));
    cmb_hash(Expression::hash(_e0));
    cmb_hash(Expression::hash(_e1));
  }

  namespace {
    
    class OpToString {
    protected:
      std::vector<KeepAlive> rootSet;
    public:
      Id* sBOT_PLUS;
      Id* sBOT_MINUS;
      Id* sBOT_MULT;
      Id* sBOT_DIV;
      Id* sBOT_IDIV;
      Id* sBOT_MOD;
      Id* sBOT_LE;
      Id* sBOT_LQ;
      Id* sBOT_GR;
      Id* sBOT_GQ;
      Id* sBOT_EQ;
      Id* sBOT_NQ;
      Id* sBOT_IN;
      Id* sBOT_SUBSET;
      Id* sBOT_SUPERSET;
      Id* sBOT_UNION;
      Id* sBOT_DIFF;
      Id* sBOT_SYMDIFF;
      Id* sBOT_INTERSECT;
      Id* sBOT_PLUSPLUS;
      Id* sBOT_EQUIV;
      Id* sBOT_IMPL;
      Id* sBOT_RIMPL;
      Id* sBOT_OR;
      Id* sBOT_AND;
      Id* sBOT_XOR;
      Id* sBOT_DOTDOT;
      Id* sBOT_NOT;
      
      OpToString(void) {
        GCLock lock;
        sBOT_PLUS = new Id(Location(),"@+",NULL);
        rootSet.push_back(sBOT_PLUS);
        sBOT_MINUS = new Id(Location(),"@-",NULL);
        rootSet.push_back(sBOT_MINUS);
        sBOT_MULT = new Id(Location(),"@*",NULL);
        rootSet.push_back(sBOT_MULT);
        sBOT_DIV = new Id(Location(),"@/",NULL);
        rootSet.push_back(sBOT_DIV);
        sBOT_IDIV = new Id(Location(),"@div",NULL);
        rootSet.push_back(sBOT_IDIV);
        sBOT_MOD = new Id(Location(),"@mod",NULL);
        rootSet.push_back(sBOT_MOD);
        sBOT_LE = new Id(Location(),"@<",NULL);
        rootSet.push_back(sBOT_LE);
        sBOT_LQ = new Id(Location(),"@<=",NULL);
        rootSet.push_back(sBOT_LQ);
        sBOT_GR = new Id(Location(),"@>",NULL);
        rootSet.push_back(sBOT_GR);
        sBOT_GQ = new Id(Location(),"@>=",NULL);
        rootSet.push_back(sBOT_GQ);
        sBOT_EQ = new Id(Location(),"@=",NULL);
        rootSet.push_back(sBOT_EQ);
        sBOT_NQ = new Id(Location(),"@!=",NULL);
        rootSet.push_back(sBOT_NQ);
        sBOT_IN = new Id(Location(),"@in",NULL);
        rootSet.push_back(sBOT_IN);
        sBOT_SUBSET = new Id(Location(),"@subset",NULL);
        rootSet.push_back(sBOT_SUBSET);
        sBOT_SUPERSET = new Id(Location(),"@superset",NULL);
        rootSet.push_back(sBOT_SUPERSET);
        sBOT_UNION = new Id(Location(),"@union",NULL);
        rootSet.push_back(sBOT_UNION);
        sBOT_DIFF = new Id(Location(),"@diff",NULL);
        rootSet.push_back(sBOT_DIFF);
        sBOT_SYMDIFF = new Id(Location(),"@symdiff",NULL);
        rootSet.push_back(sBOT_SYMDIFF);
        sBOT_INTERSECT = new Id(Location(),"@intersect",NULL);
        rootSet.push_back(sBOT_INTERSECT);
        sBOT_PLUSPLUS = new Id(Location(),"@++",NULL);
        rootSet.push_back(sBOT_PLUSPLUS);
        sBOT_EQUIV = new Id(Location(),"@<->",NULL);
        rootSet.push_back(sBOT_EQUIV);
        sBOT_IMPL = new Id(Location(),"@->",NULL);
        rootSet.push_back(sBOT_IMPL);
        sBOT_RIMPL = new Id(Location(),"@<-",NULL);
        rootSet.push_back(sBOT_RIMPL);
        sBOT_OR = new Id(Location(),"@\\/",NULL);
        rootSet.push_back(sBOT_OR);
        sBOT_AND = new Id(Location(),"@/\\",NULL);
        rootSet.push_back(sBOT_AND);
        sBOT_XOR = new Id(Location(),"@xor",NULL);
        rootSet.push_back(sBOT_XOR);
        sBOT_DOTDOT = new Id(Location(),"@..",NULL);
        rootSet.push_back(sBOT_DOTDOT);
        sBOT_NOT = new Id(Location(),"@not",NULL);
        rootSet.push_back(sBOT_NOT);
      }
            
      static OpToString& o(void) {
        static OpToString _o;
        return _o;
      }
      
    };
  }

  ASTString
  BinOp::opToString(void) const {
    switch (op()) {
    case BOT_PLUS: return OpToString::o().sBOT_PLUS->v();
    case BOT_MINUS: return OpToString::o().sBOT_MINUS->v();
    case BOT_MULT: return OpToString::o().sBOT_MULT->v();
    case BOT_DIV: return OpToString::o().sBOT_DIV->v();
    case BOT_IDIV: return OpToString::o().sBOT_IDIV->v();
    case BOT_MOD: return OpToString::o().sBOT_MOD->v();
    case BOT_LE: return OpToString::o().sBOT_LE->v();
    case BOT_LQ: return OpToString::o().sBOT_LQ->v();
    case BOT_GR: return OpToString::o().sBOT_GR->v();
    case BOT_GQ: return OpToString::o().sBOT_GQ->v();
    case BOT_EQ: return OpToString::o().sBOT_EQ->v();
    case BOT_NQ: return OpToString::o().sBOT_NQ->v();
    case BOT_IN: return OpToString::o().sBOT_IN->v();
    case BOT_SUBSET: return OpToString::o().sBOT_SUBSET->v();
    case BOT_SUPERSET: return OpToString::o().sBOT_SUPERSET->v();
    case BOT_UNION: return OpToString::o().sBOT_UNION->v();
    case BOT_DIFF: return OpToString::o().sBOT_DIFF->v();
    case BOT_SYMDIFF: return OpToString::o().sBOT_SYMDIFF->v();
    case BOT_INTERSECT: return OpToString::o().sBOT_INTERSECT->v();
    case BOT_PLUSPLUS: return OpToString::o().sBOT_PLUSPLUS->v();
    case BOT_EQUIV: return OpToString::o().sBOT_EQUIV->v();
    case BOT_IMPL: return OpToString::o().sBOT_IMPL->v();
    case BOT_RIMPL: return OpToString::o().sBOT_RIMPL->v();
    case BOT_OR: return OpToString::o().sBOT_OR->v();
    case BOT_AND: return OpToString::o().sBOT_AND->v();
    case BOT_XOR: return OpToString::o().sBOT_XOR->v();
    case BOT_DOTDOT: return OpToString::o().sBOT_DOTDOT->v();
    default: assert(false);
    }
  }

  UnOpType
  UnOp::op(void) const {
    return static_cast<UnOpType>(_sec_id);
  }
  void
  UnOp::rehash(void) {
    init_hash();
    std::hash<int> h;
    cmb_hash(h(static_cast<int>(_sec_id)));
    cmb_hash(Expression::hash(_e0));
  }

  ASTString
  UnOp::opToString(void) const {
    switch (op()) {
    case UOT_PLUS: return OpToString::o().sBOT_PLUS->v();
    case UOT_MINUS: return OpToString::o().sBOT_MINUS->v();
    case UOT_NOT: return OpToString::o().sBOT_NOT->v();
    default: assert(false);
    }
  }

  void
  Call::rehash(void) {
    init_hash();
    cmb_hash(_id.hash());
    std::hash<FunctionI*> hf;
    cmb_hash(hf(_decl));
    std::hash<unsigned int> hu;
    cmb_hash(hu(_args.size()));
    for (unsigned int i=_args.size(); i--;)
      cmb_hash(Expression::hash(_args[i]));
  }

  void
  VarDecl::rehash(void) {
    init_hash();
    cmb_hash(Expression::hash(_ti));
    cmb_hash(_id->hash());
    cmb_hash(Expression::hash(_e));
  }
  
  
  void
  Let::rehash(void) {
    init_hash();
    cmb_hash(Expression::hash(_in));
    std::hash<unsigned int> h;
    cmb_hash(h(_let.size()));
    for (unsigned int i=_let.size(); i--;)
      cmb_hash(Expression::hash(_let[i]));
  }
  void
  Let::pushbindings(void) {
    GC::mark();
    for (unsigned int i=_let.size(); i--;) {
      if (_let[i]->isa<VarDecl>()) {
        VarDecl* vd = _let[i]->cast<VarDecl>();
        GC::trail(&vd->_e,vd->e());
      }
    }
  }
  void
  Let::popbindings(void) {
    GC::untrail();
  }

  void
  TypeInst::rehash(void) {
    init_hash();
    std::hash<unsigned int> h;
    unsigned int rsize = _ranges.size();
    cmb_hash(h(rsize));
    for (unsigned int i=rsize; i--;)
      cmb_hash(Expression::hash(_ranges[i]));
    cmb_hash(Expression::hash(domain()));
  }

  void
  TypeInst::setRanges(const std::vector<TypeInst*>& ranges) {
    _ranges = ASTExprVec<TypeInst>(ranges);
    if (ranges.size()==1 && ranges[0] && ranges[0]->isa<TypeInst>() &&
        ranges[0]->cast<TypeInst>()->domain() &&
        ranges[0]->cast<TypeInst>()->domain()->isa<TIId>())
      _type._dim=-1;
    else
      _type._dim=ranges.size();
    rehash();
  }

  bool
  TypeInst::hasTiVariable(void) const {
    if (domain() && domain()->isa<TIId>())
      return true;
    if (_ranges.size()==1 &&
        _ranges[0]->isa<TIId>())
      return true;
    return false;
  }

  namespace {
    Type getType(Expression* e) { return e->type(); }
    Type getType(const Type& t) { return t; }
    const Location& getLoc(Expression* e, FunctionI*) { return e->loc(); }
    const Location& getLoc(const Type&, FunctionI* fi) { return fi->loc(); }

    template<class T>
    Type return_type(FunctionI* fi, const std::vector<T>& ta) {
      if (fi->id()==constants().var_redef->id())
        return Type::varbool();
      Type ret = fi->ti()->type();
      ASTString dh;
      if (fi->ti()->domain() && fi->ti()->domain()->isa<TIId>())
        dh = fi->ti()->domain()->cast<TIId>()->v();
      ASTString rh;
      if (fi->ti()->ranges().size()==1 &&
          fi->ti()->ranges()[0]->domain() && fi->ti()->ranges()[0]->domain()->isa<TIId>())
        rh = fi->ti()->ranges()[0]->domain()->cast<TIId>()->v();
      
      ASTStringMap<Type>::t tmap;
      for (unsigned int i=0; i<ta.size(); i++) {
        TypeInst* tii = fi->params()[i]->ti();
        if (tii->domain() && tii->domain()->isa<TIId>()) {
          ASTString tiid = tii->domain()->cast<TIId>()->v();
          Type tiit = getType(ta[i]);
          tiit._dim=0;
          if (tii->type()._st==Type::ST_SET)
            tiit._st = Type::ST_PLAIN;
          ASTStringMap<Type>::t::iterator it = tmap.find(tiid);
          if (it==tmap.end()) {
            tmap.insert(std::pair<ASTString,Type>(tiid,tiit));
          } else {
            if (it->second._dim > 0) {
              throw TypeError(getLoc(ta[i],fi),"type-inst variable $"+
                              tiid.str()+" used in both array and non-array position");
            } else {
              Type tiit_par = tiit;
              tiit_par._ti = Type::TI_PAR;
              tiit_par._ot = Type::OT_PRESENT;
              Type its_par = it->second;
              its_par._ti = Type::TI_PAR;
              its_par._ot = Type::OT_PRESENT;
              if (tiit_par._bt==Type::BT_TOP || tiit_par._bt==Type::BT_BOT) {
                tiit_par._bt = its_par._bt;
              }
              if (its_par._bt==Type::BT_TOP || its_par._bt==Type::BT_BOT) {
                its_par._bt = tiit_par._bt;
              }
              if (tiit_par != its_par) {
                throw TypeError(getLoc(ta[i],fi),"type-inst variable $"+
                                tiid.str()+" instantiated with different types ("+
                                tiit.toString()+" vs "+
                                it->second.toString()+")");
              }
              if (it->second._bt == Type::BT_TOP)
                it->second._bt = tiit._bt;
            }
          }
        }
        if (tii->ranges().size()==1 &&
            tii->ranges()[0]->domain() &&
            tii->ranges()[0]->domain()->isa<TIId>()) {
          ASTString tiid = tii->ranges()[0]->domain()->cast<TIId>()->v();
          if (getType(ta[i])._dim==0) {
            throw TypeError(getLoc(ta[i],fi),"type-inst variable $"+tiid.str()+
                            " must be an array index");
          }
          Type tiit = Type::top(getType(ta[i])._dim);
          ASTStringMap<Type>::t::iterator it = tmap.find(tiid);
          if (it==tmap.end()) {
            tmap.insert(std::pair<ASTString,Type>(tiid,tiit));
          } else {
            if (it->second._dim == 0) {
              throw TypeError(getLoc(ta[i],fi),"type-inst variable $"+
                              tiid.str()+" used in both array and non-array position");
            } else if (it->second!=tiit) {
              throw TypeError(getLoc(ta[i],fi),"type-inst variable $"+
                              tiid.str()+" instantiated with different types ("+
                              tiit.toString()+" vs "+
                              it->second.toString()+")");
            }
          }
        }
      }
      if (dh.size() != 0) {
        ASTStringMap<Type>::t::iterator it = tmap.find(dh);
        if (it==tmap.end())
          throw TypeError(fi->loc(),"type-inst variable $"+dh.str()+" used but not defined");
        ret._bt = it->second._bt;
        if (ret._st==Type::ST_PLAIN)
          ret._st = it->second._st;
      }
      if (rh.size() != 0) {
        ASTStringMap<Type>::t::iterator it = tmap.find(rh);
        if (it==tmap.end())
          throw TypeError(fi->loc(),"type-inst variable $"+rh.str()+" used but not defined");
        ret._dim = it->second._dim;
      }
      return ret;
    }
  }
  
  Type
  FunctionI::rtype(const std::vector<Expression*>& ta) {
    return return_type(this, ta);
  }

  Type
  FunctionI::rtype(const std::vector<Type>& ta) {
    return return_type(this, ta);
  }

  bool
  Expression::equal(const Expression* e0, const Expression* e1) {
    if (e0==e1) return true;
    if (e0 == NULL || e1 == NULL) return false;
    if (e0->_id != e1->_id) return false;
    if (e0->type() != e1->type()) return false;
    switch (e0->eid()) {
    case Expression::E_INTLIT:
      return e0->cast<IntLit>()->v() == e1->cast<IntLit>()->v();
    case Expression::E_FLOATLIT:
      return e0->cast<FloatLit>()->v() == e1->cast<FloatLit>()->v();
    case Expression::E_SETLIT:
      {
        const SetLit* s0 = e0->cast<SetLit>();
        const SetLit* s1 = e1->cast<SetLit>();
        if (s0->isv()) {
          if (s1->isv()) {
            IntSetRanges r0(s0->isv());
            IntSetRanges r1(s1->isv());
            return Ranges::equal(r0,r1);
          } else {
            return false;
          }
        } else {
          if (s1->isv()) return false;
          if (s0->v().size() != s1->v().size()) return false;
          for (unsigned int i=0; i<s0->v().size(); i++)
            if (!Expression::equal( s0->v()[i], s1->v()[i] ))
              return false;
          return true;
        }
      }
    case Expression::E_BOOLLIT:
      return e0->cast<BoolLit>()->v() == e1->cast<BoolLit>()->v();
    case Expression::E_STRINGLIT:
      return e0->cast<StringLit>()->v() == e1->cast<StringLit>()->v();
    case Expression::E_ID:
      {
        const Id* id0 = e0->cast<Id>();
        const Id* id1 = e1->cast<Id>();
        if (id0->decl()==NULL || id1->decl()==NULL) {
          return id0->v()==id1->v() && id0->idn()==id1->idn();
        }
        return id0->decl()==id1->decl() ||
          ( id0->decl()->flat() != NULL && id0->decl()->flat() == id1->decl()->flat() );
      }
    case Expression::E_ANON:
      return false;
    case Expression::E_ARRAYLIT:
      {
        const ArrayLit* a0 = e0->cast<ArrayLit>();
        const ArrayLit* a1 = e1->cast<ArrayLit>();
        if (a0->v().size() != a1->v().size()) return false;
        if (a0->_dims.size() != a1->_dims.size()) return false;
        for (unsigned int i=0; i<a0->_dims.size(); i++)
          if ( a0->_dims[i] != a1->_dims[i] ) return false;
        for (unsigned int i=0; i<a0->v().size(); i++)
          if (!Expression::equal( a0->v()[i], a1->v()[i] ))
            return false;
        return true;
      }
    case Expression::E_ARRAYACCESS:
      {
        const ArrayAccess* a0 = e0->cast<ArrayAccess>();
        const ArrayAccess* a1 = e1->cast<ArrayAccess>();
        if (!Expression::equal( a0->v(), a1->v() )) return false;
        if (a0->idx().size() != a1->idx().size()) return false;
        for (unsigned int i=0; i<a0->idx().size(); i++)
          if (!Expression::equal( a0->idx()[i], a1->idx()[i] ))
            return false;
        return true;
      }
    case Expression::E_COMP:
      {
        const Comprehension* c0 = e0->cast<Comprehension>();
        const Comprehension* c1 = e1->cast<Comprehension>();
        if (c0->set() != c1->set()) return false;
        if (!Expression::equal ( c0->_e, c1->_e )) return false;
        if (!Expression::equal ( c0->_where, c1->_where )) return false;
        if (c0->_g.size() != c1->_g.size()) return false;
        for (unsigned int i=0; i<c0->_g.size(); i++) {
          if (!Expression::equal( c0->_g[i], c1->_g[i] ))
            return false;
        }
        for (unsigned int i=0; i<c0->_g_idx.size(); i++) {
          if (c0->_g_idx[i] != c1->_g_idx[i])
            return false;
        }
        return true;
      }
    case Expression::E_ITE:
      {
        const ITE* i0 = e0->cast<ITE>();
        const ITE* i1 = e1->cast<ITE>();
        if (i0->_e_if_then.size() != i1->_e_if_then.size()) return false;
        for (unsigned int i=i0->_e_if_then.size(); i--; ) {
          if (!Expression::equal ( i0->_e_if_then[i],
                                   i1->_e_if_then[i]))
            return false;
        }
        if (!Expression::equal (i0->e_else(), i1->e_else())) return false;
        return true;
      }
    case Expression::E_BINOP:
      {
        const BinOp* b0 = e0->cast<BinOp>();
        const BinOp* b1 = e1->cast<BinOp>();
        if (b0->op() != b1->op()) return false;
        if (!Expression::equal (b0->lhs(), b1->lhs())) return false;
        if (!Expression::equal (b0->rhs(), b1->rhs())) return false;
        return true;
      }
    case Expression::E_UNOP:
      {
        const UnOp* b0 = e0->cast<UnOp>();
        const UnOp* b1 = e1->cast<UnOp>();
        if (b0->op() != b1->op()) return false;
        if (!Expression::equal (b0->e(), b1->e())) return false;
        return true;
      }
    case Expression::E_CALL:
      {
        const Call* c0 = e0->cast<Call>();
        const Call* c1 = e1->cast<Call>();
        if (c0->id() != c1->id()) return false;
        if (c0->_decl != c1->_decl) return false;
        if (c0->args().size() != c1->args().size()) return false;
        for (unsigned int i=0; i<c0->args().size(); i++)
          if (!Expression::equal ( c0->args()[i], c1->args()[i] ))
            return false;
        return true;
      }
    case Expression::E_VARDECL:
      {
        const VarDecl* v0 = e0->cast<VarDecl>();
        const VarDecl* v1 = e1->cast<VarDecl>();
        if (!Expression::equal ( v0->ti(), v1->ti() )) return false;
        if (!Expression::equal ( v0->id(), v1->id())) return false;
        if (!Expression::equal ( v0->e(), v1->e() )) return false;
        return true;
      }
    case Expression::E_LET:
      {
        const Let* l0 = e0->cast<Let>();
        const Let* l1 = e1->cast<Let>();
        if (!Expression::equal ( l0->in(), l1->in() )) return false;
        if (l0->let().size() != l1->let().size()) return false;
        for (unsigned int i=l0->let().size(); i--;)
          if (!Expression::equal ( l0->let()[i], l1->let()[i]))
            return false;
        return true;
      }
    case Expression::E_TI:
      {
        const TypeInst* t0 = e0->cast<TypeInst>();
        const TypeInst* t1 = e1->cast<TypeInst>();
        if (t0->ranges().size() != t1->ranges().size()) return false;
        for (unsigned int i=t0->ranges().size(); i--;)
          if (!Expression::equal ( t0->ranges()[i], t1->ranges()[i]))
            return false;
        if (!Expression::equal (t0->domain(), t1->domain())) return false;
        return true;
      }
    case Expression::E_TIID:
      return false;
    default:
      assert(false);
      return false;
    }
  }    

  Constants::Constants(void) {
    GC::init();
    GCLock lock;
    TypeInst* ti = new TypeInst(Location(), Type::parbool());
    lit_true = new BoolLit(Location(), true);
    var_true = new VarDecl(Location(), ti, "_bool_true", lit_true);
    lit_false = new BoolLit(Location(), false);
    var_false = new VarDecl(Location(), ti, "_bool_false", lit_false);
    
    ids.forall = ASTString("forall");
    ids.forall_reif = ASTString("forall_reif");
    ids.exists = ASTString("exists");
    ids.clause = ASTString("clause");
    ids.bool2int = ASTString("bool2int");
    ids.assert = ASTString("assert");
    ids.trace = ASTString("trace");

    ids.sum = ASTString("sum");
    ids.lin_exp = ASTString("lin_exp");
    
    ids.int_.lin_eq = ASTString("int_lin_eq");
    ids.int_.lin_le = ASTString("int_lin_le");
    ids.int_.lin_ne = ASTString("int_lin_ne");
    ids.int_.plus = ASTString("int_plus");
    ids.int_.minus = ASTString("int_minus");
    ids.int_.times = ASTString("int_times");
    ids.int_.div = ASTString("int_div");
    ids.int_.mod = ASTString("int_mod");
    ids.int_.lt = ASTString("int_lt");
    ids.int_.le = ASTString("int_le");
    ids.int_.gt = ASTString("int_gt");
    ids.int_.ge = ASTString("int_ge");
    ids.int_.eq = ASTString("int_eq");
    ids.int_.ne = ASTString("int_ne");

    ids.int_reif.lin_eq = ASTString("int_lin_eq_reif");
    ids.int_reif.lin_le = ASTString("int_lin_le_reif");
    ids.int_reif.lin_ne = ASTString("int_lin_ne_reif");
    ids.int_reif.plus = ASTString("int_plus_reif");
    ids.int_reif.minus = ASTString("int_minus_reif");
    ids.int_reif.times = ASTString("int_times_reif");
    ids.int_reif.div = ASTString("int_div_reif");
    ids.int_reif.mod = ASTString("int_mod_reif");
    ids.int_reif.lt = ASTString("int_lt_reif");
    ids.int_reif.le = ASTString("int_le_reif");
    ids.int_reif.gt = ASTString("int_gt_reif");
    ids.int_reif.ge = ASTString("int_ge_reif");
    ids.int_reif.eq = ASTString("int_eq_reif");
    ids.int_reif.ne = ASTString("int_ne_reif");

    ids.float_.lin_eq = ASTString("float_lin_eq");
    ids.float_.lin_le = ASTString("float_lin_le");
    ids.float_.lin_lt = ASTString("float_lin_lt");
    ids.float_.lin_ne = ASTString("float_lin_ne");
    ids.float_.plus = ASTString("float_plus");
    ids.float_.minus = ASTString("float_minus");
    ids.float_.times = ASTString("float_times");
    ids.float_.div = ASTString("float_div");
    ids.float_.mod = ASTString("float_mod");
    ids.float_.lt = ASTString("float_lt");
    ids.float_.le = ASTString("float_le");
    ids.float_.gt = ASTString("float_gt");
    ids.float_.ge = ASTString("float_ge");
    ids.float_.eq = ASTString("float_eq");
    ids.float_.ne = ASTString("float_ne");

    ids.float_reif.lin_eq = ASTString("float_lin_eq_reif");
    ids.float_reif.lin_le = ASTString("float_lin_le_reif");
    ids.float_reif.lin_lt = ASTString("float_lin_lt_reif");
    ids.float_reif.lin_ne = ASTString("float_lin_ne_reif");
    ids.float_reif.plus = ASTString("float_plus_reif");
    ids.float_reif.minus = ASTString("float_minus_reif");
    ids.float_reif.times = ASTString("float_times_reif");
    ids.float_reif.div = ASTString("float_div_reif");
    ids.float_reif.mod = ASTString("float_mod_reif");
    ids.float_reif.lt = ASTString("float_lt_reif");
    ids.float_reif.le = ASTString("float_le_reif");
    ids.float_reif.gt = ASTString("float_gt_reif");
    ids.float_reif.ge = ASTString("float_ge_reif");
    ids.float_reif.eq = ASTString("float_eq_reif");
    ids.float_reif.ne = ASTString("float_ne_reif");

    ids.bool_eq = ASTString("bool_eq");
    ids.bool_eq_reif = ASTString("bool_eq_reif");
    ids.bool_clause = ASTString("bool_clause");
    ids.bool_clause_reif = ASTString("bool_clause_reif");
    ids.array_bool_or = ASTString("array_bool_or");
    ids.array_bool_and = ASTString("array_bool_and");
    ids.set_eq = ASTString("set_eq");
    ids.set_in = ASTString("set_in");
    ids.set_card = ASTString("set_card");
    
    ids.introduced_var = ASTString("__INTRODUCED");

    ctx.root = new Id(Location(),ASTString("ctx_root"),NULL);
    ctx.root->type(Type::ann());
    ctx.pos = new Id(Location(),ASTString("ctx_pos"),NULL);
    ctx.pos->type(Type::ann());
    ctx.neg = new Id(Location(),ASTString("ctx_neg"),NULL);
    ctx.neg->type(Type::ann());
    ctx.mix = new Id(Location(),ASTString("ctx_mix"),NULL);
    ctx.mix->type(Type::ann());
    
    ann.output_var = new Id(Location(), ASTString("output_var"), NULL);
    ann.output_var->type(Type::ann());
    ann.output_array = ASTString("output_array");
    ann.is_defined_var = new Id(Location(), ASTString("is_defined_var"), NULL);
    ann.is_defined_var->type(Type::ann());
    ann.defines_var = ASTString("defines_var");
    ann.is_reverse_map = new Id(Location(), ASTString("is_reverse_map"), NULL);
    ann.is_reverse_map->type(Type::ann());
    ann.promise_total = new Id(Location(), ASTString("promise_total"), NULL);
    ann.promise_total->type(Type::ann());
    
    var_redef = new FunctionI(Location(),"__internal_var_redef",new TypeInst(Location(),Type::varbool()),
                              std::vector<VarDecl*>());
    
    std::vector<Expression*> v;
    v.push_back(ti);
    v.push_back(lit_true);
    v.push_back(var_true);
    v.push_back(lit_false);
    v.push_back(var_false);
    v.push_back(new StringLit(Location(),ids.forall));
    v.push_back(new StringLit(Location(),ids.exists));
    v.push_back(new StringLit(Location(),ids.clause));
    v.push_back(new StringLit(Location(),ids.bool2int));
    v.push_back(new StringLit(Location(),ids.sum));
    v.push_back(new StringLit(Location(),ids.lin_exp));
    
    v.push_back(new StringLit(Location(),ids.int_.lin_eq));
    v.push_back(new StringLit(Location(),ids.int_.lin_le));
    v.push_back(new StringLit(Location(),ids.int_.lin_ne));
    v.push_back(new StringLit(Location(),ids.int_.plus));
    v.push_back(new StringLit(Location(),ids.int_.minus));
    v.push_back(new StringLit(Location(),ids.int_.times));
    v.push_back(new StringLit(Location(),ids.int_.div));
    v.push_back(new StringLit(Location(),ids.int_.mod));
    v.push_back(new StringLit(Location(),ids.int_.lt));
    v.push_back(new StringLit(Location(),ids.int_.le));
    v.push_back(new StringLit(Location(),ids.int_.gt));
    v.push_back(new StringLit(Location(),ids.int_.ge));
    v.push_back(new StringLit(Location(),ids.int_.eq));
    v.push_back(new StringLit(Location(),ids.int_.ne));

    v.push_back(new StringLit(Location(),ids.int_reif.lin_eq));
    v.push_back(new StringLit(Location(),ids.int_reif.lin_le));
    v.push_back(new StringLit(Location(),ids.int_reif.lin_ne));
    v.push_back(new StringLit(Location(),ids.int_reif.plus));
    v.push_back(new StringLit(Location(),ids.int_reif.minus));
    v.push_back(new StringLit(Location(),ids.int_reif.times));
    v.push_back(new StringLit(Location(),ids.int_reif.div));
    v.push_back(new StringLit(Location(),ids.int_reif.mod));
    v.push_back(new StringLit(Location(),ids.int_reif.lt));
    v.push_back(new StringLit(Location(),ids.int_reif.le));
    v.push_back(new StringLit(Location(),ids.int_reif.gt));
    v.push_back(new StringLit(Location(),ids.int_reif.ge));
    v.push_back(new StringLit(Location(),ids.int_reif.eq));
    v.push_back(new StringLit(Location(),ids.int_reif.ne));

    v.push_back(new StringLit(Location(),ids.float_.lin_eq));
    v.push_back(new StringLit(Location(),ids.float_.lin_le));
    v.push_back(new StringLit(Location(),ids.float_.lin_lt));
    v.push_back(new StringLit(Location(),ids.float_.lin_ne));
    v.push_back(new StringLit(Location(),ids.float_.plus));
    v.push_back(new StringLit(Location(),ids.float_.minus));
    v.push_back(new StringLit(Location(),ids.float_.times));
    v.push_back(new StringLit(Location(),ids.float_.div));
    v.push_back(new StringLit(Location(),ids.float_.mod));
    v.push_back(new StringLit(Location(),ids.float_.lt));
    v.push_back(new StringLit(Location(),ids.float_.le));
    v.push_back(new StringLit(Location(),ids.float_.gt));
    v.push_back(new StringLit(Location(),ids.float_.ge));
    v.push_back(new StringLit(Location(),ids.float_.eq));
    v.push_back(new StringLit(Location(),ids.float_.ne));

    v.push_back(new StringLit(Location(),ids.float_reif.lin_eq));
    v.push_back(new StringLit(Location(),ids.float_reif.lin_le));
    v.push_back(new StringLit(Location(),ids.float_reif.lin_lt));
    v.push_back(new StringLit(Location(),ids.float_reif.lin_ne));
    v.push_back(new StringLit(Location(),ids.float_reif.plus));
    v.push_back(new StringLit(Location(),ids.float_reif.minus));
    v.push_back(new StringLit(Location(),ids.float_reif.times));
    v.push_back(new StringLit(Location(),ids.float_reif.div));
    v.push_back(new StringLit(Location(),ids.float_reif.mod));
    v.push_back(new StringLit(Location(),ids.float_reif.lt));
    v.push_back(new StringLit(Location(),ids.float_reif.le));
    v.push_back(new StringLit(Location(),ids.float_reif.gt));
    v.push_back(new StringLit(Location(),ids.float_reif.ge));
    v.push_back(new StringLit(Location(),ids.float_reif.eq));
    v.push_back(new StringLit(Location(),ids.float_reif.ne));

    v.push_back(new StringLit(Location(),ids.bool_eq));
    v.push_back(new StringLit(Location(),ids.bool_clause));
    v.push_back(new StringLit(Location(),ids.bool_clause_reif));
    v.push_back(new StringLit(Location(),ids.array_bool_or));
    v.push_back(new StringLit(Location(),ids.array_bool_and));
    v.push_back(new StringLit(Location(),ids.set_eq));
    v.push_back(new StringLit(Location(),ids.set_in));
    v.push_back(new StringLit(Location(),ids.set_card));

    v.push_back(new StringLit(Location(),ids.assert));
    v.push_back(new StringLit(Location(),ids.trace));
    v.push_back(new StringLit(Location(),ids.introduced_var));
    v.push_back(ctx.root);
    v.push_back(ctx.pos);
    v.push_back(ctx.neg);
    v.push_back(ctx.mix);
    v.push_back(ann.output_var);
    v.push_back(new StringLit(Location(),ann.output_array));
    v.push_back(ann.is_defined_var);
    v.push_back(new StringLit(Location(),ann.defines_var));
    v.push_back(ann.is_reverse_map);
    v.push_back(ann.promise_total);
    
    m = new Model();
    m->addItem(new ConstraintI(Location(),new ArrayLit(Location(),v)));
    m->addItem(var_redef);
  }
  
  Constants& constants(void) {
    static Constants _c;
    return _c;
  }


  Annotation::~Annotation(void) {
    delete _s;
  }
  
  bool
  Annotation::contains(Expression* e) const {
    return _s && _s->contains(e);
  }

  bool
  Annotation::isEmpty(void) const {
    return _s == NULL || _s->isEmpty();
  }
  
  ExpressionSetIter
  Annotation::begin(void) const {
    return _s == NULL ? ExpressionSetIter(true) : _s->begin();
  }
  
  ExpressionSetIter
  Annotation::end(void) const {
    return _s == NULL ? ExpressionSetIter(true) : _s->end();
  }

  void
  Annotation::add(Expression* e) {
    if (_s == NULL)
      _s = new ExpressionSet;
    _s->insert(e);
  }
  
  void
  Annotation::add(std::vector<Expression*> e) {
    if (_s == NULL)
      _s = new ExpressionSet;
    for (unsigned int i=e.size(); i--;)
      _s->insert(e[i]);
  }
  
  void
  Annotation::remove(Expression* e) {
    if (_s) {
      _s->remove(e);
    }
  }

  void
  Annotation::removeCall(const ASTString& id) {
    if (_s==NULL)
      return;
    std::vector<Expression*> toRemove;
    for (ExpressionSetIter it=_s->begin(); it != _s->end(); ++it) {
      if (Call* c = (*it)->dyn_cast<Call>()) {
        if (c->id() == id)
          toRemove.push_back(*it);
      }
    }
    for (unsigned int i=toRemove.size(); i--;)
      _s->remove(toRemove[i]);
  }
  
  void
  Annotation::clear(void) {
    if (_s) {
      _s->clear();
    }
  }
  
  void
  Annotation::merge(const Annotation& ann) {
    if (ann._s == NULL)
      return;
    if (_s == NULL) {
      _s = new ExpressionSet;
    }
    for (ExpressionSetIter it=ann.begin(); it != ann.end(); ++it) {
      _s->insert(*it);
    }
  }
  
}
