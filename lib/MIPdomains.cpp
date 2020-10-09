/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was ! distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <minizinc/MIPdomains.hh>
#include <minizinc/astexception.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/copy.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/flatten.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/hash.hh>

// temporary
#include <minizinc/prettyprinter.hh>
//#include <ostream>

#include <map>
#include <unordered_map>
#include <unordered_set>

/// TODOs
/// TODO  Not going to work for float vars because of round-offs in the domain interval sorting...
/// set_in etc. are ! propagated between views
/// CLEANUP after work: ~destructor
/// Also check initexpr of all vars?  DONE
/// In case of only_range_domains we'd need to register inequalities
///   - so better turn that off TODO
/// CSE for lineq coefs     TODO

///  TODO use integer division instead of INT_EPS
#define INT_EPS 1e-5  // the absolute epsilon for integrality of integer vars.

#define __MZN__MIPDOMAINS__PRINTMORESTATS
#define MZN_DBG_CHECK_ITER_CUTOUT

//   #define __MZN__DBGOUT__MIPDOMAINS__
#ifdef __MZN__DBGOUT__MIPDOMAINS__
#define DBGOUT_MIPD(s) std::cerr << s << std::endl
#define DBGOUT_MIPD__(s) std::cerr << s << std::flush
#define DBGOUT_MIPD_SELF(op) op
#else
#define DBGOUT_MIPD(s) \
  do {                 \
  } while (false)
#define DBGOUT_MIPD__(s) \
  do {                   \
  } while (false)
#define DBGOUT_MIPD_SELF(op) \
  do {                       \
  } while (false)
#endif

namespace MiniZinc {

enum EnumStatIdx__MIPD {
  N_POSTs__all,  // N all POSTs in the model
  N_POSTs__intCmpReif,
  N_POSTs__floatCmpReif,  // in detail
  N_POSTs__intNE,
  N_POSTs__floatNE,
  N_POSTs__setIn,
  N_POSTs__domain,
  N_POSTs__setInReif,
  N_POSTs__eq_encode,
  N_POSTs__intAux,
  N_POSTs__floatAux,
  // Kind of equality connections between involved variables
  N_POSTs__eq2intlineq,
  N_POSTs__eq2floatlineq,
  N_POSTs__int2float,
  N_POSTs__internalvarredef,
  N_POSTs__initexpr1id,
  N_POSTs__initexpr1linexp,
  N_POSTs__initexprN,
  N_POSTs__eqNlineq,
  N_POSTs__eqNmapsize,
  // other
  N_POSTs__varsDirect,
  N_POSTs__varsInvolved,
  N_POSTs__NSubintvMin,
  N_POSTs__NSubintvSum,
  N_POSTs__NSubintvMax,  // as N subintervals
  N_POSTs__SubSizeMin,
  N_POSTs__SubSizeSum,
  N_POSTs__SubSizeMax,  // subintv. size
  N_POSTs__linCoefMin,
  N_POSTs__linCoefMax,
  N_POSTs__cliquesWithEqEncode,
  N_POSTs__clEEEnforced,
  N_POSTs__clEEFound,
  N_POSTs__size
};
extern std::vector<double> MIPD__stats;

enum EnumReifType { RIT_None, RIT_Static, RIT_Reif, RIT_Halfreif };
enum EnumConstrType { CT_None, CT_Comparison, CT_SetIn, CT_Encode };
enum EnumCmpType {
  CMPT_None = 0,
  CMPT_LE = -4,
  CMPT_GE = 4,
  CMPT_EQ = 1,
  CMPT_NE = 3,
  CMPT_LT = -5,
  CMPT_GT = 5,
  CMPT_LE_0 = -6,
  CMPT_GE_0 = 6,
  CMPT_EQ_0 = 2,
  CMPT_LT_0 = -7,
  CMPT_GT_0 = 7
};
enum EnumVarType { VT_None, VT_Int, VT_Float };

/// struct DomainCallType describes & characterizes a possible domain constr call
struct DCT {
  const char* sFuncName = nullptr;
  const std::vector<Type>& aParams;
  //     unsigned iItem;          // call's item number in the flat
  EnumReifType nReifType = RIT_None;     // 0/static/halfreif/reif
  EnumConstrType nConstrType = CT_None;  //
  EnumCmpType nCmpType = CMPT_None;
  EnumVarType nVarType = VT_None;
  FunctionI*& pfi;
  //     double dEps = -1.0;
  DCT(const char* fn, const std::vector<Type>& prm, EnumReifType er, EnumConstrType ec,
      EnumCmpType ecmp, EnumVarType ev, FunctionI*& pfi__)
      : sFuncName(fn),
        aParams(prm),
        nReifType(er),
        nConstrType(ec),
        nCmpType(ecmp),
        nVarType(ev),
        pfi(pfi__) {}
};

template <class N>
struct Interval {
  N left = infMinus(), right = infPlus();
  mutable VarDecl* varFlag = nullptr;
  /*constexpr*/ static N infMinus() {
    return (std::numeric_limits<N>::has_infinity) ? -std::numeric_limits<N>::infinity()
                                                  : std::numeric_limits<N>::lowest();
  }
  /*constexpr*/ static N infPlus() {
    return (std::numeric_limits<N>::has_infinity) ? std::numeric_limits<N>::infinity()
                                                  : std::numeric_limits<N>::max();
  }
  Interval(N a = infMinus(), N b = infPlus()) : left(a), right(b) {}
  bool operator<(const Interval& intv) const { return left < intv.left; }
};
typedef Interval<double> IntvReal;

template <class N>
std::ostream& operator<<(std::ostream& os, const Interval<N>& ii) {
  os << "[ " << ii.left << ", " << ii.right << " ] ";
  return os;
}

template <class N>
class SetOfIntervals : public std::multiset<Interval<N> > {
public:
  using Intv = Interval<N>;
  typedef std::multiset<Interval<N> > Base;
  typedef typename Base::iterator iterator;
  SetOfIntervals() : Base() {}
  SetOfIntervals(std::initializer_list<Interval<N> > il) : Base(il) {}
  template <class Iter>
  SetOfIntervals(Iter i1, Iter i2) : Base(i1, i2) {}
  /// Number of integer values in all the intervals
  /// Assumes the interval bounds are ints
  int cardInt() const;
  /// Max interval length
  N maxInterval() const;
  /// Special insert function: check if interval is ok
  iterator insert(const Interval<N>& iv) {
    if (iv.left > iv.right) {
      DBGOUT_MIPD("Interval " << iv.left << ".." << iv.right
                              << " is empty, difference: " << (iv.right - iv.left) << ". Skipping");
      return Base::end();
    }
    return Base::insert(iv);
  }
  template <class N1>
  void intersect(const SetOfIntervals<N1>& s2);
  /// Assumes open intervals to cut out from closed
  template <class N1>
  void cutDeltas(const SetOfIntervals<N1>& s2, N1 delta);
  template <class N1>
  void cutDeltas(N1 left, N1 right, N1 delta) {
    SetOfIntervals<N1> soi;
    soi.insert(Interval<N1>(left, right));
    cutDeltas(soi, delta);
  }
  /// Cut out an open interval from a set of closed ones (except for infinities)
  void cutOut(const Interval<N>& intv);
  typedef std::pair<iterator, iterator> SplitResult;
  SplitResult split(iterator& it, N pos);
  bool checkFiniteBounds();
  /// Check there are no useless interval splittings
  bool checkDisjunctStrict();
  Interval<N> getBounds() const;
  /// Split domain into the integer values
  /// May assume integer bounds
  void split2Bits();
};  // class SetOfIntervals
typedef SetOfIntervals<double> SetOfIntvReal;

template <class N>
std::ostream& operator<<(std::ostream& os, const SetOfIntervals<N>& soi) {
  os << "[[ ";
  for (auto& ii : soi) {
    os << "[ " << ii.left << ", " << ii.right;
    if (ii.varFlag) {
      os << " @" << ii.varFlag;
    }
    os << " ] ";
  }
  os << "]]";
  return os;
}

template <class Coefs, class Vars>
class LinEqHelper {
public:
  Coefs coefs;
  Vars vd;
  double rhs;
};

template <class Coefs, class Vars>
static std::ostream& operator<<(std::ostream& os, LinEqHelper<Coefs, Vars>& led) {
  os << "( [";
  for (auto c : led.coefs) {
    os << c << ' ';
  }
  os << " ] * [ ";
  for (auto v : led.vd) {
    os << v->id()->str() << ' ';
  }
  os << " ] ) == " << led.rhs;
  return os;
}

typedef LinEqHelper<std::array<double, 2>, std::array<VarDecl*, 2> > LinEq2Vars;
typedef LinEqHelper<std::vector<double>, std::vector<VarDecl*> > LinEq;
//     struct LinEq2Vars {
//       std::array<double, 2> coefs;
//       std::array<PVarDecl, 2> vd = { { 0, 0 } };
//       double rhs;
//     };
//
//     struct LinEq {
//       std::vector<double> coefs;
//       std::vector<VarDecl*> vd;
//       double rhs;
//     };

std::vector<double> MIPD__stats(N_POSTs__size);

template <class T>
static std::vector<T> make_vec(T t1, T t2) {
  T c_array[] = {t1, t2};
  std::vector<T> result(c_array, c_array + sizeof(c_array) / sizeof(c_array[0]));
  return result;
}
template <class T>
static std::vector<T> make_vec(T t1, T t2, T t3) {
  T c_array[] = {t1, t2, t3};
  std::vector<T> result(c_array, c_array + sizeof(c_array) / sizeof(c_array[0]));
  return result;
}
template <class T>
static std::vector<T> make_vec(T t1, T t2, T t3, T t4) {
  T c_array[] = {t1, t2, t3, t4};
  std::vector<T> result(c_array, c_array + sizeof(c_array) / sizeof(c_array[0]));
  return result;
}

class MIPD {
public:
  MIPD(Env* env, bool fV, int nmi, double dmd)
      : nMaxIntv2Bits(nmi), dMaxNValueDensity(dmd), _env(env) {
    getEnv();
    fVerbose = fV;
  }
  static bool fVerbose;
  const int nMaxIntv2Bits = 0;           // Maximal interval length to enforce equality encoding
  const double dMaxNValueDensity = 3.0;  // Maximal ratio cardInt() / size() of a domain
                                         // to enforce ee
  bool doMIPdomains() {
    MIPD__stats[N_POSTs__NSubintvMin] = 1e100;
    MIPD__stats[N_POSTs__SubSizeMin] = 1e100;

    if (!registerLinearConstraintDecls()) {
      return true;
    }
    if (!registerPOSTConstraintDecls()) {  // not declared => no conversions
      return true;
    }
    registerPOSTVariables();
    if (_vVarDescr.empty()) {
      return true;
    }
    constructVarViewCliques();
    if (!decomposeDomains()) {
      return false;
    }
    if (fVerbose) {
      printStats(std::cerr);
    }
    return true;
  }

private:
  Env* _env = nullptr;
  Env* getEnv() {
    MZN_MIPD__assert_hard(_env);
    return _env;
  }

  typedef VarDecl* PVarDecl;

  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* int_lin_eq;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* int_lin_le;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* float_lin_eq;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* float_lin_le;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* int2float;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* lin_exp_int;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* lin_exp_float;

  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> int_lin_eq_t = make_vec(Type::parint(1), Type::varint(1), Type::parint());
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> float_lin_eq_t =
      make_vec(Type::parfloat(1), Type::varfloat(1), Type::parfloat());
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> t_VIVF = make_vec(Type::varint(), Type::varfloat());

  //     double float_lt_EPS_coef__ = 1e-5;

  bool registerLinearConstraintDecls() {
    EnvI& env = getEnv()->envi();
    GCLock lock;

    int_lin_eq = env.model->matchFn(env, constants().ids.int_.lin_eq, int_lin_eq_t, false);
    DBGOUT_MIPD("  int_lin_eq = " << int_lin_eq);
    //       MZN_MIPD__assert_hard(fi);
    //       int_lin_eq = (fi && fi->e()) ? fi : NULL;
    int_lin_le = env.model->matchFn(env, constants().ids.int_.lin_le, int_lin_eq_t, false);
    float_lin_eq = env.model->matchFn(env, constants().ids.float_.lin_eq, float_lin_eq_t, false);
    float_lin_le = env.model->matchFn(env, constants().ids.float_.lin_le, float_lin_eq_t, false);
    int2float = env.model->matchFn(env, constants().ids.int2float, t_VIVF, false);

    lin_exp_int = env.model->matchFn(env, constants().ids.lin_exp, int_lin_eq_t, false);
    lin_exp_float = env.model->matchFn(env, constants().ids.lin_exp, float_lin_eq_t, false);

    return (int_lin_eq != nullptr) && (int_lin_le != nullptr) && (float_lin_eq != nullptr) &&
           (float_lin_le != nullptr);
    // say something...

    //       std::cerr << "  lin_exp_int=" << lin_exp_int << std::endl;
    //       std::cerr << "  lin_exp_float=" << lin_exp_float << std::endl;
    // For this to work, need to define a function, see mzn_only_range_domains()
    //       {
    //         GCLock lock;
    //         Call* call_EPS_for_LT =
    //           new Call(Location(),"mzn_float_lt_EPS_coef__", std::vector<Expression*>());
    //         call_EPS_for_LT->type(Type::parfloat());
    //         call_EPS_for_LT->decl(env.model->matchFn(getEnv()->envi(), call_EPS_for_LT));
    //         float_lt_EPS_coef__ = eval_float(getEnv()->envi(), call_EPS_for_LT);
    //       }
  }
  //   bool matchAndMarkFunction();
  //   std::set<FunctionI*> funcs;

