#include <minizinc/ast.hh>

namespace MiniZinc {

  Location
  Location::a(void) {
    Location l;
    l.filename = NULL;
    l.first_line = 0;
    l.first_column = 0;
    l.last_line = 0;
    l.last_column = 0;
    return l;
  }

  Annotation*
  Annotation::a(const ASTContext& ctx, const Location& loc,
                Expression* e) {
    return new (ctx) Annotation(loc,e);
  }
  void
  Annotation::merge(Annotation* a) {
    Annotation* r = this;
    while (r->_a != NULL) r=r->_a;
    r->_a = a;
  }

  void
  Expression::annotate(Annotation* ann) {
    if (_ann) _ann->merge(ann); else _ann=ann;
  }

  IntLit*
  IntLit::a(const ASTContext& ctx, const Location& loc,
            int v) {
    return new (ctx) IntLit(loc,v);
  }

  FloatLit*
  FloatLit::a(const ASTContext& ctx, const Location& loc,
              double v) {
    return new (ctx) FloatLit(loc,v);
  }

  SetLit*
  SetLit::a(const ASTContext& ctx,
            const Location& loc,
            const std::vector<Expression*>& v) {
    SetLit* sl = new (ctx) SetLit(loc);
    sl->_v = CtxVec<Expression*>::a(ctx,v);
    // sl->_rs = NULL;
    return sl;
  }

  BoolLit*
  BoolLit::a(const ASTContext& ctx, const Location& loc,
             bool v) {
    return new (ctx) BoolLit(loc,v);
  }

  StringLit*
  StringLit::a(const ASTContext& ctx, const Location& loc,
               const std::string& v) {
    return new (ctx) StringLit(loc,CtxString::a(ctx,v));
  }

  Id*
  Id::a(const ASTContext& ctx, const Location& loc,
        const std::string& v, VarDecl* decl) {
    return new (ctx) Id(loc,CtxString::a(ctx,v),decl);
  }

  AnonVar*
  AnonVar::a(const ASTContext& ctx, const Location& loc) {
    return new (ctx) AnonVar(loc);
  }

  ArrayLit*
  ArrayLit::a(const ASTContext& ctx,
              const Location& loc,
              const std::vector<Expression*>& v,
              const std::vector<pair<int,int> >& dims) {
    ArrayLit* al = new (ctx) ArrayLit(loc);
    al->_v = CtxVec<Expression*>::a(ctx,v);
    al->_dims = CtxVec<pair<int,int> >::a(ctx,dims);
    return al;
  }
  ArrayLit*
  ArrayLit::a(const ASTContext& ctx,
              const Location& loc,
              const std::vector<Expression*>& v) {
    std::vector<pair<int,int> > dims;
    dims.push_back(pair<int,int>(1,v.size()));
    return a(ctx,loc,v,dims);
  }
  ArrayLit*
  ArrayLit::a(const ASTContext& ctx,
              const Location& loc,
              const std::vector<std::vector<Expression*> >& v) {
    std::vector<pair<int,int> > dims;
    dims.push_back(pair<int,int>(1,v.size()));
    dims.push_back(pair<int,int>(1,v[0].size()));
    std::vector<Expression*> vv;
    for (unsigned int i=0; i<v.size(); i++)
      for (unsigned int j=0; j<v[i].size(); j++)
        vv.push_back(v[i][j]);
    return a(ctx,loc,vv,dims);
  }

  ArrayAccess*
  ArrayAccess::a(const ASTContext& ctx,
                 const Location& loc,
                 Expression* v,
                 const std::vector<Expression*>& idx) {
    ArrayAccess* aa = new (ctx) ArrayAccess(loc);
    aa->_v = v;
    aa->_idx = CtxVec<Expression*>::a(ctx,idx);
    return aa;
  }

