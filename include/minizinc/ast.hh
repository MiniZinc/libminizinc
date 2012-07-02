#ifndef __MINIZINC_AST_HH__
#define __MINIZINC_AST_HH__

#include <minizinc/context.hh>

#include <utility>
#include <vector>

namespace MiniZinc {

  using std::pair;

  class Annotation;
  class IntLit;
  class FloatLit;
  class SetLit;
  class BoolLit;
  class StringLit;
  class Id;
  class AnonVar;
  class ArrayLit;
  class ArrayAccess;
  class Comprehension;
  class ITE;
  class BinOp;
  class UnOp;
  class Call;
  class VarDecl;
  class Let;
  class TiExpr;

  class Item;

  class ASTNode {
  public:

    void* operator new(size_t size, const ASTContext& c) throw() {
      return c.alloc(size);
    }

    void* operator new(size_t size, const ASTContext* c) throw() {
      return c->alloc(size);
    }

    void* operator new(size_t, void* n) throw() {
      return n;
    }

    void operator delete(void*, ASTContext&, unsigned) throw() { }
    void operator delete(void*, ASTContext*, unsigned) throw() { }
    void operator delete(void*, size_t) throw() { }
    void operator delete(void*, void*) throw() { }

  private:
    /// Disabled
    void* operator new(size_t) throw();
    /// Disabled
    void operator delete(void*) throw();
  };

  template<class T>
  class ASTVec {
  public:
    unsigned int _n;
    T _v[1];
  protected:
    ASTVec(const std::vector<T>& v) : _n(v.size()) {
      for (unsigned int i=_n; i--;)
        _v[i]=v[i];
    }
  public:
    static ASTVec* a(const ASTContext& ctx, const std::vector<T>& x) {
      ASTVec<T>* v = static_cast<ASTVec<T>*>(
        ctx.alloc(sizeof(ASTVec<T>)+(x.size()-1)*sizeof(T)));
      new (v) ASTVec<T>(x);
      return v;
    }

    static ASTVec* a(const ASTContext& ctx) {
      ASTVec<T>* v = static_cast<ASTVec<T>*>(
        ctx.alloc(sizeof(ASTVec<T>)));
      new (v) ASTVec<T>(std::vector<T>());
      return v;
    }

    bool empty(void) const { return _n==0; }
  };

  class Location {
  public:
    char* filename;
    unsigned int first_line;
    unsigned int first_column;
    unsigned int last_line;
    unsigned int last_column;
  };

  template<class Char, class Traits>
  std::basic_ostream<Char,Traits>&
  operator <<(std::basic_ostream<Char,Traits>& os, const Location& loc) {
    std::basic_ostringstream<Char,Traits> s;
    s.copyfmt(os); s.width(0);
    s << " in file " << loc.filename << "." << loc.first_line;
    return os << s.str();
  }

  class Expression : public ASTNode {
  public:
    Annotation* _ann;
    Location _loc;

    enum ExpressionId {
      E_INTLIT, E_FLOATLIT, E_SETLIT, E_BOOLLIT,
      E_STRINGLIT, E_ID, E_ANON, E_ARRAYLIT,
      E_ARRAYACCESS, E_COMP, E_ITE,
      E_BINOP, E_UNOP, E_CALL, E_VARDECL, E_LET,
      E_ANN, E_TI
    } _eid;

  protected:
    /// Constructor
    Expression(const Location& loc, const ExpressionId& eid)
      : _ann(NULL), _loc(loc), _eid(eid) {}

  public:

    template<class T> bool isa(void) const {
      return _eid==T::eid;
    }
    template<class T> T* cast(void) {
      assert(isa<T>());
      return static_cast<T*>(this);
    }
    template<class T> const T* cast(void) const {
      assert(isa<T>());
      return static_cast<const T*>(this);
    }
    template<class T> T* dyn_cast(void) {
      return isa<T>() ? static_cast<T*>(this) : NULL;
    }
    template<class T> const T* dyn_cast(void) const {
      return isa<T>() ? static_cast<const T*>(this) : NULL;
    }
    
