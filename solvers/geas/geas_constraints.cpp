/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solvers/geas/geas_constraints.hh>
#include <minizinc/solvers/geas_solverinstance.hh>

#include <geas/constraints/builtins.h>

namespace MiniZinc {
  namespace GeasConstraints {

    void p_int_abs(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* org = call->arg(0);
      Expression* res = call->arg(1);
      geas::int_abs(gi.solver_data(), gi.arg2intvar(res), gi.arg2intvar(org));
    }

    void p_int_times(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      Expression* res = call->arg(2);
      geas::int_mul(gi.solver_data(), gi.arg2intvar(lhs), gi.arg2intvar(rhs), gi.arg2intvar(res));
    }

    void p_int_div(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      Expression* res = call->arg(2);
      geas::int_div(gi.solver_data(), gi.arg2intvar(res), gi.arg2intvar(lhs), gi.arg2intvar(rhs));
    }

    void p_int_max(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      vec<geas::intvar> vars = {gi.arg2intvar(lhs), gi.arg2intvar(rhs)};
      Expression* res = call->arg(2);
      geas::int_max(gi.solver_data(), gi.arg2intvar(res), vars);
    }

    void p_int_min(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      vec<geas::intvar> vars = {-gi.arg2intvar(lhs), -gi.arg2intvar(rhs)};
      Expression* res = call->arg(2);
      geas::int_max(gi.solver_data(), -gi.arg2intvar(res), vars);
    }

    void p_int_lin_eq(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      ArrayLit* as = eval_array_lit(gi.env().envi(), call->arg(0));
      ArrayLit* bs = eval_array_lit(gi.env().envi(), call->arg(1));
      IntVal c = eval_int(gi.env().envi(), call->arg(2));
      vec<int> pos(as->size());
      vec<int> neg(as->size());
      for (int i = 0; i < as->size(); ++i) {
        IntVal iv = eval_int(gi.env().envi(), (*as)[i]);
        pos[i] = static_cast<int>(iv.toInt());
        neg[i] = -static_cast<int>(iv.toInt());
      }
      vec<geas::intvar> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2intvar((*bs)[i]);
      }
      // TODO: Rewrite using MiniZinc Library??
      geas::linear_le(gi.solver_data(), pos, vars, static_cast<int>(c.toInt()));
      geas::linear_le(gi.solver_data(), neg, vars, -static_cast<int>(c.toInt()));
    }