  /// Possible function param sets
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> t_VII = make_vec(Type::varint(), Type::parint());
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> t_VIVI = make_vec(Type::varint(), Type::varint());
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> t_VIIVI = make_vec(Type::varint(), Type::parint(), Type::varint());
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> t_VFVI = make_vec(Type::varfloat(), Type::varint());
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> t_VFVF = make_vec(Type::varfloat(), Type::varfloat());
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> t_VFFVI = make_vec(Type::varfloat(), Type::parfloat(), Type::varint());
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> t_VFFVIF =
      make_vec(Type::varfloat(), Type::parfloat(), Type::varint(), Type::parfloat());
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> t_VFVIF = make_vec(Type::varfloat(), Type::varint(), Type::parfloat());
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> t_VFVIVF = make_vec(Type::varfloat(), Type::varint(), Type::varfloat());
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> t_VFVIVFF =
      make_vec(Type::varfloat(), Type::varint(), Type::varfloat(), Type::parfloat());
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> t_VFVFF = make_vec(Type::varfloat(), Type::varfloat(), Type::parfloat());
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> t_VFFF = make_vec(Type::varfloat(), Type::parfloat(), Type::parfloat());
  //     std::vector<Type> t_VFVFVIF({ Type::varfloat(), Type::varfloat(), Type::varint(),
  //     Type::parfloat() });

  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> t_VIAVI = make_vec(Type::varint(), Type::varint(1));
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> t_VISI = make_vec(Type::varint(), Type::parsetint());
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<Type> t_VISIVI = make_vec(Type::varint(), Type::parsetint(), Type::varint());

  //     std::vector<Type> t_intarray(1);
  //     t_intarray[0] = Type::parint(-1);

  typedef std::unordered_map<FunctionI*, DCT*> M__POSTCallTypes;
  M__POSTCallTypes _mCallTypes;  // actually declared in the input
  std::vector<DCT> _aCT;         // all possible

  // Fails:
  //   DomainCallType a = { NULL, t_VII, RIT_Halfreif, CT_Comparison, CMPT_EQ, VT_Float };

  /// struct VarDescr stores some info about variables involved in domain constr
  struct VarDescr {
    typedef unsigned char boolShort;
    VarDescr(VarDecl* vd_, boolShort fi, double l_ = 0.0, double u_ = 0.0)
        : lb(l_), ub(u_), vd(vd_), fInt(fi) {}
    double lb, ub;
    VarDecl* vd = nullptr;
    int nClique = -1;  // clique number
                       //       std::vector<Call*> aCalls;
    std::vector<ConstraintI*> aCalls;
    boolShort fInt = 0;
    ConstraintI* pEqEncoding = nullptr;
    boolShort fDomainConstrProcessed = 0;
    //       boolShort fPropagatedViews=0;
    //       boolShort fPropagatedLargerEqns=0;
  };

  std::vector<VarDescr> _vVarDescr;

  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* int_le_reif__POST = nullptr;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* int_ge_reif__POST = nullptr;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* int_eq_reif__POST = nullptr;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* int_ne__POST = nullptr;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* float_le_reif__POST = nullptr;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* float_ge_reif__POST = nullptr;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* aux_float_lt_zero_iff_1__POST = nullptr;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* float_eq_reif__POST = nullptr;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* float_ne__POST = nullptr;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* aux_float_eq_zero_if_1__POST = nullptr;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* aux_int_le_zero_if_1__POST = nullptr;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* aux_float_le_zero_if_1__POST = nullptr;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* aux_float_lt_zero_if_1__POST = nullptr;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* equality_encoding__POST = nullptr;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* set_in__POST = nullptr;
  // NOLINTNEXTLINE(readability-identifier-naming)
  FunctionI* set_in_reif__POST = nullptr;

  bool registerPOSTConstraintDecls() {
    EnvI& env = getEnv()->envi();
    GCLock lock;

    _aCT.clear();
    _aCT.emplace_back("int_le_reif__POST", t_VIIVI, RIT_Reif, CT_Comparison, CMPT_LE, VT_Int,
                      int_le_reif__POST);
    _aCT.emplace_back("int_ge_reif__POST", t_VIIVI, RIT_Reif, CT_Comparison, CMPT_GE, VT_Int,
                      int_ge_reif__POST);
    _aCT.emplace_back("int_eq_reif__POST", t_VIIVI, RIT_Reif, CT_Comparison, CMPT_EQ, VT_Int,
                      int_eq_reif__POST);
    _aCT.emplace_back("int_ne__POST", t_VII, RIT_Static, CT_Comparison, CMPT_NE, VT_Int,
                      int_ne__POST);

    _aCT.emplace_back("float_le_reif__POST", t_VFFVIF, RIT_Reif, CT_Comparison, CMPT_LE, VT_Float,
                      float_le_reif__POST);
    _aCT.emplace_back("float_ge_reif__POST", t_VFFVIF, RIT_Reif, CT_Comparison, CMPT_GE, VT_Float,
                      float_ge_reif__POST);
    _aCT.emplace_back("aux_float_lt_zero_iff_1__POST", t_VFVIF, RIT_Reif, CT_Comparison, CMPT_LT,
                      VT_Float, aux_float_lt_zero_iff_1__POST);
    _aCT.emplace_back("float_eq_reif__POST", t_VFFVIF, RIT_Reif, CT_Comparison, CMPT_EQ, VT_Float,
                      float_eq_reif__POST);
    _aCT.emplace_back("float_ne__POST", t_VFFF, RIT_Static, CT_Comparison, CMPT_NE, VT_Float,
                      float_ne__POST);

    _aCT.emplace_back("aux_float_eq_zero_if_1__POST", t_VFVIVF, RIT_Halfreif, CT_Comparison,
                      CMPT_EQ_0, VT_Float, aux_float_eq_zero_if_1__POST);
    _aCT.emplace_back("aux_int_le_zero_if_1__POST", t_VIVI, RIT_Halfreif, CT_Comparison, CMPT_LE_0,
                      VT_Int, aux_int_le_zero_if_1__POST);
    _aCT.emplace_back("aux_float_le_zero_if_1__POST", t_VFVIVF, RIT_Halfreif, CT_Comparison,
                      CMPT_LE_0, VT_Float, aux_float_le_zero_if_1__POST);
    _aCT.emplace_back("aux_float_lt_zero_if_1__POST", t_VFVIVFF, RIT_Halfreif, CT_Comparison,
                      CMPT_LT_0, VT_Float, aux_float_lt_zero_if_1__POST);

    _aCT.emplace_back("equality_encoding__POST", t_VIAVI, RIT_Static, CT_Encode, CMPT_None, VT_Int,
                      equality_encoding__POST);
    _aCT.emplace_back("set_in__POST", t_VISI, RIT_Static, CT_SetIn, CMPT_None, VT_Int,
                      set_in__POST);
    _aCT.emplace_back("set_in_reif__POST", t_VISIVI, RIT_Reif, CT_SetIn, CMPT_None, VT_Int,
                      set_in_reif__POST);
    /// Registering all declared & compatible __POST constraints
    /// (First, cleanup FunctionIs' payload:  -- ! doing now)
    for (int i = 0; i < _aCT.size(); ++i) {
      FunctionI* fi = env.model->matchFn(env, ASTString(_aCT[i].sFuncName), _aCT[i].aParams, false);
      if (fi != nullptr) {
        _mCallTypes[fi] = _aCT.data() + i;
        _aCT[i].pfi = fi;
        //         fi->pPayload = (void*)this;
        //         std::cerr << "  FOund declaration: " << _aCT[i].sFuncName << std::endl;
      } else {
        _aCT[i].pfi = nullptr;
        DBGOUT_MIPD("  MIssing declaration: " << _aCT[i].sFuncName);
        return false;
      }
    }
    return true;
  }

  /// Registering all __POST calls' domain-constrained variables
  void registerPOSTVariables() {
    EnvI& env = getEnv()->envi();
    GCLock lock;
    Model& mFlat = *getEnv()->flat();
    // First, cleanup VarDecls' payload which stores index in _vVarDescr
    for (VarDeclIterator ivd = mFlat.vardecls().begin(); ivd != mFlat.vardecls().end(); ++ivd) {
      ivd->e()->payload(-1);
    }
    // Now add variables with non-contiguous domain
    for (VarDeclIterator ivd = mFlat.vardecls().begin(); ivd != mFlat.vardecls().end(); ++ivd) {
      VarDecl* vd0 = ivd->e();
      bool fNonCtg = false;
      if (vd0->type().isint()) {  // currently only for int vars   TODO
        if (Expression* eDom = vd0->ti()->domain()) {
          IntSetVal* dom = eval_intset(env, eDom);
          fNonCtg = (dom->size() > 1);
        }
      }
      if (fNonCtg) {
        DBGOUT_MIPD(" Variable " << vd0->id()->str() << ": non-contiguous domain "
                                 << (*(vd0->ti()->domain())));
        if (vd0->payload() == -1) {  // ! yet visited
          vd0->payload(static_cast<int>(_vVarDescr.size()));
          _vVarDescr.emplace_back(vd0, vd0->type().isint());  // can use /prmTypes/ as well
          if (vd0->e() != nullptr) {
            checkInitExpr(vd0);
          }
        } else {
          DBGOUT_MIPD__(" (already touched)");
        }
        ++MIPD__stats[N_POSTs__domain];
        ++MIPD__stats[N_POSTs__all];
      }
    }
    // Iterate thru original __POST constraints to mark constrained vars:
    for (ConstraintIterator ic = mFlat.constraints().begin(); ic != mFlat.constraints().end();
         ++ic) {
      if (ic->removed()) {
        continue;
      }
      if (Call* c = ic->e()->dynamicCast<Call>()) {
        auto ipct = _mCallTypes.find(c->decl());
        if (ipct != _mCallTypes.end()) {
          // No ! here because might be deleted immediately in later versions.
          //             ic->remove();                              // mark removed at once
          MZN_MIPD__assert_hard(c->argCount() > 1);
          ++MIPD__stats[N_POSTs__all];
          VarDecl* vd0 = expr2VarDecl(c->arg(0));
          if (nullptr == vd0) {
            DBGOUT_MIPD__("  Call " << *c << ": 1st arg not a VarDecl, removing if eq_encoding...");
            /// Only allow literals as main argument for equality_encoding
            if (equality_encoding__POST ==
                ipct->first) {  //  was MZN_MIPD__assert_hard before MZN 2017
              ic->remove();
            }
            continue;  // ignore this call
          }
          DBGOUT_MIPD__("  Call " << c->id().str() << " uses variable " << vd0->id()->str());
          if (vd0->payload() == -1) {  // ! yet visited
            vd0->payload(static_cast<int>(_vVarDescr.size()));
            _vVarDescr.emplace_back(vd0, vd0->type().isint());  // can use /prmTypes/ as well
            // bounds/domains later for each involved var TODO
            if (vd0->e() != nullptr) {
              checkInitExpr(vd0);
            }
          } else {
            DBGOUT_MIPD__(" (already touched)");
          }
          DBGOUT_MIPD("");
          if (equality_encoding__POST == c->decl()) {
            MZN_MIPD__assert_hard(!_vVarDescr[vd0->payload()].pEqEncoding);
            _vVarDescr[vd0->payload()].pEqEncoding = &*ic;
            DBGOUT_MIPD(" Variable " << vd0->id()->str() << " has eq_encode.");
          }  // + if has aux_ constraints?
          else {
            _vVarDescr[vd0->payload()].aCalls.push_back(&*ic);
          }
        }
      }
    }
    MIPD__stats[N_POSTs__varsDirect] = static_cast<double>(_vVarDescr.size());
  }

