/*
 *  Main authors:
 *     Kevin Leo <kevin.leo@monash.edu>
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/eval_par.hh>
#include <minizinc/solvers/gecode_solverinstance.hh>
#include <minizinc/solvers/gecode/gecode_constraints.hh>
#include <minizinc/solvers/gecode/fzn_space.hh>

using namespace Gecode;


namespace MiniZinc {
  namespace GecodeConstraints {        

    void p_distinct(SolverInstanceBase& s, const Call* call) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs va = gi.arg2intvarargs(call->args()[0]);
      MZ_IntConLevel icl = gi.ann2icl(call->ann());
      unshare(*gi._current_space, va);
      distinct(*gi._current_space, va, icl == MZ_ICL_DEF ? MZ_ICL_DOM : icl);
    }

    void p_distinctOffset(SolverInstanceBase& s, const Call* call) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs va = gi.arg2intvarargs(call->args()[1]);
      unshare(*gi._current_space, va);
      IntArgs oa = gi.arg2intargs(call->args()[0]);
      MZ_IntConLevel icl = gi.ann2icl(call->ann());
      distinct(*gi._current_space, oa, va, icl == MZ_ICL_DEF ? MZ_ICL_DOM : icl);
    }

    void p_all_equal(SolverInstanceBase& s, const Call* call) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs va = gi.arg2intvarargs(call->args()[0]);
      rel(*gi._current_space, va, IRT_EQ, gi.ann2icl(call->ann()));
    }

    void p_int_CMP(GecodeSolverInstance& s, IntRelType irt, const Call* ce) {
      const Annotation& ann = ce->ann();
      Expression* lhs = ce->args()[0];
      Expression* rhs = ce->args()[1];
      if (lhs->type().isvarint()) { 
        if (rhs->type().isvarint()) {
          rel(*s._current_space, s.arg2intvar(lhs), irt, s.arg2intvar(rhs), s.ann2icl(ann));
        } else {
          rel(*s._current_space, s.arg2intvar(lhs), irt, rhs->cast<IntLit>()->v().toInt(), s.ann2icl(ann));
        }
      } else {
        rel(*s._current_space, s.arg2intvar(rhs), swap(irt), lhs->cast<IntLit>()->v().toInt(), s.ann2icl(ann));
      }
    }

    void p_int_eq(SolverInstanceBase& s, const Call* call) {
      p_int_CMP(static_cast<GecodeSolverInstance&>(s), IRT_EQ, call);
    }
    void p_int_ne(SolverInstanceBase& s, const Call* call) {
      p_int_CMP(static_cast<GecodeSolverInstance&>(s), IRT_NQ, call);
    }
    void p_int_ge(SolverInstanceBase& s, const Call* call) {
      p_int_CMP(static_cast<GecodeSolverInstance&>(s), IRT_GQ, call);
    }
    void p_int_gt(SolverInstanceBase& s, const Call* call) {
      p_int_CMP(static_cast<GecodeSolverInstance&>(s), IRT_GR, call);
    }
    void p_int_le(SolverInstanceBase& s, const Call* call) {
      p_int_CMP(static_cast<GecodeSolverInstance&>(s), IRT_LQ, call);
    }
    void p_int_lt(SolverInstanceBase& s, const Call* call) {
      p_int_CMP(static_cast<GecodeSolverInstance&>(s), IRT_LE, call);
    }
    void p_int_CMP_reif(GecodeSolverInstance& s, IntRelType irt, ReifyMode rm, const Call* call) {
      const Annotation& ann =call->ann();
      if (rm == RM_EQV && !call->args()[2]->type().isvar()) {
        if (call->args()[2]->cast<BoolLit>()->v()) {
          p_int_CMP(s, irt, call);
        } else {
          p_int_CMP(s, neg(irt), call);
        }
        return;
      }
      if (call->args()[0]->type().isvarint()) {
        if (call->args()[1]->type().isvarint()) {
          rel(*s._current_space, s.arg2intvar(call->args()[0]), irt, s.arg2intvar(call->args()[1]),
              Reify(s.arg2boolvar(call->args()[2]), rm), s.ann2icl(ann));
        } else {
          rel(*s._current_space, s.arg2intvar(call->args()[0]), irt,
              call->args()[1]->cast<IntLit>()->v().toInt(),
              Reify(s.arg2boolvar(call->args()[2]), rm), s.ann2icl(ann));
        }
      } else {
        rel(*s._current_space, s.arg2intvar(call->args()[1]), swap(irt),
            call->args()[0]->cast<IntLit>()->v().toInt(),
            Reify(s.arg2boolvar(call->args()[2]), rm), s.ann2icl(ann));
      }
    }

    ///* Comparisons */
    void p_int_eq_reif(SolverInstanceBase& s, const Call* call) {
      p_int_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_EQ, RM_EQV, call);
    }
    void p_int_ne_reif(SolverInstanceBase& s, const Call* call) {
      p_int_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_NQ, RM_EQV, call);
    }
    void p_int_ge_reif(SolverInstanceBase& s, const Call* call) {
      p_int_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_GQ, RM_EQV, call);
    }
    void p_int_gt_reif(SolverInstanceBase& s, const Call* call) {
      p_int_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_GR, RM_EQV, call);
    }
    void p_int_le_reif(SolverInstanceBase& s, const Call* call) {
      p_int_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_LQ, RM_EQV, call);
    }
    void p_int_lt_reif(SolverInstanceBase& s, const Call* call) {
      p_int_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_LE, RM_EQV, call);
    }

    void p_int_eq_imp(SolverInstanceBase& s, const Call* call) {
      p_int_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_EQ, RM_IMP, call);
    }
    void p_int_ne_imp(SolverInstanceBase& s, const Call* call) {
      p_int_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_NQ, RM_IMP, call);
    }
    void p_int_ge_imp(SolverInstanceBase& s, const Call* call) {
      p_int_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_GQ, RM_IMP, call);
    }
    void p_int_gt_imp(SolverInstanceBase& s, const Call* call) {
      p_int_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_GR, RM_IMP, call);
    }
    void p_int_le_imp(SolverInstanceBase& s, const Call* call) {
      p_int_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_LQ, RM_IMP, call);
    }
    void p_int_lt_imp(SolverInstanceBase& s, const Call* call) {
      p_int_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_LE, RM_IMP, call);
    }

    void p_int_lin_CMP(GecodeSolverInstance& s, IntRelType irt, const Call* call) {
      const Annotation& ann =call->ann();
      IntArgs ia = s.arg2intargs(call->args()[0]);
      ArrayLit* vars = s.arg2arraylit(call->args()[1]);
      int singleIntVar;
      if (s.isBoolArray(vars,singleIntVar)) {
        if (singleIntVar != -1) {
          if (std::abs(ia[singleIntVar]) == 1 && call->args()[2]->cast<IntLit>()->v().toInt() == 0) {
            IntVar siv = s.arg2intvar(vars->v()[singleIntVar]);
            BoolVarArgs iv = s.arg2boolvarargs(vars, 0, singleIntVar);
            IntArgs ia_tmp(ia.size()-1);
            int count = 0;
            for (int i=0; i<ia.size(); i++) {
              if (i != singleIntVar)
                ia_tmp[count++] = ia[singleIntVar] == -1 ? ia[i] : -ia[i];
            }
            IntRelType t = (ia[singleIntVar] == -1 ? irt : swap(irt));
            linear(*s._current_space, ia_tmp, iv, t, siv, s.ann2icl(ann));
          } else {
            IntVarArgs iv = s.arg2intvarargs(vars);
            linear(*s._current_space, ia, iv, irt, call->args()[2]->cast<IntLit>()->v().toInt(), s.ann2icl(ann));
          }
        } else {
          BoolVarArgs iv = s.arg2boolvarargs(vars);
          linear(*s._current_space, ia, iv, irt, call->args()[2]->cast<IntLit>()->v().toInt(), s.ann2icl(ann));
        }
      } else {
        IntVarArgs iv = s.arg2intvarargs(vars);
        linear(*s._current_space, ia, iv, irt, call->args()[2]->cast<IntLit>()->v().toInt(), s.ann2icl(ann));
      }
    }
    void p_int_lin_CMP_reif(GecodeSolverInstance& s, IntRelType irt, ReifyMode rm, const Call* call) {
      const Annotation& ann =call->ann();
      if (rm == RM_EQV && call->args()[2]->type().isbool()) {
        if (call->args()[2]->cast<BoolLit>()->v()) {
          p_int_lin_CMP(s, irt, call);
        } else {
          p_int_lin_CMP(s, neg(irt), call);
        }
        return;
      }
      IntArgs ia = s.arg2intargs(call->args()[0]);
      ArrayLit* vars = s.arg2arraylit(call->args()[1]);
      int singleIntVar;
      if (s.isBoolArray(vars,singleIntVar)) {
        if (singleIntVar != -1) {
          if (std::abs(ia[singleIntVar]) == 1 && call->args()[2]->cast<IntLit>()->v().toInt() == 0) {
            IntVar siv = s.arg2intvar(vars->v()[singleIntVar]);
            BoolVarArgs iv = s.arg2boolvarargs(vars, 0, singleIntVar);
            IntArgs ia_tmp(ia.size()-1);
            int count = 0;
            for (int i=0; i<ia.size(); i++) {
              if (i != singleIntVar)
                ia_tmp[count++] = ia[singleIntVar] == -1 ? ia[i] : -ia[i];
            }
            IntRelType t = (ia[singleIntVar] == -1 ? irt : swap(irt));
            linear(*s._current_space, ia_tmp, iv, t, siv, Reify(s.arg2boolvar(call->args()[3]), rm), 
                s.ann2icl(ann));
          } else {
            IntVarArgs iv = s.arg2intvarargs(vars);
            linear(*s._current_space, ia, iv, irt, call->args()[2]->cast<IntLit>()->v().toInt(),
                Reify(s.arg2boolvar(call->args()[3]), rm), s.ann2icl(ann));
          }
        } else {
          BoolVarArgs iv = s.arg2boolvarargs(vars);
          linear(*s._current_space, ia, iv, irt, call->args()[2]->cast<IntLit>()->v().toInt(),
              Reify(s.arg2boolvar(call->args()[3]), rm), s.ann2icl(ann));
        }
      } else {
        IntVarArgs iv = s.arg2intvarargs(vars);
        linear(*s._current_space, ia, iv, irt, call->args()[2]->cast<IntLit>()->v().toInt(),
            Reify(s.arg2boolvar(call->args()[3]), rm), 
            s.ann2icl(ann));
      }
    }

    void p_int_lin_eq(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP(static_cast<GecodeSolverInstance&>(s), IRT_EQ, call);
    }
    void p_int_lin_eq_reif(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_EQ, RM_EQV, call);
    }
    void p_int_lin_eq_imp(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_EQ, RM_IMP, call);
    }
    void p_int_lin_ne(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP(static_cast<GecodeSolverInstance&>(s), IRT_NQ, call);
    }
    void p_int_lin_ne_reif(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_NQ, RM_EQV, call);
    }
    void p_int_lin_ne_imp(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_NQ, RM_IMP, call);
    }
    void p_int_lin_le(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP(static_cast<GecodeSolverInstance&>(s), IRT_LQ, call);
    }
    void p_int_lin_le_reif(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_LQ, RM_EQV, call);
    }
    void p_int_lin_le_imp(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_LQ, RM_IMP, call);
    }
    void p_int_lin_lt(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP(static_cast<GecodeSolverInstance&>(s), IRT_LE, call);
    }
    void p_int_lin_lt_reif(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_LE, RM_EQV, call);
    }
    void p_int_lin_lt_imp(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_LE, RM_IMP, call);
    }
    void p_int_lin_ge(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP(static_cast<GecodeSolverInstance&>(s), IRT_GQ, call);
    }
    void p_int_lin_ge_reif(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_GQ, RM_EQV, call);    
    }
    void p_int_lin_ge_imp(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_GQ, RM_IMP, call);    
    }
    void p_int_lin_gt(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP(static_cast<GecodeSolverInstance&>(s), IRT_GR, call);
    }
    void p_int_lin_gt_reif(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_GR, RM_EQV, call);    
    }
    void p_int_lin_gt_imp(SolverInstanceBase& s, const Call* call) {
      p_int_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_GR, RM_IMP, call);    
    }

    void p_bool_lin_CMP(GecodeSolverInstance& s, IntRelType irt, const Call* call) {
      const Annotation& ann =call->ann();
      IntArgs ia = s.arg2intargs(call->args()[0]);
      BoolVarArgs iv = s.arg2boolvarargs(call->args()[1]);
      if (call->args()[2]->type().isvarint())
        linear(*s._current_space, ia, iv, irt, s.resolveVar(call->args()[2]->cast<Id>()->decl()).intVar(s._current_space), s.ann2icl(ann));
      else
        linear(*s._current_space, ia, iv, irt, call->args()[2]->cast<IntLit>()->v().toInt(), s.ann2icl(ann));
    }
    void p_bool_lin_CMP_reif(GecodeSolverInstance& s, IntRelType irt, ReifyMode rm, const Call* call) {
      const Annotation& ann =call->ann();
      if (rm == RM_EQV && call->args()[2]->type().isbool()) {
        if (call->args()[2]->cast<BoolLit>()->v()) {
          p_bool_lin_CMP(s, irt, call);
        } else {
          p_bool_lin_CMP(s, neg(irt), call);
        }
        return;
      }
      IntArgs ia = s.arg2intargs(call->args()[0]);
      BoolVarArgs iv = s.arg2boolvarargs(call->args()[1]);
      if (call->args()[2]->type().isvarint())
        linear(*s._current_space, ia, iv, irt, s.resolveVar(call->args()[2]->cast<Id>()->decl()).intVar(s._current_space),
            Reify(s.arg2boolvar(call->args()[3]), rm), 
            s.ann2icl(ann));
      else
        linear(*s._current_space, ia, iv, irt, call->args()[2]->cast<IntLit>()->v().toInt(),
            Reify(s.arg2boolvar(call->args()[3]), rm), 
            s.ann2icl(ann));
    }
    void p_bool_lin_eq(SolverInstanceBase& s, const Call* call) {
      p_bool_lin_CMP(static_cast<GecodeSolverInstance&>(s), IRT_EQ, call);
    }
    void p_bool_lin_eq_reif(SolverInstanceBase& s, const Call* call) 
    {
      p_bool_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_EQ, RM_EQV, call);
    }
    void p_bool_lin_eq_imp(SolverInstanceBase& s, const Call* call) 
    {
      p_bool_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_EQ, RM_IMP, call);
    }
    void p_bool_lin_ne(SolverInstanceBase& s, const Call* call) {
      p_bool_lin_CMP(static_cast<GecodeSolverInstance&>(s), IRT_NQ, call);
    }
    void p_bool_lin_ne_reif(SolverInstanceBase& s, const Call* call) 
    {
      p_bool_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_NQ, RM_EQV, call);
    }
    void p_bool_lin_ne_imp(SolverInstanceBase& s, const Call* call) 
    {
      p_bool_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_NQ, RM_IMP, call);
    }
    void p_bool_lin_le(SolverInstanceBase& s, const Call* call) {
      p_bool_lin_CMP(static_cast<GecodeSolverInstance&>(s), IRT_LQ, call);
    }
    void p_bool_lin_le_reif(SolverInstanceBase& s, const Call* call) 
    {
      p_bool_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_LQ, RM_EQV, call);
    }
    void p_bool_lin_le_imp(SolverInstanceBase& s, const Call* call) 
    {
      p_bool_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_LQ, RM_IMP, call);
    }
    void p_bool_lin_lt(SolverInstanceBase& s, const Call* call) 
    {
      p_bool_lin_CMP(static_cast<GecodeSolverInstance&>(s), IRT_LE, call);
    }
    void p_bool_lin_lt_reif(SolverInstanceBase& s, const Call* call) 
    {
      p_bool_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_LE, RM_EQV, call);
    }
    void p_bool_lin_lt_imp(SolverInstanceBase& s, const Call* call) 
    {
      p_bool_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_LE, RM_IMP, call);
    }
    void p_bool_lin_ge(SolverInstanceBase& s, const Call* call) {
      p_bool_lin_CMP(static_cast<GecodeSolverInstance&>(s), IRT_GQ, call);
    }
    void p_bool_lin_ge_reif(SolverInstanceBase& s, const Call* call) 
    {
      p_bool_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_GQ, RM_EQV, call);
    }
    void p_bool_lin_ge_imp(SolverInstanceBase& s, const Call* call) 
    {
      p_bool_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_GQ, RM_IMP, call);
    }
    void p_bool_lin_gt(SolverInstanceBase& s, const Call* call) {
      p_bool_lin_CMP(static_cast<GecodeSolverInstance&>(s), IRT_GR, call);
    }
    void p_bool_lin_gt_reif(SolverInstanceBase& s, const Call* call) 
    {
      p_bool_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_GR, RM_EQV, call);
    }
    void p_bool_lin_gt_imp(SolverInstanceBase& s, const Call* call) 
    {
      p_bool_lin_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_GR, RM_IMP, call);
    }

    ///* arithmetic constraints */

    void p_int_plus(SolverInstanceBase& s, const Call* call) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      if (!call->args()[0]->type().isvarint()) {
        rel(*gi._current_space, call->args()[0]->cast<IntLit>()->v().toInt() + gi.arg2intvar(call->args()[1])
            == gi.arg2intvar(call->args()[2]), gi.ann2icl(call->ann()));
      } else if (!call->args()[1]->type().isvarint()) {
        rel(*gi._current_space, gi.arg2intvar(call->args()[0]) + call->args()[1]->cast<IntLit>()->v().toInt()
            == gi.arg2intvar(call->args()[2]), gi.ann2icl(call->ann()));
      } else if (!call->args()[2]->type().isvarint()) {
        rel(*gi._current_space, gi.arg2intvar(call->args()[0]) + gi.arg2intvar(call->args()[1]) 
            == call->args()[2]->cast<IntLit>()->v().toInt(), gi.ann2icl(call->ann()));
      } else {
        rel(*gi._current_space, gi.arg2intvar(call->args()[0]) + gi.arg2intvar(call->args()[1]) 
            == gi.arg2intvar(call->args()[2]), gi.ann2icl(call->ann()));
      }
    }

    void p_int_minus(SolverInstanceBase& s, const Call* call) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      if (!call->args()[0]->type().isvarint()) {
        rel(*gi._current_space, call->args()[0]->cast<IntLit>()->v().toInt() - gi.arg2intvar(call->args()[1])
            == gi.arg2intvar(call->args()[2]), gi.ann2icl(call->ann()));
      } else if (!call->args()[1]->type().isvarint()) {
        rel(*gi._current_space, gi.arg2intvar(call->args()[0]) - call->args()[1]->cast<IntLit>()->v().toInt()
            == gi.arg2intvar(call->args()[2]), gi.ann2icl(call->ann()));
      } else if (!call->args()[2]->type().isvarint()) {
        rel(*gi._current_space, gi.arg2intvar(call->args()[0]) - gi.arg2intvar(call->args()[1]) 
            == call->args()[2]->cast<IntLit>()->v().toInt(), gi.ann2icl(call->ann()));
      } else {
        rel(*gi._current_space, gi.arg2intvar(call->args()[0]) - gi.arg2intvar(call->args()[1]) 
            == gi.arg2intvar(call->args()[2]), gi.ann2icl(call->ann()));
      }
    }

    void p_int_times(SolverInstanceBase& s, const Call* call) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVar x0 = gi.arg2intvar(call->args()[0]);
      IntVar x1 = gi.arg2intvar(call->args()[1]);
      IntVar x2 = gi.arg2intvar(call->args()[2]);
      mult(*gi._current_space, x0, x1, x2, gi.ann2icl(call->ann()));
    }
    void p_int_div(SolverInstanceBase& s, const Call* call) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVar x0 = gi.arg2intvar(call->args()[0]);
      IntVar x1 = gi.arg2intvar(call->args()[1]);
      IntVar x2 = gi.arg2intvar(call->args()[2]);
      div(*gi._current_space,x0,x1,x2, gi.ann2icl(call->ann()));
    }
    void p_int_mod(SolverInstanceBase& s, const Call* call) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVar x0 = gi.arg2intvar(call->args()[0]);
      IntVar x1 = gi.arg2intvar(call->args()[1]);
      IntVar x2 = gi.arg2intvar(call->args()[2]);
      mod(*gi._current_space,x0,x1,x2, gi.ann2icl(call->ann()));
    }

    void p_int_min(SolverInstanceBase& s, const Call* call) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVar x0 = gi.arg2intvar(call->args()[0]);
      IntVar x1 = gi.arg2intvar(call->args()[1]);
      IntVar x2 = gi.arg2intvar(call->args()[2]);
      min(*gi._current_space, x0, x1, x2, gi.ann2icl(call->ann()));
    }
    void p_int_max(SolverInstanceBase& s, const Call* call) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVar x0 = gi.arg2intvar(call->args()[0]);
      IntVar x1 = gi.arg2intvar(call->args()[1]);
      IntVar x2 = gi.arg2intvar(call->args()[2]);
      max(*gi._current_space, x0, x1, x2, gi.ann2icl(call->ann()));
    }
    void p_int_negate(SolverInstanceBase& s, const Call* call) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVar x0 = gi.arg2intvar(call->args()[0]);
      IntVar x1 = gi.arg2intvar(call->args()[1]);
      rel(*gi._current_space, x0 == -x1, gi.ann2icl(call->ann()));
    }

    ///* Boolean constraints */
    void p_bool_CMP(GecodeSolverInstance& s, IntRelType irt, const Call* call) {
      const Annotation& ann =call->ann();
      rel(*s._current_space, s.arg2boolvar(call->args()[0]), irt, s.arg2boolvar(call->args()[1]), s.ann2icl(ann));
    }
    void p_bool_CMP_reif(GecodeSolverInstance& s, IntRelType irt, ReifyMode rm, const Call* call) {
      const Annotation& ann =call->ann();
      rel(*s._current_space, s.arg2boolvar(call->args()[0]), irt, s.arg2boolvar(call->args()[1]),
          Reify(s.arg2boolvar(call->args()[2]), rm), s.ann2icl(ann));
    }
    void p_bool_eq(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP(static_cast<GecodeSolverInstance&>(s), IRT_EQ, call);
    }
    void p_bool_eq_reif(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_EQ, RM_EQV, call);
    }
    void p_bool_eq_imp(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_EQ, RM_IMP, call);
    }
    void p_bool_ne(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP(static_cast<GecodeSolverInstance&>(s), IRT_NQ, call);
    }
    void p_bool_ne_reif(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_NQ, RM_EQV, call);
    }
    void p_bool_ne_imp(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_NQ, RM_IMP, call);
    }
    void p_bool_ge(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP(static_cast<GecodeSolverInstance&>(s), IRT_GQ, call);
    }
    void p_bool_ge_reif(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_GQ, RM_EQV, call);
    }
    void p_bool_ge_imp(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_GQ, RM_IMP, call);
    }
    void p_bool_le(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP(static_cast<GecodeSolverInstance&>(s), IRT_LQ, call);
    }
    void p_bool_le_reif(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_LQ, RM_EQV, call);
    }
    void p_bool_le_imp(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_LQ, RM_IMP, call);
    }
    void p_bool_gt(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP(static_cast<GecodeSolverInstance&>(s), IRT_GR, call);
    }
    void p_bool_gt_reif(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_GR, RM_EQV, call);
    }
    void p_bool_gt_imp(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_GR, RM_IMP, call);
    }
    void p_bool_lt(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP(static_cast<GecodeSolverInstance&>(s), IRT_LE, call);
    }
    void p_bool_lt_reif(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_LE, RM_EQV, call);
    }
    void p_bool_lt_imp(SolverInstanceBase& s, const Call* call) {
      p_bool_CMP_reif(static_cast<GecodeSolverInstance&>(s), IRT_LE, RM_IMP, call);
    }