    void annotate(Annotation* ann);
  };

  class Annotation : public Expression {
  protected:
    Annotation(const Location& loc, Expression* e)
     : Expression(loc,E_ANN), _e(e), _a(NULL) {}
  public:
    static const ExpressionId eid = E_ANN;
    Expression* _e;
    Annotation* _a;
    static Annotation* a(const ASTContext& ctx, const Location& loc,
                         Expression* e);
    void merge(Annotation* a);
  };
  
  class IntLit : public Expression {
  protected:
    IntLit(const Location& loc, int v)
      : Expression(loc,E_INTLIT), _v(v) {}
  public:
    static const ExpressionId eid = E_INTLIT;
    int _v;
    static IntLit* a(const ASTContext& ctx, const Location& loc,
                     int v);
  };
  class FloatLit : public Expression {
  protected:
    FloatLit(const Location& loc, double v)
      : Expression(loc,E_FLOATLIT), _v(v) {}
  public:
    static const ExpressionId eid = E_FLOATLIT;
    int _v;
    static FloatLit* a(const ASTContext& ctx, const Location& loc,
                       double v);
  };
  class SetLit : public Expression {
  protected:
    SetLit(const Location& loc) : Expression(loc,E_SETLIT) {}
  public:
    static const ExpressionId eid = E_SETLIT;
    ASTVec<Expression*>* _v;
    // RangeSet* _rs;
    static SetLit* a(const ASTContext& ctx,
                     const Location& loc,
                     const std::vector<Expression*>& v);
    static SetLit* a(const ASTContext& ctx,
                     const Location& loc,
                     Expression* m, Expression* n);
  };
  class BoolLit : public Expression {
  protected:
    BoolLit(const Location& loc, bool v)
      : Expression(loc,E_BOOLLIT), _v(v) {}
  public:
    static const ExpressionId eid = E_BOOLLIT;
    bool _v;
    static BoolLit* a(const ASTContext& ctx, const Location& loc,
                      bool v);
  };
  class StringLit : public Expression {
  protected:
    StringLit(const Location& loc, char* v)
      : Expression(loc,E_STRINGLIT), _v(v) {}
  public:
    static const ExpressionId eid = E_STRINGLIT;
    char* _v;
    static StringLit* a(const ASTContext& ctx, const Location& loc,
                        const std::string& v);
  };
  class Id : public Expression {
  protected:
    Id(const Location& loc, char* v, VarDecl* decl)
      : Expression(loc,E_ID), _v(v), _decl(decl) {}
  public:
    static const ExpressionId eid = E_ID;
    char* _v;
    VarDecl* _decl;
    static Id* a(const ASTContext& ctx, const Location& loc,
                 const std::string& v, VarDecl* decl);
  };
  class AnonVar : public Expression {
  protected:
    AnonVar(const Location& loc) : Expression(loc,E_ANON) {}
  public:
    static AnonVar* a(const ASTContext& ctx, const Location& loc);
  };
  class ArrayLit : public Expression {
  protected:
    ArrayLit(const Location& loc) : Expression(loc,E_ARRAYLIT) {}
  public:
    static const ExpressionId eid = E_ARRAYLIT;
    ASTVec<Expression*>* _v;
    ASTVec<pair<int,int> >* _dims;
    static ArrayLit* a(const ASTContext& ctx,
                       const Location& loc,
                       const std::vector<Expression*>& v,
                       const std::vector<pair<int,int> >& dims);
    static ArrayLit* a(const ASTContext& ctx,
                       const Location& loc,
                       const std::vector<Expression*>& v);
    static ArrayLit* a(const ASTContext& ctx,
                       const Location& loc,
                       const std::vector<std::vector<Expression*> >& v);
  };
  class ArrayAccess : public Expression {
  protected:
    ArrayAccess(const Location& loc) : Expression(loc,E_ARRAYACCESS) {}
  public:
    static const ExpressionId eid = E_ARRAYACCESS;
    Expression* _v;
    ASTVec<Expression*>* _idx;
    static ArrayAccess* a(const ASTContext& ctx,
                          const Location& loc,
                          Expression* v,
                          const std::vector<Expression*>& idx);
  };
  class Generator : public ASTNode {
  protected:
    Generator(void) {}
  public:
    ASTVec<VarDecl*>* _v;
    Expression* _in;
    static Generator* a(const ASTContext& ctx,
                        const std::vector<std::string>& v,
                        Expression* in);
  };
  struct Generators {
    std::vector<Generator*> _g;
    Expression* _w;
  };
  class Comprehension : public Expression {
  protected:
    Comprehension(const Location& loc) : Expression(loc,E_COMP) {}
  public:
    static const ExpressionId eid = E_COMP;
    Expression* _e;
    ASTVec<Generator*>* _g;
    Expression* _where;
    bool _set;
    static Comprehension* a(const ASTContext& ctx,
                            const Location& loc,
                            Expression* e,
                            Generators& g,
                            bool set);
  };
  class ITE : public Expression {
  public:
    static const ExpressionId eid = E_ITE;
    typedef pair<Expression*,Expression*> IfThen;
  protected:
    ITE(const Location& loc) : Expression(loc,E_ITE) {}
  public:
    ASTVec<IfThen>* _e_if;
    Expression* _e_else;
    static ITE* a(const ASTContext& ctx, const Location& loc,
                  const std::vector<IfThen>& e_if, Expression* e_else);
  };