  // Should only be called on a newly added variable
  // OR when looking thru all non-touched vars
  /// Checks init expr of a variable
  /// Return true IFF new connection
  /// The bool param requires RHS to be POST-touched
  // Guido: can! be recursive in FZN
  bool checkInitExpr(VarDecl* vd, bool fCheckArg = false) {
    MZN_MIPD__assert_hard(vd->e());
    if (!vd->type().isint() && !vd->type().isfloat()) {
      return false;
    }
    if (!fCheckArg) {
      MZN_MIPD__assert_hard(vd->payload() >= 0);
    }
    if (Id* id = vd->e()->dynamicCast<Id>()) {
      //         const int f1 = ( vd->payload()>=0 );
      //         const int f2 = ( id->decl()->payload()>=0 );
      if (!fCheckArg || (id->decl()->payload() >= 0)) {
        DBGOUT_MIPD__("  Checking init expr  ");
        DBGOUT_MIPD_SELF(debugprint(vd));
        LinEq2Vars led;
        // FAILS:
        //         led.vd = { vd, expr2VarDecl(id->decl()->e()) };
        led.vd = {{vd, expr2VarDecl(vd->e())}};
        led.coefs = {{1.0, -1.0}};
        led.rhs = 0.0;
        put2VarsConnection(led, false);
        ++MIPD__stats[N_POSTs__initexpr1id];
        if (id->decl()->e() != nullptr) {  // no initexpr for initexpr  FAILS on cc-base.mzn
          checkInitExpr(id->decl());
        }
        return true;  // in any case
      }
    } else if (Call* c = vd->e()->dynamicCast<Call>()) {
      if (lin_exp_int == c->decl() || lin_exp_float == c->decl()) {
        //             std::cerr << "  !E call " << std::flush;
        //             debugprint(c);
        MZN_MIPD__assert_hard(c->argCount() == 3);
        //           ArrayLit* al = c->args()[1]->dynamicCast<ArrayLit>();
        auto* al = follow_id(c->arg(1))->cast<ArrayLit>();
        MZN_MIPD__assert_hard(al);
        MZN_MIPD__assert_hard(al->size() >= 1);
        if (al->size() == 1) {  // 1-term scalar product in the rhs
          LinEq2Vars led;
          led.vd = {{vd, expr2VarDecl((*al)[0])}};
          //             const int f1 = ( vd->payload()>=0 );
          //             const int f2 = ( led.vd[1]->payload()>=0 );
          if (!fCheckArg || (led.vd[1]->payload() >= 0)) {
            // Can use a!her map here:
            //               if ( _sCallLinEq2.end() != _sCallLinEq2.find(c) )
            //                 continue;
            //               _sCallLinEq2.insert(c);     // memorize this call
            DBGOUT_MIPD__("  REG 1-LINEXP ");
            DBGOUT_MIPD_SELF(debugprint(vd));
            std::array<double, 1> coef0;
            expr2Array(c->arg(0), coef0);
            led.coefs = {{-1.0, coef0[0]}};
            led.rhs = -expr2Const(c->arg(2));  // MINUS
            put2VarsConnection(led, false);
            ++MIPD__stats[N_POSTs__initexpr1linexp];
            if (led.vd[1]->e() != nullptr) {  // no initexpr for initexpr   FAILS  TODO
              checkInitExpr(led.vd[1]);
            }
            return true;  // in any case
          }
        } else if (true) {  // NOLINT: check larger views always. OK? TODO
          //             if ( vd->payload()>=0 )  {                      // larger views
          // TODO should be here?
          //             std::cerr << " LE_" << al->v().size() << ' ' << std::flush;
          DBGOUT_MIPD("      REG N-LINEXP ");
          DBGOUT_MIPD_SELF(debugprint(vd));
          // Checking all but adding only touched defined vars?
          return findOrAddDefining(vd->id(), c);
        }
      }
    }
    return false;
  }

  /// Build var cliques (i.e. of var pairs viewing each other)
  void constructVarViewCliques() {
    //       std::cerr << "  Model: " << std::endl;
    //       debugprint(getEnv()->flat());

    //     TAgenda agenda(_vVarDescr.size()), agendaNext;
    //     for ( int i=0; i<agenda.size(); ++i )
    //       agenda[i] = i;
    bool fChanges;
    do {
      fChanges = false;
      propagateViews(fChanges);
      propagateImplViews(fChanges);
    } while (fChanges);

    MIPD__stats[N_POSTs__varsInvolved] = static_cast<double>(_vVarDescr.size());
  }

  void propagateViews(bool& fChanges) {
    //      EnvI& env = getEnv()->envi();
    GCLock lock;

    // Iterate thru original 2-variable equalities to mark views:
    Model& mFlat = *getEnv()->flat();

    DBGOUT_MIPD("  CHECK ALL INITEXPR if they access a touched variable:");
    for (VarDeclIterator ivd = mFlat.vardecls().begin(); ivd != mFlat.vardecls().end(); ++ivd) {
      if (ivd->removed()) {
        continue;
      }
      if ((ivd->e()->e() != nullptr) && ivd->e()->payload() < 0           // untouched
          && (ivd->e()->type().isint() || ivd->e()->type().isfloat())) {  // scalars
        if (checkInitExpr(ivd->e(), true)) {
          fChanges = true;
        }
      }
    }

    DBGOUT_MIPD("  CHECK ALL CONSTRAINTS for 2-var equations:");
    for (ConstraintIterator ic = mFlat.constraints().begin(); ic != mFlat.constraints().end();
         ++ic) {
      //         std::cerr << "  SEE constraint: " << "      ";
      //         debugprint(&*ic);
      //         debugprint(c->decl());
      if (ic->removed()) {
        continue;
      }
      if (Call* c = ic->e()->dynamicCast<Call>()) {
        const bool fIntLinEq = int_lin_eq == c->decl();
        const bool fFloatLinEq = float_lin_eq == c->decl();
        if (fIntLinEq || fFloatLinEq) {
          //             std::cerr << "  !E call " << std::flush;
          //             debugprint(c);
          MZN_MIPD__assert_hard(c->argCount() == 3);
          auto* al = follow_id(c->arg(1))->cast<ArrayLit>();
          MZN_MIPD__assert_hard(al);
          if (al->size() == 2) {  // 2-term eqn
            LinEq2Vars led;
            expr2DeclArray(c->arg(1), led.vd);
            // At least 1 touched var:
            if (nullptr != led.vd[0] && nullptr != led.vd[1]) {
              if (led.vd[0]->payload() >= 0 || led.vd[1]->payload() >= 0) {
                if (_sCallLinEq2.end() != _sCallLinEq2.find(c)) {
                  continue;
                }
                _sCallLinEq2.insert(c);  // memorize this call
                DBGOUT_MIPD("  REG 2-call ");
                DBGOUT_MIPD_SELF(debugprint(c));
                led.rhs = expr2Const(c->arg(2));
                expr2Array(c->arg(0), led.coefs);
                MZN_MIPD__assert_hard(2 == led.coefs.size());
                fChanges = true;
                put2VarsConnection(led);
                ++MIPD__stats[fIntLinEq ? N_POSTs__eq2intlineq : N_POSTs__eq2floatlineq];
              }
            }
          } else if (al->size() == 1) {
            static int nn = 0;
            if (++nn <= 7) {
              std::cerr << "  MIPD: LIN_EQ with 1 variable::: " << std::flush;
              std::cerr << (*c) << std::endl;
            }
          } else {  // larger eqns
            // TODO should be here?
            auto* eVD = get_annotation(c->ann(), constants().ann.defines_var);
            if (eVD != nullptr) {
              if (_sCallLinEqN.end() != _sCallLinEqN.find(c)) {
                continue;
              }
              _sCallLinEqN.insert(c);  // memorize this call
              DBGOUT_MIPD("       REG N-call ");
              DBGOUT_MIPD_SELF(debugprint(c));
              Call* pC = eVD->dynamicCast<Call>();
              MZN_MIPD__assert_hard(pC);
              MZN_MIPD__assert_hard(pC->argCount());
              // Checking all but adding only touched defined vars? Seems too long.
              VarDecl* vd = expr2VarDecl(pC->arg(0));
              if ((vd != nullptr) && vd->payload() >= 0) {  // only if touched
                if (findOrAddDefining(pC->arg(0), c)) {
                  fChanges = true;
                }
              }
            }
          }
        } else
            //             const bool fI2F = (int2float==c->decl());
            //             const bool fIVR = (constants().varRedef==c->decl());
            //             if ( fI2F || fIVR ) {
            if (int2float == c->decl() || constants().varRedef == c->decl()) {
          //             std::cerr << "  !E call " << std::flush;
          //             debugprint(c);
          MZN_MIPD__assert_hard(c->argCount() == 2);
          LinEq2Vars led;
          //             led.vd.resize(2);
          led.vd[0] = expr2VarDecl(c->arg(0));
          led.vd[1] = expr2VarDecl(c->arg(1));
          // At least 1 touched var:
          if (led.vd[0]->payload() >= 0 || led.vd[1]->payload() >= 0) {
            if (_sCallInt2Float.end() != _sCallInt2Float.find(c)) {
              continue;
            }
            _sCallInt2Float.insert(c);  // memorize this call
            DBGOUT_MIPD("  REG call ");
            DBGOUT_MIPD_SELF(debugprint(c));
            led.rhs = 0.0;
            led.coefs = {{1.0, -1.0}};
            fChanges = true;
            put2VarsConnection(led);
            ++MIPD__stats[int2float == c->decl() ? N_POSTs__int2float : N_POSTs__internalvarredef];
          }
        }
      }
    }
  }

  /// This vector stores the linear part of a general view
  /// x = <linear part> + rhs
  typedef std::vector<std::pair<VarDecl*, float> > TLinExpLin;
  /// This struct has data describing the rest of a general view
  struct NViewData {
    VarDecl* pVarDefined = nullptr;
    double coef0 = 1.0;
    double rhs;
  };
  typedef std::map<TLinExpLin, NViewData> NViewMap;
  NViewMap _mNViews;

  /// compare to an existing defining linexp, || just add it to the map
  /// adds only touched defined vars
  /// return true iff new linear connection
  // linexp: z = a^T x+b
  // _lin_eq: a^T x == b
  bool findOrAddDefining(Expression* exp, Call* pC) {
    Id* pId = exp->dynamicCast<Id>();
    MZN_MIPD__assert_hard(pId);
    VarDecl* vd = pId->decl();
    MZN_MIPD__assert_hard(vd);
    MZN_MIPD__assert_hard(pC->argCount() == 3);

    TLinExpLin rhsLin;
    NViewData nVRest;
    nVRest.pVarDefined = vd;
    nVRest.rhs = expr2Const(pC->arg(2));

    std::vector<VarDecl*> vars;
    expr2DeclArray(pC->arg(1), vars);
    std::vector<double> coefs;
    expr2Array(pC->arg(0), coefs);
    MZN_MIPD__assert_hard(vars.size() == coefs.size());

    int nVD = 0;
    for (int i = 0; i < vars.size(); ++i) {
      //         MZN_MIPD__assert_hard( 0.0!=std::fabs
      if (vd ==
          vars[i]) {  // when int/float_lin_eq :: defines_var(vd) "Recursive definition of " << *vd
        nVRest.coef0 = -coefs[i];
        nVRest.rhs = -nVRest.rhs;
        ++nVD;
      } else {
        rhsLin.push_back(std::make_pair(vars[i], coefs[i]));
      }
    }
    MZN_MIPD__assert_hard(1 >= nVD);
    std::sort(rhsLin.begin(), rhsLin.end());

    // Divide the equation by the 1st coef
    const double coef1 = rhsLin.begin()->second;
    MZN_MIPD__assert_hard(0.0 != std::fabs(coef1));
    nVRest.coef0 /= coef1;
    nVRest.rhs /= coef1;
    for (auto& rhsL : rhsLin) {
      rhsL.second /= coef1;
    }

    auto it = _mNViews.find(rhsLin);
    if (_mNViews.end() != it &&
        nVRest.pVarDefined != it->second.pVarDefined) {  // don't connect to itself
      LinEq2Vars leq;
      leq.vd = {{nVRest.pVarDefined, it->second.pVarDefined}};
      leq.coefs = {{nVRest.coef0, -it->second.coef0}};  // +, -
      leq.rhs = nVRest.rhs - it->second.rhs;
      put2VarsConnection(leq, false);
      ++MIPD__stats[nVD != 0 ? N_POSTs__eqNlineq : N_POSTs__initexprN];
      return true;
    }
    if (vd->payload() >= 0) {  // only touched
      _mNViews[rhsLin] = nVRest;
      return true;  // can lead to a new connection
    }

    return false;
  }