    void p_int_lin_ne(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      ArrayLit* as = eval_array_lit(gi.env().envi(), call->arg(0));
      ArrayLit* bs = eval_array_lit(gi.env().envi(), call->arg(1));
      IntVal c = eval_int(gi.env().envi(), call->arg(2));
      vec<int> cons(as->size());
      for (int i = 0; i < as->size(); ++i) {
        IntVal iv = eval_int(gi.env().envi(), (*as)[i]);
        cons[i] = static_cast<int>(iv.toInt());
      }
      vec<geas::intvar> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2intvar((*bs)[i]);
      }
      geas::linear_ne(gi.solver_data(), cons, vars, static_cast<int>(c.toInt()));
    }

    void p_int_lin_le(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      ArrayLit* as = eval_array_lit(gi.env().envi(), call->arg(0));
      ArrayLit* bs = eval_array_lit(gi.env().envi(), call->arg(1));
      IntVal c = eval_int(gi.env().envi(), call->arg(2));
      vec<int> cons(as->size());
      for (int i = 0; i < as->size(); ++i) {
        IntVal iv = eval_int(gi.env().envi(), (*as)[i]);
        cons[i] = static_cast<int>(iv.toInt());
      }

      vec<geas::intvar> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2intvar((*bs)[i]);
      }
      geas::linear_le(gi.solver_data(), cons, vars, static_cast<int>(c.toInt()));
    }

    void p_int_lin_eq_imp(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      ArrayLit* as = eval_array_lit(gi.env().envi(), call->arg(0));
      ArrayLit* bs = eval_array_lit(gi.env().envi(), call->arg(1));
      IntVal c = eval_int(gi.env().envi(), call->arg(2));
      Expression* r = call->arg(3);
      vec<int> pos(as->size());
      vec<int> neg(as->size());
      for (int i = 0; i < as->size(); ++i) {
        IntVal iv = eval_int(gi.env().envi(), (*as)[i]);
        pos[i] = static_cast<int>(iv.toInt());
        neg[i] = -static_cast<int>(iv.toInt());
      }
      vec<geas::intvar> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2intvar((*bs)[i]);
      }
      // TODO: Rewrite using MiniZinc Library??
      geas::linear_le(gi.solver_data(), pos, vars, static_cast<int>(c.toInt()), gi.arg2boolvar(r));
      geas::linear_le(gi.solver_data(), neg, vars, -static_cast<int>(c.toInt()), gi.arg2boolvar(r));
    }

    void p_int_lin_ne_imp(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      ArrayLit* as = eval_array_lit(gi.env().envi(), call->arg(0));
      ArrayLit* bs = eval_array_lit(gi.env().envi(), call->arg(1));
      IntVal c = eval_int(gi.env().envi(), call->arg(2));
      Expression* r = call->arg(3);
      vec<int> cons(as->size());
      for (int i = 0; i < as->size(); ++i) {
        IntVal iv = eval_int(gi.env().envi(), (*as)[i]);
        cons[i] = static_cast<int>(iv.toInt());
      }
      vec<geas::intvar> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2intvar((*bs)[i]);
      }
      geas::linear_ne(gi.solver_data(), cons, vars, static_cast<int>(c.toInt()), gi.arg2boolvar(r));
    }

    void p_int_lin_le_imp(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      ArrayLit* as = eval_array_lit(gi.env().envi(), call->arg(0));
      ArrayLit* bs = eval_array_lit(gi.env().envi(), call->arg(1));
      IntVal c = eval_int(gi.env().envi(), call->arg(2));
      Expression* r = call->arg(3);
      vec<int> cons(as->size());
      for (int i = 0; i < as->size(); ++i) {
        IntVal iv = eval_int(gi.env().envi(), (*as)[i]);
        cons[i] = static_cast<int>(iv.toInt());
      }
      vec<geas::intvar> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2intvar((*bs)[i]);
      }
      geas::linear_le(gi.solver_data(), cons, vars, static_cast<int>(c.toInt()), gi.arg2boolvar(r));
    }

    void p_int_lin_eq_reif(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      ArrayLit* as = eval_array_lit(gi.env().envi(), call->arg(0));
      ArrayLit* bs = eval_array_lit(gi.env().envi(), call->arg(1));
      IntVal c = eval_int(gi.env().envi(), call->arg(2));
      Expression* r = call->arg(3);
      vec<int> pos(as->size());
      vec<int> neg(as->size());
      for (int i = 0; i < as->size(); ++i) {
        IntVal iv = eval_int(gi.env().envi(), (*as)[i]);
        pos[i] = static_cast<int>(iv.toInt());
        neg[i] = -static_cast<int>(iv.toInt());
      }
      vec<geas::intvar> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2intvar((*bs)[i]);
      }
      // TODO: Rewrite using MiniZinc Library??
      geas::linear_le(gi.solver_data(), pos, vars, static_cast<int>(c.toInt()), gi.arg2boolvar(r));
      geas::linear_le(gi.solver_data(), neg, vars, -static_cast<int>(c.toInt()), gi.arg2boolvar(r));
      geas::linear_ne(gi.solver_data(), pos, vars, static_cast<int>(c.toInt()), ~gi.arg2boolvar(r));
    }

    void p_int_lin_ne_reif(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      ArrayLit* as = eval_array_lit(gi.env().envi(), call->arg(0));
      ArrayLit* bs = eval_array_lit(gi.env().envi(), call->arg(1));
      IntVal c = eval_int(gi.env().envi(), call->arg(2));
      Expression* r = call->arg(3);
      vec<int> pos(as->size());
      vec<int> neg(as->size());
      for (int i = 0; i < as->size(); ++i) {
        IntVal iv = eval_int(gi.env().envi(), (*as)[i]);
        pos[i] = static_cast<int>(iv.toInt());
        neg[i] = -static_cast<int>(iv.toInt());
      }
      vec<geas::intvar> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2intvar((*bs)[i]);
      }
      geas::linear_ne(gi.solver_data(), pos, vars, static_cast<int>(c.toInt()), gi.arg2boolvar(r));
      geas::linear_le(gi.solver_data(), pos, vars, static_cast<int>(c.toInt()), ~gi.arg2boolvar(r));
      geas::linear_le(gi.solver_data(), neg, vars, -static_cast<int>(c.toInt()), ~gi.arg2boolvar(r));
    }

    void p_int_lin_le_reif(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      ArrayLit* as = eval_array_lit(gi.env().envi(), call->arg(0));
      ArrayLit* bs = eval_array_lit(gi.env().envi(), call->arg(1));
      IntVal c = eval_int(gi.env().envi(), call->arg(2));
      Expression* r = call->arg(3);
      vec<int> pos(as->size());
      vec<int> neg(as->size());
      for (int i = 0; i < as->size(); ++i) {
        IntVal iv = eval_int(gi.env().envi(), (*as)[i]);
        pos[i] = static_cast<int>(iv.toInt());
        neg[i] = -static_cast<int>(iv.toInt());
      }
      vec<geas::intvar> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2intvar((*bs)[i]);
      }
      geas::linear_le(gi.solver_data(), pos, vars, static_cast<int>(c.toInt()), gi.arg2boolvar(r));
      geas::linear_le(gi.solver_data(), neg, vars, -static_cast<int>(c.toInt()), ~gi.arg2boolvar(r));
    }

  }
}