  Generator*
  Generator::a(const ASTContext& ctx,
               const std::vector<CtxString*>& v,
               Expression* in) {
    Generator* g = new (ctx) Generator();
    std::vector<VarDecl*> vd(v.size());
    for (unsigned int i=0; i<v.size(); i++)
      vd[i] = VarDecl::a(ctx,in->_loc,
        TiExpr::par(ctx,in->_loc,IntTiExpr::a(ctx)),v[i]);
    g->_v = CtxVec<VarDecl*>::a(ctx,vd);
    g->_in = in;
    return g;
  }
  Generator*
  Generator::a(const ASTContext& ctx,
               const std::vector<std::string>& v,
               Expression* in) {
    std::vector<CtxString*> vv(v.size());
    for (unsigned int i=0; i<v.size(); i++)
      vv[i] = CtxString::a(ctx,v[i]);
    return a(ctx,vv,in);
  }

  Comprehension*
  Comprehension::a(const ASTContext& ctx,
                   const Location& loc,
                   Expression* e,
                   Generators& g,
                   bool set) {
    Comprehension* c = new (ctx) Comprehension(loc);
    c->_e = e;
    c->_g = CtxVec<Generator*>::a(ctx,g._g);
    c->_where = g._w;
    c->_set = set;
    return c;
  }

  ITE*
  ITE::a(const ASTContext& ctx, const Location& loc,
         const std::vector<IfThen>& e_if, Expression* e_else) {
    ITE* ite = new (ctx) ITE(loc);
    ite->_e_if = CtxVec<IfThen>::a(ctx,e_if);
    ite->_e_else = e_else;
    return ite;
  }

  BinOp*
  BinOp::a(const ASTContext& ctx, const Location& loc,
           Expression* e0, BinOpType op, Expression* e1) {
    return new (ctx) BinOp(loc,e0,op,e1);
  }

  UnOp*
  UnOp::a(const ASTContext& ctx, const Location& loc,
          UnOpType op, Expression* e) {
    return new (ctx) UnOp(loc,op,e);
  }

  Call*
  Call::a(const ASTContext& ctx, const Location& loc,
          const std::string& id,
          const std::vector<Expression*>& args,
          Item* decl) {
    Call* c = new (ctx) Call(loc);
    c->_id = CtxString::a(ctx,id);
    c->_args = CtxVec<Expression*>::a(ctx,args);
    c->_decl = decl;
    return c;
  }

  VarDecl*
  VarDecl::a(const ASTContext& ctx, const Location& loc,
             TiExpr* ti, CtxString* id, Expression* e) {
    VarDecl* v = new (ctx) VarDecl(loc);
    v->_ti = ti;
    v->_id = id;
    v->_e = e;
    return v;
  }
  VarDecl*
  VarDecl::a(const ASTContext& ctx, const Location& loc,
             TiExpr* ti, const std::string& id, Expression* e) {
    return a(ctx,loc,ti,CtxString::a(ctx,id));
  }

  Let*
  Let::a(const ASTContext& ctx, const Location& loc,
         const std::vector<Expression*>& let, Expression* in) {
    Let* l = new (ctx) Let(loc);
    l->_let = CtxVec<Expression*>::a(ctx,let);
    l->_in = in;
    return l;
  }