  static void propagateImplViews(bool& fChanges) {
    //      EnvI& env = getEnv()->envi();
    GCLock lock;

    // TODO
  }

  /// Could be better to mark the calls instead:
  std::unordered_set<Call*> _sCallLinEq2, _sCallInt2Float, _sCallLinEqN;

  class TClique : public std::vector<LinEq2Vars> {  // need more info?
  public:
    /// This function takes the 1st variable && relates all to it
    /// Return false if contrad / disconnected graph
    //       bool findRelations0() {
    //         return true;
    //       }
  };
  typedef std::vector<TClique> TCLiqueList;
  TCLiqueList _aCliques;

  /// register a 2-variable lin eq
  /// add it to the var clique, joining the participants' cliques if needed
  void put2VarsConnection(LinEq2Vars& led, bool fCheckinitExpr = true) {
    MZN_MIPD__assert_hard(led.coefs.size() == led.vd.size());
    MZN_MIPD__assert_hard(led.vd.size() == 2);
    DBGOUT_MIPD__("  Register 2-var connection: " << led);
    /// Check it's not same 2 vars
    if (led.vd[0] == led.vd[1]) {
      MZN_MIPD__assert_soft(
          0, "MIPD: STRANGE: registering var connection to itself: " << led << ", skipping");
      MZN_MIPD__ASSERT_FOR_SAT(fabs(led.coefs[0] + led.coefs[1]) < 1e-6,  // TODO param
                               getEnv()->envi(), led.vd[0]->loc(),
                               "Var connection to itself seems to indicate UNSAT: " << led);
      return;
    }
    // register if new variables
    //       std::vector<bool> fHaveClq(led.vd.size(), false);
    int nCliqueAvailable = -1;
    for (auto* vd : led.vd) {
      if (vd->payload() < 0) {  // ! yet visited
        vd->payload(static_cast<int>(_vVarDescr.size()));
        _vVarDescr.emplace_back(vd, vd->type().isint());  // can use /prmTypes/ as well
        if (fCheckinitExpr && (vd->e() != nullptr)) {
          checkInitExpr(vd);
        }
      } else {
        int nMaybeClq = _vVarDescr[vd->payload()].nClique;
        if (nMaybeClq >= 0) {
          nCliqueAvailable = nMaybeClq;
        }
        //           MZN_MIPD__assert_hard( nCliqueAvailable>=0 );
        //           fHaveClq[i] = true;
      }
    }
    if (nCliqueAvailable < 0) {  // no clique found
      nCliqueAvailable = static_cast<int>(_aCliques.size());
      _aCliques.resize(_aCliques.size() + 1);
    }
    DBGOUT_MIPD(" ...adding to clique " << nCliqueAvailable << " of size "
                                        << _aCliques[nCliqueAvailable].size());
    TClique& clqNew = _aCliques[nCliqueAvailable];
    clqNew.push_back(led);
    for (auto* vd : led.vd) {  // merging cliques
      int& nMaybeClq = _vVarDescr[vd->payload()].nClique;
      if (nMaybeClq >= 0 && nMaybeClq != nCliqueAvailable) {
        TClique& clqOld = _aCliques[nMaybeClq];
        MZN_MIPD__assert_hard(clqOld.size());
        for (auto& eq2 : clqOld) {
          for (auto* vd : eq2.vd) {  // point all the variables to the new clique
            _vVarDescr[vd->payload()].nClique = nCliqueAvailable;
          }
        }
        clqNew.insert(clqNew.end(), clqOld.begin(), clqOld.end());
        clqOld.clear();  // Can use C++11 move      TODO
        DBGOUT_MIPD("    +++ Joining cliques");
      }
      nMaybeClq = nCliqueAvailable;  // Could mark as 'unused'  TODO
    }
  }

  /// Finds a clique variable to which all domain constr are related
  class TCliqueSorter {
    MIPD& _mipd;
    const int _iVarStart;  // this is the first var to which all others are related
  public:
    //       VarDecl* varRef0=0;  // this is the first var to which all others are related
    VarDecl* varRef1 = nullptr;  // this is the 2nd main reference.
                                 // it is a var with eq_encode, ||
                                 // an (integer if any) variable with the least rel. factor
    bool fRef1HasEqEncode = false;
    /// This map stores the relations y = ax+b of all the clique's vars to y
    typedef std::unordered_map<VarDecl*, std::pair<double, double> > TMapVars;
    TMapVars mRef0, mRef1;  // to the main var 0, 1

    class TMatrixVars : public std::unordered_map<VarDecl*, TMapVars> {
    public:
      /// Check existing connection
      template <class IVarDecl>
      bool checkExistingArc(IVarDecl begV, double A, double B, bool fReportRepeat = true) {
        auto it1 = this->find(*begV);
        if (this->end() != it1) {
          auto it2 = it1->second.find(*(begV + 1));
          if (it1->second.end() != it2) {
            MZN_MIPD__assert_hard(std::fabs(it2->second.first - A) <
                                  1e-6 * std::max(std::fabs(it2->second.first), std::fabs(A)));
            MZN_MIPD__assert_hard(std::fabs(it2->second.second - B) <
                                  1e-6 * std::max(std::fabs(it2->second.second), std::fabs(B)) +
                                      1e-6);
            MZN_MIPD__assert_hard(std::fabs(A) != 0.0);
            MZN_MIPD__assert_soft(!fVerbose || std::fabs(A) > 1e-12,
                                  " Very small coef: " << (*begV)->id()->str() << " = " << A
                                                       << " * " << (*(begV + 1))->id()->str()
                                                       << " + " << B);
            if (fReportRepeat) {
              MZN_MIPD__assert_soft(!fVerbose, "LinEqGraph: eqn between "
                                                   << (*begV)->id()->str() << " && "
                                                   << (*(begV + 1))->id()->str()
                                                   << " is repeated. ");
            }
            return true;
          }
        }
        return false;
      }
    };

    class LinEqGraph : public TMatrixVars {
    public:
      static double dCoefMin, dCoefMax;
      /// Stores the arc (x1, x2) as x1 = a*x2 + b
      /// so that a constraint on x2, say x2<=c <-> f,
      /// is equivalent to one for x1:  x1 <=/>= a*c+b <-> f
      //// ( the other way involves division:
      ////   so that a constraint on x1, say x1<=c <-> f,
      ////   can easily be converted into one for x2 as a*x2 <= c-b <-> f
      ////   <=> x2 (care for sign) (c-b)/a <-> f )

      template <class ICoef, class IVarDecl>
      void addArc(ICoef begC, IVarDecl begV, double rhs) {
        MZN_MIPD__assert_soft(!fVerbose || std::fabs(*begC) >= 1e-10,
                              "  Vars " << (*begV)->id()->str() << "  to "
                                        << (*(begV + 1))->id()->str() << ": coef=" << (*begC));
        // Transform Ax+By=C into x = -B/Ay+C/A
        const double negBA = -(*(begC + 1)) / (*begC);
        const double CA = rhs / (*begC);
        checkExistingArc(begV, negBA, CA);
        (*this)[*begV][*(begV + 1)] = std::make_pair(negBA, CA);
        const double dCoefAbs = std::fabs(negBA);
        if (dCoefAbs < dCoefMin) {
          dCoefMin = dCoefAbs;
        }
        if (dCoefAbs > dCoefMax) {
          dCoefMax = dCoefAbs;
        }
      }
      void addEdge(const LinEq2Vars& led) {
        addArc(led.coefs.begin(), led.vd.begin(), led.rhs);
        addArc(led.coefs.rbegin(), led.vd.rbegin(), led.rhs);
      }
      /// Propagate linear relations from the given variable
      void propagate(iterator itStart, TMapVars& mWhereStore) {
        MZN_MIPD__assert_hard(this->end() != itStart);
        TMatrixVars mTemp;
        mTemp[itStart->first] = itStart->second;  // init with existing
        DBGOUT_MIPD("Propagation started from " << itStart->first->id()->str() << "  having "
                                                << itStart->second.size() << " connections");
        propagate2(itStart, itStart, std::make_pair(1.0, 0.0), mTemp);
        mWhereStore = mTemp.begin()->second;
        MZN_MIPD__assert_hard_msg(
            mWhereStore.size() == this->size() - 1,
            "Variable " << (*(mTemp.begin()->first))
                        << " should be connected to all others in the clique, but "
                        << "|edges|==" << mWhereStore.size() << ", |all nodes|==" << this->size());
      }
      /// Propagate linear relations from it1 via it2
      void propagate2(iterator itSrc, iterator itVia, std::pair<double, double> rel,
                      TMatrixVars& mWhereStore) {
        for (auto itDst = itVia->second.begin(); itDst != itVia->second.end(); ++itDst) {
          // Transform x1=A1x2+B1, x2=A2x3+B2 into x1=A1A2x3+A1B2+B1
          if (itDst->first == itSrc->first) {
            continue;
          }
          const double A1A2 = rel.first * itDst->second.first;
          const double A1B2plusB1 = rel.first * itDst->second.second + rel.second;
          bool fDive = true;
          if (itSrc != itVia) {
            PVarDecl vd[2] = {itSrc->first, itDst->first};
            if (!mWhereStore.checkExistingArc(vd, A1A2, A1B2plusB1, false)) {
              mWhereStore[vd[0]][vd[1]] = std::make_pair(A1A2, A1B2plusB1);
              DBGOUT_MIPD("   PROPAGATING: " << vd[0]->id()->str() << " = " << A1A2 << " * "
                                             << vd[1]->id()->str() << " + " << A1B2plusB1);
            } else {
              fDive = false;
            }
          }
          if (fDive) {
            auto itDST = this->find(itDst->first);
            MZN_MIPD__assert_hard(this->end() != itDST);
            propagate2(itSrc, itDST, std::make_pair(A1A2, A1B2plusB1), mWhereStore);
          }
        }
      }
    };
    LinEqGraph leg;

    TCliqueSorter(MIPD* pm, int iv) : _mipd(*pm), _iVarStart(iv) {}
    void doRelate() {
      MZN_MIPD__assert_hard(_mipd._vVarDescr[_iVarStart].nClique >= 0);
      const TClique& clq = _mipd._aCliques[_mipd._vVarDescr[_iVarStart].nClique];
      for (const auto& eq2 : clq) {
        leg.addEdge(eq2);
      }
      DBGOUT_MIPD(" Clique " << _mipd._vVarDescr[_iVarStart].nClique << ": " << leg.size()
                             << " variables, " << clq.size() << " connections.");
      for (auto& it1 : leg) {
        _mipd._vVarDescr[it1.first->payload()].fDomainConstrProcessed = 1U;
      }

      // Propagate the 1st var's relations:
      leg.propagate(leg.begin(), mRef0);

      // Find a best main variable according to:
      // 1. isInt 2. hasEqEncode 3. abs linFactor to ref0
      varRef1 = leg.begin()->first;
      std::array<double, 3> aCrit = {
          {(double)_mipd._vVarDescr[varRef1->payload()].fInt,
           static_cast<double>(_mipd._vVarDescr[varRef1->payload()].pEqEncoding != nullptr), 1.0}};
      for (auto& it2 : mRef0) {
        VarDescr& vard = _mipd._vVarDescr[it2.first->payload()];
        std::array<double, 3> aCrit1 = {{(double)vard.fInt,
                                         static_cast<double>(vard.pEqEncoding != nullptr),
                                         std::fabs(it2.second.first)}};
        if (aCrit1 > aCrit) {
          varRef1 = it2.first;
          aCrit = aCrit1;
        }
      }
      leg.propagate(leg.find(varRef1), mRef1);
    }
  };  // class TCliqueSorter

  /// Build a domain decomposition for a clique
  /// a clique can consist of just 1 var without a clique object
  class DomainDecomp {
  public:
    MIPD& mipd;
    const int iVarStart;
    TCliqueSorter cls;
    SetOfIntvReal sDomain;

