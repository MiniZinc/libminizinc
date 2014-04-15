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
#include <cstddef>

#include <iostream>

namespace MiniZinc {

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
    void mark(void) const;
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
      s << " in file " << loc.filename << ":" << loc.first_line;
    }
    return os << s.str();
  }

  /**
   * \brief Base class for expressions
   */
  class Expression : public ASTNode {
  protected:
    /// An annotation (or NULL)
    Annotation* _ann;
    /// The location of the expression
    Location _loc;
    /// The %MiniZinc type of the expression
    Type _type;
    /// The hash value of the expression
    size_t _hash;
  public:
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

    Annotation* ann(void) const {
      return _ann;
    }
    const Location& loc(void) const {
      return _loc;
    }
    void loc(const Location& l) {
      _loc = l;
    }
    const Type& type(void) const {
      return _type;
    }
    void type(const Type& t);
    size_t hash(void) const {
      return _hash;
    }
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

    /// Cast expression to type \a T*
    template<class T> static T* cast(Expression* e) {
      return e==NULL ? NULL : e->cast<T>();
    }
    /// Cast expression to type \a const T*
    template<class T> static const T* cast(const Expression* e) {
      return e==NULL ? NULL : e->cast<T>();
    }
    /// Cast expression to type \a T* or NULL if types do not match
    template<class T> static T* dyn_cast(Expression* e) {
      return e==NULL ? NULL : e->dyn_cast<T>();
    }
    /// Cast expression to type \a const T* or NULL if types do not match
    template<class T> static const T* dyn_cast(const Expression* e) {
      return e==NULL ? NULL : e->dyn_cast<T>();
    }
    
    
    /// Set annotation to \a ann
    void ann(Annotation* ann);
    
    /// Add annotation \a ann to the expression
    void addAnnotation(Annotation* ann);
    
    /// Return hash value of \a e
    static size_t hash(const Expression* e) {
      return e==NULL ? 0 : e->_hash;
    }
    
    /// Check if \a e0 and \a e1 are equal
    static bool equal(const Expression* e0, const Expression* e1);
    
    /// Mark \a e as alive for garbage collection
    static void mark(Expression* e);
  };

  /**
   * \brief Annotations
   */
  class Annotation : public Expression {
  protected:
    /// The actual annotation expression
    Expression* _e;
    /// The next annotation in a list or NULL
    Annotation* _a;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ANN;

    /// Access expression
    Expression* e(void) const { return _e; }
    /// Access next annotation in the list
    Annotation* next(void) const { return _a; }

    /// Set next annotation in the list to \a a
    void next(Annotation* a) { _a = a; }
    
    /// Constructor
    Annotation(const Location& loc, Expression* e, Annotation* a = NULL);
    /// Add annotation \a a to end of list of annotations
    void merge(Annotation* a);
    /// Recompute hash value
    void rehash(void);
  };
  
  /// \brief Integer literal expression
  class IntLit : public Expression {
  protected:
    /// The value of this expression
    IntVal _v;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_INTLIT;
    /// Constructor
    IntLit(const Location& loc, IntVal v);
    /// Access value
    IntVal v(void) const { return _v; }
    /// Set value
    void v(IntVal val) { _v = val; }
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief Float literal expression
  class FloatLit : public Expression {
  protected:
    /// The value of this expression
    FloatVal _v;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_FLOATLIT;
    /// Constructor
    FloatLit(const Location& loc, FloatVal v);
    /// Access value
    FloatVal v(void) const { return _v; }
    /// Set value
    void v(FloatVal val) { _v = val; }
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief Set literal expression
  class SetLit : public Expression {
  protected:
    /// The value of this expression
    ASTExprVec<Expression> _v;
    /// A range-list based representation for an integer set, or NULL
    IntSetVal* _isv;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_SETLIT;
    /// Construct set \$f\{v1,\dots,vn\}\$f
    SetLit(const Location& loc, const std::vector<Expression*>& v);
    /// Construct set \$f\{v1,\dots,vn\}\$f
    SetLit(const Location& loc, ASTExprVec<Expression> v);
    /// Construct set
    SetLit(const Location& loc, IntSetVal* isv);
    /// Access value
    ASTExprVec<Expression> v(void) const { return _v; }
    /// Set value
    void v(const ASTExprVec<Expression>& val) { _v = val; }
    /// Access value
    IntSetVal* isv(void) const { return _isv; }
    /// Set value
    void isv(IntSetVal* val) { _isv = val; }
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief Boolean literal expression
  class BoolLit : public Expression {
  protected:
    /// The value of this expression
    bool _v;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_BOOLLIT;
    /// Constructor
    BoolLit(const Location& loc, bool v);
    /// Access value
    bool v(void) const { return _v; }
    /// Set value
    void v(bool val) { _v = val; }
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief String literal expression
  class StringLit : public Expression {
  protected:
    /// The value of this expression
    ASTString _v;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_STRINGLIT;
    /// Constructor
    StringLit(const Location& loc, const std::string& v);
    /// Constructor
    StringLit(const Location& loc, const ASTString& v);
    /// Access value
    ASTString v(void) const { return _v; }
    /// Set value
    void v(const ASTString& val) { _v = val; }
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief Identifier expression
  class Id : public Expression {
  protected:
    /// The string identifier
    ASTString _v;
    /// The declaration corresponding to this identifier (may be NULL)
    VarDecl* _decl;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ID;
    /// Constructor (\a decl may be NULL)
    Id(const Location& loc, const std::string& v, VarDecl* decl);
    /// Constructor (\a decl may be NULL)
    Id(const Location& loc, const ASTString& v, VarDecl* decl);
    /// Access identifier
    ASTString v(void) const { return _v; }
    /// Set identifier
    void v(const ASTString& val) { _v = val; }
    /// Access declaration
    VarDecl* decl(void) const { return _decl; }
    /// Set declaration
    void decl(VarDecl* d) { _decl = d; }
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief Type-inst identifier expression
  class TIId : public Expression {
  protected:
    /// The string identifier
    ASTString _v;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_TIID;
    /// Constructor
    TIId(const Location& loc, const std::string& v);
    /// Access identifier
    ASTString v(void) const { return _v; }
    /// Set identifier
    void v(const ASTString& val) { _v = val; }
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
    friend class Expression;
  protected:
    /// The array
    ASTExprVec<Expression> _v;
    /// The declared array dimensions
    ASTIntVec _dims;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ARRAYLIT;
    /// Constructor
    ArrayLit(const Location& loc,
             const std::vector<Expression*>& v,
             const std::vector<std::pair<int,int> >& dims);
    /// Constructor (existing content)
    ArrayLit(const Location& loc,
             ASTExprVec<Expression> v,
             const std::vector<std::pair<int,int> >& dims);
    /// Constructor (one-dimensional)
    ArrayLit(const Location& loc,
             const std::vector<Expression*>& v);
    /// Constructor (two-dimensional)
    ArrayLit(const Location& loc,
             const std::vector<std::vector<Expression*> >& v);
    /// Recompute hash value
    void rehash(void);
    
    /// Access value
    ASTExprVec<Expression> v(void) const { return _v; }
    /// Set value
    void v(const ASTExprVec<Expression>& val) { _v = val; }

    /// Return number of dimensions
    int dims(void) const;
    /// Return minimum index of dimension \a i
    int min(int i) const;
    /// Return maximum index of dimension \a i
    int max(int i) const;
    /// Return the length of the array
    int length(void) const;
    /// Set dimension vector
    void setDims(ASTIntVec dims) { _dims = dims; }
  };
  /// \brief Array access expression
  class ArrayAccess : public Expression {
  protected:
    /// The array to access
    Expression* _v;
    /// The indexes (for all array dimensions)
    ASTExprVec<Expression> _idx;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ARRAYACCESS;
    /// Constructor
    ArrayAccess(const Location& loc,
                Expression* v,
                const std::vector<Expression*>& idx);
    /// Constructor
    ArrayAccess(const Location& loc,
                Expression* v,
                ASTExprVec<Expression> idx);
    /// Access value
    Expression* v(void) const { return _v; }
    /// Set value
    void v(Expression* val) { _v = val; }
    /// Access index sets
    ASTExprVec<Expression> idx(void) const { return _idx; }
    /// Set index sets
    void idx(const ASTExprVec<Expression>& idx) { _idx = idx; }
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
    friend class Comprehension;
  protected:
    /// Variable declarations
    std::vector<VarDecl*> _v;
    /// in-expression
    Expression* _in;
  public:
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
    friend class Expression;
  protected:
    /// The expression to generate
    Expression* _e;
    /// A list of generator expressions
    ASTExprVec<Expression> _g;
    /// A list of indices where generators start
    ASTIntVec _g_idx;
    /// The where-clause (or NULL)
    Expression* _where;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_COMP;
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
    /// Return where clause
    Expression* where(void) const { return _where; }
    /// Return generator body
    Expression* e(void) const { return _e; }
  };
  /// \brief If-then-else expression
  class ITE : public Expression {
    friend class Expression;
  protected:
    /// List of if-then-pairs
    ASTExprVec<Expression> _e_if_then;
    /// Else-expression
    Expression* _e_else;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ITE;
    /// Constructor
    ITE(const Location& loc,
        const std::vector<Expression*>& e_if_then,
        Expression* e_else);
    int size(void) const { return _e_if_then.size()/2; }
    Expression* e_if(int i) { return _e_if_then[2*i]; }
    Expression* e_then(int i) { return _e_if_then[2*i+1]; }
    Expression* e_else(void) { return _e_else; }
    const Expression* e_if(int i) const { return _e_if_then[2*i]; }
    const Expression* e_then(int i) const { return _e_if_then[2*i+1]; }
    const Expression* e_else(void) const { return _e_else; }
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
  protected:
    /// Left hand side expression
    Expression* _e0;
    /// Right hand side expression
    Expression* _e1;
    /// The predicate or function declaration (or NULL)
    FunctionI* _decl;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_BINOP;
    /// Constructor
    BinOp(const Location& loc,
          Expression* e0, BinOpType op, Expression* e1);
    /// Access left hand side
    Expression* lhs(void) const { return _e0; }
    /// Set left hand side
    void lhs(Expression* e) { _e0 = e; }
    /// Access right hand side
    Expression* rhs(void) const { return _e1; }
    /// Set right hand side
    void rhs(Expression* e) { _e1 = e; }
    /// Access declaration
    FunctionI* decl(void) const { return _decl; }
    /// Set declaration
    void decl(FunctionI* f) { _decl = f; }
    /// Return string representation of the operator
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
  protected:
    /// %Expression
    Expression* _e0;
    /// The predicate or function declaration (or NULL)
    FunctionI* _decl;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_UNOP;
    /// Constructor
    UnOp(const Location& loc,
         UnOpType op, Expression* e);
    /// Access expression
    Expression* e(void) const { return _e0; }
    /// Set expression
    void e(Expression* e0) { _e0 = e0; }
    /// Access declaration
    FunctionI* decl(void) const { return _decl; }
    /// Set declaration
    void decl(FunctionI* f) { _decl = f; }
    ASTString opToString(void) const;
    /// Recompute hash value
    void rehash(void);
    /// Return operator type
    UnOpType op(void) const;
  };
  
  /// \brief A predicate or function call expression
  class Call : public Expression {
    friend class Expression;
  protected:
    /// Identifier of called predicate or function
    ASTString _id;
    /// Arguments to the call
    ASTExprVec<Expression> _args;
    /// The predicate or function declaration (or NULL)
    FunctionI* _decl;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_CALL;
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
    /// Access identifier
    ASTString id(void) const { return _id; }
    /// Set identifier
    void id(const ASTString& i) { _id = i; }
    /// Access arguments
    ASTExprVec<Expression> args(void) const { return _args; }
    /// Set arguments
    void args(const ASTExprVec<Expression>& a) { _args = a; }
    /// Access declaration
    FunctionI* decl(void) const { return _decl; }
    /// Set declaration
    void decl(FunctionI* f) { _decl = f; }
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief A variable declaration expression
  class VarDecl : public Expression {
    friend class Let;
  protected:
    /// Type-inst of the declared variable
    TypeInst* _ti;
    /// Identifier
    Id* _id;
    /// Initialisation expression (can be NULL)
    Expression* _e;
    /// Flattened version of the VarDecl
    WeakRef _flat;
    /// Integer payload
    int _payload;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_VARDECL;
    /// Constructor
    VarDecl(const Location& loc,
            TypeInst* ti, const std::string& id,
            Expression* e=NULL);
    /// Constructor
    VarDecl(const Location& loc,
            TypeInst* ti, const ASTString& id, Expression* e=NULL);

    /// Access TypeInst
    TypeInst* ti(void) const { return _ti; }
    /// Set TypeInst
    void ti(TypeInst* t) { _ti=t; }
    /// Access identifier
    Id* id(void) const { return _id; }
    /// Access initialisation expression
    Expression* e(void) const;
    /// Set initialisation expression
    void e(Expression* rhs) { assert(!Expression::equal(rhs,_id)); _e = rhs; }
    /// Access flattened version
    VarDecl* flat(void) { return _flat() ? _flat()->cast<VarDecl>() : NULL; }
    /// Set flattened version
    void flat(VarDecl* vd) { _flat = WeakRef(vd); }
    
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
    /// Whether variable has been evaluated
    bool evaluated(void) const;
    /// Whether variable has been evaluated
    void evaluated(bool t);
    /// Access payload
    int payload(void) const { return _payload; }
    /// Set payload
    void payload(int i) { _payload = i; }
  };
  /// \brief %Let expression
  class Let : public Expression {
  protected:
    /// List of local declarations
    ASTExprVec<Expression> _let;
    /// Body of the let
    Expression* _in;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_LET;
    /// Constructor
    Let(const Location& loc,
        const std::vector<Expression*>& let, Expression* in);
    /// Recompute hash value
    void rehash(void);

    /// Access local declarations
    ASTExprVec<Expression> let(void) const { return _let; }
    /// Access body
    Expression* in(void) const { return _in; }
    
    /// Remember current let bindings
    void pushbindings(void);
    /// Restore previous let bindings
    void popbindings(void);
    
  };

  /// \brief Type-inst expression
  class TypeInst : public Expression {
  protected:
    /// Ranges of an array expression
    ASTExprVec<TypeInst> _ranges;
    /// Declared domain (or NULL)
    Expression* _domain;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_TI;
    /// Constructor
    TypeInst(const Location& loc,
             const Type& t,
             ASTExprVec<TypeInst> ranges,
             Expression* domain=NULL);
    /// Constructor
    TypeInst(const Location& loc,
             const Type& t,
             Expression* domain=NULL);
    
    /// Access ranges
    ASTExprVec<TypeInst> ranges(void) const { return _ranges; }
    /// Access domain
    Expression* domain(void) const { return _domain; }
    //// Set domain
    void domain(Expression* d) { _domain = d; }
    
    /// Set ranges to \a ranges
    void setRanges(const std::vector<TypeInst*>& ranges);
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
  protected:
    /// Location of the item
    Location _loc;
  public:
    /// Identifier of the concrete item type
    enum ItemId {
      II_INC = Expression::EID_END+1, II_VD, II_ASN, II_CON, II_SOL,
      II_OUT, II_FUN, II_END = II_FUN
    };
    ItemId iid(void) const {
      return static_cast<ItemId>(_id);
    }
    
    const Location& loc(void) const {
      return _loc;
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
  protected:
    /// Filename to include
    ASTString _f;
    /// Model for that file
    Model* _m;
  public:
    /// The identifier of this item type
    static const ItemId iid = II_INC;
    /// Constructor
    IncludeI(const Location& loc, const ASTString& f);
    /// Access filename
    ASTString f(void) const { return _f; }
    /// Set filename
    void f(const ASTString& nf) { _f = nf; }
    /// Access model
    Model* m(void) const { return _m; }
    /// Set the model
    void m(Model* m0, bool own=true) {
      assert(_m==NULL || m0==NULL); _m = m0; _flag_2 = own;
    }
    bool own(void) const {
      return _flag_2;
    }
  };

  /// \brief Variable declaration item
  class VarDeclI : public Item {
  protected:
    /// The declaration expression
    VarDecl* _e;
  public:
    /// The identifier of this item type
    static const ItemId iid = II_VD;
    /// Constructor
    VarDeclI(const Location& loc, VarDecl* e);
    /// Access expression
    VarDecl* e(void) const { return _e; }
    /// Set expression
    void e(VarDecl* vd) { _e = vd; }
  };

  /// \brief Assign item
  class AssignI : public Item {
  protected:
    /// Identifier of variable to assign to
    ASTString _id;
    /// Expression to assign to the variable
    Expression* _e;
    /// Declaration of the variable to assign to
    VarDecl* _decl;
  public:
    /// The identifier of this item type
    static const ItemId iid = II_ASN;
    /// Constructor
    AssignI(const Location& loc,
            const std::string& id, Expression* e);
    /// Access identifier
    ASTString id(void) const { return _id; }
    /// Access expression
    Expression* e(void) const { return _e; }
    /// Set expression
    void e(Expression* e0) { _e = e0; }
    /// Access declaration
    VarDecl* decl(void) const { return _decl; }
    /// Set declaration
    void decl(VarDecl* d) { _decl = d; }
  };

  /// \brief Constraint item
  class ConstraintI : public Item {
  protected:
    /// Constraint expression
    Expression* _e;
  public:
    /// The identifier of this item type
    static const ItemId iid = II_CON;
    /// Constructor
    ConstraintI(const Location& loc, Expression* e);
    /// Access expression
    Expression* e(void) const { return _e; }
    /// Set expression
    void e(Expression* e0) { _e = e0; }
  };

  /// \brief Solve item
  class SolveI : public Item {
  protected:
    /// Solve item annotation
    Annotation* _ann;
    /// Expression for minimisation/maximisation (or NULL)
    Expression* _e;
    /// Constructor
    SolveI(const Location& loc, Annotation* a, Expression* e);
  public:
    /// The identifier of this item type
    static const ItemId iid = II_SOL;
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
    /// Access solve annotation
    Annotation* ann(void) const { return _ann; }
    /// Access expression for optimisation
    Expression* e(void) const { return _e; }
    /// Set expression for optimisation
    void e(Expression* e0) { _e=e0; }
    /// Return type of solving
    SolveType st(void) const;
    /// Set type of solving
    void st(SolveType s);
  };

  /// \brief Output item
  class OutputI : public Item {
  protected:
    /// Expression to output
    Expression* _e;
  public:
    /// The identifier of this item type
    static const ItemId iid = II_OUT;
    /// Constructor
    OutputI(const Location& loc, Expression* e);
    /// Access expression
    Expression* e(void) const { return _e; }
  };

  /// \brief Function declaration item
  class FunctionI : public Item {
  protected:
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
  public:
    /// The identifier of this item type
    static const ItemId iid = II_FUN;
    
    /// Type of builtin expression-valued functions
    typedef Expression* (*builtin_e) (ASTExprVec<Expression>);
    /// Type of builtin int-valued functions
    typedef IntVal (*builtin_i) (ASTExprVec<Expression>);
    /// Type of builtin bool-valued functions
    typedef bool (*builtin_b) (ASTExprVec<Expression>);
    /// Type of builtin float-valued functions
    typedef FloatVal (*builtin_f) (ASTExprVec<Expression>);
    /// Type of builtin set-valued functions
    typedef IntSetVal* (*builtin_s) (ASTExprVec<Expression>);
    /// Type of builtin string-valued functions
    typedef std::string (*builtin_str) (ASTExprVec<Expression>);

    /// Builtin functions (or NULL)
    struct {
      builtin_e e;
      builtin_i i;
      builtin_f f;
      builtin_b b;
      builtin_s s;
      builtin_str str;
    } _builtins;

    /// Constructor
    FunctionI(const Location& loc,
              const std::string& id, TypeInst* ti,
              const std::vector<VarDecl*>& params,
              Expression* e = NULL, Annotation* ann = NULL);

    /// Access identifier
    ASTString id(void) const { return _id; }
    /// Access TypeInst
    TypeInst* ti(void) const { return _ti; }
    /// Access parameters
    ASTExprVec<VarDecl> params(void) const { return _params; }
    /// Access annotation
    Annotation* ann(void) const { return _ann; }
    /// Access body
    Expression* e(void) const { return _e; }
    
    /** \brief Compute return type given argument types \a ta
     */
    Type rtype(const std::vector<Expression*>& ta);
    /** \brief Compute return type given argument types \a ta
     */
    Type rtype(const std::vector<Type>& ta);
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

  /// Statically allocated constants
  class Constants {
  private:
      /// Garbage collection root set for constants
      Model* m;
  public:
      /// Literal true
      BoolLit* lit_true;
      /// Variable bound to true
      VarDecl* var_true;
      /// Literal false
      BoolLit* lit_false;
      /// Variable bound to false
      VarDecl* var_false;
      /// Function item used to keep track of redefined variables
      FunctionI* var_redef;
      /// Identifiers for builtins
      struct {
        ASTString forall;
        ASTString exists;
        ASTString clause;
        ASTString bool2int;
        ASTString bool_eq;
        ASTString assert;
        ASTString trace;
        ASTString promise_total;

        ASTString sum;
        ASTString lin_exp;
        ASTString int_lin_eq;
        ASTString int_lin_le;
        ASTString int_lin_ne;
        ASTString int_eq;
      } ids;
    
      /// Identifiers for Boolean contexts
      struct {
        Id* root;
        Id* pos;
        Id* neg;
        Id* mix;
      } ctx;
      /// Common annotations
      struct {
        Id* output_var;
        ASTString output_array;
        Id* is_defined_var;
        ASTString defines_var;
      } ann;
      /// Constructor
      Constants(void);
      /// Return shared BoolLit
      BoolLit* boollit(bool b) {
        return b ? lit_true : lit_false;
      }
  };
    
  /// Return static instance
  Constants& constants(void);

}

#include <minizinc/ast.hpp>

#endif
