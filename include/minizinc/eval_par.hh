/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/astexception.hh>
#include <minizinc/iter.hh>
#include <minizinc/model.hh>
#include <minizinc/prettyprinter.hh>

namespace MiniZinc {

/// Evaluate par int expression \a e
IntVal eval_int_internal(EnvI& env, Expression* e);
/// Evaluate par int expression \a e
inline IntVal eval_int(EnvI& env, Expression* e) {
  if (e->isUnboxedInt()) {
    return e->unboxedIntToIntVal();
  }
  return eval_int_internal(env, e);
}
/// Evaluate par bool expression \a e
bool eval_bool(EnvI& env, Expression* e);
/// Evaluate par float expression \a e
FloatVal eval_float(EnvI& env, Expression* e);
/// Evaluate an array expression \a e into an array literal
ArrayLit* eval_array_lit(EnvI& env, Expression* e);
/// Evaluate an access to array \a al with indices \a idx and return whether
/// access succeeded in \a success
template <class IdxV>
Expression* eval_arrayaccess(EnvI& env, ArrayLit* al, const IdxV& idx, bool& success) {
  success = true;
  assert(al->dims() == idx.size());
  IntVal realidx = 0;
  int realdim = 1;
  for (int i = 0; i < al->dims(); i++) {
    realdim *= al->max(i) - al->min(i) + 1;
  }
  for (int i = 0; i < al->dims(); i++) {
    IntVal ix = idx[i];
    if (ix < al->min(i) || ix > al->max(i)) {
      success = false;
      Type t = al->type();
      t.dim(0);
      if (t.isint()) {
        return IntLit::a(0);
      }
      if (t.isbool()) {
        return Constants::constants().literalFalse;
      }
      if (t.isfloat()) {
        return FloatLit::a(0.0);
      }
      if (t.st() == Type::ST_SET || t.isbot()) {
        auto* ret = new SetLit(Location(), std::vector<Expression*>());
        ret->type(t);
        return ret;
      }
      if (t.isstring()) {
        return new StringLit(Location(), "");
      }
      throw EvalError(env, al->loc(), "Internal error: unexpected type in array access expression");
    }
    realdim /= al->max(i) - al->min(i) + 1;
    realidx += (ix - al->min(i)) * realdim;
  }
  assert(realidx >= 0 && realidx <= al->size());
  return (*al)[static_cast<unsigned int>(realidx.toInt())];
}

/// Evaluate an array access \a e and return whether access succeeded in \a success
Expression* eval_arrayaccess(EnvI& env, ArrayAccess* e, bool& success);
/// Evaluate a set expression \a e into a set literal
SetLit* eval_set_lit(EnvI& env, Expression* e);
/// Evaluate a par integer set \a e
IntSetVal* eval_intset(EnvI& env, Expression* e);
/// Evaluate a par bool set \a e
IntSetVal* eval_boolset(EnvI& env, Expression* e);
/// Evaluate a par float set \a e
FloatSetVal* eval_floatset(EnvI& env, Expression* e);
/// Evaluate a par string \a e
std::string eval_string(EnvI& env, Expression* e);
/// Evaluate a par expression \a e and return it wrapped in a literal
Expression* eval_par(EnvI& env, Expression* e);
/// Check if variable declaration \a vd satisfies the domain and index set constraints
void check_par_declaration(EnvI& env, VarDecl* vd);

/// Representation for bounds of an integer expression
struct IntBounds {
  /// Lower bound
  IntVal l;
  /// Upper bound
  IntVal u;
  /// Whether the bounds are valid
  bool valid;
  /// Constructor
  IntBounds(IntVal l0, IntVal u0, bool valid0) : l(l0), u(u0), valid(valid0) {}
};

/// Compute bounds of an integer expression
IntBounds compute_int_bounds(EnvI& env, Expression* e);

/// Representation for bounds of a float expression
struct FloatBounds {
  /// Lower bound
  FloatVal l;
  /// Upper bound
  FloatVal u;
  /// Whether the bounds are valid
  bool valid;
  /// Constructor
  FloatBounds(FloatVal l0, FloatVal u0, bool valid0) : l(l0), u(u0), valid(valid0) {}
};

/// Compute bounds of an integer expression
FloatBounds compute_float_bounds(EnvI& env, Expression* e);

/**
 * \brief Compute bounds of a set of int expression
 *
 * Returns NULL if bounds cannot be determined
 */
IntSetVal* compute_intset_bounds(EnvI& env, Expression* e);

class EvalBase {
public:
  /// Evaluate bool expression that may contain variables
  static bool evalBoolCV(EnvI& env, Expression* e);
  /// Flatten expression that may contain variables
  static KeepAlive flattenCV(EnvI& env, Expression* e);
};

template <class T>
class EvaluatedComp {
public:
  std::vector<T> a;
  std::vector<std::pair<int, int>> dims;
};

template <class T>
class EvaluatedCompTmp {
public:
  std::vector<T> a;
  std::vector<int> indexes;
  std::vector<IntVal> idxMin;
  std::vector<IntVal> idxMax;
  EvaluatedCompTmp(unsigned int dim) : idxMin(dim), idxMax(dim) {
    for (unsigned int i = 0; i < dim; i++) {
      idxMin[i] = IntVal::infinity();
      idxMax[i] = -IntVal::infinity();
    }
  }
};

template <class Eval, bool isIndexed>
void eval_comp_array(EnvI& env, Eval& eval, Comprehension* e, int gen, int id, KeepAlive in,
                     EvaluatedCompTmp<typename Eval::ArrayVal>& a);

template <class Eval, bool isIndexed>
void eval_comp_set(EnvI& env, Eval& eval, Comprehension* e, int gen, int id, KeepAlive in,
                   EvaluatedCompTmp<typename Eval::ArrayVal>& a);

template <class Eval, bool isSet, bool isIndexed>
void eval_comp_array(EnvI& env, Eval& eval, Comprehension* e, int gen, int id, IntVal i,
                     KeepAlive in, EvaluatedCompTmp<typename Eval::ArrayVal>& a) {
  GC::mark();
  e->decl(gen, id)->trail();
  CallStackItem csi(env, e->decl(gen, id)->id(), i);
  if (isSet) {
    GCLock lock;
    e->decl(gen, id)->e(IntLit::a(i));
  } else {
    if (in() == nullptr) {
      // this is an assignment generator
      KeepAlive asn;
      if (e->where(gen)->type().isvar() || e->where(gen)->type().cv()) {
        asn = eval.flattenCV(env, e->where(gen));
      } else {
        GCLock lock;
        asn = eval_par(env, e->where(gen));
      }
      e->decl(gen, id)->e(asn());
      e->rehash();
    } else {
      auto* al = in()->cast<ArrayLit>();
      e->decl(gen, id)->e((*al)[static_cast<int>(i.toInt())]);
      e->rehash();
    }
  }
  if (id == e->numberOfDecls(gen) - 1) {
    bool where = true;
    if (e->in(gen) != nullptr && e->where(gen) != nullptr && !e->where(gen)->type().isvar()) {
      where = eval.evalBoolCV(env, e->where(gen));
    }
    if (where) {
      if (gen == e->numberOfGenerators() - 1) {
        if (isIndexed) {
          auto* t = e->e()->cast<ArrayLit>();
          for (unsigned int i = 0; i < t->size() - 1; i++) {
            IntVal curIdx = eval_int(env, (*t)[i]);
            a.indexes.push_back(curIdx.toInt());
            a.idxMin[i] = std::min(a.idxMin[i], curIdx);
            a.idxMax[i] = std::max(a.idxMax[i], curIdx);
          }
          a.a.push_back(eval.e(env, (*t)[t->size() - 1]));
        } else {
          a.a.push_back(eval.e(env, e->e()));
        }
      } else {
        if (e->in(gen + 1) == nullptr) {
          eval_comp_array<Eval, false, isIndexed>(env, eval, e, gen + 1, 0, 0, e->in(gen + 1), a);
        } else {
          KeepAlive nextin;
          KeepAlive gen_in = e->in(gen + 1);
          if (gen_in()->type().isvar() || gen_in()->type().cv()) {
            gen_in = eval.flattenCV(env, e->in(gen + 1));
          }
          if (gen_in()->type().dim() == 0) {
            GCLock lock;
            nextin = new SetLit(Location(), eval_intset(env, gen_in()));
          } else {
            GCLock lock;
            nextin = eval_array_lit(env, gen_in());
          }
          if (gen_in()->type().dim() == 0) {
            eval_comp_set<Eval, isIndexed>(env, eval, e, gen + 1, 0, nextin, a);
          } else {
            eval_comp_array<Eval, isIndexed>(env, eval, e, gen + 1, 0, nextin, a);
          }
        }
      }
    }
  } else {
    if (isSet) {
      eval_comp_set<Eval, isIndexed>(env, eval, e, gen, id + 1, in, a);
    } else {
      eval_comp_array<Eval, isIndexed>(env, eval, e, gen, id + 1, in, a);
    }
  }
  GC::untrail();
  e->decl(gen, id)->flat(nullptr);
}

/**
 * \brief Evaluate comprehension expression
 *
 * Calls \a eval.e for every element of the comprehension \a e,
 * where \a gen is the current generator, \a id is the current identifier
 * in that generator, \a in is the expression of that generator, and
 * \a a is the array in which to place the result.
 */
template <class Eval, bool isIndexed>
void eval_comp_set(EnvI& env, Eval& eval, Comprehension* e, int gen, int id, KeepAlive in,
                   EvaluatedCompTmp<typename Eval::ArrayVal>& a) {
  IntSetVal* isv = eval_intset(env, in());
  if (isv->card().isPlusInfinity()) {
    throw EvalError(env, in()->loc(), "comprehension iterates over an infinite set");
  }
  IntSetRanges rsi(isv);
  Ranges::ToValues<IntSetRanges> rsv(rsi);
  for (; rsv(); ++rsv) {
    eval_comp_array<Eval, true, isIndexed>(env, eval, e, gen, id, rsv.val(), in, a);
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
template <class Eval, bool isIndexed>
void eval_comp_array(EnvI& env, Eval& eval, Comprehension* e, int gen, int id, KeepAlive in,
                     EvaluatedCompTmp<typename Eval::ArrayVal>& a) {
  auto* al = in()->cast<ArrayLit>();
  for (unsigned int i = 0; i < al->size(); i++) {
    eval_comp_array<Eval, false, isIndexed>(env, eval, e, gen, id, i, in, a);
  }
}

/**
 * \brief Evaluate comprehension expression
 *
 * Calls \a eval.e for every element of the comprehension \a e and
 * returns a vector with all the evaluated results.
 */
template <class Eval>
EvaluatedComp<typename Eval::ArrayVal> eval_comp(EnvI& env, Eval& eval, Comprehension* e) {
  EvaluatedComp<typename Eval::ArrayVal> a;
  bool isIndexed = e->e()->isa<ArrayLit>() && e->e()->cast<ArrayLit>()->isTuple();
  unsigned int dim = 0;
  if (isIndexed) {
    dim = e->e()->cast<ArrayLit>()->size() - 1;
  }
  EvaluatedCompTmp<typename Eval::ArrayVal> a_tmp(dim);
  if (e->in(0) == nullptr) {
    if (isIndexed) {
      eval_comp_array<Eval, false, true>(env, eval, e, 0, 0, 0, e->in(0), a_tmp);
    } else {
      eval_comp_array<Eval, false, false>(env, eval, e, 0, 0, 0, e->in(0), a_tmp);
    }
  } else {
    KeepAlive in;
    {
      GCLock lock;
      if (e->in(0)->type().dim() == 0) {
        if (e->in(0)->type().isvar()) {
          in = new SetLit(Location(), compute_intset_bounds(env, e->in(0)));
        } else {
          in = new SetLit(Location(), eval_intset(env, e->in(0)));
        }
      } else {
        if (e->in(0)->type().isvar() || e->in(0)->type().cv()) {
          in = eval_array_lit(env, eval.flattenCV(env, e->in(0))());
        } else {
          in = eval_array_lit(env, e->in(0));
        }
      }
    }
    if (e->in(0)->type().dim() == 0) {
      if (isIndexed) {
        eval_comp_set<Eval, true>(env, eval, e, 0, 0, in, a_tmp);
      } else {
        eval_comp_set<Eval, false>(env, eval, e, 0, 0, in, a_tmp);
      }
    } else {
      if (isIndexed) {
        eval_comp_array<Eval, true>(env, eval, e, 0, 0, in, a_tmp);
      } else {
        eval_comp_array<Eval, false>(env, eval, e, 0, 0, in, a_tmp);
      }
    }
  }

  if (isIndexed) {
    IntVal size = 1;
    std::vector<int> dimSize(a_tmp.idxMin.size());
    a.dims.resize(a_tmp.idxMin.size());
    for (unsigned int i = a_tmp.idxMin.size(); (i--) != 0U;) {
      if (!a_tmp.idxMin[i].isFinite() || !a_tmp.idxMax[i].isFinite()) {
        throw EvalError(env, e->loc(), "indexes don't match size of generated array");
      }
      if (a_tmp.idxMin[i] > a_tmp.idxMax[i]) {
        size = 0;
        break;
      }
      IntVal s = (a_tmp.idxMax[i] - a_tmp.idxMin[i] + 1);
      dimSize[i] = size.toInt();  // before multiplication!
      size *= s;
      a.dims[i] = std::make_pair(a_tmp.idxMin[i].toInt(), a_tmp.idxMax[i].toInt());
    }
    if (size != a_tmp.a.size()) {
      throw EvalError(env, e->loc(), "indexes don't match size of generated array");
    }
    a.a.resize(a_tmp.a.size());
    std::vector<bool> seen(a_tmp.a.size(), false);
    unsigned int j = 0;
    for (unsigned int i = 0; i < a_tmp.a.size(); i++) {
      int idx = 0;
      for (unsigned int k = 0; k < a_tmp.idxMin.size(); k++) {
        IntVal curIdx = a_tmp.indexes[j++] - a_tmp.idxMin[k];
        curIdx *= dimSize[k];
        idx += static_cast<int>(curIdx.toInt());
      }
      if (seen[idx]) {
        throw EvalError(env, e->loc(), "comprehension generates multiple entries for same index");
      }
      seen[idx] = true;
      a.a[idx] = a_tmp.a[i];
    }
  } else {
    a.a = a_tmp.a;
    a.dims.emplace_back(1, a.a.size());
  }
  return a;
}

/**
 * \brief Evaluate comprehension expression
 *
 * Calls \a Eval::e for every element of the comprehension \a e and
 * returns a vector with all the evaluated results.
 */
template <class Eval>
EvaluatedComp<typename Eval::ArrayVal> eval_comp(EnvI& env, Comprehension* e) {
  Eval eval;
  return eval_comp<Eval>(env, eval, e);
}

Expression* follow_id(Expression* e);
Expression* follow_id_to_decl(Expression* e);
Expression* follow_id_to_value(Expression* e);

}  // namespace MiniZinc