    DomainDecomp(MIPD* pm, int iv) : mipd(*pm), iVarStart(iv), cls(pm, iv) {
      sDomain.insert(IntvReal());  // the decomposed domain. Init to +-inf
    }
    void doProcess() {
      // Choose the main variable && relate all others to it
      const int nClique = mipd._vVarDescr[iVarStart].nClique;
      if (nClique >= 0) {
        cls.doRelate();
      } else {
        cls.varRef1 = mipd._vVarDescr[iVarStart].vd;
      }
      // Adding itself:
      cls.mRef1[cls.varRef1] = std::make_pair(1.0, 0.0);

      int iVarRef1 = cls.varRef1->payload();
      MZN_MIPD__assert_hard(nClique == mipd._vVarDescr[iVarRef1].nClique);
      cls.fRef1HasEqEncode = (mipd._vVarDescr[iVarRef1].pEqEncoding != nullptr);

      // First, construct the domain decomposition in any case
      //         projectVariableConstr( cls.varRef1, std::make_pair(1.0, 0.0) );
      //         if ( nClique >= 0 ) {
      for (auto& iRef1 : cls.mRef1) {
        projectVariableConstr(iRef1.first, iRef1.second);
      }

      DBGOUT_MIPD("Clique " << nClique << ": main ref var " << cls.varRef1->id()->str()
                            << ", domain dec: " << sDomain);

      MZN_MIPD__ASSERT_FOR_SAT(!sDomain.empty(), mipd.getEnv()->envi(), cls.varRef1->loc(),
                               "clique " << nClique << ": main ref var " << *cls.varRef1->id()
                                         << ", domain decomposition seems empty: " << sDomain);

      MZN_MIPD__FLATTENING_ERROR__IF_NOT(sDomain.checkFiniteBounds(), mipd.getEnv()->envi(),
                                         cls.varRef1->loc(),
                                         "variable " << *cls.varRef1->id()
                                                     << " needs finite bounds for linearisation."
                                                        " Or, use indicator constraints. "
                                                     << "Current domain is " << sDomain);

      MZN_MIPD__assert_hard(sDomain.checkDisjunctStrict());

      makeRangeDomains();

      // Then, use equality_encoding if available
      if (cls.fRef1HasEqEncode) {
        syncWithEqEncoding();
        syncOtherEqEncodings();
      } else {                      // ! cls.fRef1HasEqEncode
        if (sDomain.size() >= 2) {  // need to simplify stuff otherwise
          considerDenseEncoding();
          createDomainFlags();
        }
      }
      implementPOSTs();

      // Statistics
      if (sDomain.size() < MIPD__stats[N_POSTs__NSubintvMin]) {
        MIPD__stats[N_POSTs__NSubintvMin] = static_cast<double>(sDomain.size());
      }
      MIPD__stats[N_POSTs__NSubintvSum] += sDomain.size();
      if (sDomain.size() > MIPD__stats[N_POSTs__NSubintvMax]) {
        MIPD__stats[N_POSTs__NSubintvMax] = static_cast<double>(sDomain.size());
      }
      for (const auto& intv : sDomain) {
        const auto nSubSize = intv.right - intv.left;
        if (nSubSize < MIPD__stats[N_POSTs__SubSizeMin]) {
          MIPD__stats[N_POSTs__SubSizeMin] = nSubSize;
        }
        MIPD__stats[N_POSTs__SubSizeSum] += nSubSize;
        if (nSubSize > MIPD__stats[N_POSTs__SubSizeMax]) {
          MIPD__stats[N_POSTs__SubSizeMax] = nSubSize;
        }
      }
      if (cls.fRef1HasEqEncode) {
        ++MIPD__stats[N_POSTs__cliquesWithEqEncode];
      }
    }

    /// Project the domain-related constraints of a variable into the clique
    /// Deltas should be scaled but to a minimum of the target's discr
    /// COmparison sense changes on negated vars
    void projectVariableConstr(VarDecl* vd, std::pair<double, double> eq1) {
      DBGOUT_MIPD__("  MIPD: projecting variable  ");
      DBGOUT_MIPD_SELF(debugprint(vd));
      // Always check if domain becomes empty?         TODO
      const double A = eq1.first;  // vd = A*arg + B.  conversion
      const double B = eq1.second;
      // process domain info
      double lb = B;
      double ub = A + B;  // projected bounds for bool
      if (vd->ti()->domain() != nullptr) {
        if (vd->type().isint() || vd->type().isfloat()) {  // INT VAR OR FLOAT VAR
          SetOfIntvReal sD1;
          convertIntSet(vd->ti()->domain(), sD1, cls.varRef1, A, B);
          sDomain.intersect(sD1);
          DBGOUT_MIPD(" Clique domain after proj of the init. domain "
                      << sD1 << " of " << (vd->type().isint() ? "varint" : "varfloat") << A << " * "
                      << vd->id()->str() << " + " << B << ":  " << sDomain);
          auto bnds = sD1.getBounds();
          lb = bnds.left;
          ub = bnds.right;
        } else {
          MZN_MIPD__FLATTENING_ERROR__IF_NOT(0, mipd.getEnv()->envi(), cls.varRef1->loc(),
                                             "Variable " << vd->id()->str() << " of type "
                                                         << vd->type().toString(mipd._env->envi())
                                                         << " has a domain.");
        }
        //           /// Deleting var domain:
        //           vd->ti()->domain( NULL );
      } else {
        if (nullptr == vd->ti()->domain() && !vd->type().isbool()) {
          lb = IntvReal::infMinus();
          ub = IntvReal::infPlus();
        }
      }
      auto bnds = sDomain.getBounds();  // can change    TODO
      // process calls. Can use the constr type info.
      auto& aCalls = mipd._vVarDescr[vd->payload()].aCalls;
      for (Item* pItem : aCalls) {
        auto* pCI = pItem->dynamicCast<ConstraintI>();
        MZN_MIPD__assert_hard(pCI != nullptr);
        Call* pCall = pCI->e()->dynamicCast<Call>();
        MZN_MIPD__assert_hard(pCall != nullptr);
        DBGOUT_MIPD__("PROPAG CALL  ");
        DBGOUT_MIPD_SELF(debugprint(pCall));
        // check the bounds for bool in reifs?                     TODO
        auto ipct = mipd._mCallTypes.find(pCall->decl());
        MZN_MIPD__assert_hard(mipd._mCallTypes.end() != ipct);
        const DCT& dct = *ipct->second;
        int nCmpType_ADAPTED = dct.nCmpType;
        if (A < 0.0) {                            // negative factor
          if (std::abs(nCmpType_ADAPTED) >= 4) {  // inequality
            nCmpType_ADAPTED = -nCmpType_ADAPTED;
          }
        }
        switch (dct.nConstrType) {
          case CT_SetIn: {
            SetOfIntvReal SS;
            convertIntSet(pCall->arg(1), SS, cls.varRef1, A, B);
            if (RIT_Static == dct.nReifType) {
              sDomain.intersect(SS);
              ++MIPD__stats[N_POSTs__setIn];
            } else {
              sDomain.cutDeltas(SS, std::max(1.0, std::fabs(A)));  // deltas to scale
              ++MIPD__stats[N_POSTs__setInReif];
            }
          } break;
          case CT_Comparison:
            if (RIT_Reif == dct.nReifType) {
              const double rhs = (mipd.aux_float_lt_zero_iff_1__POST == pCall->decl())
                                     ? B /* + A*0.0, relating to 0 */
                                     // The 2nd argument is constant:
                                     : A * MIPD::expr2Const(pCall->arg(1)) + B;
              const double rhsUp = rndUpIfInt(cls.varRef1, rhs);
              const double rhsDown = rndDownIfInt(cls.varRef1, rhs);
              const double rhsRnd = rndIfInt(cls.varRef1, rhs);
              /// Strictly, for delta we should finish domain reductions first...   TODO?
              const double delta = computeDelta(cls.varRef1, vd, bnds, A, pCall, 3);
              switch (nCmpType_ADAPTED) {
                case CMPT_LE:
                  sDomain.cutDeltas(IntvReal::infMinus(), rhsDown, delta);
                  break;
                case CMPT_GE:
                  sDomain.cutDeltas(rhsUp, IntvReal::infPlus(), delta);
                  break;
                case CMPT_LT_0:
                  sDomain.cutDeltas(IntvReal::infMinus(), rhsDown - delta, delta);
                  break;
                case CMPT_GT_0:
                  sDomain.cutDeltas(rhsUp + delta, IntvReal::infPlus(), delta);
                  break;
                case CMPT_EQ:
                  if (!(cls.varRef1->type().isint() &&         // skip if int target var
                        std::fabs(rhs - rhsRnd) > INT_EPS)) {  // && fract value
                    sDomain.cutDeltas(rhsRnd, rhsRnd, delta);
                  }
                  break;
                default:
                  MZN_MIPD__assert_hard_msg(0, " No other reified cmp type ");
              }
              ++MIPD__stats[(vd->ti()->type().isint()) ? N_POSTs__intCmpReif
                                                       : N_POSTs__floatCmpReif];
            } else if (RIT_Static == dct.nReifType) {
              // _ne, later maybe static ineq                                 TODO
              MZN_MIPD__assert_hard(CMPT_NE == dct.nCmpType);
              const double rhs = A * MIPD::expr2Const(pCall->arg(1)) + B;
              const double rhsRnd = rndIfInt(cls.varRef1, rhs);
              bool fSkipNE = (cls.varRef1->type().isint() && std::fabs(rhs - rhsRnd) > INT_EPS);
              if (!fSkipNE) {
                const double delta = computeDelta(cls.varRef1, vd, bnds, A, pCall, 2);
                sDomain.cutOut({rhsRnd - delta, rhsRnd + delta});
              }
              ++MIPD__stats[(vd->ti()->type().isint()) ? N_POSTs__intNE : N_POSTs__floatNE];
            } else {  // aux_ relate to 0.0
                      // But we don't modify domain splitting for them currently
              ++MIPD__stats[(vd->ti()->type().isint()) ? N_POSTs__intAux : N_POSTs__floatAux];
              MZN_MIPD__assert_hard(RIT_Halfreif == dct.nReifType);
              //                 const double rhs = B;               // + A*0
              //                 const double delta = vd->type().isint() ? 1.0 : 1e-5;           //
              //                 TODO : eps
            }
            break;
          case CT_Encode:
            // See if any further constraints here?                             TODO
            ++MIPD__stats[N_POSTs__eq_encode];
            break;
          default:
            MZN_MIPD__assert_hard_msg(0, "Unknown constraint type");
        }
      }
      DBGOUT_MIPD(" Clique domain after proj of " << A << " * " << vd->id()->str() << " + " << B
                                                  << ":  " << sDomain);
    }

    static double rndIfInt(VarDecl* vdTarget, double v) {
      return vdTarget->type().isint() ? std::round(v) : v;
    }
    static double rndIfBothInt(VarDecl* vdTarget, double v) {
      if (!vdTarget->type().isint()) {
        return v;
      }
      const double vRnd = std::round(v);
      return (fabs(v - vRnd) < INT_EPS) ? vRnd : v;
    }
    static double rndUpIfInt(VarDecl* vdTarget, double v) {
      return vdTarget->type().isint() ? std::ceil(v - INT_EPS) : v;
    }
    static double rndDownIfInt(VarDecl* vdTarget, double v) {
      return vdTarget->type().isint() ? std::floor(v + INT_EPS) : v;
    }

    void makeRangeDomains() {
      auto bnds = sDomain.getBounds();
      for (auto& iRef1 : cls.mRef1) {
        VarDecl* vd = iRef1.first;
        // projecting the bounds back:
        double lb0 = (bnds.left - iRef1.second.second) / iRef1.second.first;
        double ub0 = (bnds.right - iRef1.second.second) / iRef1.second.first;
        if (lb0 > ub0) {
          MZN_MIPD__assert_hard(iRef1.second.first < 0.0);
          std::swap(lb0, ub0);
        }
        if (vd->type().isint()) {
          lb0 = rndUpIfInt(vd, lb0);
          ub0 = rndDownIfInt(vd, ub0);
        }
        setVarDomain(vd, lb0, ub0);
      }
    }

    /// tightens element bounds in the existing eq_encoding of varRef1
    /// necessary because if one exists, int_ne is not translated into it
    /// Can also back-check from there?   TODO
    /// And further checks                TODO
    void syncWithEqEncoding() {
      std::vector<Expression*> pp;
      auto bnds = sDomain.getBounds();
      const long long iMin = mipd.expr2ExprArray(
          mipd._vVarDescr[cls.varRef1->payload()].pEqEncoding->e()->dynamicCast<Call>()->arg(1),
          pp);
      MZN_MIPD__assert_hard(pp.size() >= bnds.right - bnds.left + 1);
      MZN_MIPD__assert_hard(iMin <= bnds.left);
      long long vEE = iMin;
      DBGOUT_MIPD__(
          "   SYNC EQ_ENCODE( "
          << (*cls.varRef1) << ",   bitflags: "
          << *(mipd._vVarDescr[cls.varRef1->payload()].pEqEncoding->e()->dynamicCast<Call>()->arg(
                 1))
          << " ):  SETTING 0 FLAGS FOR VALUES: ");
      for (const auto& intv : sDomain) {
        for (; static_cast<double>(vEE) < intv.left; ++vEE) {
          if (vEE >= static_cast<long long>(iMin + pp.size())) {
            return;
          }
          if (pp[vEE - iMin]->isa<Id>()) {
            if (pp[vEE - iMin]->dynamicCast<Id>()->decl()->type().isvar()) {
              DBGOUT_MIPD__(vEE << ", ");
              setVarDomain(pp[vEE - iMin]->dynamicCast<Id>()->decl(), 0.0, 0.0);
            }
          }
        }
        vEE = static_cast<long long>(intv.right + 1);
      }
      for (; vEE < static_cast<long long>(iMin + pp.size()); ++vEE) {
        if (pp[vEE - iMin]->isa<Id>()) {
          if (pp[vEE - iMin]->dynamicCast<Id>()->decl()->type().isvar()) {
            DBGOUT_MIPD__(vEE << ", ");
            setVarDomain(pp[vEE - iMin]->dynamicCast<Id>()->decl(), 0.0, 0.0);
          }
        }
      }
      DBGOUT_MIPD("");
    }