  enum BinOpType {
    BOT_PLUS, BOT_MINUS, BOT_MULT, BOT_DIV, BOT_IDIV, BOT_MOD,
    BOT_LE, BOT_LQ, BOT_GR, BOT_GQ, BOT_EQ, BOT_NQ,
    BOT_IN, BOT_SUBSET, BOT_SUPERSET, BOT_UNION, BOT_DIFF, BOT_SYMDIFF,
    BOT_INTERSECT,
    BOT_PLUSPLUS,
    BOT_EQUIV, BOT_IMPL, BOT_RIMPL, BOT_OR, BOT_AND, BOT_XOR,
  };
  class BinOp : public Expression {
  protected:
    BinOp(const Location& loc, Expression* e0, BinOpType op, Expression* e1)
     : Expression(loc,E_BINOP), _e0(e0), _e1(e1), _op(op) {}
  public:
    static const ExpressionId eid = E_BINOP;
    Expression* _e0;
    Expression* _e1;
    BinOpType _op;
    static BinOp* a(const ASTContext& ctx, const Location& loc,
                    Expression* e0, BinOpType op, Expression* e1);
  };

  enum UnOpType {
    UOT_NOT, UOT_PLUS, UOT_MINUS
  };
  class UnOp : public Expression {
  protected:
    UnOp(const Location& loc, UnOpType op, Expression* e)
     : Expression(loc,E_UNOP), _e0(e), _op(op) {}
  public:
    static const ExpressionId eid = E_UNOP;
    Expression* _e0;
    UnOpType _op;
    static UnOp* a(const ASTContext& ctx, const Location& loc,
                   UnOpType op, Expression* e);
  };
  class Call : public Expression {
  protected:
    Call(const Location& loc) : Expression(loc, E_CALL) {}
  public:
    static const ExpressionId eid = E_CALL;
    char* _id;
    ASTVec<Expression*>* _args;
    Item* _decl;
    static Call* a(const ASTContext& ctx, const Location& loc,
                   const std::string& id,
                   const std::vector<Expression*>& args,
                   Item* decl=NULL);
  };
  class VarDecl : public Expression {
  protected:
    VarDecl(const Location& loc) : Expression(loc,E_VARDECL) {}
  public:
    static const ExpressionId eid = E_VARDECL;
    TiExpr* _ti;
    char* _id;
    Expression* _e;
    static VarDecl* a(const ASTContext& ctx, const Location& loc,
                      TiExpr* ti, const std::string& id, Expression* e=NULL);
  };
  class Let : public Expression {
  protected:
    Let(const Location& loc) : Expression(loc,E_LET) {}
  public:
    static const ExpressionId eid = E_LET;
    ASTVec<Expression*>* _let;
    Expression* _in;
    static Let* a(const ASTContext& ctx, const Location& loc,
                  const std::vector<Expression*>& let, Expression* in);
  };