#define BOOL_OP(op) \
    BoolVar b0 = gi.arg2boolvar(call->args()[0]); \
    BoolVar b1 = gi.arg2boolvar(call->args()[1]); \
    if (!call->args()[2]->type().isvar() && call->args()[2]->type().isbool()) { \
      rel(*gi._current_space, b0, op, b1, call->args()[2]->cast<BoolLit>()->v(), gi.ann2icl(ann)); \
    } else { \
      rel(*gi._current_space, b0, op, b1, gi.resolveVar(gi.getVarDecl(call->args()[2])).boolVar(gi._current_space), gi.ann2icl(ann)); \
    }


#define BOOL_ARRAY_OP(op) \
    BoolVarArgs bv = gi.arg2boolvarargs(call->args()[0]); \
    if (call->args().size()==1) { \
      rel(*gi._current_space, op, bv, 1, gi.ann2icl(ann)); \
    } else if (!call->args()[1]->type().isvar() && call->args()[1]->type().isbool()) { \
      rel(*gi._current_space, op, bv, call->args()[1]->cast<BoolLit>()->v(), gi.ann2icl(ann)); \
    } else { \
      rel(*gi._current_space, op, bv, gi.resolveVar(gi.getVarDecl(call->args()[1])).boolVar(gi._current_space), gi.ann2icl(ann)); \
    }

    void p_bool_or(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BOOL_OP(BoolOpType::BOT_OR);
    }
    void p_bool_or_imp(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVar b0 = gi.arg2boolvar(call->args()[0]);
      BoolVar b1 = gi.arg2boolvar(call->args()[1]);
      BoolVar b2 = gi.arg2boolvar(call->args()[2]);
      clause(*gi._current_space, BoolOpType::BOT_OR, BoolVarArgs()<<b0<<b1, BoolVarArgs()<<b2, 1, 
          gi.ann2icl(ann));
    }
    void p_bool_and(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BOOL_OP(BoolOpType::BOT_AND);
    }
    void p_bool_and_imp(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVar b0 = gi.arg2boolvar(call->args()[0]);
      BoolVar b1 = gi.arg2boolvar(call->args()[1]);
      BoolVar b2 = gi.arg2boolvar(call->args()[2]);
      rel(*gi._current_space, b2, BoolOpType::BOT_IMP, b0, 1, gi.ann2icl(ann));
      rel(*gi._current_space, b2, BoolOpType::BOT_IMP, b1, 1, gi.ann2icl(ann));
    }
    void p_array_bool_and(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BOOL_ARRAY_OP(Gecode::BoolOpType::BOT_AND);
    }
    void p_array_bool_and_imp(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVarArgs bv = gi.arg2boolvarargs(call->args()[0]);
      BoolVar b1 = gi.arg2boolvar(call->args()[1]);
      for (unsigned int i=bv.size(); i--;)
        rel(*gi._current_space, b1, Gecode::BoolOpType::BOT_IMP, bv[i], 1, gi.ann2icl(ann));
    }
    void p_array_bool_or(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BOOL_ARRAY_OP(BoolOpType::BOT_OR);
    }
    void p_array_bool_or_imp(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVarArgs bv = gi.arg2boolvarargs(call->args()[0]);
      BoolVar b1 = gi.arg2boolvar(call->args()[1]);
      clause(*gi._current_space, BoolOpType::BOT_OR, bv, BoolVarArgs()<<b1, 1, gi.ann2icl(ann));
    }
    void p_array_bool_xor(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BOOL_ARRAY_OP(BoolOpType::BOT_XOR);
    }
    void p_array_bool_xor_imp(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVarArgs bv = gi.arg2boolvarargs(call->args()[0]);
      BoolVar tmp(*gi._current_space,0,1);
      rel(*gi._current_space, BoolOpType::BOT_XOR, bv, tmp, gi.ann2icl(ann));
      rel(*gi._current_space, gi.arg2boolvar(call->args()[1]), BoolOpType::BOT_IMP, tmp, 1);
    }
    void p_array_bool_clause(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVarArgs bvp = gi.arg2boolvarargs(call->args()[0]);
      BoolVarArgs bvn = gi.arg2boolvarargs(call->args()[1]);
      clause(*gi._current_space, BoolOpType::BOT_OR, bvp, bvn, 1, gi.ann2icl(ann));
    }
    void p_array_bool_clause_reif(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVarArgs bvp = gi.arg2boolvarargs(call->args()[0]);
      BoolVarArgs bvn = gi.arg2boolvarargs(call->args()[1]);
      BoolVar b0 = gi.arg2boolvar(call->args()[2]);
      clause(*gi._current_space, BoolOpType::BOT_OR, bvp, bvn, b0, gi.ann2icl(ann));
    }
    void p_array_bool_clause_imp(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVarArgs bvp = gi.arg2boolvarargs(call->args()[0]);
      BoolVarArgs bvn = gi.arg2boolvarargs(call->args()[1]);
      BoolVar b0 = gi.arg2boolvar(call->args()[2]);
      clause(*gi._current_space, BoolOpType::BOT_OR, bvp, bvn, b0, gi.ann2icl(ann));
    }
    void p_bool_xor(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BOOL_OP(BoolOpType::BOT_XOR);
    }
    void p_bool_xor_imp(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVar b0 = gi.arg2boolvar(call->args()[0]);
      BoolVar b1 = gi.arg2boolvar(call->args()[1]);
      BoolVar b2 = gi.arg2boolvar(call->args()[2]);
      clause(*gi._current_space, BoolOpType::BOT_OR, BoolVarArgs()<<b0<<b1, BoolVarArgs()<<b2, 1,
          gi.ann2icl(ann));
      clause(*gi._current_space, BoolOpType::BOT_OR, BoolVarArgs(), BoolVarArgs()<<b0<<b1<<b2, 1,
          gi.ann2icl(ann));
    }
    void p_bool_l_imp(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVar b0 = gi.arg2boolvar(call->args()[0]);
      BoolVar b1 = gi.arg2boolvar(call->args()[1]);
      if (call->args()[2]->type().isbool()) {
        rel(*gi._current_space, b1, BoolOpType::BOT_IMP, b0, call->args()[2]->cast<BoolLit>()->v(), gi.ann2icl(ann));
      } else {
        rel(*gi._current_space, b1, BoolOpType::BOT_IMP, b0, gi.resolveVar(call->args()[2]->cast<Id>()->decl()).boolVar(gi._current_space), gi.ann2icl(ann));
      }
    }
    void p_bool_r_imp(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BOOL_OP(BoolOpType::BOT_IMP);
    }
    void p_bool_not(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVar x0 = gi.arg2boolvar(call->args()[0]);
      BoolVar x1 = gi.arg2boolvar(call->args()[1]);
      rel(*gi._current_space, x0, BoolOpType::BOT_XOR, x1, 1, gi.ann2icl(ann));
    }

    ///* element constraints */
    void p_array_int_element(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVar selector = gi.arg2intvar(call->args()[0]);
      rel(*gi._current_space, selector > 0);
      if (call->args()[1]->type().isvar()) {
        IntVarArgs iv = gi.arg2intvarargs(call->args()[1], 1);
        element(*gi._current_space, iv, selector, gi.arg2intvar(call->args()[2]), gi.ann2icl(ann));
      } else {
        IntArgs ia = gi.arg2intargs(call->args()[1], 1);
        element(*gi._current_space, ia, selector, gi.arg2intvar(call->args()[2]), gi.ann2icl(ann));
      }
    }
    void p_array_bool_element(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVar selector = gi.arg2intvar(call->args()[0]);
      rel(*gi._current_space, selector > 0);
      if (call->args()[1]->type().isvar()) {
        BoolVarArgs iv = gi.arg2boolvarargs(call->args()[1], 1);
        element(*gi._current_space, iv, selector, gi.arg2boolvar(call->args()[2]), gi.ann2icl(ann));
      } else {
        IntArgs ia = gi.arg2boolargs(call->args()[1], 1);
        element(*gi._current_space, ia, selector, gi.arg2boolvar(call->args()[2]), gi.ann2icl(ann));
      }
    }

    ///* coercion constraints */
    void p_bool2int(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVar x0 = gi.arg2boolvar(call->args()[0]);
      IntVar x1 = gi.arg2intvar(call->args()[1]);
      if (call->args()[0]->type().isvarbool() && call->args()[1]->type().isvarint()) { 
        int index = gi.resolveVar(call->args()[0]->cast<Id>()->decl()).index();
        gi.resolveVar(call->args()[1]->cast<Id>()->decl()).setBoolAliasIndex(index);
      }
      channel(*gi._current_space, x0, x1, gi.ann2icl(ann));
    }

    void p_int_in(SolverInstanceBase& s, const Call* call) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntSet d = gi.arg2intset(s.env().envi(), call->args()[1]);
      if (call->args()[0]->type().isvarbool()) {
        Gecode::IntSetRanges dr(d);
        Iter::Ranges::Singleton sr(0,1);
        Iter::Ranges::Inter<Gecode::IntSetRanges,Iter::Ranges::Singleton> i(dr,sr);
        IntSet d01(i);
        if (d01.size() == 0) {
          gi._current_space->fail();
        } else {
          rel(*gi._current_space, gi.arg2boolvar(call->args()[0]), IRT_GQ, d01.min());
          rel(*gi._current_space, gi.arg2boolvar(call->args()[0]), IRT_LQ, d01.max());
        }
      } else {
        dom(*gi._current_space, gi.arg2intvar(call->args()[0]), d);
      }
    }
    void p_int_in_reif(SolverInstanceBase& s, const Call* call) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntSet d = gi.arg2intset(s.env().envi(), call->args()[1]);
      if (call->args()[0]->type().isvarbool()) {
        Gecode::IntSetRanges dr(d);
        Iter::Ranges::Singleton sr(0,1);
        Iter::Ranges::Inter<Gecode::IntSetRanges,Iter::Ranges::Singleton> i(dr,sr);
        IntSet d01(i);
        if (d01.size() == 0) {
          rel(*gi._current_space, gi.arg2boolvar(call->args()[2]) == 0);
        } else if (d01.max() == 0) {
          rel(*gi._current_space, gi.arg2boolvar(call->args()[2]) == !gi.arg2boolvar(call->args()[0]));
        } else if (d01.min() == 1) {
          rel(*gi._current_space, gi.arg2boolvar(call->args()[2]) == gi.arg2boolvar(call->args()[0]));
        } else {
          rel(*gi._current_space, gi.arg2boolvar(call->args()[2]) == 1);
        }
      } else {
        dom(*gi._current_space, gi.arg2intvar(call->args()[0]), d, gi.arg2boolvar(call->args()[2]));
      }
    }
    void p_int_in_imp(SolverInstanceBase& s, const Call* call) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntSet d = gi.arg2intset(s.env().envi(), call->args()[1]);
      if (call->args()[0]->type().isvarbool()) {
        Gecode::IntSetRanges dr(d);
        Iter::Ranges::Singleton sr(0,1);
        Iter::Ranges::Inter<Gecode::IntSetRanges,Iter::Ranges::Singleton> i(dr,sr);
        IntSet d01(i);
        if (d01.size() == 0) {
          rel(*gi._current_space, gi.arg2boolvar(call->args()[2]) == 0);
        } else if (d01.max() == 0) {
          rel(*gi._current_space, gi.arg2boolvar(call->args()[2]) >> !gi.arg2boolvar(call->args()[0]));
        } else if (d01.min() == 1) {
          rel(*gi._current_space, gi.arg2boolvar(call->args()[2]) >> gi.arg2boolvar(call->args()[0]));
        }
      } else {
        dom(*gi._current_space, gi.arg2intvar(call->args()[0]), d, Reify(gi.arg2boolvar(call->args()[2]),RM_IMP));
      }
    }

    ///* constraints from the standard library */

    void p_abs(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVar x0 = gi.arg2intvar(call->args()[0]);
      IntVar x1 = gi.arg2intvar(call->args()[1]);
      abs(*gi._current_space, x0, x1, gi.ann2icl(ann));
    }

    void p_array_int_lt(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs iv0 = gi.arg2intvarargs(call->args()[0]);
      IntVarArgs iv1 = gi.arg2intvarargs(call->args()[1]);
      rel(*gi._current_space, iv0, IRT_LE, iv1, gi.ann2icl(ann));
    }

    void p_array_int_lq(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs iv0 = gi.arg2intvarargs(call->args()[0]);
      IntVarArgs iv1 = gi.arg2intvarargs(call->args()[1]);
      rel(*gi._current_space, iv0, IRT_LQ, iv1, gi.ann2icl(ann));
    }

    void p_array_bool_lt(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVarArgs bv0 = gi.arg2boolvarargs(call->args()[0]);
      BoolVarArgs bv1 = gi.arg2boolvarargs(call->args()[1]);
      rel(*gi._current_space, bv0, IRT_LE, bv1, gi.ann2icl(ann));
    }

    void p_array_bool_lq(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVarArgs bv0 = gi.arg2boolvarargs(call->args()[0]);
      BoolVarArgs bv1 = gi.arg2boolvarargs(call->args()[1]);
      rel(*gi._current_space, bv0, IRT_LQ, bv1, gi.ann2icl(ann));
    }

    void p_count(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs iv = gi.arg2intvarargs(call->args()[0]);
      if (!call->args()[1]->type().isvarint()) {
        if (!call->args()[2]->type().isvarint()) {
          count(*gi._current_space, iv, call->args()[1]->cast<IntLit>()->v().toInt(), IRT_EQ, call->args()[2]->cast<IntLit>()->v().toInt(), 
              gi.ann2icl(ann));
        } else {
          count(*gi._current_space, iv, call->args()[1]->cast<IntLit>()->v().toInt(), IRT_EQ, gi.arg2intvar(call->args()[2]), 
              gi.ann2icl(ann));
        }
      } else if (!call->args()[2]->type().isvarint()) {
        count(*gi._current_space, iv, gi.arg2intvar(call->args()[1]), IRT_EQ, call->args()[2]->cast<IntLit>()->v().toInt(), 
            gi.ann2icl(ann));
      } else {
        count(*gi._current_space, iv, gi.arg2intvar(call->args()[1]), IRT_EQ, gi.arg2intvar(call->args()[2]), 
            gi.ann2icl(ann));
      }
    }

    void p_count_reif(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs iv = gi.arg2intvarargs(call->args()[0]);
      IntVar x = gi.arg2intvar(call->args()[1]);
      IntVar y = gi.arg2intvar(call->args()[2]);
      BoolVar b = gi.arg2boolvar(call->args()[3]);
      IntVar c(*gi._current_space,0,Int::Limits::max);
      count(*gi._current_space,iv,x,IRT_EQ,c,gi.ann2icl(ann));
      rel(*gi._current_space, b == (c==y));
    }
    void p_count_imp(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs iv = gi.arg2intvarargs(call->args()[0]);
      IntVar x = gi.arg2intvar(call->args()[1]);
      IntVar y = gi.arg2intvar(call->args()[2]);
      BoolVar b = gi.arg2boolvar(call->args()[3]);
      IntVar c(*gi._current_space,0,Int::Limits::max);
      count(*gi._current_space,iv,x,IRT_EQ,c,gi.ann2icl(ann));
      rel(*gi._current_space, b >> (c==y));
    }

    void count_rel(IntRelType irt, SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs iv = gi.arg2intvarargs(call->args()[1]);
      count(*gi._current_space, iv, call->args()[2]->cast<IntLit>()->v().toInt(), irt,
          call->args()[0]->cast<IntLit>()->v().toInt(), gi.ann2icl(ann));
    }

    void p_at_most(SolverInstanceBase& s, const Call* call) {
      count_rel(IRT_LQ, s, call);
    }

    void p_at_least(SolverInstanceBase& s, const Call* call) {
      count_rel(IRT_GQ, s, call);
    }

    void p_bin_packing_load(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      int minIdx = call->args()[3]->cast<IntLit>()->v().toInt();
      IntVarArgs load = gi.arg2intvarargs(call->args()[0]);
      IntVarArgs l;
      IntVarArgs bin = gi.arg2intvarargs(call->args()[1]);
      for (int i=bin.size(); i--;)
        rel(*gi._current_space, bin[i] >= minIdx);
      if (minIdx > 0) {
        for (int i=minIdx; i--;)
          l << IntVar(*gi._current_space,0,0);
      } else if (minIdx < 0) {
        IntVarArgs bin2(bin.size());
        for (int i=bin.size(); i--;)
          bin2[i] = expr(*gi._current_space, bin[i]-minIdx, gi.ann2icl(ann));
        bin = bin2;
      }
      l << load;
      IntArgs sizes = gi.arg2intargs(call->args()[2]);

      IntVarArgs allvars = l + bin;
      unshare(*gi._current_space, allvars);
      binpacking(*gi._current_space, allvars.slice(0,1,l.size()), allvars.slice(l.size(),1,bin.size()),
                 sizes, gi.ann2icl(ann));
    }

    void p_global_cardinality(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs iv0 = gi.arg2intvarargs(call->args()[0]);
      IntArgs cover = gi.arg2intargs(call->args()[1]);
      IntVarArgs iv1 = gi.arg2intvarargs(call->args()[2]);

      Region re(*gi._current_space);
      IntSet cover_s(cover);
      Gecode::IntSetRanges cover_r(cover_s);
      Gecode::IntVarRanges* iv0_ri = re.alloc<Gecode::IntVarRanges>(iv0.size());
      for (int i=iv0.size(); i--;)
        iv0_ri[i] = Gecode::IntVarRanges(iv0[i]);
      Iter::Ranges::NaryUnion iv0_r(re,iv0_ri,iv0.size());
      Iter::Ranges::Diff<Iter::Ranges::NaryUnion,Gecode::IntSetRanges> 
        extra_r(iv0_r,cover_r);
      Iter::Ranges::ToValues<Iter::Ranges::Diff<
        Iter::Ranges::NaryUnion,Gecode::IntSetRanges> > extra(extra_r);
      for (; extra(); ++extra) {
        cover << extra.val();
        iv1 << IntVar(*gi._current_space,0,iv0.size());
      }

      MZ_IntConLevel icl = gi.ann2icl(ann);
      if (icl==MZ_ICL_DOM) {
        IntVarArgs allvars = iv0+iv1;
        unshare(*gi._current_space, allvars);
        count(*gi._current_space, allvars.slice(0,1,iv0.size()), 
            allvars.slice(iv0.size(),1,iv1.size()),
            cover, gi.ann2icl(ann));
      } else {
        unshare(*gi._current_space, iv0);
        count(*gi._current_space, iv0, iv1, cover, gi.ann2icl(ann));
      }
    }

    void p_global_cardinality_closed(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs iv0 = gi.arg2intvarargs(call->args()[0]);
      IntArgs cover = gi.arg2intargs(call->args()[1]);
      IntVarArgs iv1 = gi.arg2intvarargs(call->args()[2]);
      unshare(*gi._current_space, iv0);
      count(*gi._current_space, iv0, iv1, cover, gi.ann2icl(ann));
    }

    void p_global_cardinality_low_up(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs x = gi.arg2intvarargs(call->args()[0]);
      IntArgs cover = gi.arg2intargs(call->args()[1]);

      IntArgs lbound = gi.arg2intargs(call->args()[2]);
      IntArgs ubound = gi.arg2intargs(call->args()[3]);
      IntSetArgs y(cover.size());
      for (int i=cover.size(); i--;)
        y[i] = IntSet(lbound[i],ubound[i]);

      IntSet cover_s(cover);
      Region re(*gi._current_space);
      IntVarRanges* xrs = re.alloc<IntVarRanges>(x.size());
      for (int i=x.size(); i--;)
        xrs[i].init(x[i]);
      Iter::Ranges::NaryUnion u(re, xrs, x.size());
      Iter::Ranges::ToValues<Iter::Ranges::NaryUnion> uv(u);
      for (; uv(); ++uv) {
        if (!cover_s.in(uv.val())) {
          cover << uv.val();
          y << IntSet(0,x.size());
        }
      }
  
      unshare(*gi._current_space, x);
      count(*gi._current_space, x, y, cover, gi.ann2icl(ann));
    }

    void p_global_cardinality_low_up_closed(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs x = gi.arg2intvarargs(call->args()[0]);
      IntArgs cover = gi.arg2intargs(call->args()[1]);

      IntArgs lbound = gi.arg2intargs(call->args()[2]);
      IntArgs ubound = gi.arg2intargs(call->args()[3]);
      IntSetArgs y(cover.size());
      for (int i=cover.size(); i--;)
        y[i] = IntSet(lbound[i],ubound[i]);

      unshare(*gi._current_space, x);
      count(*gi._current_space, x, y, cover, gi.ann2icl(ann));
    }

    void p_minimum(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs iv = gi.arg2intvarargs(call->args()[1]);
      min(*gi._current_space, iv, gi.arg2intvar(call->args()[0]), gi.ann2icl(ann));
    }

    void p_maximum(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs iv = gi.arg2intvarargs(call->args()[1]);
      max(*gi._current_space, iv, gi.arg2intvar(call->args()[0]), gi.ann2icl(ann));
    }

    void p_maximum_arg(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs iv = gi.arg2intvarargs(call->args()[0]);
      argmax(*gi._current_space, iv, gi.arg2intvar(call->args()[1]), true, gi.ann2icl(ann));
    }

    void p_minimum_arg(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs iv = gi.arg2intvarargs(call->args()[0]);
      argmin(*gi._current_space, iv, gi.arg2intvar(call->args()[1]), true, gi.ann2icl(ann));
    }

    void p_regular(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs iv = gi.arg2intvarargs(call->args()[0]);
      int q = call->args()[1]->cast<IntLit>()->v().toInt();
      int symbols = call->args()[2]->cast<IntLit>()->v().toInt();
      IntArgs d = gi.arg2intargs(call->args()[3]);
      int q0 = call->args()[4]->cast<IntLit>()->v().toInt();

      int noOfTrans = 0;
      for (int i=1; i<=q; i++) {
        for (int j=1; j<=symbols; j++) {
          if (d[(i-1)*symbols+(j-1)] > 0)
            noOfTrans++;
        }
      }

      Region re(*gi._current_space);
      DFA::Transition* t = re.alloc<DFA::Transition>(noOfTrans+1);
      noOfTrans = 0;
      for (int i=1; i<=q; i++) {
        for (int j=1; j<=symbols; j++) {
          if (d[(i-1)*symbols+(j-1)] > 0) {
            t[noOfTrans].i_state = i;
            t[noOfTrans].symbol  = j;
            t[noOfTrans].o_state = d[(i-1)*symbols+(j-1)];
            noOfTrans++;
          }
        }
      }
      t[noOfTrans].i_state = -1;

      //Final states
      IntSetVal* isv = eval_intset(s.env().envi(), call->args()[5]);
      IntSetRanges isr(isv);

      int *f = static_cast<int*>(malloc(sizeof(int)*(isv->card().toInt())+1));
      int i=0;
      for(Ranges::ToValues<IntSetRanges> val_iter(isr); val_iter(); ++val_iter, ++i) {
        f[i] = val_iter.val().toInt();
      }
      f[i] = -1;

      DFA dfa(q0,t,f);
      free(f);
      unshare(*gi._current_space, iv);
      extensional(*gi._current_space, iv, dfa, gi.ann2icl(ann));
    }

    void p_sort(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs x = gi.arg2intvarargs(call->args()[0]);
      IntVarArgs y = gi.arg2intvarargs(call->args()[1]);
      IntVarArgs xy(x.size()+y.size());
      for (int i=x.size(); i--;)
        xy[i] = x[i];
      for (int i=y.size(); i--;)
        xy[i+x.size()] = y[i];
      unshare(*gi._current_space, xy);
      for (int i=x.size(); i--;)
        x[i] = xy[i];
      for (int i=y.size(); i--;)
        y[i] = xy[i+x.size()];
      sorted(*gi._current_space, x, y, gi.ann2icl(ann));
    }

    void p_inverse_offsets(SolverInstanceBase& s, const Call* call) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs x = gi.arg2intvarargs(call->args()[0]);
      unshare(*gi._current_space, x);
      int xoff = call->args()[1]->cast<IntLit>()->v().toInt();
      IntVarArgs y = gi.arg2intvarargs(call->args()[2]);
      unshare(*gi._current_space, y);
      int yoff = call->args()[3]->cast<IntLit>()->v().toInt();
      MZ_IntConLevel icl = gi.ann2icl(call->ann());
      channel(*gi._current_space, x, xoff, y, yoff, icl == MZ_ICL_DEF ? MZ_ICL_DOM : icl);
    }

    void p_increasing_int(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs x = gi.arg2intvarargs(call->args()[0]);
      rel(*gi._current_space,x,IRT_LQ,gi.ann2icl(ann));
    }

    void p_increasing_bool(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVarArgs x = gi.arg2boolvarargs(call->args()[0]);
      rel(*gi._current_space,x,IRT_LQ,gi.ann2icl(ann));
    }

    void p_decreasing_int(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs x = gi.arg2intvarargs(call->args()[0]);
      rel(*gi._current_space,x,IRT_GQ,gi.ann2icl(ann));
    }

    void p_decreasing_bool(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVarArgs x = gi.arg2boolvarargs(call->args()[0]);
      rel(*gi._current_space,x,IRT_GQ,gi.ann2icl(ann));
    }

    void p_table_int(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs x = gi.arg2intvarargs(call->args()[0]);
      IntArgs tuples = gi.arg2intargs(call->args()[1]);
      int noOfVars   = x.size();
      int noOfTuples = tuples.size() == 0 ? 0 : (tuples.size()/noOfVars);
      TupleSet ts;
      for (int i=0; i<noOfTuples; i++) {
        IntArgs t(noOfVars);
        for (int j=0; j<x.size(); j++) {
          t[j] = tuples[i*noOfVars+j];
        }
        ts.add(t);
      }
      ts.finalize();
#ifdef HAS_GECODE_VERSION_5
      extensional(*gi._current_space,x,ts,gi.ann2icl(ann));
#else
      extensional(*gi._current_space,x,ts,MZ_EPK_DEF,gi.ann2icl(ann));
#endif
    }
    void p_table_bool(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVarArgs x = gi.arg2boolvarargs(call->args()[0]);
      IntArgs tuples = gi.arg2boolargs(call->args()[1]);
      int noOfVars   = x.size();
      int noOfTuples = tuples.size() == 0 ? 0 : (tuples.size()/noOfVars);
      TupleSet ts;
      for (int i=0; i<noOfTuples; i++) {
        IntArgs t(noOfVars);
        for (int j=0; j<x.size(); j++) {
          t[j] = tuples[i*noOfVars+j];
        }
        ts.add(t);
      }
      ts.finalize();
#ifdef HAS_GECODE_VERSION_5
      extensional(*gi._current_space,x,ts,gi.ann2icl(ann));
#else
      extensional(*gi._current_space,x,ts,MZ_EPK_DEF,gi.ann2icl(ann));
#endif
    }

    void p_cumulatives(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs start = gi.arg2intvarargs(call->args()[0]);
      IntVarArgs duration = gi.arg2intvarargs(call->args()[1]);
      IntVarArgs height = gi.arg2intvarargs(call->args()[2]);
      int n = start.size();
      IntVar bound = gi.arg2intvar(call->args()[3]);

      int minHeight = INT_MAX; int minHeight2 = INT_MAX;
      for (int i=n; i--;)
        if (height[i].min() < minHeight)
          minHeight = height[i].min();
        else if (height[i].min() < minHeight2)
          minHeight2 = height[i].min();
      bool disjunctive =
        (minHeight > bound.max()/2) ||
        (minHeight2 > bound.max()/2 && minHeight+minHeight2>bound.max());
      if (disjunctive) {
        rel(*gi._current_space, bound >= max(height));
        //Unary
        if (duration.assigned()) {
          IntArgs durationI(n);
          for (int i=n; i--;)
            durationI[i] = duration[i].val();
          unshare(*gi._current_space,start);
          unary(*gi._current_space,start,durationI);
        } else {
          IntVarArgs end(n);
          for (int i=n; i--;)
            end[i] = expr(*gi._current_space,start[i]+duration[i]);
          unshare(*gi._current_space,start);
          unary(*gi._current_space,start,duration,end);
        }
      } else if (height.assigned()) {
        IntArgs heightI(n);
        for (int i=n; i--;)
          heightI[i] = height[i].val();
        if (duration.assigned()) {
          IntArgs durationI(n);
          for (int i=n; i--;)
            durationI[i] = duration[i].val();
          cumulative(*gi._current_space, bound, start, durationI, heightI);
        } else {
          IntVarArgs end(n);
          for (int i = n; i--; )
            end[i] = expr(*gi._current_space,start[i]+duration[i]);
          cumulative(*gi._current_space, bound, start, duration, end, heightI);
        }
      } else if (bound.assigned()) {
        IntArgs machine = IntArgs::create(n,0,0);
        IntArgs limit(1, bound.val());
        IntVarArgs end(n);
        for (int i=n; i--;)
          end[i] = expr(*gi._current_space,start[i]+duration[i]);
        cumulatives(*gi._current_space, machine, start, duration, end, height, limit, true,
            gi.ann2icl(ann));
      } else {
        int min = Gecode::Int::Limits::max;
        int max = Gecode::Int::Limits::min;
        IntVarArgs end(start.size());
        for (int i = start.size(); i--; ) {
          min = std::min(min, start[i].min());
          max = std::max(max, start[i].max() + duration[i].max());
          end[i] = expr(*gi._current_space, start[i] + duration[i]);
        }
        for (int time = min; time < max; ++time) {
          IntVarArgs x(start.size());
          for (int i = start.size(); i--; ) {
            IntVar overlaps = channel(*gi._current_space, expr(*gi._current_space, (start[i] <= time) && 
                  (time < end[i])));
            x[i] = expr(*gi._current_space, overlaps * height[i]);
          }
          linear(*gi._current_space, x, IRT_LQ, bound);
        }
      }
    }

    void p_among_seq_int(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      Gecode::IntVarArgs x = gi.arg2intvarargs(call->args()[0]);
      IntSet S = gi.arg2intset(s.env().envi(), call->args()[1]);
      int q = call->args()[2]->cast<IntLit>()->v().toInt();
      int l = call->args()[3]->cast<IntLit>()->v().toInt();
      int u = call->args()[4]->cast<IntLit>()->v().toInt();
      unshare(*gi._current_space, x);
      sequence(*gi._current_space, x, S, q, l, u, gi.ann2icl(ann));
    }

    void p_among_seq_bool(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVarArgs x = gi.arg2boolvarargs(call->args()[0]);
      bool val = call->args()[1]->cast<BoolLit>()->v();
      int q = call->args()[2]->cast<IntLit>()->v().toInt();
      int l = call->args()[3]->cast<IntLit>()->v().toInt();
      int u = call->args()[4]->cast<IntLit>()->v().toInt();
      IntSet S(val, val);
      unshare(*gi._current_space, x);
      sequence(*gi._current_space, x, S, q, l, u, gi.ann2icl(ann));
    }

    void p_schedule_unary(SolverInstanceBase& s, const Call* call) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs x = gi.arg2intvarargs(call->args()[0]);
      IntArgs p = gi.arg2intargs(call->args()[1]);
      unshare(*gi._current_space, x);
      unary(*gi._current_space, x, p);
    }

    void p_schedule_unary_optional(SolverInstanceBase& s, const Call* call) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs x = gi.arg2intvarargs(call->args()[0]);
      IntArgs p = gi.arg2intargs(call->args()[1]);
      BoolVarArgs m = gi.arg2boolvarargs(call->args()[2]);
      unshare(*gi._current_space, x);
      unary(*gi._current_space, x, p, m);
    }


    void p_cumulative_opt(SolverInstanceBase& s, const Call* ce) {
      const Annotation& ann = ce->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs start = gi.arg2intvarargs(ce->args()[0]);
      IntArgs duration = gi.arg2intargs(ce->args()[1]);
      IntArgs height = gi.arg2intargs(ce->args()[2]);
      BoolVarArgs opt = gi.arg2boolvarargs(ce->args()[3]);
      int bound = ce->args()[4]->cast<IntLit>()->v().toInt();
      unshare(*gi._current_space, start);
      cumulative(*gi._current_space,bound,start,duration,height,opt,gi.ann2icl(ann));
    }

    void p_circuit(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      int off = call->args()[0]->cast<IntLit>()->v().toInt();
      IntVarArgs xv = gi.arg2intvarargs(call->args()[1]);
      unshare(*gi._current_space, xv);
      circuit(*gi._current_space,off,xv,gi.ann2icl(ann));
    }
    void p_circuit_cost_array(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntArgs c = gi.arg2intargs(call->args()[0]);
      IntVarArgs xv = gi.arg2intvarargs(call->args()[1]);
      IntVarArgs yv = gi.arg2intvarargs(call->args()[2]);
      IntVar z = gi.arg2intvar(call->args()[3]);
      unshare(*gi._current_space, xv);
      circuit(*gi._current_space,c,xv,yv,z,gi.ann2icl(ann));
    }
    void p_circuit_cost(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntArgs c = gi.arg2intargs(call->args()[0]);
      IntVarArgs xv = gi.arg2intvarargs(call->args()[1]);
      IntVar z = gi.arg2intvar(call->args()[2]);
      unshare(*gi._current_space, xv);
      circuit(*gi._current_space,c,xv,z,gi.ann2icl(ann));
    }

    void p_nooverlap(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs x0 = gi.arg2intvarargs(call->args()[0]);
      IntVarArgs w  = gi.arg2intvarargs(call->args()[1]);
      IntVarArgs y0 = gi.arg2intvarargs(call->args()[2]);
      IntVarArgs h  = gi.arg2intvarargs(call->args()[3]);
      if (w.assigned() && h.assigned()) {
        IntArgs iw(w.size());
        for (int i=w.size(); i--;)
          iw[i] = w[i].val();
        IntArgs ih(h.size());
        for (int i=h.size(); i--;)
          ih[i] = h[i].val();
        nooverlap(*gi._current_space,x0,iw,y0,ih,gi.ann2icl(ann));
      } else {
        IntVarArgs x1(x0.size()), y1(y0.size());
        for (int i=x0.size(); i--; )
          x1[i] = expr(*gi._current_space, x0[i] + w[i]);
        for (int i=y0.size(); i--; )
          y1[i] = expr(*gi._current_space, y0[i] + h[i]);
        nooverlap(*gi._current_space,x0,w,x1,y0,h,y1,gi.ann2icl(ann));
      }
    }

    void p_precede(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs x = gi.arg2intvarargs(call->args()[0]);
      int p_s = call->args()[1]->cast<IntLit>()->v().toInt();
      int p_t = call->args()[2]->cast<IntLit>()->v().toInt();
      precede(*gi._current_space,x,p_s,p_t,gi.ann2icl(ann));
    }

    void p_nvalue(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs x = gi.arg2intvarargs(call->args()[1]);
      if (call->args()[0]->type().isvarint()) {
        IntVar y = gi.arg2intvar(call->args()[0]);
        nvalues(*gi._current_space,x,IRT_EQ,y,gi.ann2icl(ann));
      } else {
        nvalues(*gi._current_space,x,IRT_EQ,call->args()[0]->cast<IntLit>()->v().toInt(),gi.ann2icl(ann));
      }
    }

    void p_among(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs x = gi.arg2intvarargs(call->args()[1]);
      IntSet v = gi.arg2intset(s.env().envi(), call->args()[2]);
      if (call->args()[0]->type().isvarint()) {
        IntVar n = gi.arg2intvar(call->args()[0]);
        unshare(*gi._current_space, x);
        count(*gi._current_space,x,v,IRT_EQ,n,gi.ann2icl(ann));
      } else {
        unshare(*gi._current_space, x);
        count(*gi._current_space,x,v,IRT_EQ,call->args()[0]->cast<IntLit>()->v().toInt(),gi.ann2icl(ann));
      }
    }

    void p_member_int(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs x = gi.arg2intvarargs(call->args()[0]);
      IntVar y = gi.arg2intvar(call->args()[1]);
      member(*gi._current_space,x,y,gi.ann2icl(ann));
    }
    void p_member_int_reif(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVarArgs x = gi.arg2intvarargs(call->args()[0]);
      IntVar y = gi.arg2intvar(call->args()[1]);
      BoolVar b = gi.arg2boolvar(call->args()[2]);
      member(*gi._current_space,x,y,b,gi.ann2icl(ann));
    }
    void p_member_bool(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVarArgs x = gi.arg2boolvarargs(call->args()[0]);
      BoolVar y = gi.arg2boolvar(call->args()[1]);
      member(*gi._current_space,x,y,gi.ann2icl(ann));
    }
    void p_member_bool_reif(SolverInstanceBase& s, const Call* call) {
      const Annotation& ann =call->ann();
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      BoolVarArgs x = gi.arg2boolvarargs(call->args()[0]);
      BoolVar y = gi.arg2boolvar(call->args()[1]);
      member(*gi._current_space,x,y,gi.arg2boolvar(call->args()[2]),gi.ann2icl(ann));
    }


    // FLOAT CONSTRAINTS
