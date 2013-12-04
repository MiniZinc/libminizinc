/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_AST_HH__
#define __MINIZINC_AST_HH__

#include <minizinc/gc.hh>
#include <minizinc/aststring.hh>
#include <minizinc/astvec.hh>

#include <minizinc/values.hh>
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
  class FunctionI;

  /// %Location of an expression in the source code
  class Location {
  public:
    /// Source code file name
    ASTString filename;
    /// Line where expression starts
    unsigned int first_line;
    /// Column where expression starts
    unsigned int first_column;
    /// Line where expression ends
    unsigned int last_line;
    /// Column where expression ends
    unsigned int last_column;
    
    /// Construct empty location
    Location(void);
    
    /// Return string representation
    std::string toString(void) const;
    
    /// Mark as alive for garbage collection
    void mark(void);
  };

  /// Output operator for locations
  template<class Char, class Traits>
  std::basic_ostream<Char,Traits>&
  operator <<(std::basic_ostream<Char,Traits>& os, const Location& loc) {
    std::basic_ostringstream<Char,Traits> s;
    s.copyfmt(os); s.width(0);
    if (loc.filename=="") {
      s << " in unknown file";
    } else {
      s << " in file " << loc.filename.str() << ":" << loc.first_line;
    }
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
      E_INTLIT = ASTNode::NID_END+1, E_FLOATLIT, E_SETLIT, E_BOOLLIT,
      E_STRINGLIT, E_ID, E_ANON, E_ARRAYLIT,
      E_ARRAYACCESS, E_COMP, E_ITE,
      E_BINOP, E_UNOP, E_CALL, E_VARDECL, E_LET,
      E_ANN, E_TI, E_TIID, EID_END = E_TIID
    };

    ExpressionId eid(void) const {
      return static_cast<ExpressionId>(_id);
    }

    /// The %MiniZinc type of the expression
    Type _type;

    /// The hash value of the expression
    size_t _hash;

  protected:
    /// Combination function for hash values
    void cmb_hash(size_t h) {
      _hash ^= h + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
    }
    /// Combination function for hash values
    size_t cmb_hash(size_t seed, size_t h) {
      seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      return seed;
    }

    /// Compute base hash value
    void init_hash(void) { _hash = cmb_hash(0,_id); }

    /// Constructor
    Expression(const Location& loc, const ExpressionId& eid, const Type& t)
      : ASTNode(eid), _ann(NULL), _loc(loc), _type(t) {}

  public:

    /// Test if expression is of type \a T
    template<class T> bool isa(void) const {
      return _id==T::eid;
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
    
    /// Return hash value of \a e
    static size_t hash(const Expression* e) {
      return e==NULL ? 0 : e->_hash;
    }
    
    static bool equal(const Expression* e0, const Expression* e1);
    
    /// Mark \a e as alive for garbage collection
    static void mark(Expression* e);
  };

  /**
   * \brief Annotations
   */
  class Annotation : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ANN;
    /// The actual annotation expression
    Expression* _e;
    /// The next annotation in a list or NULL
    Annotation* _a;

    /// Constructor
    Annotation(const Location& loc, Expression* e, Annotation* a = NULL);
    /// Add annotation \a a to end of list of annotations
    void merge(Annotation* a);
    /// Recompute hash value
    void rehash(void);
  };
  
  /// \brief Integer literal expression
  class IntLit : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_INTLIT;
    /// The value of this expression
    IntVal _v;
    /// Constructor
    IntLit(const Location& loc, IntVal v);
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief Float literal expression
  class FloatLit : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_FLOATLIT;
    /// The value of this expression
    FloatVal _v;
    /// Constructor
    FloatLit(const Location& loc, FloatVal v);
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief Set literal expression
  class SetLit : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_SETLIT;
    /// The value of this expression, or NULL
    ASTExprVec<Expression> _v;
    /// A range-list based representation for an integer set
    IntSetVal* _isv;
    /// Construct set \$f\{v1,\dots,vn\}\$f
    SetLit(const Location& loc, const std::vector<Expression*>& v);
    /// Construct set \$f\{v1,\dots,vn\}\$f
    SetLit(const Location& loc, ASTExprVec<Expression> v);
    /// Construct set
    SetLit(const Location& loc, IntSetVal* isv);
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief Boolean literal expression
  class BoolLit : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_BOOLLIT;
    /// The value of this expression
    bool _v;
    /// Constructor
    BoolLit(const Location& loc, bool v);
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief String literal expression
  class StringLit : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_STRINGLIT;
    /// The value of this expression
    ASTString _v;
    /// Constructor
    StringLit(const Location& loc, const std::string& v);
    /// Constructor
    StringLit(const Location& loc, const ASTString& v);
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief Identifier expression
  class Id : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ID;
    /// The string identifier
    ASTString _v;
    /// The declaration corresponding to this identifier (may be NULL)
    VarDecl* _decl;
    /// Constructor (\a decl may be NULL)
    Id(const Location& loc, const std::string& v, VarDecl* decl);
    /// Constructor (\a decl may be NULL)
    Id(const Location& loc, const ASTString& v, VarDecl* decl);
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief Type-inst identifier expression
  class TIId : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_TIID;
    /// The string identifier
    ASTString _v;
    /// Constructor
    TIId(const Location& loc, const std::string& v);
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief Anonymous variable expression
  class AnonVar : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ANON;
    /// Constructor
    AnonVar(const Location& loc);
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief Array literal expression
  class ArrayLit : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ARRAYLIT;
    /// The array
    ASTExprVec<Expression> _v;
    /// The declared array dimensions
    ASTIntVec _dims;
    /// Constructor
    ArrayLit(const Location& loc,
             const std::vector<Expression*>& v,
             const std::vector<pair<int,int> >& dims);
    /// Constructor (existing content)
    ArrayLit(const Location& loc,
             ASTExprVec<Expression> v,
             const std::vector<pair<int,int> >& dims);
    /// Constructor (one-dimensional)
    ArrayLit(const Location& loc,
             const std::vector<Expression*>& v);
    /// Constructor (two-dimensional)
    ArrayLit(const Location& loc,
             const std::vector<std::vector<Expression*> >& v);
    /// Recompute hash value
    void rehash(void);
    
    /// Return number of dimensions
    int dims(void) const;
    /// Return minimum index of dimension \a i
    int min(int i) const;
    /// Return maximum index of dimension \a i
    int max(int i) const;
    /// Return the length of the array
    int length(void) const;
  };
  /// \brief Array access expression
  class ArrayAccess : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ARRAYACCESS;
    /// The array to access
    Expression* _v;
    /// The indexes (for all array dimensions)
    ASTExprVec<Expression> _idx;
    /// Constructor
    ArrayAccess(const Location& loc,
                Expression* v,
                const std::vector<Expression*>& idx);
    /// Constructor
    ArrayAccess(const Location& loc,
                Expression* v,
                ASTExprVec<Expression> idx);
    /// Recompute hash value
    void rehash(void);
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
  class Generator {
  public:
    /// Variable declarations
    std::vector<VarDecl*> _v;
    /// in-expression
    Expression* _in;
    /// Allocate
    Generator(const std::vector<std::string>& v,
              Expression* in);
    /// Allocate
    Generator(const std::vector<ASTString>& v,
              Expression* in);
    /// Allocate
    Generator(const std::vector<VarDecl*>& v,
              Expression* in);
  };
  /// \brief A list of generators with one where-expression
  struct Generators {
    /// %Generators
    std::vector<Generator> _g;
    //// where-expression
    Expression* _w;
  };
  /// \brief An expression representing an array- or set-comprehension
  class Comprehension : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_COMP;
    /// The expression to generate
    Expression* _e;
    /// A list of generator expressions
    ASTExprVec<Expression> _g;
    /// A list of indices where generators start
    ASTIntVec _g_idx;
    /// The where-clause (or NULL)
    Expression* _where;
    /// Constructor
    Comprehension(const Location& loc,
                  Expression* e, Generators& g, bool set);
    /// Recompute hash value
    void rehash(void);
    /// Whether comprehension is a set
    bool set(void) const;
    
    /// Return number of generators
    int n_generators(void) const;
    /// Return "in" expression for generator \a i
    Expression* in(int i);
    /// Return "in" expression for generator \a i
    const Expression* in(int i) const;
    /// Return number of declarations for generator \a i
    int n_decls(int i) const;
    /// Return declaration \a i for generator \a gen
    VarDecl* decl(int gen, int i);
    /// Return declaration \a i for generator \a gen
    const VarDecl* decl(int gen, int i) const;
  };
  /// \brief If-then-else expression
  class ITE : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ITE;
  public:
    /// List of if-then-pairs
    ASTExprVec<Expression> _e_if_then;
    /// Else-expression
    Expression* _e_else;
    /// Constructor
    ITE(const Location& loc,
        const std::vector<Expression*>& e_if_then,
        Expression* e_else);
    /// Recompute hash value
    void rehash(void);
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
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_BINOP;
    /// Left hand side expression
    Expression* _e0;
    /// Right hand side expression
    Expression* _e1;
    /// The predicate or function declaration (or NULL)
    FunctionI* _decl;
    /// Constructor
    BinOp(const Location& loc,
          Expression* e0, BinOpType op, Expression* e1);
    ASTString opToString(void) const;
    /// Recompute hash value
    void rehash(void);
    /// Return operator type
    BinOpType op(void) const;
  };

  /// Type of unary operators
  enum UnOpType {
    UOT_NOT, UOT_PLUS, UOT_MINUS
  };
  /// \brief Unary-operator expressions
  class UnOp : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_UNOP;
    /// %Expression
    Expression* _e0;
    /// The predicate or function declaration (or NULL)
    FunctionI* _decl;
    /// Constructor
    UnOp(const Location& loc,
         UnOpType op, Expression* e);
    ASTString opToString(void) const;
    /// Recompute hash value
    void rehash(void);
    /// Return operator type
    UnOpType op(void) const;
  };
  
  /// \brief A predicate or function call expression
  class Call : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_CALL;
    /// Identifier of called predicate or function
    ASTString _id;
    /// Arguments to the call
    ASTExprVec<Expression> _args;
    /// The predicate or function declaration (or NULL)
    FunctionI* _decl;
    /// Constructor
    Call(const Location& loc,
         const std::string& id,
         const std::vector<Expression*>& args,
         FunctionI* decl=NULL);
    /// Constructor
    Call(const Location& loc,
         const ASTString& id,
         const std::vector<Expression*>& args,
         FunctionI* decl=NULL);
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief A variable declaration expression
  class VarDecl : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_VARDECL;
    /// Type-inst of the declared variable
    TypeInst* _ti;
    /// Identifier
    ASTString _id;
    /// Initialisation expression (can be NULL)
    Expression* _e;
    /// Constructor
    VarDecl(const Location& loc,
            TypeInst* ti, const std::string& id,
            Expression* e=NULL);
    /// Constructor
    VarDecl(const Location& loc,
            TypeInst* ti, const ASTString& id, Expression* e=NULL);
    /// Recompute hash value
    void rehash(void);
    /// Whether variable is toplevel
    bool toplevel(void) const;
    /// Whether variable is toplevel
    void toplevel(bool t);
    /// Whether variable is introduced
    bool introduced(void) const;
    /// Whether variable is introduced
    void introduced(bool t);
  };
  /// \brief %Let expression
  class Let : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_LET;
    /// List of local declarations
    ASTExprVec<Expression> _let;
    /// Body of the let
    Expression* _in;
    /// Constructor
    Let(const Location& loc,
        const std::vector<Expression*>& let, Expression* in);
    /// Recompute hash value
    void rehash(void);

    /// Remember current let bindings
    void pushbindings(void);
    /// Restore previous let bindings
    void popbindings(void);
    
  };

  /// \brief Type-inst expression
  class TypeInst : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_TI;
    /// Ranges of an array expression
    ASTExprVec<TypeInst> _ranges;
    /// Declared domain (or NULL)
    Expression* _domain;
    /// Constructor
    TypeInst(const Location& loc,
             const Type& t,
             ASTExprVec<TypeInst> ranges,
             Expression* domain=NULL);
    /// Constructor
    TypeInst(const Location& loc,
             const Type& t,
             Expression* domain=NULL);
    
    /// Add \a ranges to expression
    void addRanges(const std::vector<TypeInst*>& ranges);
    bool isarray(void) const { return _ranges.size()>0; }
    bool hasTiVariable(void) const;
    /// Recompute hash value
    void rehash(void);
    /// Check if domain is computed from right hand side of variable
    bool computedDomain(void) const { return _flag_1; }
    /// Set if domain is computed from right hand side of variable
    void setComputedDomain(bool b) { _flag_1=b; }
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
      II_INC = Expression::EID_END+1, II_VD, II_ASN, II_CON, II_SOL,
      II_OUT, II_FUN, II_END = II_FUN
    };
    ItemId iid(void) const {
      return static_cast<ItemId>(_id);
    }
    
  protected:
    /// Constructor
    Item(const Location& loc, const ItemId& iid)
      : ASTNode(iid), _loc(loc) { _flag_1 = false; }

  public:

    /// Test if item is of type \a T
    template<class T> bool isa(void) const {
      return _id==T::iid;
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
    
    /// Check if item should be removed
    bool removed(void) const { return _flag_1; }
    /// Set flag to remove item
    void remove(void) { _flag_1 = true; }
  };

  class Model;

  /// \brief Include item
  class IncludeI : public Item {
  public:
    /// The identifier of this item type
    static const ItemId iid = II_INC;
    /// Filename to include
    ASTString _f;
    /// Model for that file
    Model* _m;
    /// Constructor
    IncludeI(const Location& loc, const ASTString& f);
    /// Set the model
    void setModel(Model* m, bool own=true) {
      assert(_m==NULL); _m = m; _flag_2 = own;
    }
    bool own(void) const {
      return _flag_2;
    }
  };

  /// \brief Variable declaration item
  class VarDeclI : public Item {
  public:
    /// The identifier of this item type
    static const ItemId iid = II_VD;
    /// The declaration expression
    VarDecl* _e;
    /// Constructor
    VarDeclI(const Location& loc, VarDecl* e);
  };

  /// \brief Assign item
  class AssignI : public Item {
  public:
    /// The identifier of this item type
    static const ItemId iid = II_ASN;
    /// Identifier of variable to assign to
    ASTString _id;
    /// Expression to assign to the variable
    Expression* _e;
    /// Declaration of the variable to assign to
    VarDecl* _decl;
    /// Constructor
    AssignI(const Location& loc,
            const std::string& id, Expression* e);
  };

  /// \brief Constraint item
  class ConstraintI : public Item {
  public:
    /// The identifier of this item type
    static const ItemId iid = II_CON;
    /// Constraint expression
    Expression* _e;
    /// Constructor
    ConstraintI(const Location& loc, Expression* e);
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
    enum SolveType { ST_SAT, ST_MIN, ST_MAX };
    /// Allocate solve satisfy item
    static SolveI* sat(const Location& loc,
                       Annotation* ann = NULL);
    /// Allocate solve minimize item
    static SolveI* min(const Location& loc,
                       Expression* e, Annotation* ann = NULL);
    /// Allocate solve maximize item
    static SolveI* max(const Location& loc,
                       Expression* e, Annotation* ann = NULL);
    /// Return type of solving
    SolveType st(void) const;
    /// Set type of solving
    void st(SolveType s);
  };

  /// \brief Output item
  class OutputI : public Item {
  public:
    /// The identifier of this item type
    static const ItemId iid = II_OUT;
    /// Expression to output
    Expression* _e;
    /// Constructor
    OutputI(const Location& loc, Expression* e);
  };

  /// \brief Function declaration item
  class FunctionI : public Item {
  public:
    /// The identifier of this item type
    static const ItemId iid = II_FUN;
    /// Identifier of this function
    ASTString _id;
    /// Type-inst of the return value
    TypeInst* _ti;
    /// List of parameter declarations
    ASTExprVec<VarDecl> _params;
    /// Annotation
    Annotation* _ann;
    /// Function body (or NULL)
    Expression* _e;
    
    /// Type of builtin expression-valued functions
    typedef Expression* (*builtin_e) (ASTExprVec<Expression>&);
    /// Type of builtin int-valued functions
    typedef IntVal (*builtin_i) (ASTExprVec<Expression>&);
    /// Type of builtin bool-valued functions
    typedef bool (*builtin_b) (ASTExprVec<Expression>&);
    /// Type of builtin float-valued functions
    typedef FloatVal (*builtin_f) (ASTExprVec<Expression>&);
    /// Type of builtin set-valued functions
    typedef IntSetVal* (*builtin_s) (ASTExprVec<Expression>&);

    /// Builtin functions (or NULL)
    struct {
      builtin_e e;
      builtin_i i;
      builtin_f f;
      builtin_b b;
      builtin_s s;
    } _builtins;

    /// Constructor
    FunctionI(const Location& loc,
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
    void vAnonVar(const AnonVar&) {}
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
    /// Visit annotation
    void vAnnotation(const Annotation&) {}
    /// Visit type inst
    void vTypeInst(const TypeInst&) {}
    /// Visit TIId
    void vTIId(const TIId&) {}
    /// Determine whether to enter node
    bool enter(Expression* e) { return true; }
  };

}

#include <minizinc/ast.hpp>

#endif
