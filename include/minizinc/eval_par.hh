/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_EVAL_PAR_HH__
#define __MINIZINC_EVAL_PAR_HH__

#include <minizinc/model.hh>
#include <minizinc/iter.hh>

#include <minizinc/prettyprinter.hh>

namespace MiniZinc {
  
  /// Evaluate par int expression \a e
  IntVal eval_int(Expression* e);
  /// Evaluate par bool expression \a e
  bool eval_bool(Expression* e);
  /// Evaluate par float expression \a e
  FloatVal eval_float(Expression* e);
  /// Evaluate an array expression \a e into an array literal
  ArrayLit* eval_array_lit(Expression* e);
  /// Evaluate an access to array \a with indices \a idx and return whether
  /// access succeeded in \a success
  Expression* eval_arrayaccess(ArrayLit* a, const std::vector<IntVal>& idx, bool& success);
  /// Evaluate an array access \a e and return whether access succeeded in \a success
  Expression* eval_arrayaccess(ArrayAccess* e, bool& success);
  /// Evaluate a par integer set \a e
  IntSetVal* eval_intset(Expression* e);
  /// Evaluate a par string \a e
  std::string eval_string(Expression* e);
  /// Evaluate a par expression \a e and return it wrapped in a literal
  Expression* eval_par(Expression* e);
  
  
  
  /// returns the value of the given literal expression (moved here from old SolverInterface)
  template<typename T>
  T getNumber(Expression* e) {
      if(IntLit* il = e->dyn_cast<IntLit>())
          return il->v().toInt();
      else if(FloatLit* fl = e->dyn_cast<FloatLit>())
          return fl->v();
      else if(BoolLit* bl = e->dyn_cast<BoolLit>())
          return bl->v();
      else if(UnOp* uo = e->dyn_cast<UnOp>())
          return -1 * getNumber<T>(uo->e());
      assert(false);
      return 0;
  }
  
  /// Get the par bounds of expression \a e that has been obtained by ti->domain() (moved here from old SolverInterface)
   // TODO: remove and replace as soon as compute_int_bounds is repaired
  static std::pair<double,double> getIntBounds(Expression* e){
    if(e->isa<BinOp>()){
        BinOp* bo = e->cast<BinOp>();
        long long int b, u;
        b = getNumber<long long int>(bo->lhs());
        u = getNumber<long long int>(bo->rhs());
        return std::pair<long long int,long long int>(b,u);
    } else if(e->isa<TypeInst>()){
        TypeInst* ti = e->cast<TypeInst>();
        e = ti->domain();
        if(e)
            return getIntBounds(e);
        else
            throw -1;
    } else if(e->isa<SetLit>()) {
        long long int b,u;
        IntSetVal* isv = e->cast<SetLit>()->isv();
        if(isv) {
            b = isv->min(0).toInt();
            u = isv->max(isv->size()-1).toInt();
        } else {
            ASTExprVec<Expression> v = e->cast<SetLit>()->v();
            b = getNumber<long long int>(v[0]);
            u = getNumber<long long int>(v[v.size()-1]);
        }
        return std::pair<long long int,long long int>(b,u);
    } else {
        std::stringstream ssm; 
        ssm << "getIntBounds : Expected BinOp or TypeInst, got this : " << *e;
        throw InternalError(ssm.str());
    }
  }
  /// Get the par bounds of expression \a e that has been obtained by ti->domain() (moved here from SolverInterface)
  // TODO: remove and replace as soon as compute_float_bounds is repaired
  static std::pair<double,double> getFloatBounds(Expression* e){
    if(e->isa<BinOp>()){
        BinOp* bo = e->cast<BinOp>();
        double b, u;
        b = getNumber<double>(bo->lhs());
        u = getNumber<double>(bo->rhs());
        return std::pair<double,double>(b,u);
    } else if(e->isa<TypeInst>()){
        TypeInst* ti = e->cast<TypeInst>();
        e = ti->domain();
        if(e)
            return getFloatBounds(e);
        else
            throw -1;
    } else {
        std::stringstream ssm; 
        ssm << "getIntBounds : Expected BinOp or TypeInst, got this : " << *e;
        throw InternalError(ssm.str());
    }
  }
  
  /// Representation for bounds of an integer expression
  struct IntBounds {
    /// Lower bound
    IntVal l;
    /// Upper bound
    IntVal u;
    /// Whether the bounds are valid
    bool valid;
    /// Constructor
    IntBounds(IntVal l0, IntVal u0, bool valid0)
      : l(l0), u(u0), valid(valid0) {}
  };
  
  /// Compute bounds of an integer expression
  IntBounds compute_int_bounds(Expression* e);

  /// Representation for bounds of a float expression
  struct FloatBounds {
    /// Lower bound
    FloatVal l;
    /// Upper bound
    FloatVal u;
    /// Whether the bounds are valid
    bool valid;
    /// Constructor
    FloatBounds(FloatVal l0, FloatVal u0, bool valid0)
    : l(l0), u(u0), valid(valid0) {}
  };
  
  /// Compute bounds of an integer expression
  FloatBounds compute_float_bounds(Expression* e);
  
  
  /**
   * \brief Compute bounds of a set of int expression
   *
   * Returns NULL if bounds cannot be determined
   */
  IntSetVal* compute_intset_bounds(Expression* e);

  template<class Eval>
  void
  eval_comp_array(Eval& eval, Comprehension* e, int gen, int id,
                  KeepAlive in, std::vector<typename Eval::ArrayVal>& a);