#ifdef GECODE_HAS_FLOAT_VARS

    void p_int2float(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      IntVar x0 = gi.arg2intvar(ce->args()[0]);
      FloatVar x1 = gi.arg2floatvar(ce->args()[1]);
      channel(*gi._current_space, x0, x1);
    }

    void p_float_lin_cmp(GecodeSolverInstance& s, FloatRelType frt, const Call* ce) {
      FloatValArgs fa = s.arg2floatargs(ce->args()[0]);
      FloatVarArgs fv = s.arg2floatvarargs(ce->args()[1]);
      linear(*s._current_space, fa, fv, frt, ce->args()[2]->cast<FloatLit>()->v().toDouble());
    }
    void p_float_lin_cmp_reif(GecodeSolverInstance& s, FloatRelType frt, const Call* ce) {
      FloatValArgs fa = s.arg2floatargs(ce->args()[0]);
      FloatVarArgs fv = s.arg2floatvarargs(ce->args()[1]);
      linear(*s._current_space, fa, fv, frt, ce->args()[2]->cast<FloatLit>()->v().toDouble(), s.arg2boolvar(ce->args()[3]));
    }
    void p_float_lin_eq(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      p_float_lin_cmp(gi,FRT_EQ,ce);
    }
    void p_float_lin_eq_reif(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      p_float_lin_cmp_reif(gi,FRT_EQ,ce);
    }
    void p_float_lin_le(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      p_float_lin_cmp(gi,FRT_LQ,ce);
    }
    void p_float_lin_le_reif(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      p_float_lin_cmp_reif(gi,FRT_LQ,ce);
    }

    void p_float_times(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      FloatVar x = gi.arg2floatvar(ce->args()[0]);
      FloatVar y = gi.arg2floatvar(ce->args()[1]);
      FloatVar z = gi.arg2floatvar(ce->args()[2]);
      mult(*gi._current_space,x,y,z);
    }

    void p_float_div(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      FloatVar x = gi.arg2floatvar(ce->args()[0]);
      FloatVar y = gi.arg2floatvar(ce->args()[1]);
      FloatVar z = gi.arg2floatvar(ce->args()[2]);
      div(*gi._current_space,x,y,z);
    }

    void p_float_plus(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      FloatVar x = gi.arg2floatvar(ce->args()[0]);
      FloatVar y = gi.arg2floatvar(ce->args()[1]);
      FloatVar z = gi.arg2floatvar(ce->args()[2]);
      rel(*gi._current_space,x+y==z);
    }

    void p_float_sqrt(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      FloatVar x = gi.arg2floatvar(ce->args()[0]);
      FloatVar y = gi.arg2floatvar(ce->args()[1]);
      sqrt(*gi._current_space,x,y);
    }

    void p_float_abs(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      FloatVar x = gi.arg2floatvar(ce->args()[0]);
      FloatVar y = gi.arg2floatvar(ce->args()[1]);
      abs(*gi._current_space,x,y);
    }

    void p_float_eq(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      FloatVar x = gi.arg2floatvar(ce->args()[0]);
      FloatVar y = gi.arg2floatvar(ce->args()[1]);
      rel(*gi._current_space,x,FRT_EQ,y);
    }
    void p_float_eq_reif(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      FloatVar x = gi.arg2floatvar(ce->args()[0]);
      FloatVar y = gi.arg2floatvar(ce->args()[1]);
      BoolVar  b = gi.arg2boolvar(ce->args()[2]);
      rel(*gi._current_space,x,FRT_EQ,y,b);
    }
    void p_float_le(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      FloatVar x = gi.arg2floatvar(ce->args()[0]);
      FloatVar y = gi.arg2floatvar(ce->args()[1]);
      rel(*gi._current_space,x,FRT_LQ,y);
    }
    void p_float_le_reif(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      FloatVar x = gi.arg2floatvar(ce->args()[0]);
      FloatVar y = gi.arg2floatvar(ce->args()[1]);
      BoolVar  b = gi.arg2boolvar(ce->args()[2]);
      rel(*gi._current_space,x,FRT_LQ,y,b);
    }
    void p_float_max(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      FloatVar x = gi.arg2floatvar(ce->args()[0]);
      FloatVar y = gi.arg2floatvar(ce->args()[1]);
      FloatVar z = gi.arg2floatvar(ce->args()[2]);
      max(*gi._current_space,x,y,z);
    }
    void p_float_min(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      FloatVar x = gi.arg2floatvar(ce->args()[0]);
      FloatVar y = gi.arg2floatvar(ce->args()[1]);
      FloatVar z = gi.arg2floatvar(ce->args()[2]);
      min(*gi._current_space,x,y,z);
    }
    void p_float_lt(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      FloatVar x = gi.arg2floatvar(ce->args()[0]);
      FloatVar y = gi.arg2floatvar(ce->args()[1]);
      rel(*gi._current_space, x, FRT_LQ, y);
      rel(*gi._current_space, x, FRT_EQ, y, BoolVar(*gi._current_space,0,0));
    }

    void p_float_lt_reif(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      FloatVar x = gi.arg2floatvar(ce->args()[0]);
      FloatVar y = gi.arg2floatvar(ce->args()[1]);
      BoolVar b = gi.arg2boolvar(ce->args()[2]);
      BoolVar b0(*gi._current_space,0,1);
      BoolVar b1(*gi._current_space,0,1);
      rel(*gi._current_space, b == (b0 && !b1));
      rel(*gi._current_space, x, FRT_LQ, y, b0);
      rel(*gi._current_space, x, FRT_EQ, y, b1);
    }

    void p_float_ne(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      FloatVar x = gi.arg2floatvar(ce->args()[0]);
      FloatVar y = gi.arg2floatvar(ce->args()[1]);
      rel(*gi._current_space, x, FRT_EQ, y, BoolVar(*gi._current_space,0,0));
    }