    /// sync varRef1's eq_encoding with those of other variables
    void syncOtherEqEncodings() {
      // TODO  This could be in the var projection? No, need the final domain
    }

    /// Depending on params,
    /// create an equality encoding for an integer variable
    /// TODO What if a float's domain is discrete?
    void considerDenseEncoding() {
      if (cls.varRef1->id()->type().isint()) {
        if (sDomain.maxInterval() <= mipd.nMaxIntv2Bits ||
            sDomain.cardInt() <= mipd.dMaxNValueDensity * sDomain.size()) {
          sDomain.split2Bits();
          ++MIPD__stats[N_POSTs__clEEEnforced];
        }
      }
    }

    /// if ! eq_encoding, creates a flag for each subinterval in the domain
    /// && constrains sum(flags)==1
    void createDomainFlags() {
      std::vector<Expression*> vVars(sDomain.size());  // flags for each subinterval
      std::vector<double> vIntvLB(sDomain.size() + 1);
      std::vector<double> vIntvUB__(sDomain.size() + 1);
      int i = 0;
      double dMaxIntv = -1.0;
      for (const auto& intv : sDomain) {
        intv.varFlag = addIntVar(0.0, 1.0);
        vVars[i] = intv.varFlag->id();
        vIntvLB[i] = intv.left;
        vIntvUB__[i] = -intv.right;
        dMaxIntv = std::max(dMaxIntv, intv.right - intv.left);
        ++i;
      }
      // Sum of flags == 1
      std::vector<double> ones(sDomain.size(), 1.0);
      addLinConstr(ones, vVars, CMPT_EQ, 1.0);
      // Domain decomp
      vVars.push_back(cls.varRef1->id());
      vIntvLB[i] = -1.0;  // var1 >= sum(LBi*flagi)
      /// STRICT equality encoding if small intervals
      if (dMaxIntv > 1e-6) {  // EPS = param?   TODO
        vIntvUB__[i] = 1.0;   // var1 <= sum(UBi*flagi)
        addLinConstr(vIntvLB, vVars, CMPT_LE, 0.0);
        addLinConstr(vIntvUB__, vVars, CMPT_LE, 0.0);
      } else {
        ++MIPD__stats[N_POSTs__clEEFound];
        addLinConstr(vIntvLB, vVars, CMPT_EQ, 0.0);
      }
    }

    /// deletes them as well
    void implementPOSTs() {
      auto bnds = sDomain.getBounds();
      for (auto& iRef1 : cls.mRef1) {
        //           DBGOUT_MIPD__( "  MIPD: implementing constraints of variable  " );
        //           DBGOUT_MIPD_SELF( debugprint(vd) );
        VarDecl* vd = iRef1.first;
        auto eq1 = iRef1.second;
        const double A = eq1.first;  // vd = A*arg + B.  conversion
        const double B = eq1.second;
        // process calls. Can use the constr type info.
        auto& aCalls = mipd._vVarDescr[vd->payload()].aCalls;
        for (Item* pItem : aCalls) {
          auto* pCI = pItem->dynamicCast<ConstraintI>();
          MZN_MIPD__assert_hard(pCI);
          Call* pCall = pCI->e()->dynamicCast<Call>();
          MZN_MIPD__assert_hard(pCall);
          DBGOUT_MIPD__("IMPL CALL  ");
          DBGOUT_MIPD_SELF(debugprint(pCall));
          // check the bounds for bool in reifs?                     TODO
          auto ipct = mipd._mCallTypes.find(pCall->decl());
          MZN_MIPD__assert_hard(mipd._mCallTypes.end() != ipct);
          const DCT& dct = *ipct->second;
          int nCmpType_ADAPTED = dct.nCmpType;
          if (A < 0.0) {                            // negative factor
            if (std::abs(nCmpType_ADAPTED) >= 4) {  // inequality
              nCmpType_ADAPTED = -nCmpType_ADAPTED;
            }
          }
          switch (dct.nConstrType) {
            case CT_SetIn:
              if (RIT_Reif == dct.nReifType) {
                SetOfIntvReal SS;
                convertIntSet(pCall->arg(1), SS, cls.varRef1, A, B);
                relateReifFlag(pCall->arg(2), SS);
              }
              break;
            case CT_Comparison:
              if (RIT_Reif == dct.nReifType) {
                const double rhs = (mipd.aux_float_lt_zero_iff_1__POST == pCall->decl())
                                       ? B /* + A*0.0, relating to 0 */
                                       // The 2nd argument is constant:
                                       : A * MIPD::expr2Const(pCall->arg(1)) + B;
                const double rhsUp = rndUpIfInt(cls.varRef1, rhs);
                const double rhsDown = rndDownIfInt(cls.varRef1, rhs);
                const double rhsRnd = rndIfBothInt(
                    cls.varRef1, rhs);  // if the ref var is int, need to round almost-int values
                const double delta = computeDelta(cls.varRef1, vd, bnds, A, pCall, 3);
                switch (nCmpType_ADAPTED) {
                  case CMPT_LE:
                    relateReifFlag(pCall->arg(2), {{IntvReal::infMinus(), rhsDown}});
                    break;
                  case CMPT_GE:
                    relateReifFlag(pCall->arg(2), {{rhsUp, IntvReal::infPlus()}});
                    break;
                  case CMPT_LT_0:
                    relateReifFlag(pCall->arg(1), {{IntvReal::infMinus(), rhsDown - delta}});
                    break;
                  case CMPT_GT_0:
                    relateReifFlag(pCall->arg(1), {{rhsUp + delta, IntvReal::infPlus()}});
                    break;
                  case CMPT_EQ:
                    relateReifFlag(pCall->arg(2), {{rhsRnd, rhsRnd}});
                    break;  // ... but if the value is sign. fractional for an int var, the flag is
                            // set=0
                  default:
                    break;
                }
              } else if (RIT_Static == dct.nReifType) {
                // !hing here for NE
                MZN_MIPD__assert_hard(CMPT_NE == nCmpType_ADAPTED);
              } else {  // aux_ relate to 0.0
                        // But we don't modify domain splitting for them currently
                MZN_MIPD__assert_hard(RIT_Halfreif == dct.nReifType);
                double rhs = B;  // + A*0
                const double rhsUp = rndUpIfInt(cls.varRef1, rhs);
                const double rhsDown = rndDownIfInt(cls.varRef1, rhs);
                const double rhsRnd = rndIfInt(cls.varRef1, rhs);
                double delta = 0.0;
                if (mipd.aux_float_lt_zero_if_1__POST == pCall->decl()) {  // only float && lt
                  delta = computeDelta(cls.varRef1, vd, bnds, A, pCall, 3);
                }
                if (nCmpType_ADAPTED < 0) {
                  delta = -delta;
                }
                if (cls.varRef1->type().isint() && CMPT_EQ_0 != nCmpType_ADAPTED) {
                  if (nCmpType_ADAPTED < 0) {
                    rhs = rhsDown;
                  } else {
                    rhs = rhsUp;
                  }
                } else {
                  rhs += delta;
                }
                // Now we need rhs ! to be in the inner of the domain
                bool fUseDD = true;
                if (!cls.fRef1HasEqEncode) {
                  switch (nCmpType_ADAPTED) {
                    case CMPT_EQ_0: {
                      auto itLB = sDomain.lower_bound(rhsRnd);
                      fUseDD = (itLB->left == rhsRnd && itLB->right == rhsRnd);  // exactly
                    } break;
                    case CMPT_LT_0:
                    case CMPT_LE_0: {
                      auto itUB = sDomain.upper_bound(rhsUp);
                      bool fInner = false;
                      if (sDomain.begin() != itUB) {
                        --itUB;
                        if (itUB->right > rhs) {
                          fInner = true;
                        }
                      }
                      fUseDD = !fInner;
                    } break;
                    case CMPT_GT_0:
                    case CMPT_GE_0: {
                      auto itLB = sDomain.lower_bound(rhsDown);
                      bool fInner = false;
                      if (sDomain.begin() != itLB) {
                        --itLB;
                        if (itLB->right >= rhs) {
                          fInner = true;
                        }
                      }
                      fUseDD = !fInner;
                    } break;
                    default:
                      MZN_MIPD__assert_hard_msg(0, "Unknown halfreif cmp type");
                  }
                }
                if (fUseDD) {  // use sDomain
                  if (CMPT_EQ_0 == nCmpType_ADAPTED) {
                    relateReifFlag(pCall->arg(1), {{rhsRnd, rhsRnd}}, RIT_Halfreif);
                  } else if (nCmpType_ADAPTED < 0) {
                    relateReifFlag(pCall->arg(1), {{IntvReal::infMinus(), rhsDown}}, RIT_Halfreif);
                  } else {
                    relateReifFlag(pCall->arg(1), {{rhsUp, IntvReal::infPlus()}}, RIT_Halfreif);
                  }
                } else {  // use big-M
                  DBGOUT_MIPD("   AUX BY BIG-Ms: ");
                  const bool fLE = (CMPT_EQ_0 == nCmpType_ADAPTED || 0 > nCmpType_ADAPTED);
                  const bool fGE = (CMPT_EQ_0 == nCmpType_ADAPTED || 0 < nCmpType_ADAPTED);
                  // Take integer || float indicator version, depending on the constrained var:
                  const int nIdxInd =  // (VT_Int==dct.nVarType) ?
                                       //          No:             vd->ti()->type().isint() ? 1 : 2;
                      cls.varRef1->ti()->type().isint()
                          ? 1
                          : 2;  // need the type of the variable to be constr
                  MZN_MIPD__assert_hard(static_cast<unsigned int>(nIdxInd) < pCall->argCount());
                  Expression* pInd = pCall->arg(nIdxInd);
                  if (fLE && rhs < bnds.right) {
                    if (rhs >= bnds.left) {
                      std::vector<double> coefs = {1.0, bnds.right - rhs};
                      // Use the float version of indicator:
                      std::vector<Expression*> vars = {cls.varRef1->id(), pInd};
                      addLinConstr(coefs, vars, CMPT_LE, bnds.right);
                    } else {
                      setVarDomain(MIPD::expr2VarDecl(pInd), 0.0, 0.0);
                    }
                  }
                  if (fGE && rhs > bnds.left) {
                    if (rhs <= bnds.right) {
                      std::vector<double> coefs = {-1.0, rhs - bnds.left};
                      std::vector<Expression*> vars = {cls.varRef1->id(), pInd};
                      addLinConstr(coefs, vars, CMPT_LE, -bnds.left);
                    } else {
                      setVarDomain(MIPD::expr2VarDecl(pInd), 0.0, 0.0);
                    }
                  }
                }
              }
              break;
            case CT_Encode:
              // See if any further constraints here?                             TODO
              break;
            default:
              MZN_MIPD__assert_hard_msg(0, "Unknown constraint type");
          }
          pItem->remove();  // removing the call
        }
        // removing the eq_encoding call
        if (mipd._vVarDescr[vd->payload()].pEqEncoding != nullptr) {
          mipd._vVarDescr[vd->payload()].pEqEncoding->remove();
        }
      }
    }