  template<class Eval>
  void
  eval_comp_set(Eval& eval, Comprehension* e, int gen, int id,
                KeepAlive in, std::vector<typename Eval::ArrayVal>& a);

  template<class Eval>
  void
  eval_comp_set(Eval& eval, Comprehension* e, int gen, int id,
                IntVal i, KeepAlive in, std::vector<typename Eval::ArrayVal>& a) {
    e->decl(gen,id)->e()->cast<IntLit>()->v(i);
    if (id == e->n_decls(gen)-1) {
      if (gen == e->n_generators()-1) {
        bool where = true;
        if (e->where() != NULL) {
          GCLock lock;
          where = eval_bool(e->where());
        }
        if (where) {
          a.push_back(eval.e(e->e()));
        }
      } else {
        KeepAlive nextin;
        {
          if (e->in(gen+1)->type().dim()==0) {
            GCLock lock;
            nextin = new SetLit(Location(),eval_intset(e->in(gen+1)));
          } else {
            GCLock lock;
            nextin = eval_array_lit(e->in(gen+1));
          }
        }
        if (e->in(gen+1)->type().dim()==0) {
          eval_comp_set<Eval>(eval,e,gen+1,0,nextin,a);
        } else {
          eval_comp_array<Eval>(eval,e,gen+1,0,nextin,a);
        }
      }
    } else {
      eval_comp_set<Eval>(eval,e,gen,id+1,in,a);
    }
  }

  template<class Eval>
  void
  eval_comp_array(Eval& eval, Comprehension* e, int gen, int id,
                  IntVal i, KeepAlive in, std::vector<typename Eval::ArrayVal>& a) {
    ArrayLit* al = in()->cast<ArrayLit>();
    e->decl(gen,id)->e(al->v()[i.toInt()]);
    e->rehash();
    if (id == e->n_decls(gen)-1) {
      if (gen == e->n_generators()-1) {
        bool where = true;
        if (e->where() != NULL) {
          GCLock lock;
          where = eval_bool(e->where());
        }
        if (where) {
          a.push_back(eval.e(e->e()));
        }
      } else {
        KeepAlive nextin;
        {
          if (e->in(gen+1)->type().dim()==0) {
            GCLock lock;
            nextin = new SetLit(Location(),eval_intset(e->in(gen+1)));
          } else {
            GCLock lock;
            nextin = eval_array_lit(e->in(gen+1));
          }
        }
        if (e->in(gen+1)->type().dim()==0) {
          eval_comp_set<Eval>(eval,e,gen+1,0,nextin,a);
        } else {
          eval_comp_array<Eval>(eval,e,gen+1,0,nextin,a);
        }
      }
    } else {
      eval_comp_array<Eval>(eval,e,gen,id+1,in,a);
    }
    e->decl(gen,id)->e(NULL);
    e->decl(gen,id)->flat(NULL);
  }

  /**
   * \brief Evaluate comprehension expression
   * 
   * Calls \a eval.e for every element of the comprehension \a e,
   * where \a gen is the current generator, \a id is the current identifier
   * in that generator, \a in is the expression of that generator, and
   * \a a is the array in which to place the result.
   */
  template<class Eval>
  void
  eval_comp_set(Eval& eval, Comprehension* e, int gen, int id,
                KeepAlive in, std::vector<typename Eval::ArrayVal>& a) {
    IntSetRanges rsi(in()->cast<SetLit>()->isv());
    Ranges::ToValues<IntSetRanges> rsv(rsi);
    for (; rsv(); ++rsv) {
      eval_comp_set<Eval>(eval,e,gen,id,rsv.val(),in,a);
    }
  }

  /**
   * \brief Evaluate comprehension expression
   *
   * Calls \a eval.e for every element of the comprehension \a e,
   * where \a gen is the current generator, \a id is the current identifier
   * in that generator, \a in is the expression of that generator, and
   * \a a is the array in which to place the result.
   */
  template<class Eval>
  void
  eval_comp_array(Eval& eval, Comprehension* e, int gen, int id,
                  KeepAlive in, std::vector<typename Eval::ArrayVal>& a) {
    ArrayLit* al = in()->cast<ArrayLit>();
    for (unsigned int i=0; i<al->v().size(); i++) {
      eval_comp_array<Eval>(eval,e,gen,id,i,in,a);
    }
  }

  /**
   * \brief Evaluate comprehension expression
   * 
   * Calls \a eval.e for every element of the comprehension \a e and
   * returns a vector with all the evaluated results.
   */
  template<class Eval>
  std::vector<typename Eval::ArrayVal>
  eval_comp(Eval& eval, Comprehension* e) {
    std::vector<typename Eval::ArrayVal> a;
    KeepAlive in;
    {
      GCLock lock;
      if (e->in(0)->type().dim()==0) {
        in = new SetLit(Location(),eval_intset(e->in(0)));
      } else {
        in = eval_array_lit(e->in(0));
      }
    }
    if (e->in(0)->type().dim()==0) {
      eval_comp_set<Eval>(eval,e,0,0,in,a);
    } else {
      eval_comp_array<Eval>(eval,e,0,0,in,a);
    }
    return a;
  }  

  /**
   * \brief Evaluate comprehension expression
   * 
   * Calls \a Eval::e for every element of the comprehension \a e and
   * returns a vector with all the evaluated results.
   */
  template<class Eval>
  std::vector<typename Eval::ArrayVal>
  eval_comp(Comprehension* e) {
    Eval eval;
    return eval_comp(eval,e);
  }  
  
}

#endif
