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
    return new (ctx) StringLit(loc,CtxStringH(ctx,v));
  }

  Id*
  Id::a(const ASTContext& ctx, const Location& loc,
        const std::string& v, VarDecl* decl) {
    return new (ctx) Id(loc,CtxStringH(ctx,v),decl);
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
               const std::vector<CtxStringH>& v,
               Expression* in) {
    Generator* g = new (ctx) Generator();
    std::vector<VarDecl*> vd(v.size());
    for (unsigned int i=0; i<v.size(); i++)
      vd[i] = VarDecl::a(ctx,in->_loc,
        TypeInst::a(ctx,in->_loc,Type::parint()),v[i]);
    g->_v = CtxVec<VarDecl*>::a(ctx,vd);
    g->_in = in;
    return g;
  }
  Generator*
  Generator::a(const ASTContext& ctx,
               const std::vector<std::string>& v,
               Expression* in) {
    std::vector<CtxStringH> vv(v.size());
    for (unsigned int i=0; i<v.size(); i++)
      vv[i] = CtxStringH(ctx,v[i]);
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

  namespace {
    class OpToString {
    public:
      ASTContext ctx;
      
      CtxStringH sBOT_PLUS;
      CtxStringH sBOT_MINUS;
      CtxStringH sBOT_MULT;
      CtxStringH sBOT_DIV;
      CtxStringH sBOT_IDIV;
      CtxStringH sBOT_MOD;
      CtxStringH sBOT_LE;
      CtxStringH sBOT_LQ;
      CtxStringH sBOT_GR;
      CtxStringH sBOT_GQ;
      CtxStringH sBOT_EQ;
      CtxStringH sBOT_NQ;
      CtxStringH sBOT_IN;
      CtxStringH sBOT_SUBSET;
      CtxStringH sBOT_SUPERSET;
      CtxStringH sBOT_UNION;
      CtxStringH sBOT_DIFF;
      CtxStringH sBOT_SYMDIFF;
      CtxStringH sBOT_INTERSECT;
      CtxStringH sBOT_PLUSPLUS;
      CtxStringH sBOT_EQUIV;
      CtxStringH sBOT_IMPL;
      CtxStringH sBOT_RIMPL;
      CtxStringH sBOT_OR;
      CtxStringH sBOT_AND;
      CtxStringH sBOT_XOR;
      CtxStringH sBOT_DOTDOT;
      CtxStringH sBOT_NOT;
      
      OpToString(void) {
        sBOT_PLUS = CtxStringH(ctx, "+");
        sBOT_MINUS = CtxStringH(ctx, "-");
        sBOT_MULT = CtxStringH(ctx, "*");
        sBOT_DIV = CtxStringH(ctx, "/");
        sBOT_IDIV = CtxStringH(ctx, "div");
        sBOT_MOD = CtxStringH(ctx, "mod");
        sBOT_LE = CtxStringH(ctx, "<");
        sBOT_LQ = CtxStringH(ctx, "<=");
        sBOT_GR = CtxStringH(ctx, ">");
        sBOT_GQ = CtxStringH(ctx, ">=");
        sBOT_EQ = CtxStringH(ctx, "=");
        sBOT_NQ = CtxStringH(ctx, "!=");
        sBOT_IN = CtxStringH(ctx, "in");
        sBOT_SUBSET = CtxStringH(ctx, "subset");
        sBOT_SUPERSET = CtxStringH(ctx, "superset");
        sBOT_UNION = CtxStringH(ctx, "union");
        sBOT_DIFF = CtxStringH(ctx, "diff");
        sBOT_SYMDIFF = CtxStringH(ctx, "symdiff");
        sBOT_INTERSECT = CtxStringH(ctx, "intersect");
        sBOT_PLUSPLUS = CtxStringH(ctx, "++");
        sBOT_EQUIV = CtxStringH(ctx, "<->");
        sBOT_IMPL = CtxStringH(ctx, "->");
        sBOT_RIMPL = CtxStringH(ctx, "<-");
        sBOT_OR = CtxStringH(ctx, "\\/");
        sBOT_AND = CtxStringH(ctx, "/\\");
        sBOT_XOR = CtxStringH(ctx, "xor");
        sBOT_DOTDOT = CtxStringH(ctx, "..");
        sBOT_NOT = CtxStringH(ctx, "not");
      }
    } _opToString;
  }

  CtxStringH
  BinOp::opToString(void) const {
    switch (_op) {
    case BOT_PLUS: return _opToString.sBOT_PLUS;
    case BOT_MINUS: return _opToString.sBOT_MINUS;
    case BOT_MULT: return _opToString.sBOT_MULT;
    case BOT_DIV: return _opToString.sBOT_DIV;
    case BOT_IDIV: return _opToString.sBOT_IDIV;
    case BOT_MOD: return _opToString.sBOT_MOD;
    case BOT_LE: return _opToString.sBOT_LE;
    case BOT_LQ: return _opToString.sBOT_LQ;
    case BOT_GR: return _opToString.sBOT_GR;
    case BOT_GQ: return _opToString.sBOT_GQ;
    case BOT_EQ: return _opToString.sBOT_EQ;
    case BOT_NQ: return _opToString.sBOT_NQ;
    case BOT_IN: return _opToString.sBOT_IN;
    case BOT_SUBSET: return _opToString.sBOT_SUBSET;
    case BOT_SUPERSET: return _opToString.sBOT_SUPERSET;
    case BOT_UNION: return _opToString.sBOT_UNION;
    case BOT_DIFF: return _opToString.sBOT_DIFF;
    case BOT_SYMDIFF: return _opToString.sBOT_SYMDIFF;
    case BOT_INTERSECT: return _opToString.sBOT_INTERSECT;
    case BOT_PLUSPLUS: return _opToString.sBOT_PLUSPLUS;
    case BOT_EQUIV: return _opToString.sBOT_EQUIV;
    case BOT_IMPL: return _opToString.sBOT_IMPL;
    case BOT_RIMPL: return _opToString.sBOT_RIMPL;
    case BOT_OR: return _opToString.sBOT_OR;
    case BOT_AND: return _opToString.sBOT_AND;
    case BOT_XOR: return _opToString.sBOT_XOR;
    case BOT_DOTDOT: return _opToString.sBOT_DOTDOT;
    default: assert(false);
    }
  }

  UnOp*
  UnOp::a(const ASTContext& ctx, const Location& loc,
          UnOpType op, Expression* e) {
    return new (ctx) UnOp(loc,op,e);
  }

  CtxStringH
  UnOp::opToString(void) const {
    switch (_op) {
    case UOT_PLUS: return _opToString.sBOT_PLUS;
    case UOT_MINUS: return _opToString.sBOT_MINUS;
    case UOT_NOT: return _opToString.sBOT_NOT;
    default: assert(false);
    }
  }

  Call*
  Call::a(const ASTContext& ctx, const Location& loc,
          const std::string& id,
          const std::vector<Expression*>& args,
          FunctionI* decl) {
    Call* c = new (ctx) Call(loc);
    c->_id = CtxStringH(ctx,id);
    c->_args = CtxVec<Expression*>::a(ctx,args);
    c->_decl = decl;
    return c;
  }

  VarDecl*
  VarDecl::a(const ASTContext& ctx, const Location& loc,
             TypeInst* ti, const CtxStringH& id, Expression* e) {
    VarDecl* v = new (ctx) VarDecl(loc,ti->_type);
    v->_ti = ti;
    v->_id = id;
    v->_e = e;
    return v;
  }
  VarDecl*
  VarDecl::a(const ASTContext& ctx, const Location& loc,
             TypeInst* ti, const std::string& id, Expression* e) {
    return a(ctx,loc,ti,CtxStringH(ctx,id));
  }

  Let*
  Let::a(const ASTContext& ctx, const Location& loc,
         const std::vector<Expression*>& let, Expression* in) {
    Let* l = new (ctx) Let(loc);
    l->_let = CtxVec<Expression*>::a(ctx,let);
    l->_in = in;
    return l;
  }

  TypeInst*
  TypeInst::a(const ASTContext& ctx, const Location& loc,
              const Type& type, Expression* domain,
              CtxVec<Expression*>* ranges) {
    return new (ctx) TypeInst(loc,type,domain,ranges);                     
  }

  void
  TypeInst::addRanges(const ASTContext& ctx,
                      const std::vector<Expression*>& ranges) {
    assert(_ranges == NULL);
    _ranges = CtxVec<Expression*>::a(ctx,ranges);
  }

  IncludeI*
  IncludeI::a(const ASTContext& ctx, const Location& loc,
              const CtxStringH& f) {
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
    ai->_id = CtxStringH(ctx,id);
    ai->_e = e;
    ai->_decl = NULL;
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

  FunctionI*
  FunctionI::a(const ASTContext& ctx, const Location& loc,
               const std::string& id, TypeInst* ti,
               const std::vector<VarDecl*>& params,
               Expression* e, Annotation* ann) {
    FunctionI* fi = new (ctx) FunctionI(loc);
    fi->_id = CtxStringH(ctx,id);
    fi->_ti = ti;
    fi->_params = CtxVec<VarDecl*>::a(ctx,params);
    fi->_ann = ann;
    fi->_e = e;
    return fi;
  }

  bool
  FunctionI::match(const std::vector<Type>& ta) {
    if (_params->size() != ta.size())
      return false;
    for (unsigned int i=0; i<ta.size(); i++)
      if (!ta[i].isSubtypeOf((*_params)[i]->_type))
        return false;
    return true;
    
  }

}
