/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace MiniZinc {

  inline
  Annotation::Annotation(const Location& loc, Expression* e, Annotation* a)
  : Expression(loc,E_ANN,Type::ann()), _e(e), _a(a) {
    rehash();
  }

  inline
  IntLit::IntLit(const Location& loc, IntVal v)
  : Expression(loc,E_INTLIT,Type::parint()), _v(v) {
    rehash();
  }

  inline
  FloatLit::FloatLit(const Location& loc, FloatVal v)
  : Expression(loc,E_FLOATLIT,Type::parfloat()), _v(v) {
    rehash();
  }

  inline
  SetLit::SetLit(const Location& loc,
                 const std::vector<Expression*>& v)
  : Expression(loc,E_SETLIT,Type()), _v(ASTExprVec<Expression>(v)), _isv(NULL) {
    rehash();
  }
  
  inline
  SetLit::SetLit(const Location& loc, ASTExprVec<Expression> v)
  : Expression(loc,E_SETLIT,Type()), _v(v), _isv(NULL) {
    rehash();
  }

  inline
  SetLit::SetLit(const Location& loc, IntSetVal* isv)
  : Expression(loc,E_SETLIT,Type()), _isv(isv) {
    _type = Type::parsetint();
    rehash();
  }

  inline
  BoolLit::BoolLit(const Location& loc, bool v)
  : Expression(loc,E_BOOLLIT,Type::parbool()), _v(v) {
    rehash();
  }

  inline
  StringLit::StringLit(const Location& loc, const std::string& v)
  : Expression(loc,E_STRINGLIT,Type::parstring()), _v(ASTString(v)) {
    rehash();
  }

  inline
  StringLit::StringLit(const Location& loc, const ASTString& v)
  : Expression(loc,E_STRINGLIT,Type::parstring()), _v(v) {
    rehash();
  }
  
  inline
  Id::Id(const Location& loc, const std::string& v, VarDecl* decl)
  : Expression(loc,E_ID,Type()), _v(ASTString(v)), _decl(decl) {
    rehash();
  }

  inline
  Id::Id(const Location& loc, const ASTString& v, VarDecl* decl)
  : Expression(loc,E_ID,Type()), _v(v), _decl(decl) {
    rehash();
  }

  inline
  TIId::TIId(const Location& loc, const std::string& v)
  : Expression(loc,E_TIID,Type()), _v(ASTString(v)) {
    rehash();
  }

  inline
  AnonVar::AnonVar(const Location& loc)
  : Expression(loc,E_ANON,Type::bot()) {
    rehash();
  }
  
  inline
  ArrayLit::ArrayLit(const Location& loc,
                     const std::vector<Expression*>& v,
                     const std::vector<std::pair<int,int> >& dims)
  : Expression(loc,E_ARRAYLIT,Type()) {
    std::vector<int> d(dims.size()*2);
    for (unsigned int i=dims.size(); i--;) {
      d[i*2] = dims[i].first;
      d[i*2+1] = dims[i].second;
    }
    _v = ASTExprVec<Expression>(v);
    _dims = ASTIntVec(d);
    rehash();
  }

  inline
  ArrayLit::ArrayLit(const Location& loc,
                     ASTExprVec<Expression> v,
                     const std::vector<std::pair<int,int> >& dims)
  : Expression(loc,E_ARRAYLIT,Type()) {
    std::vector<int> d(dims.size()*2);
    for (unsigned int i=dims.size(); i--;) {
      d[i*2] = dims[i].first;
      d[i*2+1] = dims[i].second;
    }
    _v = v;
    _dims = ASTIntVec(d);
    rehash();
  }

  inline
  ArrayLit::ArrayLit(const Location& loc,
                     const std::vector<Expression*>& v)
  : Expression(loc,E_ARRAYLIT,Type()) {
    std::vector<int> dims(2);
    dims[0]=1;
    dims[1]=v.size();
    _v = ASTExprVec<Expression>(v);
    _dims = ASTIntVec(dims);
    rehash();
  }

  inline
  ArrayLit::ArrayLit(const Location& loc,
                     const std::vector<std::vector<Expression*> >& v)
  : Expression(loc,E_ARRAYLIT,Type()) {
    std::vector<int> dims(4);
    dims[0]=1;
    dims[1]=v.size();
    dims[2]=1;
    dims[3]=v[0].size();
    std::vector<Expression*> vv;
    for (unsigned int i=0; i<v.size(); i++)
      for (unsigned int j=0; j<v[i].size(); j++)
        vv.push_back(v[i][j]);
    _v = ASTExprVec<Expression>(vv);
    _dims = ASTIntVec(dims);
    rehash();
  }

  inline
  ArrayAccess::ArrayAccess(const Location& loc,
                           Expression* v,
                           const std::vector<Expression*>& idx)
  : Expression(loc,E_ARRAYACCESS,Type()) {
    _v = v;
    _idx = ASTExprVec<Expression>(idx);
    rehash();
  }

  inline
  ArrayAccess::ArrayAccess(const Location& loc,
                           Expression* v,
                           ASTExprVec<Expression> idx)
  : Expression(loc,E_ARRAYACCESS,Type()) {
    _v = v;
    _idx = idx;
    rehash();
  }

  inline
  Comprehension::Comprehension(const Location& loc,
                               Expression* e,
                               Generators& g,
                               bool set)
  : Expression(loc,E_COMP,Type()) {
    _e = e;
    std::vector<Expression*> es;
    std::vector<int> idx;
    for (unsigned int i=0; i<g._g.size(); i++) {
      idx.push_back(es.size());
      es.push_back(g._g[i]._in);
      for (unsigned int j=0; j<g._g[i]._v.size(); j++) {
        es.push_back(g._g[i]._v[j]);
      }
    }
    idx.push_back(es.size());
    _g = ASTExprVec<Expression>(es);
    _g_idx = ASTIntVec(idx);
    _where = g._w;
    _flag_1 = set;
    rehash();
  }

  inline
  ITE::ITE(const Location& loc,
           const std::vector<Expression*>& e_if_then, Expression* e_else)
  : Expression(loc,E_ITE,Type()) {
    _e_if_then = ASTExprVec<Expression>(e_if_then);
    _e_else = e_else;
    rehash();
  }

  inline
  BinOp::BinOp(const Location& loc,
               Expression* e0, BinOpType op, Expression* e1)
  : Expression(loc,E_BINOP,Type()), _e0(e0), _e1(e1), _decl(NULL) {
    _sec_id = op;
    rehash();
  }

  inline
  UnOp::UnOp(const Location& loc, UnOpType op, Expression* e)
  : Expression(loc,E_UNOP,Type()), _e0(e), _decl(NULL) {
    _sec_id = op;
    rehash();
  }


  inline
  Call::Call(const Location& loc,
             const std::string& id,
             const std::vector<Expression*>& args,
             FunctionI* decl)
  : Expression(loc, E_CALL,Type()) {
    _id = ASTString(id);
    _args = ASTExprVec<Expression>(args);
    _decl = decl;
    rehash();
  }

  inline
  Call::Call(const Location& loc,
             const ASTString& id,
             const std::vector<Expression*>& args,
             FunctionI* decl)
  : Expression(loc, E_CALL,Type()) {
    _id = id;
    _args = ASTExprVec<Expression>(args);
    _decl = decl;
    rehash();
  }

  inline
  VarDecl::VarDecl(const Location& loc,
                   TypeInst* ti, const ASTString& id, Expression* e)
  : Expression(loc,E_VARDECL,ti->type()),
    _id(new Id(loc,id,this)), _flat(NULL) {
    _flag_1 = true;
    _flag_2 = false;
    _ti = ti;
    _e = e;
    _id->type(type());
    rehash();
  }

  inline
  VarDecl::VarDecl(const Location& loc,
                   TypeInst* ti, const std::string& id, Expression* e)
  : Expression(loc,E_VARDECL,ti->type()),
    _id(new Id(loc,ASTString(id),this)), _flat(NULL) {
    _flag_1 = true;
    _flag_2 = false;
    _ti = ti;
    _e = e;
    _id->type(type());
    rehash();
  }

  inline
  Let::Let(const Location& loc,
           const std::vector<Expression*>& let, Expression* in)
  : Expression(loc,E_LET,Type()) {
    _let = ASTExprVec<Expression>(let);
    _in = in;
    rehash();
  }

  inline
  TypeInst::TypeInst(const Location& loc,
                     const Type& type,
                     ASTExprVec<TypeInst> ranges,
                     Expression* domain)
  : Expression(loc,E_TI,type), _ranges(ranges), _domain(domain) {
    _flag_1 = false;
    rehash();
  }

  inline
  TypeInst::TypeInst(const Location& loc,
                     const Type& type,
                     Expression* domain)
  : Expression(loc,E_TI,type), _domain(domain) {
    rehash();
  }

  inline
  IncludeI::IncludeI(const Location& loc, const ASTString& f)
  : Item(loc, II_INC), _f(f), _m(NULL) {}
  
  inline
  VarDeclI::VarDeclI(const Location& loc, VarDecl* e)
  : Item(loc, II_VD), _e(e) {}

  inline
  AssignI::AssignI(const Location& loc, const std::string& id, Expression* e)
    : Item(loc, II_ASN), _id(ASTString(id)), _e(e), _decl(NULL) {}

  inline
  ConstraintI::ConstraintI(const Location& loc, Expression* e)
  : Item(loc, II_CON), _e(e) {}

  inline
  SolveI::SolveI(const Location& loc, Annotation* a, Expression* e)
  : Item(loc, II_SOL), _ann(a), _e(e) {}
  inline SolveI*
  SolveI::sat(const Location& loc, Annotation* ann) {
    SolveI* si = new SolveI(loc,ann,NULL);
    si->_sec_id = ST_SAT;
    return si;
  }
  inline SolveI*
  SolveI::min(const Location& loc, Expression* e, Annotation* ann) {
    SolveI* si = new SolveI(loc,ann,e);
    si->_sec_id = ST_MIN;
    return si;
  }
  inline SolveI*
  SolveI::max(const Location& loc, Expression* e, Annotation* ann) {
    SolveI* si = new SolveI(loc,ann,e);
    si->_sec_id = ST_MAX;
    return si;
  }
  inline SolveI::SolveType
  SolveI::st(void) const {
    return static_cast<SolveType>(_sec_id);
  }
  inline void
  SolveI::st(SolveI::SolveType s) {
    _sec_id = s;
  }

  inline
  OutputI::OutputI(const Location& loc, Expression* e)
  : Item(loc, II_OUT), _e(e) {}
  
  inline
  FunctionI::FunctionI(const Location& loc,
                       const std::string& id, TypeInst* ti,
                       const std::vector<VarDecl*>& params,
                       Expression* e, Annotation* ann)
  : Item(loc, II_FUN),
    _id(ASTString(id)),
    _ti(ti),
    _params(ASTExprVec<VarDecl>(params)),
    _ann(ann),
    _e(e) {
    _builtins.e = NULL;
    _builtins.b = NULL;
    _builtins.f = NULL;
    _builtins.i = NULL;
    _builtins.s = NULL;
  }

}