  class BaseTiExpr : public ASTNode {
  public:
    enum TiExprId { TI_INT, TI_FLOAT, TI_BOOL, TI_STRING, TI_ANN } _tiid;
  protected:
    BaseTiExpr(const TiExprId& tiid) : _tiid(tiid) {}
  };
  class IntTiExpr;

  class TiExpr : public Expression {
  public:
    enum VarType { VT_PAR, VT_VAR, VT_SVAR };
  protected:
    TiExpr(const Location& loc, ASTVec<IntTiExpr*>* ranges,
           const VarType& vartype, bool set, BaseTiExpr* ti)
     : Expression(loc,E_TI), _ranges(ranges), _vartype(vartype),
       _set(set), _ti(ti) {}
  public:
    static const ExpressionId eid = E_TI;
    ASTVec<IntTiExpr*>* _ranges;
    VarType _vartype;
    bool _set;
    BaseTiExpr* _ti;
    static TiExpr* var(const ASTContext& ctx, const Location& loc,
                       const std::vector<IntTiExpr*>& ranges,
                       BaseTiExpr* ti);
    static TiExpr* par(const ASTContext& ctx, const Location& loc,
                       const std::vector<IntTiExpr*>& ranges,
                       BaseTiExpr* ti);
    static TiExpr* varset(const ASTContext& ctx, const Location& loc, 
                          const std::vector<IntTiExpr*>& ranges,
                          BaseTiExpr* ti);
    static TiExpr* parset(const ASTContext& ctx, const Location& loc,
                          const std::vector<IntTiExpr*>& ranges,
                          BaseTiExpr* ti);
    static TiExpr* var(const ASTContext& ctx, const Location& loc,
                       BaseTiExpr* ti);
    static TiExpr* par(const ASTContext& ctx, const Location& loc,
                       BaseTiExpr* ti);
    static TiExpr* varset(const ASTContext& ctx, const Location& loc, 
                          BaseTiExpr* ti);
    static TiExpr* parset(const ASTContext& ctx, const Location& loc, 
                          BaseTiExpr* ti);
    static TiExpr* var(const ASTContext& ctx, const Location& loc,
                       IntTiExpr* range0, BaseTiExpr* ti);
    static TiExpr* par(const ASTContext& ctx, const Location& loc,
                       IntTiExpr* range0, BaseTiExpr* ti);
    static TiExpr* varset(const ASTContext& ctx, const Location& loc,
                       IntTiExpr* range0, BaseTiExpr* ti);
    static TiExpr* parset(const ASTContext& ctx, const Location& loc,
                       IntTiExpr* range0, BaseTiExpr* ti);
    static TiExpr* var(const ASTContext& ctx, const Location& loc,
                       IntTiExpr* range0, IntTiExpr* range1,
                       BaseTiExpr* ti);
    static TiExpr* par(const ASTContext& ctx, const Location& loc,
                       IntTiExpr* range0, IntTiExpr* range1,
                       BaseTiExpr* ti);
    static TiExpr* varset(const ASTContext& ctx, const Location& loc,
                          IntTiExpr* range0, IntTiExpr* range1,
                          BaseTiExpr* ti);
    static TiExpr* parset(const ASTContext& ctx, const Location& loc,
                          IntTiExpr* range0, IntTiExpr* range1,
                          BaseTiExpr* ti);
    