#ifdef GECODE_HAS_MPFR
#define P_FLOAT_OP(Op) \
    void p_float_ ## Op (SolverInstanceBase& s, const Call* ce) {\
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s); \
      FloatVar x = gi.arg2floatvar(ce->args()[0]);\
      FloatVar y = gi.arg2floatvar(ce->args()[1]);\
      Op(gi ,x,y);\
    }
    P_FLOAT_OP(acos)
      P_FLOAT_OP(asin)
      P_FLOAT_OP(atan)
      P_FLOAT_OP(cos)
      P_FLOAT_OP(exp)
      P_FLOAT_OP(sin)
      P_FLOAT_OP(tan)
      // P_FLOAT_OP(sinh)
      // P_FLOAT_OP(tanh)
      // P_FLOAT_OP(cosh)
#undef P_FLOAT_OP

      void p_float_ln(SolverInstanceBase& s, const Call* ce) {
        GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
        FloatVar x = gi.arg2floatvar(ce->args()[0]);
        FloatVar y = gi.arg2floatvar(ce->args()[1]);
        log(*gi._current_space,x,y);
      }
    void p_float_log10(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      FloatVar x = gi.arg2floatvar(ce->args()[0]);
      FloatVar y = gi.arg2floatvar(ce->args()[1]);
      log(*gi._current_space,10.0,x,y);
    }
    void p_float_log2(SolverInstanceBase& s, const Call* ce) {
      GecodeSolverInstance& gi = static_cast<GecodeSolverInstance&>(s);
      FloatVar x = gi.arg2floatvar(ce->args()[0]);
      FloatVar y = gi.arg2floatvar(ce->args()[1]);
      log(*gi._current_space,2.0,x,y);
    }

#endif
#endif

    }

  }