  TiExpr*
  TiExpr::var(const ASTContext& ctx, const Location& loc,
              const std::vector<IntTiExpr*>& ranges,
              BaseTiExpr* ti) {
    return new (ctx) TiExpr(loc,CtxVec<IntTiExpr*>::a(ctx,ranges),
                            VT_VAR,false,ti);                     
  }
  TiExpr*
  TiExpr::par(const ASTContext& ctx, const Location& loc,
              const std::vector<IntTiExpr*>& ranges,
              BaseTiExpr* ti) {
    return new (ctx) TiExpr(loc,CtxVec<IntTiExpr*>::a(ctx,ranges),
                            VT_PAR,false,ti);                     
  }
  TiExpr*
  TiExpr::varset(const ASTContext& ctx, const Location& loc, 
                 const std::vector<IntTiExpr*>& ranges,
                 BaseTiExpr* ti) {
    return new (ctx) TiExpr(loc,CtxVec<IntTiExpr*>::a(ctx,ranges),
                            VT_VAR,true,ti);                     
  }
  TiExpr*
  TiExpr::parset(const ASTContext& ctx, const Location& loc,
                 const std::vector<IntTiExpr*>& ranges,
                 BaseTiExpr* ti) {
    return new (ctx) TiExpr(loc,CtxVec<IntTiExpr*>::a(ctx,ranges),
                            VT_PAR,true,ti);                     
  }
  TiExpr*
  TiExpr::var(const ASTContext& ctx, const Location& loc,
              BaseTiExpr* ti) {
    return var(ctx,loc,std::vector<IntTiExpr*>(), ti);
  }
  TiExpr*
  TiExpr::par(const ASTContext& ctx, const Location& loc,
              BaseTiExpr* ti) {
    return par(ctx,loc,std::vector<IntTiExpr*>(), ti);
  }
  TiExpr*
  TiExpr::varset(const ASTContext& ctx, const Location& loc, 
                 BaseTiExpr* ti) {
    return varset(ctx,loc,std::vector<IntTiExpr*>(), ti);
  }
  TiExpr*
  TiExpr::parset(const ASTContext& ctx, const Location& loc, 
                 BaseTiExpr* ti) {
    return parset(ctx,loc,std::vector<IntTiExpr*>(), ti);
  }
  TiExpr*
  TiExpr::var(const ASTContext& ctx, const Location& loc,
              IntTiExpr* range0, BaseTiExpr* ti) {
    std::vector<IntTiExpr*> ranges;
    ranges.push_back(range0);
    return var(ctx, loc, ranges, ti);
  }
  TiExpr*
  TiExpr::par(const ASTContext& ctx, const Location& loc,
              IntTiExpr* range0, BaseTiExpr* ti) {
    std::vector<IntTiExpr*> ranges;
    ranges.push_back(range0);
    return par(ctx, loc, ranges, ti);
  }
  TiExpr*
  TiExpr::varset(const ASTContext& ctx, const Location& loc,
                 IntTiExpr* range0, BaseTiExpr* ti) {
    std::vector<IntTiExpr*> ranges;
    ranges.push_back(range0);
    return varset(ctx, loc, ranges, ti);
  }
  TiExpr*
  TiExpr::parset(const ASTContext& ctx, const Location& loc,
                 IntTiExpr* range0, BaseTiExpr* ti) {
    std::vector<IntTiExpr*> ranges;
    ranges.push_back(range0);
    return parset(ctx, loc, ranges, ti);
  }
  TiExpr*
  TiExpr::var(const ASTContext& ctx, const Location& loc,
              IntTiExpr* range0, IntTiExpr* range1,
              BaseTiExpr* ti) {
    std::vector<IntTiExpr*> ranges;
    ranges.push_back(range0);
    ranges.push_back(range1);
    return var(ctx, loc, ranges, ti);
  }
  TiExpr*
  TiExpr::par(const ASTContext& ctx, const Location& loc,
              IntTiExpr* range0, IntTiExpr* range1,
              BaseTiExpr* ti) {
    std::vector<IntTiExpr*> ranges;
    ranges.push_back(range0);
    ranges.push_back(range1);
    return par(ctx, loc, ranges, ti);
  }
  TiExpr*
  TiExpr::varset(const ASTContext& ctx, const Location& loc,
                 IntTiExpr* range0, IntTiExpr* range1,
                 BaseTiExpr* ti) {
    std::vector<IntTiExpr*> ranges;
    ranges.push_back(range0);
    ranges.push_back(range1);
    return varset(ctx, loc, ranges, ti);
  }
  TiExpr*
  TiExpr::parset(const ASTContext& ctx, const Location& loc,
                 IntTiExpr* range0, IntTiExpr* range1,
                 BaseTiExpr* ti) {
    std::vector<IntTiExpr*> ranges;
    ranges.push_back(range0);
    ranges.push_back(range1);
    return parset(ctx, loc, ranges, ti);
  }

