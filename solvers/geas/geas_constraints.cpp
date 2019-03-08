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

    void p_int_eq(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      geas::int_eq(gi.solver_data(), gi.arg2intvar(lhs), gi.arg2intvar(rhs));
    }

    void p_int_ne(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      geas::int_ne(gi.solver_data(), gi.arg2intvar(lhs), gi.arg2intvar(rhs));
    }

    void p_int_le(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      geas::int_le(gi.solver_data(), gi.arg2intvar(lhs), gi.arg2intvar(rhs), 0);
    }

    void p_int_lt(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      geas::int_le(gi.solver_data(), gi.arg2intvar(lhs), gi.arg2intvar(rhs), -1);
    }

    void p_int_eq_imp(SolverInstanceBase& s, const Call* call) {
      if (!call->arg(2)->type().isvar()) {
        if (call->arg(2)->cast<BoolLit>()->v()) {
          p_int_eq(s, call);
        }
      } else {
        auto& gi = static_cast<GeasSolverInstance&>(s);
        Expression* lhs = call->arg(0);
        Expression* rhs = call->arg(1);
        Expression* r = call->arg(2);
        geas::int_eq(gi.solver_data(), gi.arg2intvar(lhs), gi.arg2intvar(rhs), gi.arg2boolvar(r));
      }
    }

    void p_int_ne_imp(SolverInstanceBase& s, const Call* call) {
      if (!call->arg(2)->type().isvar()) {
        if (call->arg(2)->cast<BoolLit>()->v()) {
          p_int_ne(s, call);
        }
      } else {
        auto& gi = static_cast<GeasSolverInstance&>(s);
        Expression* lhs = call->arg(0);
        Expression* rhs = call->arg(1);
        Expression* r = call->arg(2);
        geas::int_ne(gi.solver_data(), gi.arg2intvar(lhs), gi.arg2intvar(rhs), gi.arg2boolvar(r));
      }
    }

    void p_int_le_imp(SolverInstanceBase& s, const Call* call) {
      if (!call->arg(2)->type().isvar()) {
        if (call->arg(2)->cast<BoolLit>()->v()) {
          p_int_le(s, call);
        }
      } else {
        auto& gi = static_cast<GeasSolverInstance&>(s);
        Expression* lhs = call->arg(0);
        Expression* rhs = call->arg(1);
        Expression* r = call->arg(2);
        geas::int_le(gi.solver_data(), gi.arg2intvar(lhs),  gi.arg2intvar(rhs), 0, gi.arg2boolvar(r));
      }
    }

    void p_int_lt_imp(SolverInstanceBase& s, const Call* call) {
      if (!call->arg(2)->type().isvar()) {
        if (call->arg(2)->cast<BoolLit>()->v()) {
          p_int_lt(s, call);
        }
      } else {
        auto& gi = static_cast<GeasSolverInstance&>(s);
        Expression* lhs = call->arg(0);
        Expression* rhs = call->arg(1);
        Expression* r = call->arg(2);
        geas::int_le(gi.solver_data(), gi.arg2intvar(lhs),  gi.arg2intvar(rhs), -1, gi.arg2boolvar(r));
      }
    }

    void p_int_eq_reif(SolverInstanceBase& s, const Call* call) {
      if (!call->arg(2)->type().isvar()) {
        if (call->arg(2)->cast<BoolLit>()->v()) {
          p_int_eq(s, call);
        } else {
          p_int_ne(s, call);
        }
      } else {
        auto& gi = static_cast<GeasSolverInstance&>(s);
        Expression* lhs = call->arg(0);
        Expression* rhs = call->arg(1);
        Expression* r = call->arg(2);
        geas::int_eq(gi.solver_data(), gi.arg2intvar(lhs), gi.arg2intvar(rhs), gi.arg2boolvar(r));
        geas::int_ne(gi.solver_data(), gi.arg2intvar(lhs), gi.arg2intvar(rhs), ~gi.arg2boolvar(r));
      }
    }

    void p_int_ne_reif(SolverInstanceBase& s, const Call* call) {
      if (!call->arg(2)->type().isvar()) {
        if (call->arg(2)->cast<BoolLit>()->v()) {
          p_int_ne(s, call);
        } else {
          p_int_eq(s, call);
        }
      } else {
        auto& gi = static_cast<GeasSolverInstance&>(s);
        Expression* lhs = call->arg(0);
        Expression* rhs = call->arg(1);
        Expression* r = call->arg(2);
        geas::int_ne(gi.solver_data(), gi.arg2intvar(lhs), gi.arg2intvar(rhs), gi.arg2boolvar(r));
        geas::int_eq(gi.solver_data(), gi.arg2intvar(lhs), gi.arg2intvar(rhs), ~gi.arg2boolvar(r));
      }
    }

    void p_int_le_reif(SolverInstanceBase& s, const Call* call) {
      if (!call->arg(2)->type().isvar()) {
        if (call->arg(2)->cast<BoolLit>()->v()) {
          p_int_le(s, call);
        } else {
          auto nc = new Call(Location().introduce(), call->id(), {call->arg(1), call->arg(0)});
          p_int_lt(s, nc);
        }
      } else {
        auto& gi = static_cast<GeasSolverInstance&>(s);
        Expression* lhs = call->arg(0);
        Expression* rhs = call->arg(1);
        Expression* r = call->arg(2);
        geas::int_le(gi.solver_data(), gi.arg2intvar(lhs),  gi.arg2intvar(rhs), 0, gi.arg2boolvar(r));
        geas::int_le(gi.solver_data(), gi.arg2intvar(rhs),  gi.arg2intvar(lhs), -1, ~gi.arg2boolvar(r));
      }
    }

    void p_int_lt_reif(SolverInstanceBase& s, const Call* call) {
      if (!call->arg(2)->type().isvar()) {
        if (call->arg(2)->cast<BoolLit>()->v()) {
          p_int_lt(s, call);
        } else {
          auto nc = new Call(Location().introduce(), call->id(), {call->arg(1), call->arg(0)});
          p_int_le(s, nc);
        }
      } else {
        auto& gi = static_cast<GeasSolverInstance&>(s);
        Expression* lhs = call->arg(0);
        Expression* rhs = call->arg(1);
        Expression* r = call->arg(2);
        geas::int_le(gi.solver_data(), gi.arg2intvar(lhs),  gi.arg2intvar(rhs), -1, gi.arg2boolvar(r));
        geas::int_le(gi.solver_data(), gi.arg2intvar(rhs),  gi.arg2intvar(lhs), 0, ~gi.arg2boolvar(r));
      }
    }

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
      geas::linear_le(gi.solver_data(), neg, vars, -static_cast<int>(c.toInt())-1, ~gi.arg2boolvar(r));
    }

    void p_bool_eq(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      if(!lhs->type().isvarbool()) {
        std::swap(lhs, rhs);
      }
      if (!rhs->type().isvarbool()) {
        bool b = eval_bool(gi.env().envi(), rhs);
        gi.solver().post(b ? gi.arg2boolvar(lhs) : ~gi.arg2boolvar(lhs));
      } else {
        geas::add_clause(gi.solver_data(), gi.arg2boolvar(lhs), ~gi.arg2boolvar(rhs));
        geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(lhs), gi.arg2boolvar(rhs));
      }
    }

    void p_bool_ne(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      if(!lhs->type().isvarbool()) {
        std::swap(lhs, rhs);
      }
      if (!rhs->type().isvarbool()) {
        bool b = eval_bool(gi.env().envi(), rhs);
        gi.solver().post(b ? ~gi.arg2boolvar(lhs) : gi.arg2boolvar(lhs));
      } else {
        geas::add_clause(gi.solver_data(), gi.arg2boolvar(lhs), gi.arg2boolvar(rhs));
        geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(lhs), ~gi.arg2boolvar(rhs));
      }
    }

    void p_bool_le(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      if(!lhs->type().isvarbool()) {
        if (eval_bool(gi.env().envi(), lhs)) {
          gi.solver().post(gi.arg2boolvar(rhs));
        }
      } else if (!rhs->type().isvarbool()) {
        if (!eval_bool(gi.env().envi(), rhs)) {
          gi.solver().post(~gi.arg2boolvar(lhs));
        }
      } else {
        geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(lhs), gi.arg2boolvar(rhs));
      }
    }

    void p_bool_lt(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      gi.solver().post(~gi.arg2boolvar(lhs));
      gi.solver().post(gi.arg2boolvar(rhs));
    }

    void p_bool_eq_imp(SolverInstanceBase& s, const Call* call) {
      if (!call->arg(2)->type().isvar()) {
        if (call->arg(2)->cast<BoolLit>()->v()) {
          p_bool_eq(s, call);
        }
      } else {
        auto& gi = static_cast<GeasSolverInstance&>(s);
        Expression* lhs = call->arg(0);
        Expression* rhs = call->arg(1);
        Expression* r = call->arg(2);
        geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), ~gi.arg2boolvar(lhs), gi.arg2boolvar(rhs));
        geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), gi.arg2boolvar(lhs), ~gi.arg2boolvar(rhs));
      }
    }

    void p_bool_ne_imp(SolverInstanceBase& s, const Call* call) {
      if (!call->arg(2)->type().isvar()) {
        if (call->arg(2)->cast<BoolLit>()->v()) {
          p_bool_ne(s, call);
        }
      } else {
        auto& gi = static_cast<GeasSolverInstance&>(s);
        Expression* lhs = call->arg(0);
        Expression* rhs = call->arg(1);
        Expression* r = call->arg(2);
        geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), gi.arg2boolvar(lhs), gi.arg2boolvar(rhs));
        geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), ~gi.arg2boolvar(lhs), ~gi.arg2boolvar(rhs));
      }
    }

    void p_bool_le_imp(SolverInstanceBase& s, const Call* call) {
      if (!call->arg(2)->type().isvar()) {
        if (call->arg(2)->cast<BoolLit>()->v()) {
          p_bool_le(s, call);
        }
      } else {
        auto& gi = static_cast<GeasSolverInstance&>(s);
        Expression* lhs = call->arg(0);
        Expression* rhs = call->arg(1);
        Expression* r = call->arg(2);
        geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), ~gi.arg2boolvar(lhs), gi.arg2boolvar(rhs));
      }
    }

    void p_bool_lt_imp(SolverInstanceBase& s, const Call* call) {
      if (!call->arg(2)->type().isvar()) {
        if (call->arg(2)->cast<BoolLit>()->v()) {
          p_bool_lt(s, call);
        }
      } else {
        auto& gi = static_cast<GeasSolverInstance&>(s);
        Expression* lhs = call->arg(0);
        Expression* rhs = call->arg(1);
        Expression* r = call->arg(2);
        geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), ~gi.arg2boolvar(lhs));
        geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), gi.arg2boolvar(rhs));
      }
    }

    void p_bool_eq_reif(SolverInstanceBase& s, const Call* call) {
      if (!call->arg(2)->type().isvar()) {
        if (call->arg(2)->cast<BoolLit>()->v()) {
          p_bool_eq(s, call);
        } else {
          p_bool_ne(s, call);
        }
      } else {
        auto& gi = static_cast<GeasSolverInstance&>(s);
        Expression* lhs = call->arg(0);
        Expression* rhs = call->arg(1);
        Expression* r = call->arg(2);
        geas::add_clause(gi.solver_data(), gi.arg2boolvar(r), gi.arg2boolvar(lhs), gi.arg2boolvar(rhs));
        geas::add_clause(gi.solver_data(), gi.arg2boolvar(r), ~gi.arg2boolvar(lhs), ~gi.arg2boolvar(rhs));
        geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), ~gi.arg2boolvar(lhs), gi.arg2boolvar(rhs));
        geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), gi.arg2boolvar(lhs), ~gi.arg2boolvar(rhs));
      }
    }

    void p_bool_ne_reif(SolverInstanceBase& s, const Call* call) {
      if (!call->arg(2)->type().isvar()) {
        if (call->arg(2)->cast<BoolLit>()->v()) {
          p_bool_ne(s, call);
        } else {
          p_bool_eq(s, call);
        }
      } else {
        auto& gi = static_cast<GeasSolverInstance&>(s);
        Expression* lhs = call->arg(0);
        Expression* rhs = call->arg(1);
        Expression* r = call->arg(2);
        geas::add_clause(gi.solver_data(), gi.arg2boolvar(r), ~gi.arg2boolvar(lhs), gi.arg2boolvar(rhs));
        geas::add_clause(gi.solver_data(), gi.arg2boolvar(r), gi.arg2boolvar(lhs), ~gi.arg2boolvar(rhs));
        geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), gi.arg2boolvar(lhs), gi.arg2boolvar(rhs));
        geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), ~gi.arg2boolvar(lhs), ~gi.arg2boolvar(rhs));
      }
    }

    void p_bool_le_reif(SolverInstanceBase& s, const Call* call) {
      if (!call->arg(2)->type().isvar()) {
        if (call->arg(2)->cast<BoolLit>()->v()) {
          p_bool_le(s, call);
        } else {
          auto nc = new Call(Location().introduce(), call->id(), {call->arg(1), call->arg(0)});
          p_bool_lt(s, nc);
        }
      } else {
        auto& gi = static_cast<GeasSolverInstance&>(s);
        Expression* lhs = call->arg(0);
        Expression* rhs = call->arg(1);
        Expression* r = call->arg(2);
        geas::add_clause(gi.solver_data(), gi.arg2boolvar(r), ~gi.arg2boolvar(rhs));
        geas::add_clause(gi.solver_data(), gi.arg2boolvar(r), gi.arg2boolvar(lhs));
        geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), ~gi.arg2boolvar(lhs), gi.arg2boolvar(rhs));
      }
    }

    void p_bool_lt_reif(SolverInstanceBase& s, const Call* call) {
      if (!call->arg(2)->type().isvar()) {
        if (call->arg(2)->cast<BoolLit>()->v()) {
          p_int_lt(s, call);
        } else {
          auto nc = new Call(Location().introduce(), call->id(), {call->arg(1), call->arg(0)});
          p_int_le(s, nc);
        }
      } else {
        auto& gi = static_cast<GeasSolverInstance&>(s);
        Expression* lhs = call->arg(0);
        Expression* rhs = call->arg(1);
        Expression* r = call->arg(2);
        geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), ~gi.arg2boolvar(lhs));
        geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), gi.arg2boolvar(rhs));
        geas::add_clause(gi.solver_data(), gi.arg2boolvar(r), gi.arg2boolvar(lhs), ~gi.arg2boolvar(rhs));
      }
    }

    void p_bool_or(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      Expression* r = call->arg(2);
      geas::add_clause(gi.solver_data(), gi.arg2boolvar(r), ~gi.arg2boolvar(lhs));
      geas::add_clause(gi.solver_data(), gi.arg2boolvar(r), ~gi.arg2boolvar(rhs));
      geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), gi.arg2boolvar(lhs), gi.arg2boolvar(rhs));
    }

    void p_bool_and(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      Expression* r = call->arg(2);
      geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), gi.arg2boolvar(lhs));
      geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), gi.arg2boolvar(rhs));
      geas::add_clause(gi.solver_data(), gi.arg2boolvar(r), ~gi.arg2boolvar(lhs), ~gi.arg2boolvar(rhs));
    }

    void p_bool_xor(SolverInstanceBase& s, const Call* call) {
      if (call->n_args() == 2) {
        p_bool_ne(s, call);
      } else {
        p_bool_ne_reif(s, call);
      }
    }

    void p_bool_not(SolverInstanceBase& s, const Call* call) {
      p_bool_ne(s, call);
    }

    void p_bool_or_imp(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      Expression* r = call->arg(2);
      geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), gi.arg2boolvar(lhs), gi.arg2boolvar(rhs));
    }

    void p_bool_and_imp(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      Expression* r = call->arg(2);
      geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), gi.arg2boolvar(lhs));
      geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(r), gi.arg2boolvar(rhs));
    }

    void p_bool_xor_imp(SolverInstanceBase& s, const Call* call) {
      p_bool_ne_imp(s, call);
    }

    void p_bool_clause(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      auto pos = eval_array_lit(gi.env().envi(), call->arg(0));
      auto neg = eval_array_lit(gi.env().envi(), call->arg(1));
      vec<geas::clause_elt> clause;
      for (int i = 0; i < pos->size(); ++i) {
        clause.push(gi.arg2boolvar((*pos)[i]));
      }
      for (int j = 0; j < neg->size(); ++j) {
        clause.push(~gi.arg2boolvar((*neg)[j]));
      }
      geas::add_clause(*gi.solver_data(), clause);
    }

    void p_array_bool_or(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      auto arr = eval_array_lit(gi.env().envi(), call->arg(0));
      geas::patom_t r = gi.arg2boolvar(call->arg(1));
      vec<geas::clause_elt> clause;
      clause.push(~r);
      for (int i = 0; i < arr->size(); ++i) {
        geas::patom_t elem = gi.arg2boolvar((*arr)[i]);
        geas::add_clause(gi.solver_data(), r, ~elem);
        clause.push(elem);
      }
      geas::add_clause(*gi.solver_data(), clause);
    }

    void p_array_bool_and(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      auto arr = eval_array_lit(gi.env().envi(), call->arg(0));
      geas::patom_t r = gi.arg2boolvar(call->arg(1));
      vec<geas::clause_elt> clause;
      clause.push(r);
      for (int i = 0; i < arr->size(); ++i) {
        geas::patom_t elem = gi.arg2boolvar((*arr)[i]);
        geas::add_clause(gi.solver_data(), ~r, elem);
        clause.push(~elem);
      }
      geas::add_clause(*gi.solver_data(), clause);
    }

    void p_bool_clause_imp(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      auto pos = eval_array_lit(gi.env().envi(), call->arg(0));
      auto neg = eval_array_lit(gi.env().envi(), call->arg(1));
      geas::patom_t r = gi.arg2boolvar(call->arg(2));
      vec<geas::clause_elt> clause;
      clause.push(~r);
      for (int i = 0; i < pos->size(); ++i) {
        clause.push(gi.arg2boolvar((*pos)[i]));
      }
      for (int j = 0; j < neg->size(); ++j) {
        clause.push(~gi.arg2boolvar((*neg)[j]));
      }
      geas::add_clause(*gi.solver_data(), clause);
    }

    void p_array_bool_or_imp(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      auto arr = eval_array_lit(gi.env().envi(), call->arg(0));
      geas::patom_t r = gi.arg2boolvar(call->arg(1));
      vec<geas::clause_elt> clause;
      clause.push(~r);
      for (int i = 0; i < arr->size(); ++i) {
        geas::patom_t elem = gi.arg2boolvar((*arr)[i]);
        clause.push(elem);
      }
      geas::add_clause(*gi.solver_data(), clause);
    }

    void p_array_bool_and_imp(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      auto arr = eval_array_lit(gi.env().envi(), call->arg(0));
      geas::patom_t r = gi.arg2boolvar(call->arg(1));
      for (int i = 0; i < arr->size(); ++i) {
        geas::add_clause(gi.solver_data(), ~r, gi.arg2boolvar((*arr)[i]));
      }
    }

    void p_bool_clause_reif(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      auto pos = eval_array_lit(gi.env().envi(), call->arg(0));
      auto neg = eval_array_lit(gi.env().envi(), call->arg(1));
      geas::patom_t r = gi.arg2boolvar(call->arg(2));
      vec<geas::clause_elt> clause;
      clause.push(~r);
      for (int i = 0; i < pos->size(); ++i) {
        geas::patom_t elem = gi.arg2boolvar((*pos)[i]);
        geas::add_clause(gi.solver_data(), r, ~elem);
        clause.push(elem);
      }
      for (int j = 0; j < neg->size(); ++j) {
        geas::patom_t elem = gi.arg2boolvar((*neg)[j]);
        geas::add_clause(gi.solver_data(), r, elem);
        clause.push(~elem);
      }
      geas::add_clause(*gi.solver_data(), clause);
    }

    void p_bool_lin_eq(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      ArrayLit* as = eval_array_lit(gi.env().envi(), call->arg(0));
      ArrayLit* bs = eval_array_lit(gi.env().envi(), call->arg(1));
      IntVal c = eval_int(gi.env().envi(), call->arg(2));
      vec<int> cons(as->size());
      for (int i = 0; i < as->size(); ++i) {
        IntVal iv = eval_int(gi.env().envi(), (*as)[i]);
        cons[i] = static_cast<int>(iv.toInt());
      }
      vec<geas::patom_t> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2boolvar((*bs)[i]);
      }
      // TODO: Rewrite using MiniZinc Library??
      geas::bool_linear_le(gi.solver_data(), geas::at_True, gi.zero, cons, vars, -static_cast<int>(c.toInt()));
      geas::bool_linear_ge(gi.solver_data(), geas::at_True, gi.zero, cons, vars, -static_cast<int>(c.toInt()));
    }

    void p_bool_lin_ne(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      ArrayLit* as = eval_array_lit(gi.env().envi(), call->arg(0));
      ArrayLit* bs = eval_array_lit(gi.env().envi(), call->arg(1));
      IntVal c = eval_int(gi.env().envi(), call->arg(2));
      vec<int> cons(as->size());
      for (int i = 0; i < as->size(); ++i) {
        IntVal iv = eval_int(gi.env().envi(), (*as)[i]);
        cons[i] = static_cast<int>(iv.toInt());
      }
      vec<geas::patom_t> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2boolvar((*bs)[i]);
      }
      geas::bool_linear_ne(gi.solver_data(), cons, vars, static_cast<int>(c.toInt()));
    }

    void p_bool_lin_le(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      ArrayLit* as = eval_array_lit(gi.env().envi(), call->arg(0));
      ArrayLit* bs = eval_array_lit(gi.env().envi(), call->arg(1));
      IntVal c = eval_int(gi.env().envi(), call->arg(2));
      vec<int> cons(as->size());
      for (int i = 0; i < as->size(); ++i) {
        IntVal iv = eval_int(gi.env().envi(), (*as)[i]);
        cons[i] = static_cast<int>(iv.toInt());
      }
      vec<geas::patom_t> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2boolvar((*bs)[i]);
      }
      geas::bool_linear_le(gi.solver_data(), geas::at_True, gi.zero, cons, vars, -static_cast<int>(c.toInt()));
    }

    void p_bool_lin_eq_imp(SolverInstanceBase& s, const Call* call) {
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
      vec<geas::patom_t> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2boolvar((*bs)[i]);
      }
      // TODO: Rewrite using MiniZinc Library??
      geas::bool_linear_le(gi.solver_data(), gi.arg2boolvar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt()));
      geas::bool_linear_ge(gi.solver_data(), gi.arg2boolvar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt()));
    }

    void p_bool_lin_ne_imp(SolverInstanceBase& s, const Call* call) {
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
      vec<geas::patom_t> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2boolvar((*bs)[i]);
      }
      geas::bool_linear_ne(gi.solver_data(), cons, vars, static_cast<int>(c.toInt()), gi.arg2boolvar(r));
    }

    void p_bool_lin_le_imp(SolverInstanceBase& s, const Call* call) {
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
      vec<geas::patom_t> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2boolvar((*bs)[i]);
      }
      geas::bool_linear_le(gi.solver_data(), gi.arg2boolvar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt()));
    }

    void p_bool_lin_eq_reif(SolverInstanceBase& s, const Call* call) {
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
      vec<geas::patom_t> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2boolvar((*bs)[i]);
      }
      // TODO: Rewrite using MiniZinc Library??
      geas::bool_linear_le(gi.solver_data(), gi.arg2boolvar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt()));
      geas::bool_linear_ge(gi.solver_data(), gi.arg2boolvar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt()));
      geas::bool_linear_ne(gi.solver_data(), cons, vars, static_cast<int>(c.toInt()), ~gi.arg2boolvar(r));
    }

    void p_bool_lin_ne_reif(SolverInstanceBase& s, const Call* call) {
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
      vec<geas::patom_t> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2boolvar((*bs)[i]);
      }
      // TODO: Rewrite using MiniZinc Library??
      geas::bool_linear_ne(gi.solver_data(), cons, vars, static_cast<int>(c.toInt()), gi.arg2boolvar(r));
      geas::bool_linear_le(gi.solver_data(), ~gi.arg2boolvar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt()));
      geas::bool_linear_ge(gi.solver_data(), ~gi.arg2boolvar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt()));
    }

    void p_bool_lin_le_reif(SolverInstanceBase& s, const Call* call) {
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
      vec<geas::patom_t> vars(bs->size());
      for (int i = 0; i < bs->size(); ++i) {
        vars[i] = gi.arg2boolvar((*bs)[i]);
      }
      // TODO: Rewrite using MiniZinc Library??
      geas::bool_linear_le(gi.solver_data(), gi.arg2boolvar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt()));
      geas::bool_linear_ge(gi.solver_data(), ~gi.arg2boolvar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt())-1);
    }

    void p_bool2int(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* b = call->arg(0);
      Expression* i = call->arg(1);
      geas::add_clause(gi.solver_data(), gi.arg2boolvar(b), gi.arg2intvar(i) <= 0);
      geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(b), gi.arg2intvar(i) >= 1);
    }

    void p_array_int_element(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* i = call->arg(0);
      ArrayLit* array = eval_array_lit(gi.env().envi(), call->arg(1));
      assert(array->min(0) == 1 && array->max(0) == array->size()+1);
      Expression* res = call->arg(1);
      if (!i->type().isvar()) {
        int ival = static_cast<int>(eval_int(gi.env().envi(), i).toInt());
        int elem = static_cast<int>(eval_int(gi.env().envi(), (*array)[ival-1]).toInt());
        gi.solver().post(gi.arg2intvar(res)==elem);
      } else if (!res->type().isvar()) {
        IntVal resval = eval_int(gi.env().envi(), res);
        geas::intvar ivar = gi.arg2intvar(i);
        for (int j = 0; j < array->size(); ++j) {
          if (eval_int(gi.env().envi(), (*array)[j]) != resval) {
            gi.solver().post(ivar != j+1);
          }
        }
      } else {
        vec<int> vals;
        for (int j = 0; j < array->size(); ++j) {
          int val = static_cast<int>(eval_int(gi.env().envi(), (*array)[j]).toInt());
          vals.push(val);
        }
        geas::int_element(gi.solver_data(), gi.arg2intvar(res), gi.arg2intvar(i), vals);
      }
    }

    void p_array_bool_element(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* i = call->arg(0);
      ArrayLit* array = eval_array_lit(gi.env().envi(), call->arg(1));
      assert(array->min(0) == 1 && array->max(0) == array->size()+1);
      Expression* res = call->arg(1);
      if (!i->type().isvar()) {
        int ival = static_cast<int>(eval_int(gi.env().envi(), i).toInt());
        bool elem = eval_bool(gi.env().envi(), (*array)[ival-1]);
        gi.solver().post(elem ? gi.arg2boolvar(res) : ~gi.arg2boolvar(res));
      } else if (!res->type().isvar()) {
        bool resval = eval_bool(gi.env().envi(), res);
        geas::intvar ivar = gi.arg2intvar(i);
        for (int j = 0; j < array->size(); ++j) {
          if (eval_bool(gi.env().envi(), (*array)[j]) != resval) {
            gi.solver().post(ivar != j+1);
          }
        }
      } else {
        geas::intvar ivar = gi.arg2intvar(i);
        geas::patom_t resvar = gi.arg2boolvar(res);
        for (int j = 0; j < array->size(); ++j) {
          bool b = eval_bool(gi.env().envi(), (*array)[j]);
          geas::add_clause(gi.solver_data(), ivar != j+1, b ? resvar : ~resvar);
        }
      }
    }

    void p_array_var_int_element(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* i = call->arg(0);
      ArrayLit* array = eval_array_lit(gi.env().envi(), call->arg(1));
      assert(array->min(0) == 1 && array->max(0) == array->size()+1);
      Expression* res = call->arg(1);
      if (!array->type().isvar()) {
        return p_array_int_element(s, call);
      }
      if (!i->type().isvar() && !res->type().isvar()) {
        int ival = static_cast<int>(eval_int(gi.env().envi(), i).toInt());
        int resval = static_cast<int>(eval_int(gi.env().envi(), res).toInt());
        gi.solver().post(gi.arg2intvar((*array)[ival-1]) == resval);
      } else if (!i->type().isvar()) {
        int ival = static_cast<int>(eval_int(gi.env().envi(), i).toInt());
        Expression* elem = (*array)[ival-1];
        if (!elem->type().isvar()) {
          return p_array_int_element(s, call);
        } else {
          geas::int_eq(gi.solver_data(), gi.arg2intvar(elem), gi.arg2intvar(res));
        }
      } else if (!res->type().isvar()) {
        IntVal resval = eval_int(gi.env().envi(), res);
        geas::intvar ivar = gi.arg2intvar(i);
        for (int j = 0; j < array->size(); ++j) {
          Expression* elem = (*array)[j];
          if (elem->type().isvar()) {
            geas::add_clause(gi.solver_data(), ivar != j+1, gi.arg2intvar(elem) == resval.toInt());
          } else {
            if (eval_int(gi.env().envi(), elem) != resval) {
              gi.solver().post(ivar != j+1);
            }
          }
        }
      } else {
        vec<geas::intvar> vals;
        for (int j = 0; j < array->size(); ++j) {
          geas::intvar val = gi.arg2intvar((*array)[j]);
          vals.push(val);
        }
        geas::var_int_element(gi.solver_data(), gi.arg2intvar(res), gi.arg2intvar(i), vals);
      }
    }

    void p_array_var_bool_element(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* i = call->arg(0);
      ArrayLit* array = eval_array_lit(gi.env().envi(), call->arg(1));
      assert(array->min(0) == 1 && array->max(0) == array->size()+1);
      Expression* res = call->arg(1);
      if (!array->type().isvar()) {
        return p_array_bool_element(s, call);
      }
      if (!i->type().isvar() && !res->type().isvar()) {
        int ival = static_cast<int>(eval_int(gi.env().envi(), i).toInt());
        bool resval = eval_bool(gi.env().envi(), res);
        gi.solver().post(resval ? gi.arg2boolvar((*array)[ival-1]) : ~gi.arg2boolvar((*array)[ival-1]));
      } else if (!i->type().isvar()) {
        int ival = static_cast<int>(eval_int(gi.env().envi(), i).toInt());
        Expression* elem = (*array)[ival-1];
        if (!elem->type().isvar()) {
          return p_array_bool_element(s, call);
        } else {
          geas::add_clause(gi.solver_data(), gi.arg2boolvar(res), ~gi.arg2boolvar(elem));
          geas::add_clause(gi.solver_data(), ~gi.arg2boolvar(res), gi.arg2boolvar(elem));
        }
      } else if (!res->type().isvar()) {
        bool resval = eval_bool(gi.env().envi(), res);
        geas::intvar ivar = gi.arg2intvar(i);
        for (int j = 0; j < array->size(); ++j) {
          Expression* elem = (*array)[j];
          if (elem->type().isvar()) {
            geas::add_clause(gi.solver_data(), ivar != j+1, resval ? gi.arg2boolvar(elem) : ~gi.arg2boolvar(elem));
          } else {
            if (eval_bool(gi.env().envi(), elem) != resval) {
              gi.solver().post(ivar != j+1);
            }
          }
        }
      } else {
        geas::intvar ivar = gi.arg2intvar(i);
        geas::patom_t resvar = gi.arg2boolvar(res);
        for (int j = 0; j < array->size(); ++j) {
          geas::patom_t bvar = gi.arg2boolvar((*array)[j]);
          geas::add_clause(gi.solver_data(), ivar != j+1, ~bvar, resvar);
          geas::add_clause(gi.solver_data(), ivar != j+1, bvar, ~resvar);
        }
      }
    }

  }
}