    void addRanges(const ASTContext& ctx,
                   const std::vector<IntTiExpr*>& ranges);
  };

  class IntTiExpr : public BaseTiExpr {
  protected:
    IntTiExpr(Expression* domain)
     : BaseTiExpr(TI_INT), _domain(domain) {}
  public:
    static const TiExprId tiid = TI_INT;
    Expression* _domain;
    static IntTiExpr* a(const ASTContext& ctx, Expression* domain=NULL);
  };
  class BoolTiExpr : public BaseTiExpr  {
  protected:
    BoolTiExpr(void) : BaseTiExpr(TI_BOOL) {}
  public:
    static const TiExprId tiid = TI_BOOL;
    static BoolTiExpr* a(const ASTContext& ctx);
  };
  class FloatTiExpr : public BaseTiExpr {
  protected:
    FloatTiExpr(Expression* domain)
     : BaseTiExpr(TI_FLOAT), _domain(domain) {}
  public:
    static const TiExprId tiid = TI_FLOAT;
    Expression* _domain;
    static FloatTiExpr* a(const ASTContext& ctx, Expression* domain=NULL);
  };
  class StringTiExpr : public BaseTiExpr  {
  protected:
    StringTiExpr(void) : BaseTiExpr(TI_STRING) {}
  public:
    static const TiExprId tiid = TI_STRING;
    static StringTiExpr* a(const ASTContext& ctx);
  };
  class AnnTiExpr : public BaseTiExpr  {
  protected:
    AnnTiExpr(void) : BaseTiExpr(TI_ANN) {}
  public:
    static const TiExprId tiid = TI_ANN;
    static AnnTiExpr* a(const ASTContext& ctx);
  };


  class Item : public ASTNode {
  public:
    Location _loc;
    enum ItemId {
      II_INC, II_VD, II_ASN, II_CON, II_SOL,
      II_OUT, II_PRED, II_FUN
    } _iid;
    
  protected:
    /// Constructor
    Item(const Location& loc, const ItemId& iid)
      : _loc(loc), _iid(iid) {}

  public:

    template<class T> bool isa(void) const {
      return _iid==T::iid;
    }
    template<class T> T* cast(void) {
      assert(isa<T>());
      return static_cast<T*>(this);
    }
    template<class T> const T* cast(void) const {
      assert(isa<T>());
      return static_cast<const T*>(this);
    }
    template<class T> T* dyn_cast(void) {
      return isa<T>() ? static_cast<T*>(this) : NULL;
    }
    template<class T> const T* dyn_cast(void) const {
      return isa<T>() ? static_cast<const T*>(this) : NULL;
    }
  };

