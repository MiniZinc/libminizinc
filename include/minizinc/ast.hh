#ifndef __MINIZINC_AST_HH__
#define __MINIZINC_AST_HH__

#include <minizinc/context.hh>
#include <minizinc/type.hh>

#include <utility>
#include <vector>

#include <iostream>

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
  class TypeInst;

  class Item;

  /**
   * \brief Base class for abstract syntax tree nodes
   */
  class ASTNode {
  public:

    /// Allocate node from context
    void* operator new(size_t size, const ASTContext& c) throw() {
      return c.alloc(size);
    }

    /// Placement-new
    void* operator new(size_t, void* n) throw() {
      return n;
    }

    /// Delete node (no-op)
    void operator delete(void*, ASTContext&, unsigned) throw() { }
    /// Delete node (no-op)
    void operator delete(void*, size_t) throw() { }
    /// Delete node (no-op)
    void operator delete(void*, void*) throw() { }

  private:
    /// Disabled
    void* operator new(size_t) throw();
    /// Disabled
    void operator delete(void*) throw();
  };


  /// %Location of an expression in the source code
  class Location {
  public:
    /// Source code file name (context-allocated) or NULL
    CtxString* filename;
    /// Line where expression starts
    unsigned int first_line;
    /// Column where expression starts
    unsigned int first_column;
    /// Line where expression ends
    unsigned int last_line;
    /// Column where expression ends
    unsigned int last_column;
    
    /// Allocate empty location
    static Location a(void);
  };

  /// Output operator for locations
  template<class Char, class Traits>
  std::basic_ostream<Char,Traits>&
  operator <<(std::basic_ostream<Char,Traits>& os, const Location& loc) {
    std::basic_ostringstream<Char,Traits> s;
    s.copyfmt(os); s.width(0);
    s << " in file " << loc.filename->str() << ":" << loc.first_line;
    return os << s.str();
  }

  /**
   * \brief Base class for expressions
   */
  class Expression : public ASTNode {
  public:
    /// An annotation (or NULL)
    Annotation* _ann;
    /// The location of the expression
    Location _loc;

    /// Identifier of the concrere expression type
    enum ExpressionId {
      E_INTLIT, E_FLOATLIT, E_SETLIT, E_BOOLLIT,
      E_STRINGLIT, E_ID, E_ANON, E_ARRAYLIT,
      E_ARRAYACCESS, E_COMP, E_ITE,
      E_BINOP, E_UNOP, E_CALL, E_VARDECL, E_LET,
      E_ANN, E_TI, E_TIID
    } _eid;

    /// The %MiniZinc type of the expression
    Type _type;

  protected:
    /// Constructor
    Expression(const Location& loc, const ExpressionId& eid, const Type& t)
      : _ann(NULL), _loc(loc), _eid(eid), _type(t) {}

  public:

    /// Test if expression is of type \a T
    template<class T> bool isa(void) const {
      return _eid==T::eid;
    }
    /// Cast expression to type \a T*
    template<class T> T* cast(void) {
      assert(isa<T>());
      return static_cast<T*>(this);
    }
    /// Cast expression to type \a const T*
    template<class T> const T* cast(void) const {
      assert(isa<T>());
      return static_cast<const T*>(this);
    }
    /// Cast expression to type \a T* or NULL if types do not match
    template<class T> T* dyn_cast(void) {
      return isa<T>() ? static_cast<T*>(this) : NULL;
    }
    /// Cast expression to type \a const T* or NULL if types do not match
    template<class T> const T* dyn_cast(void) const {
      return isa<T>() ? static_cast<const T*>(this) : NULL;
    }

    /// Add annotation \a ann to the expression
    void annotate(Annotation* ann);
  };

  /**
   * \brief Annotations
   */
  class Annotation : public Expression {
  protected:
    /// Constructor
    Annotation(const Location& loc, Expression* e)
     : Expression(loc,E_ANN,Type::ann()), _e(e), _a(NULL) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ANN;
    /// The actual annotation expression
    Expression* _e;
    /// The next annotation in a list or NULL
    Annotation* _a;
    /// Allocate annotation \a e
    static Annotation* a(const ASTContext& ctx, const Location& loc,
                         Expression* e);
    /// Add annotation \a a to end of list of annotations
    void merge(Annotation* a);
  };
  
  /// \brief Integer literal expression
  class IntLit : public Expression {
  protected:
    /// Constructor
    IntLit(const Location& loc, int v)
      : Expression(loc,E_INTLIT,Type::parint()), _v(v) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_INTLIT;
    /// The value of this expression
    int _v;
    /// Allocate from context
    static IntLit* a(const ASTContext& ctx, const Location& loc,
                     int v);
  };
  /// \brief Float literal expression
  class FloatLit : public Expression {
  protected:
    FloatLit(const Location& loc, double v)
      : Expression(loc,E_FLOATLIT,Type::parfloat()), _v(v) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_FLOATLIT;
    /// The value of this expression
    double _v;
    /// Allocate from context
    static FloatLit* a(const ASTContext& ctx, const Location& loc,
                       double v);
  };
  /// \brief Set literal expression
  class SetLit : public Expression {
  protected:
    SetLit(const Location& loc) : Expression(loc,E_SETLIT,Type()) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_SETLIT;
    /// The value of this expression, or NULL
    CtxVec<Expression*>* _v;
    /// TODO
    // RangeSet* _rs;
    /// Allocate set \$f\{v1,\dots,vn\}\$f from context
    static SetLit* a(const ASTContext& ctx,
		     const Location& loc,
                     const std::vector<Expression*>& v);
  };
  /// \brief Boolean literal expression
  class BoolLit : public Expression {
  protected:
    /// Constructor
    BoolLit(const Location& loc, bool v)
      : Expression(loc,E_BOOLLIT,Type::parbool()), _v(v) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_BOOLLIT;
    /// The value of this expression
    bool _v;
    /// Allocate from context
    static BoolLit* a(const ASTContext& ctx, const Location& loc,
                      bool v);
  };
  /// \brief String literal expression
  class StringLit : public Expression {
  protected:
    /// Constructor
    StringLit(const Location& loc, CtxStringH v)
      : Expression(loc,E_STRINGLIT,Type::parstring()), _v(v) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_STRINGLIT;
    /// The value of this expression (context-allocated)
    CtxStringH _v;
    /// Allocate from context
    static StringLit* a(const ASTContext& ctx, const Location& loc,
                        const std::string& v);
  };
  /// \brief Identifier expression
  class Id : public Expression {
  protected:
    /// Constructor
    Id(const Location& loc, CtxStringH v, VarDecl* decl)
      : Expression(loc,E_ID,Type()), _v(v), _decl(decl) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ID;
    /// The string identifier (context-allocated)
    CtxStringH _v;
    /// The declaration corresponding to this identifier (may be NULL)
    VarDecl* _decl;
    /// Allocate from context (\a decl may be NULL)
    static Id* a(const ASTContext& ctx, const Location& loc,
                 const std::string& v, VarDecl* decl);
  };
  /// \brief Type-inst identifier expression
  class TIId : public Expression {
  protected:
    /// Constructor
    TIId(const Location& loc, CtxStringH v)
      : Expression(loc,E_TIID,Type()), _v(v) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_TIID;
    /// The string identifier (context-allocated)
    CtxStringH _v;
    /// Allocate from context
    static TIId* a(const ASTContext& ctx, const Location& loc,
                   const std::string& v);
  };
  /// \brief Anonymous variable expression
  class AnonVar : public Expression {
  protected:
    /// Constructor
    AnonVar(const Location& loc) : Expression(loc,E_ANON,Type::any()) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ANON;
    /// Allocate from context
    static AnonVar* a(const ASTContext& ctx, const Location& loc);
  };
  /// \brief Array literal expression
  class ArrayLit : public Expression {
  protected:
    /// Constructor
    ArrayLit(const Location& loc) : Expression(loc,E_ARRAYLIT,Type()) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ARRAYLIT;
    /// The array
    CtxVec<Expression*>* _v;
    /// The declared array dimensions
    CtxVec<pair<int,int> >* _dims;
    /// Allocate from context
    static ArrayLit* a(const ASTContext& ctx,
                       const Location& loc,
                       const std::vector<Expression*>& v,
                       const std::vector<pair<int,int> >& dims);
    /// Allocate from context (one-dimensional)
    static ArrayLit* a(const ASTContext& ctx,
                       const Location& loc,
                       const std::vector<Expression*>& v);
    /// Allocate from context (two-dimensional)
    static ArrayLit* a(const ASTContext& ctx,
                       const Location& loc,
                       const std::vector<std::vector<Expression*> >& v);
  };
  /// \brief Array access expression
  class ArrayAccess : public Expression {
  protected:
    /// Constructor
    ArrayAccess(const Location& loc) : Expression(loc,E_ARRAYACCESS,Type()) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ARRAYACCESS;
    /// The array to access
    Expression* _v;
    /// The indexes (for all array dimensions)
    CtxVec<Expression*>* _idx;
    /// Allocate from context
    static ArrayAccess* a(const ASTContext& ctx,
                          const Location& loc,
                          Expression* v,
                          const std::vector<Expression*>& idx);
  };
  /**
   * \brief Generators for comprehensions
   *
   * A generator consists of a list of variable declarations, one for
   * each generated variable, and the expression to generate. E.g.,
   * the Zinc expression [ x[i,j,k] | i,j in 1..10, k in 1..5] contains
   * two generators. The first one has variable declarations for i and j
   * and the expression 1..10, and the second one has a variable declaration
   * for k and the expression 1..5.
   *
   */
  class Generator : public ASTNode {
  protected:
    /// Constructor
    Generator(void) {}
  public:
    /// Variable declarations
    CtxVec<VarDecl*>* _v;
    /// in-expression
    Expression* _in;
    /// Allocate from context
    static Generator* a(const ASTContext& ctx,
                        const std::vector<std::string>& v,
                        Expression* in);
    /// Allocate from context
    static Generator* a(const ASTContext& ctx,
                        const std::vector<CtxStringH>& v,
                        Expression* in);
  };
  /// \brief A list of generators with one where-expression
  struct Generators {
    /// %Generators
    std::vector<Generator*> _g;
    //// where-expression
    Expression* _w;
  };
  /// \brief An expression representing an array- or set-comprehension
  class Comprehension : public Expression {
  protected:
    /// Constructor
    Comprehension(const Location& loc) : Expression(loc,E_COMP,Type()) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_COMP;
    /// The expression to generate
    Expression* _e;
    /// A list of generators
    CtxVec<Generator*>* _g;
    /// The where-clause (or NULL)
    Expression* _where;
    /// Whether this is a set (true) or array (false) comprehension
    bool _set;
    /// Allocate from context
    static Comprehension* a(const ASTContext& ctx,
                            const Location& loc,
                            Expression* e,
                            Generators& g,
                            bool set);
  };
  /// \brief If-then-else expression
  class ITE : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ITE;
    /// Type of if-then pairs
    typedef pair<Expression*,Expression*> IfThen;
  protected:
    /// Constructor
    ITE(const Location& loc) : Expression(loc,E_ITE,Type()) {}
  public:
    /// List of if-then-pairs
    CtxVec<IfThen>* _e_if;
    /// Else-expression
    Expression* _e_else;
    /// Allocate from context
    static ITE* a(const ASTContext& ctx, const Location& loc,
                  const std::vector<IfThen>& e_if, Expression* e_else);
  };

  /// Type of binary operators
  enum BinOpType {
    BOT_PLUS, BOT_MINUS, BOT_MULT, BOT_DIV, BOT_IDIV, BOT_MOD,
    BOT_LE, BOT_LQ, BOT_GR, BOT_GQ, BOT_EQ, BOT_NQ,
    BOT_IN, BOT_SUBSET, BOT_SUPERSET, BOT_UNION, BOT_DIFF, BOT_SYMDIFF,
    BOT_INTERSECT,
    BOT_PLUSPLUS,
    BOT_EQUIV, BOT_IMPL, BOT_RIMPL, BOT_OR, BOT_AND, BOT_XOR,
    BOT_DOTDOT
  };
  /// \brief Binary-operator expression
  class BinOp : public Expression {
  protected:
    /// Constructor
    BinOp(const Location& loc, Expression* e0, BinOpType op, Expression* e1)
     : Expression(loc,E_BINOP,Type()), _e0(e0), _e1(e1), _op(op) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_BINOP;
    /// Left hand side expression
    Expression* _e0;
    /// Right hand side expression
    Expression* _e1;
    /// Operator type
    BinOpType _op;
    /// Allocate from context
    static BinOp* a(const ASTContext& ctx, const Location& loc,
                    Expression* e0, BinOpType op, Expression* e1);
    CtxStringH opToString(void) const;
  };

  /// Type of unary operators
  enum UnOpType {
    UOT_NOT, UOT_PLUS, UOT_MINUS
  };
  /// \brief Unary-operator expressions
  class UnOp : public Expression {
  protected:
    /// Constructor
    UnOp(const Location& loc, UnOpType op, Expression* e)
     : Expression(loc,E_UNOP,Type()), _e0(e), _op(op) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_UNOP;
    /// %Expression
    Expression* _e0;
    /// Operator type
    UnOpType _op;
    /// Allocate from context
    static UnOp* a(const ASTContext& ctx, const Location& loc,
                   UnOpType op, Expression* e);
    CtxStringH opToString(void) const;
  };
  
  /// \brief A predicate or function call expression
  class Call : public Expression {
  protected:
    /// Constructor
    Call(const Location& loc) : Expression(loc, E_CALL,Type()) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_CALL;
    /// Identifier of called predicate or function (context-allocated)
    CtxStringH _id;
    /// Arguments to the call
    CtxVec<Expression*>* _args;
    /// The predicate or function declaration (or NULL)
    FunctionI* _decl;
    static Call* a(const ASTContext& ctx, const Location& loc,
                   const std::string& id,
                   const std::vector<Expression*>& args,
                   FunctionI* decl=NULL);
  };
  /// \brief A variable declaration expression
  class VarDecl : public Expression {
  protected:
    /// Constructor
    VarDecl(const Location& loc, const Type& t)
     : Expression(loc,E_VARDECL,t) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_VARDECL;
    /// Type-inst of the declared variable
    TypeInst* _ti;
    /// Identifier (context-allocated)
    CtxStringH _id;
    /// Initialisation expression (can be NULL)
    Expression* _e;
    /// Allocate from context
    static VarDecl* a(const ASTContext& ctx, const Location& loc,
                      TypeInst* ti, const std::string& id, Expression* e=NULL);
    /// Allocate from context
    static VarDecl* a(const ASTContext& ctx, const Location& loc,
                      TypeInst* ti, const CtxStringH& id, Expression* e=NULL);
  };
  /// \brief %Let expression
  class Let : public Expression {
  protected:
    /// Constructor
    Let(const Location& loc) : Expression(loc,E_LET,Type()) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_LET;
    /// List of local declarations
    CtxVec<Expression*>* _let;
    /// Body of the let
    Expression* _in;
    /// Allocate from context
    static Let* a(const ASTContext& ctx, const Location& loc,
                  const std::vector<Expression*>& let, Expression* in);
  };

  /// \brief Type-inst expression
  class TypeInst : public Expression {
  protected:
    /// Constructor
    TypeInst(const Location& loc, const Type& type,
             Expression* domain=NULL,
             CtxVec<Expression*>* ranges=NULL)
     : Expression(loc,E_TI,type), _ranges(ranges), _domain(domain) {}
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_TI;
    /// Ranges of an array expression
    CtxVec<Expression*>* _ranges;
    /// Declared domain (or NULL)
    Expression* _domain;
    /// Allocate from context
    static TypeInst* a(const ASTContext& ctx, const Location& loc,
                       const Type& t, Expression* domain=NULL,
                       CtxVec<Expression*>* ranges=NULL);
    
    /// Add \a ranges to expression
    void addRanges(const ASTContext& ctx,
                   const std::vector<Expression*>& ranges);
    bool isarray(void) const { return _ranges && _ranges->size()>0; }
    bool hasTiVariable(void) const;
  };

  /**
   * \brief Base-class for items
   */
  class Item : public ASTNode {
  public:
    /// Location of the item
    Location _loc;
    /// Identifier of the concrete item type
    enum ItemId {
      II_INC, II_VD, II_ASN, II_CON, II_SOL,
      II_OUT, II_FUN
    } _iid;
    
  protected:
    /// Constructor
    Item(const Location& loc, const ItemId& iid)
      : _loc(loc), _iid(iid) {}

  public:

    /// Test if item is of type \a T
    template<class T> bool isa(void) const {
      return _iid==T::iid;
    }
    /// Cast item to type \a T*
    template<class T> T* cast(void) {
      assert(isa<T>());
      return static_cast<T*>(this);
    }
    /// Cast expression to type \a const T*
    template<class T> const T* cast(void) const {
      assert(isa<T>());
      return static_cast<const T*>(this);
    }
    /// Cast item to type \a T* or NULL if types do not match
    template<class T> T* dyn_cast(void) {
      return isa<T>() ? static_cast<T*>(this) : NULL;
    }
    /// Cast item to type \a const T* or NULL if types do not match
    template<class T> const T* dyn_cast(void) const {
      return isa<T>() ? static_cast<const T*>(this) : NULL;
    }
  };

  class Model;
  /// \brief Include item
  class IncludeI : public Item {
  protected:
    /// Constructor
    IncludeI(const Location& loc) : Item(loc, II_INC) {}
  public:
    /// The identifier of this item type
    static const ItemId iid = II_INC;
    /// Filename to include (context-allocated)
    CtxStringH _f;
    /// Model for that file
    Model* _m;
    /// Whether this include-item owns the model
    bool _own;
    /// Allocate from context
    static IncludeI* a(const ASTContext& ctx, const Location& loc,
                       const CtxStringH& f);
    /// Set the model
    void setModel(Model* m, bool own=true) {
      assert(_m==NULL); _m = m; _own = own;
    }
  };
  /// \brief Variable declaration item
  class VarDeclI : public Item {
  protected:
    VarDeclI(const Location& loc) : Item(loc, II_VD) {}
  public:
    /// The identifier of this item type
    static const ItemId iid = II_VD;
    /// The declaration expression
    VarDecl* _e;
    /// Allocate from context
    static VarDeclI* a(const ASTContext& ctx, const Location& loc,
                       VarDecl* e);
  };
  /// \brief Assign item
  class AssignI : public Item {
  protected:
    /// Constructor
    AssignI(const Location& loc) : Item(loc, II_ASN) {}
  public:
    /// The identifier of this item type
    static const ItemId iid = II_ASN;
    /// Identifier of variable to assign to (context-allocated)
    CtxStringH _id;
    /// Expression to assign to the variable
    Expression* _e;
    /// Declaration of the variable to assign to
    VarDecl* _decl;
    /// Allocate from context
    static AssignI* a(const ASTContext& ctx, const Location& loc,
                      const std::string& id, Expression* e);
  };
  /// \brief Constraint item
  class ConstraintI : public Item {
  protected:
    /// Constructor
    ConstraintI(const Location& loc) : Item(loc, II_CON) {}
  public:
    /// The identifier of this item type
    static const ItemId iid = II_CON;
    /// Constraint expression
    Expression* _e;
    /// Allocate from context
    static ConstraintI* a(const ASTContext& ctx, const Location& loc, 
                          Expression* e);
  };
  /// \brief Solve item
  class SolveI : public Item {
  protected:
    /// Constructor
    SolveI(const Location& loc) : Item(loc, II_SOL) {}
  public:
    /// The identifier of this item type
    static const ItemId iid = II_SOL;
    /// Solve item annotation
    Annotation* _ann;
    /// Expression for minimisation/maximisation (or NULL)
    Expression* _e;
    /// Type of solving
    enum SolveType { ST_SAT, ST_MIN, ST_MAX } _st;
    /// Allocate solve satisfy item from context
    static SolveI* sat(const ASTContext& ctx, const Location& loc,
                       Annotation* ann = NULL);
    /// Allocate solve minimize item from context
    static SolveI* min(const ASTContext& ctx, const Location& loc,
                       Expression* e, Annotation* ann = NULL);
    /// Allocate solve maximize item from context
    static SolveI* max(const ASTContext& ctx, const Location& loc,
                       Expression* e, Annotation* ann = NULL);
  };
  /// \brief Output item
  class OutputI : public Item {
  protected:
    /// Constructor
    OutputI(const Location& loc) : Item(loc, II_OUT) {}
  public:
    /// The identifier of this item type
    static const ItemId iid = II_OUT;
    /// Expression to output
    Expression* _e;
    /// Allocate from context
    static OutputI* a(const ASTContext& ctx, const Location& loc,
                      Expression* e);
  };
  /// \brief Function declaration item
  class FunctionI : public Item {
  protected:
    /// Constructor
    FunctionI(const Location& loc) : Item(loc, II_FUN) {}
  public:
    /// The identifier of this item type
    static const ItemId iid = II_FUN;
    /// Identifier of this function (context-allocated)
    CtxStringH _id;
    /// Type-inst of the return value
    TypeInst* _ti;
    /// List of parameter declarations
    CtxVec<VarDecl*>* _params;
    /// Annotation
    Annotation* _ann;
    /// Function body (or NULL)
    Expression* _e;
    /// Allocate from context
    static FunctionI* a(const ASTContext& ctx, const Location& loc,
                        const std::string& id, TypeInst* ti,
                        const std::vector<VarDecl*>& params,
                        Expression* e = NULL, Annotation* ann = NULL);
    
    /** \brief Compute return type given argument types \a ta
     */
    Type rtype(const std::vector<Expression*>& ta);
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

}

#endif