  void
  TiExpr::addRanges(const ASTContext& ctx,
                    const std::vector<IntTiExpr*>& ranges) {
    assert(_ranges->empty());
    _ranges = CtxVec<IntTiExpr*>::a(ctx,ranges);
  }

  IntTiExpr*
  IntTiExpr::a(const ASTContext& ctx, Expression* domain) {
    return new (ctx) IntTiExpr(domain);
  }

  BoolTiExpr*
  BoolTiExpr::a(const ASTContext& ctx, const BoolDomain& domain) {
    return new (ctx) BoolTiExpr(domain);
  }

  FloatTiExpr*
  FloatTiExpr::a(const ASTContext& ctx, Expression* domain) {
    return new (ctx) FloatTiExpr(domain);
  }

  StringTiExpr*
  StringTiExpr::a(const ASTContext& ctx) {
    return new (ctx) StringTiExpr();
  }

  AnnTiExpr*
  AnnTiExpr::a(const ASTContext& ctx) {
    return new (ctx) AnnTiExpr();
  }

  IncludeI*
  IncludeI::a(const ASTContext& ctx, const Location& loc, CtxString* f) {
    IncludeI* i = new (ctx) IncludeI(loc);
    i->_f = f;
    return i;
  }

  VarDeclI*
  VarDeclI::a(const ASTContext& ctx, const Location& loc,
              VarDecl* e) {
    VarDeclI* vi = new (ctx) VarDeclI(loc);
    vi->_e = e;
    return vi;
  }

  AssignI*
  AssignI::a(const ASTContext& ctx, const Location& loc,
             const std::string& id, Expression* e) {
    AssignI* ai = new (ctx) AssignI(loc);
    ai->_id = CtxString::a(ctx,id);
    ai->_e = e;
    return ai;
  }

  ConstraintI*
  ConstraintI::a(const ASTContext& ctx, const Location& loc, 
                 Expression* e) {
    ConstraintI* ci = new (ctx) ConstraintI(loc);
    ci->_e = e;
    return ci;
  }

  SolveI*
  SolveI::sat(const ASTContext& ctx, const Location& loc, Annotation* ann) {
    SolveI* si = new (ctx) SolveI(loc);
    si->_ann = ann;
    si->_e = NULL;
    si->_st = ST_SAT;
    return si;
  }
  SolveI*
  SolveI::min(const ASTContext& ctx, const Location& loc,
              Expression* e, Annotation* ann) {
    SolveI* si = new (ctx) SolveI(loc);
    si->_ann = ann;
    si->_e = e;
    si->_st = ST_MIN;
    return si;
  }
  SolveI*
  SolveI::max(const ASTContext& ctx, const Location& loc,
              Expression* e, Annotation* ann) {
    SolveI* si = new (ctx) SolveI(loc);
    si->_ann = ann;
    si->_e = e;
    si->_st = ST_MAX;
    return si;
  }

  OutputI*
  OutputI::a(const ASTContext& ctx, const Location& loc,
             Expression* e) {
    OutputI* oi = new (ctx) OutputI(loc);
    oi->_e = e;
    return oi;
  }

  PredicateI*
  PredicateI::a(const ASTContext& ctx, const Location& loc,
                const std::string& id,
                const std::vector<VarDecl*>& params,
                Expression* e, Annotation* ann, bool test) {
    PredicateI* pi = new (ctx) PredicateI(loc);
    pi->_id = CtxString::a(ctx,id);
    pi->_params = CtxVec<VarDecl*>::a(ctx,params);
    pi->_ann = ann;
    pi->_e = e;
    pi->_test = test;
    return pi;
  }

  FunctionI*
  FunctionI::a(const ASTContext& ctx, const Location& loc,
               const std::string& id, TiExpr* ti,
               const std::vector<VarDecl*>& params,
               Expression* e, Annotation* ann) {
    FunctionI* fi = new (ctx) FunctionI(loc);
    fi->_id = CtxString::a(ctx,id);
    fi->_ti = ti;
    fi->_params = CtxVec<VarDecl*>::a(ctx,params);
    fi->_ann = ann;
    fi->_e = e;
    return fi;
  }

}