  class Model;
  class IncludeI : public Item {
  protected:
    IncludeI(const Location& loc) : Item(loc, II_INC) {}
  public:
    static const ItemId iid = II_INC;
    char* _f;
    Model* _m;
    bool _own;
    static IncludeI* a(const ASTContext& ctx, const Location& loc, char* f);
    void setModel(Model* m, bool own=true) {
      assert(_m==NULL); _m = m; _own = own;
    }
  };
  class VarDeclI : public Item {
  protected:
    VarDeclI(const Location& loc) : Item(loc, II_VD) {}
  public:
    static const ItemId iid = II_VD;
    VarDecl* _e;
    static VarDeclI* a(const ASTContext& ctx, const Location& loc,
                       VarDecl* e);
  };
  class AssignI : public Item {
  protected:
    AssignI(const Location& loc) : Item(loc, II_ASN) {}
  public:
    static const ItemId iid = II_ASN;
    char* _id;
    Expression* _e;
    static AssignI* a(const ASTContext& ctx, const Location& loc,
                      char* id, Expression* e);
  };
  class ConstraintI : public Item {
  protected:
    ConstraintI(const Location& loc) : Item(loc, II_CON) {}
  public:
    static const ItemId iid = II_CON;
    Expression* _e;
    static ConstraintI* a(const ASTContext& ctx, const Location& loc, 
                          Expression* e);
  };
  class SolveI : public Item {
  protected:
    SolveI(const Location& loc) : Item(loc, II_SOL) {}
  public:
    static const ItemId iid = II_SOL;
    Annotation* _ann;
    Expression* _e;
    enum SolveType { ST_SAT, ST_MIN, ST_MAX } _st;
    static SolveI* sat(const ASTContext& ctx, const Location& loc,
                       Annotation* ann = NULL);
    static SolveI* min(const ASTContext& ctx, const Location& loc,
                       Expression* e, Annotation* ann = NULL);
    static SolveI* max(const ASTContext& ctx, const Location& loc,
                       Expression* e, Annotation* ann = NULL);
  };
  class OutputI : public Item {
  protected:
    OutputI(const Location& loc) : Item(loc, II_OUT) {}
  public:
    static const ItemId iid = II_OUT;
    Expression* _e;
    static OutputI* a(const ASTContext& ctx, const Location& loc,
                      Expression* e);
  };
  class PredicateI : public Item {
  protected:
    PredicateI(const Location& loc) : Item(loc, II_PRED) {}
  public:
    static const ItemId iid = II_PRED;
    char* _id;
    ASTVec<VarDecl*>* _params;
    Annotation* _ann;
    Expression* _e;
    bool _test;
    static PredicateI* a(const ASTContext& ctx, const Location& loc,
                         const std::string& id,
                         const std::vector<VarDecl*>& params,
                         Expression* e = NULL, Annotation* ann = NULL,
                         bool test = false);
  };
  class FunctionI : public Item {
  protected:
    FunctionI(const Location& loc) : Item(loc, II_FUN) {}
  public:
    static const ItemId iid = II_FUN;
    char* _id;
    TiExpr* _ti;
    ASTVec<VarDecl*>* _params;
    Annotation* _ann;
    Expression* _e;
    static FunctionI* a(const ASTContext& ctx, const Location& loc,
                        const std::string& id, TiExpr* ti,
                        const std::vector<VarDecl*>& params,
                        Expression* e = NULL, Annotation* ann = NULL);
  };

  /**
   * \brief Visitor for expressions
   *
   * This class implements no-ops for all expression types.
   * Override the methods to implement custom behaviour.
   */
  class EVisitor {
  public:
    /// Visit integer literal
    void vIntLit(const IntLit&) {}
    /// Visit floating point literal
    void vFloatLit(const FloatLit&) {}
    /// Visit Boolean literal
    void vBoolLit(const BoolLit&) {}
    /// Visit set literal
    void vSetLit(const SetLit&) {}
    /// Visit string literal
    void vStringLit(const StringLit&) {}
    /// Visit identifier
    void vId(const Id&) {}
    /// Visit anonymous variable
    void vAnon(const AnonVar&) {}
    /// Visit array literal
    void vArrayLit(const ArrayLit&) {}
    /// Visit array access
    void vArrayAccess(const ArrayAccess&) {}
    /// Visit array comprehension
    void vComprehension(const Comprehension&) {}
    /// Visit if-then-else
    void vITE(const ITE&) {}
    /// Visit binary operator
    void vBinOp(const BinOp&) {}
    /// Visit unary operator
    void vUnOp(const UnOp&) {}
    /// Visit call
    void vCall(const Call&) {}
    /// Visit let
    void vLet(const Let&) {}
    /// Visit variable declaration
    void vVarDecl(const VarDecl&) {}
  };

  template<class Visitor>
  class BottomUpVisitor {
  
  };

  template<class Visitor>
  class TopDownVisitor {
  
  };

}

#endif