    /// sets varFlag = || <= sum( intv.varFlag : SS )
    void relateReifFlag(Expression* expFlag, const SetOfIntvReal& SS, EnumReifType nRT = RIT_Reif) {
      MZN_MIPD__assert_hard(RIT_Reif == nRT || RIT_Halfreif == nRT);
      //         MZN_MIPD__assert_hard( sDomain.size()>=2 );
      VarDecl* varFlag = MIPD::expr2VarDecl(expFlag);
      std::vector<Expression*> vIntvFlags;
      if (cls.fRef1HasEqEncode) {  // use eq_encoding
        MZN_MIPD__assert_hard(varFlag->type().isint());
        std::vector<Expression*> pp;
        auto bnds = sDomain.getBounds();
        const long long iMin = mipd.expr2ExprArray(
            mipd._vVarDescr[cls.varRef1->payload()].pEqEncoding->e()->dynamicCast<Call>()->arg(1),
            pp);
        MZN_MIPD__assert_hard(pp.size() >= bnds.right - bnds.left + 1);
        MZN_MIPD__assert_hard(iMin <= bnds.left);
        for (const auto& intv : SS) {
          for (long long vv = (long long)std::max(double(iMin), ceil(intv.left));
               vv <= (long long)std::min(double(iMin) + pp.size() - 1, floor(intv.right)); ++vv) {
            vIntvFlags.push_back(pp[vv - iMin]);
          }
        }
      } else {
        MZN_MIPD__assert_hard(varFlag->type().isint());
        for (const auto& intv : SS) {
          auto it1 = sDomain.lower_bound(intv.left);
          auto it2 = sDomain.upper_bound(intv.right);
          auto it11 = it1;
          // Check that we are looking ! into a subinterval:
          if (sDomain.begin() != it11) {
            --it11;
            MZN_MIPD__assert_hard(it11->right < intv.left);
          }
          auto it12 = it2;
          if (sDomain.begin() != it12) {
            --it12;
            MZN_MIPD__assert_hard_msg(it12->right <= intv.right,
                                      "  relateReifFlag for " << intv << " in " << sDomain);
          }
          for (it12 = it1; it12 != it2; ++it12) {
            if (it12->varFlag != nullptr) {
              vIntvFlags.push_back(it12->varFlag->id());
            } else {
              MZN_MIPD__assert_hard(1 == sDomain.size());
              vIntvFlags.push_back(IntLit::a(1));  // just a constant then
            }
          }
        }
      }
      if (!vIntvFlags.empty()) {
        // Could find out if reif is true                  -- TODO && see above for 1 subinterval
        std::vector<double> onesm(vIntvFlags.size(), -1.0);
        onesm.push_back(1.0);
        vIntvFlags.push_back(varFlag->id());
        EnumCmpType nCmpType = (RIT_Reif == nRT) ? CMPT_EQ : CMPT_LE;
        addLinConstr(onesm, vIntvFlags, nCmpType, 0.0);
      } else {  // the reif is false
        setVarDomain(varFlag, 0.0, 0.0);
      }
    }

    static void setVarDomain(VarDecl* vd, double lb, double ub) {
      // need to check if the new range is in the previous bounds...   TODO
      if (vd->type().isfloat()) {
        //           if ( 0.0==lb && 0.0==ub ) {
        auto* newDom =
            new BinOp(Location().introduce(), FloatLit::a(lb), BOT_DOTDOT, FloatLit::a(ub));
        vd->ti()->domain(newDom);
        DBGOUT_MIPD("  NULL OUT:  " << vd->id()->str());
        //           }
      } else if (vd->type().isint() || vd->type().isbool()) {
        auto* newDom = new SetLit(
            Location().introduce(),
            IntSetVal::a(static_cast<long long int>(lb), static_cast<long long int>(ub)));
        //           TypeInst* nti = copy(mipd.getEnv()->envi(),varFlag->ti())->cast<TypeInst>();
        //           nti->domain(newDom);
        vd->ti()->domain(newDom);
      } else {
        MZN_MIPD__assert_hard_msg(0, "Unknown var type ");
      }
    }

    VarDecl* addIntVar(double LB, double UB) {
      //         GCLock lock;
      // Cache them? Only location can be different                    TODO
      auto* newDom =
          new SetLit(Location().introduce(),
                     IntSetVal::a(static_cast<long long int>(LB), static_cast<long long int>(UB)));
      auto* ti = new TypeInst(Location().introduce(), Type::varint(), newDom);
      auto* newVar = new VarDecl(Location().introduce(), ti, mipd.getEnv()->envi().genId());
      newVar->flat(newVar);
      mipd.getEnv()->envi().flatAddItem(new VarDeclI(Location().introduce(), newVar));
      return newVar;
    }

    void addLinConstr(std::vector<double>& coefs, std::vector<Expression*>& vars,
                      EnumCmpType nCmpType, double rhs) {
      std::vector<Expression*> args(3);
      MZN_MIPD__assert_hard(vars.size() >= 2);
      for (auto* v : vars) {
        MZN_MIPD__assert_hard(&v);
        //             throw std::string("addLinConstr: &var=NULL");
        MZN_MIPD__assert_hard_msg(v->isa<Id>() || v->isa<IntLit>() || v->isa<FloatLit>(),
                                  "  expression at " << (&v) << " eid = " << v->eid()
                                                     << " while E_INTLIT=" << Expression::E_INTLIT);
        //             throw std::string("addLinConstr: only id's as variables allowed");
      }
      MZN_MIPD__assert_hard(coefs.size() == vars.size());
      MZN_MIPD__assert_hard(CMPT_EQ == nCmpType || CMPT_LE == nCmpType);
      DBGOUT_MIPD_SELF(  // LinEq leq; leq.coefs=coefs; leq.vd=vars; leq.rhs=rhs;
          DBGOUT_MIPD__(" ADDING " << (CMPT_EQ == nCmpType ? "LIN_EQ" : "LIN_LE") << ": [ ");
          for (auto c
               : coefs) DBGOUT_MIPD__(c << ',');
          DBGOUT_MIPD__(" ] * [ "); for (auto v
                                         : vars) {
            MZN_MIPD__assert_hard(!v->isa<VarDecl>());
            if (v->isa<Id>()) DBGOUT_MIPD__(v->dynamicCast<Id>()->str() << ',');
            //             else if ( v->isa<VarDecl>() )
            //               MZN_MIPD__assert_hard ("addLinConstr: only id's as variables allowed");
            else
              DBGOUT_MIPD__(mipd.expr2Const(v) << ',');
          } DBGOUT_MIPD(" ] " << (CMPT_EQ == nCmpType ? "== " : "<= ") << rhs););
      std::vector<Expression*> nc_c;
      std::vector<Expression*> nx;
      bool fFloat = false;
      for (auto* v : vars) {
        if (!v->type().isint()) {
          fFloat = true;
          break;
        }
      }
      auto sName = constants().ids.float_.lin_eq;  // "int_lin_eq";
      FunctionI* fDecl = mipd.float_lin_eq;
      if (fFloat) {  // MZN_MIPD__assert_hard all vars of same type     TODO
        for (int i = 0; i < vars.size(); ++i) {
          if (fabs(coefs[i]) > 1e-8)  /// Only add terms with non-0 coefs. TODO Eps=param
          {
            nc_c.push_back(FloatLit::a(coefs[i]));
            if (vars[i]->type().isint()) {
              std::vector<Expression*> i2f_args(1);
              i2f_args[0] = vars[i];
              Call* i2f = new Call(Location().introduce(), constants().ids.int2float, i2f_args);
              i2f->type(Type::varfloat());
              i2f->decl(mipd.getEnv()->model()->matchFn(mipd.getEnv()->envi(), i2f, false));
              EE ret = flat_exp(mipd.getEnv()->envi(), Ctx(), i2f, nullptr, constants().varTrue);
              nx.push_back(ret.r());
            } else {
              nx.push_back(vars[i]);  // ->id();   once passing a general expression
            }
          }
        }
        args[2] = FloatLit::a(rhs);
        args[2]->type(Type::parfloat(0));
        args[0] = new ArrayLit(Location().introduce(), nc_c);
        args[0]->type(Type::parfloat(1));
        args[1] = new ArrayLit(Location().introduce(), nx);
        args[1]->type(Type::varfloat(1));
        if (CMPT_LE == nCmpType) {
          sName = constants().ids.float_.lin_le;  // "float_lin_le";
          fDecl = mipd.float_lin_le;
        }
      } else {
        for (int i = 0; i < vars.size(); ++i) {
          if (fabs(coefs[i]) > 1e-8)  /// Only add terms with non-0 coefs. TODO Eps=param
          {
            nc_c.push_back(IntLit::a(static_cast<long long int>(coefs[i])));
            nx.push_back(vars[i]);  //->id();
          }
        }
        args[2] = IntLit::a(static_cast<long long int>(rhs));
        args[2]->type(Type::parint(0));
        args[0] = new ArrayLit(Location().introduce(), nc_c);
        args[0]->type(Type::parint(1));
        args[1] = new ArrayLit(Location().introduce(), nx);
        args[1]->type(Type::varint(1));
        if (CMPT_LE == nCmpType) {
          sName = constants().ids.int_.lin_le;  // "int_lin_le";
          fDecl = mipd.int_lin_le;
        } else {
          sName = constants().ids.int_.lin_eq;  // "int_lin_eq";
          fDecl = mipd.int_lin_eq;
        }
      }
      if (mipd.getEnv()->envi().cseMapEnd() != mipd.getEnv()->envi().cseMapFind(args[0])) {
        DBGOUT_MIPD__(" Found expr ");
        DBGOUT_MIPD_SELF(debugprint(args[0]));
      }
      auto* nc = new Call(Location().introduce(), ASTString(sName), args);
      nc->type(Type::varbool());
      nc->decl(fDecl);
      mipd.getEnv()->envi().flatAddItem(new ConstraintI(Location().introduce(), nc));
    }

    /// domain / reif set of one variable into that for a!her
    void convertIntSet(Expression* e, SetOfIntvReal& s, VarDecl* varTarget, double A, double B) {
      MZN_MIPD__assert_hard(A != 0.0);
      if (e->type().isIntSet()) {
        IntSetVal* S = eval_intset(mipd.getEnv()->envi(), e);
        IntSetRanges domr(S);
        for (; domr(); ++domr) {  // * A + B
          IntVal mmin = domr.min();
          IntVal mmax = domr.max();
          if (A < 0.0) {
            std::swap(mmin, mmax);
          }
          s.insert(IntvReal(  // * A + B
              mmin.isFinite() ? rndUpIfInt(varTarget, (static_cast<double>(mmin.toInt()) * A + B))
                              : IntvReal::infMinus(),
              mmax.isFinite() ? rndDownIfInt(varTarget, (static_cast<double>(mmax.toInt()) * A + B))
                              : IntvReal::infPlus()));
        }
      } else {
        assert(e->type().isFloatSet());
        FloatSetVal* S = eval_floatset(mipd.getEnv()->envi(), e);
        FloatSetRanges domr(S);
        for (; domr(); ++domr) {  // * A + B
          FloatVal mmin = domr.min();
          FloatVal mmax = domr.max();
          if (A < 0.0) {
            std::swap(mmin, mmax);
          }
          s.insert(IntvReal(  // * A + B
              mmin.isFinite() ? rndUpIfInt(varTarget, (mmin.toDouble() * A + B))
                              : IntvReal::infMinus(),
              mmax.isFinite() ? rndDownIfInt(varTarget, (mmax.toDouble() * A + B))
                              : IntvReal::infPlus()));
        }
      }
    }

    /// compute the delta for float strict ineq
    static double computeDelta(VarDecl* var, VarDecl* varOrig, IntvReal bnds, double A, Call* pCall,
                               int nArg) {
      double delta = varOrig->type().isfloat()
                         ? MIPD::expr2Const(pCall->arg(nArg))
                         // * ( bnds.right-bnds.left )   ABANDONED 12.4.18 due to #207
                         : std::fabs(A);  // delta should be scaled as well
      if (var->type().isint()) {          // the projected-onto variable
        delta = std::max(1.0, delta);
      }
      return delta;
    }
  };  // class DomainDecomp

  /// Vars without explicit clique still need a decomposition.
  /// Have !iced all __POSTs, set_in's && eq_encode's to it BEFORE
  /// In each clique, relate all vars to one chosen
  /// Find all "smallest rel. factor" variables, integer && with eq_encode if avail
  /// Re-relate all vars to it
  /// Refer all __POSTs && dom() to it
  /// build domain decomposition
  /// Implement all domain constraints, incl. possible corresp, of eq_encode's
  ///
  /// REMARKS.
  /// ! impose effects of integrality scaling (e.g., int v = int k/3)
  /// BUT when using k's eq_encode?
  /// And when subdividing into intervals
  bool decomposeDomains() {
    //       for (int iClq=0; iClq<_aCliques.size(); ++iClq ) {
    //         TClique& clq = _aCliques[iClq];
    //       }
    bool fRetTrue = true;
    for (int iVar = 0; iVar < _vVarDescr.size(); ++iVar) {
      //         VarDescr& var = _vVarDescr[iVar];
      if (_vVarDescr[iVar].fDomainConstrProcessed == 0U) {
        GCLock lock;
        DomainDecomp dd(this, iVar);
        dd.doProcess();
        _vVarDescr[iVar].fDomainConstrProcessed = 1U;
      }
    }
    // Clean up __POSTs:
    for (auto& vVar : _vVarDescr) {
      for (auto* pCallI : vVar.aCalls) {
        pCallI->remove();
      }
      if (vVar.pEqEncoding != nullptr) {
        vVar.pEqEncoding->remove();
      }
    }
    return fRetTrue;
  }

  static VarDecl* expr2VarDecl(Expression* arg) {
    // The requirement to have actual variable objects
    // might be a limitation if more optimizations are done before...
    // Might need to flexibilize this                       TODO
    //       MZN_MIPD__assert_hard_msg( ! arg->dynamicCast<IntLit>(),
    //                                  "Expression " << *arg << " is an IntLit!" );
    //       MZN_MIPD__assert_hard( ! arg->dynamicCast<FloatLit>() );
    //       MZN_MIPD__assert_hard( ! arg->dynamicCast<BoolLit>() );
    Id* id = arg->dynamicCast<Id>();
    //       MZN_MIPD__assert_hard(id);
    if (nullptr == id) {
      return nullptr;  // the call using this should be ignored?
    }
    VarDecl* vd = id->decl();
    MZN_MIPD__assert_hard(vd);
    return vd;
  }

  /// Fills the vector of vardecls && returns the least index of the array
  template <class Array>
  long long expr2DeclArray(Expression* arg, Array& aVD) {
    ArrayLit* al = eval_array_lit(getEnv()->envi(), arg);
    checkOrResize(aVD, al->size());
    for (unsigned int i = 0; i < al->size(); i++) {
      aVD[i] = expr2VarDecl((*al)[i]);
    }
    return al->min(0);
  }

  /// Fills the vector of expressions && returns the least index of the array
  template <class Array>
  long long expr2ExprArray(Expression* arg, Array& aVD) {
    ArrayLit* al = eval_array_lit(getEnv()->envi(), arg);
    checkOrResize(aVD, al->size());
    for (unsigned int i = 0; i < al->size(); i++) {
      aVD[i] = ((*al)[i]);
    }
    return al->min(0);
  }

  static double expr2Const(Expression* arg) {
    if (auto* il = arg->dynamicCast<IntLit>()) {
      return (static_cast<double>(il->v().toInt()));
    }
    if (auto* fl = arg->dynamicCast<FloatLit>()) {
      return (fl->v().toDouble());
    }
    if (auto* bl = arg->dynamicCast<BoolLit>()) {
      return static_cast<double>(bl->v());
    }
    MZN_MIPD__assert_hard_msg(0, "unexpected expression instead of an int/float/bool literal: eid="
                                     << arg->eid() << " while E_INTLIT=" << Expression::E_INTLIT);

    return 0.0;
  }

  template <class Container, class Elem = int, size_t = 0>
  void checkOrResize(Container& cnt, size_t sz) {
    cnt.resize(sz);
  }

  template <class Elem, size_t N>
  void checkOrResize(std::array<Elem, N>& cnt, size_t sz) {
    MZN_MIPD__assert_hard(cnt.size() == sz);
  }

  template <class Array>
  void expr2Array(Expression* arg, Array& vals) {
    ArrayLit* al = eval_array_lit(getEnv()->envi(), arg);
    //       if ( typeid(typename Array::pointer) == typeid(typename Array::iterator) )  // fixed
    //       array
    //         MZN_MIPD__assert_hard( vals.size() == al->v().size() );
    //       else
    //         vals.resize( al->v().size() );
    checkOrResize(vals, al->size());
    for (unsigned int i = 0; i < al->size(); i++) {
      vals[i] = expr2Const((*al)[i]);
    }
  }

  void printStats(std::ostream& os) {
    //       if ( _aCliques.empty() )
    //         return;
    if (_vVarDescr.empty()) {
      return;
    }
    int nc = 0;
    for (auto& cl : _aCliques) {
      if (!cl.empty()) {
        ++nc;
      }
    }
    for (auto& var : _vVarDescr) {
      if (0 > var.nClique) {
        ++nc;  // 1-var cliques
      }
    }
    //       os << "N cliques " << _aCliques.size() << "  total, "
    //          << nc << " final" << std::endl;
    MZN_MIPD__assert_hard(nc);
    MIPD__stats[N_POSTs__eqNmapsize] = static_cast<double>(_mNViews.size());
    double nSubintvAve = MIPD__stats[N_POSTs__NSubintvSum] / nc;
    MZN_MIPD__assert_hard(MIPD__stats[N_POSTs__NSubintvSum]);
    double dSubSizeAve = MIPD__stats[N_POSTs__SubSizeSum] / MIPD__stats[N_POSTs__NSubintvSum];
    os << MIPD__stats[N_POSTs__all]
       << " POSTs"
#ifdef __MZN__MIPDOMAINS__PRINTMORESTATS
          " [ ";
    for (int i = N_POSTs__intCmpReif; i <= N_POSTs__floatAux; ++i) {
      os << MIPD__stats[i] << ',';
    }
    os << " ], LINEQ [ ";
    for (int i = N_POSTs__eq2intlineq; i <= N_POSTs__eqNmapsize; ++i) {
      os << MIPD__stats[i] << ',';
    }
    os << " ]"
#endif
          ", "
       << MIPD__stats[N_POSTs__varsDirect] << " / " << MIPD__stats[N_POSTs__varsInvolved]
       << " vars, " << nc << " cliques, " << MIPD__stats[N_POSTs__NSubintvMin] << " / "
       << nSubintvAve << " / " << MIPD__stats[N_POSTs__NSubintvMax] << " NSubIntv m/a/m, "
       << MIPD__stats[N_POSTs__SubSizeMin] << " / " << dSubSizeAve << " / "
       << MIPD__stats[N_POSTs__SubSizeMax] << " SubIntvSize m/a/m, "
       << MIPD__stats[N_POSTs__cliquesWithEqEncode] << "+" << MIPD__stats[N_POSTs__clEEEnforced]
       << "(" << MIPD__stats[N_POSTs__clEEFound] << ")"
       << " clq eq_encoded ";
    //       << std::flush
    if (TCliqueSorter::LinEqGraph::dCoefMax > 1.0) {
      os << TCliqueSorter::LinEqGraph::dCoefMin << "--" << TCliqueSorter::LinEqGraph::dCoefMax
         << " abs coefs";
    }
    os << " ... ";
  }

};  // namespace MiniZinc

template <class N>
template <class N1>
void SetOfIntervals<N>::intersect(const SetOfIntervals<N1>& s2) {
  if (s2.empty()) {
    this->clear();
    return;
  }
  this->cutOut(Interval<N>(Interval<N>::infMinus(), (N)s2.begin()->left));
  for (auto is2 = s2.begin(); is2 != s2.end(); ++is2) {
    auto is2next = is2;
    ++is2next;
    this->cutOut(
        Interval<N>(is2->right, s2.end() == is2next ? Interval<N>::infPlus() : (N)is2next->left));
  }
}
template <class N>
template <class N1>
void SetOfIntervals<N>::cutDeltas(const SetOfIntervals<N1>& s2, N1 delta) {
  if (this->empty()) {
    return;
  }
  // What if distance < delta?                 TODO
  for (auto is2 : s2) {
    if (is2.left > Interval<N1>::infMinus()) {
      this->cutOut(Interval<N>(is2.left - delta, is2.left));
    }
    if (is2.right < Interval<N1>::infPlus()) {
      this->cutOut(Interval<N>(is2.right, is2.right + delta));
    }
  }
}
template <class N>
void SetOfIntervals<N>::cutOut(const Interval<N>& intv) {
  DBGOUT_MIPD__("Cutting " << intv << " from " << (*this));
  if (this->empty()) {
    return;
  }
  auto it1 = (Interval<N>::infMinus() == intv.left)
                 ? this->lower_bound(Interval<N>(intv.left, intv.right))
                 : this->upper_bound(Interval<N>(intv.left, intv.right));
  auto it2Del1 = it1;  // from which to delete
  if (this->begin() != it1) {
    --it1;
    const N it1l = it1->left;
    MZN_MIPD__assert_hard(it1l <= intv.left);
    if (it1->right > intv.left) {  // split it
      it2Del1 = split(it1, intv.left).second;
      //         it1->right = intv.left;  READ-ONLY
      //         this->erase(it1);
      //         it1 = this->end();
      //         auto iR = this->insert( Interval<N>( it1l, intv.left ) );
      //         MZN_MIPD__assert_hard( iR.second );
    }
  }
  DBGOUT_MIPD__("; after split 1: " << (*this));
  // Processing the right end:
  auto it2 = this->lower_bound(Interval<N>(intv.right, intv.right + 1));
  auto it2Del2 = it2;
  if (this->begin() != it2) {
    --it2;
    MZN_MIPD__assert_hard(it2->left < intv.right);
    const N it2r = it2->right;
    if ((Interval<N>::infPlus() == intv.right) ? (it2r > intv.right)
                                               : (it2r >= intv.right)) {  // >=: split it
      //         it2Del2 = split( it2, intv.right ).second;
      const bool fEEE = (it2Del1 == it2);
      this->erase(it2);
      it2 = this->end();
      it2Del2 = this->insert(Interval<N>(intv.right, it2r));
      if (fEEE) {
        it2Del1 = it2Del2;
      }
    }
  }
  DBGOUT_MIPD__("; after split 2: " << (*this));
  DBGOUT_MIPD__("; cutting out: " << SetOfIntervals(it2Del1, it2Del2));
#ifdef MZN_DBG_CHECK_ITER_CUTOUT
  {
    auto it = this->begin();
    int nO = 0;
    do {
      if (it == it2Del1) {
        MZN_MIPD__assert_hard(!nO);
        ++nO;
      }
      if (it == it2Del2) {
        MZN_MIPD__assert_hard(1 == nO);
        ++nO;
      }
      if (this->end() == it) {
        break;
      }
      ++it;
    } while (true);
    MZN_MIPD__assert_hard(2 == nO);
  }
#endif
  this->erase(it2Del1, it2Del2);
  DBGOUT_MIPD(" ... gives " << (*this));
}
template <class N>
typename SetOfIntervals<N>::SplitResult SetOfIntervals<N>::split(iterator& it, N pos) {
  MZN_MIPD__assert_hard(pos >= it->left);
  MZN_MIPD__assert_hard(pos <= it->right);
  Interval<N> intvOld = *it;
  this->erase(it);
  auto it_01 = this->insert(Interval<N>(intvOld.left, pos));
  auto it_02 = this->insert(Interval<N>(pos, intvOld.right));
  it = this->end();
  return std::make_pair(it_01, it_02);
}
template <class N>
Interval<N> SetOfIntervals<N>::getBounds() const {
  if (this->empty()) {
    return Interval<N>(Interval<N>::infPlus(), Interval<N>::infMinus());
  }
  auto it2 = this->end();
  --it2;
  return Interval<N>(this->begin()->left, it2->right);
}
template <class N>
bool SetOfIntervals<N>::checkFiniteBounds() {
  if (this->empty()) {
    return false;
  }
  auto bnds = getBounds();
  return bnds.left > Interval<N>::infMinus() && bnds.right < Interval<N>::infPlus();
}
template <class N>
bool SetOfIntervals<N>::checkDisjunctStrict() {
  for (auto it = this->begin(); it != this->end(); ++it) {
    if (it->left > it->right) {
      return false;
    }
    if (this->begin() != it) {
      auto it_1 = it;
      --it_1;
      if (it_1->right >= it->left) {
        return false;
      }
    }
  }
  return true;
}
/// Assumes integer interval bounds
template <class N>
int SetOfIntervals<N>::cardInt() const {
  int nn = 0;
  for (auto it = this->begin(); it != this->end(); ++it) {
    ++nn;
    nn += int(round(it->right - it->left));
  }
  return nn;
}
template <class N>
N SetOfIntervals<N>::maxInterval() const {
  N ll = -1;
  for (auto it = this->begin(); it != this->end(); ++it) {
    ll = std::max(ll, it->right - it->left);
  }
  return ll;
}
/// Assumes integer interval bounds
template <class N>
void SetOfIntervals<N>::split2Bits() {
  Base bsNew;
  for (auto it = this->begin(); it != this->end(); ++it) {
    for (int v = static_cast<int>(round(it->left)); v <= round(it->right); ++v) {
      bsNew.insert(Intv(v, v));
    }
  }
  *(Base*)this = std::move(bsNew);
}

bool MIPD::fVerbose = false;

void mip_domains(Env& env, bool fVerbose, int nmi, double dmd) {
  MIPD mipd(&env, fVerbose, nmi, dmd);
  if (!mipd.doMIPdomains()) {
    GCLock lock;
    env.envi().fail();
  }
}

double MIPD::TCliqueSorter::LinEqGraph::dCoefMin = +1e100;
double MIPD::TCliqueSorter::LinEqGraph::dCoefMax = -1e100;

}  // namespace MiniZinc
