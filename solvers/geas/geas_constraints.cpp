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
#include <geas/constraints/flow/flow.h>

namespace MiniZinc {
  namespace GeasConstraints {

#define ARG(X) call->arg(X)
#define PAR(X) call->arg(X)->type().ispar()
#define ARRAY(X) eval_array_lit(s.env().envi(), call->arg(X))

    void p_int_eq(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      geas::int_eq(gi.solver_data(), gi.asIntVar(lhs), gi.asIntVar(rhs));
    }

    void p_int_ne(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      geas::int_ne(gi.solver_data(), gi.asIntVar(lhs), gi.asIntVar(rhs));
    }

    void p_int_le(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      geas::int_le(gi.solver_data(), gi.asIntVar(lhs), gi.asIntVar(rhs), 0);
    }

    void p_int_lt(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      geas::int_le(gi.solver_data(), gi.asIntVar(lhs), gi.asIntVar(rhs), -1);
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
        geas::int_eq(gi.solver_data(), gi.asIntVar(lhs), gi.asIntVar(rhs), gi.asBoolVar(r));
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
        geas::int_ne(gi.solver_data(), gi.asIntVar(lhs), gi.asIntVar(rhs), gi.asBoolVar(r));
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
        geas::int_le(gi.solver_data(), gi.asIntVar(lhs), gi.asIntVar(rhs), 0, gi.asBoolVar(r));
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
        geas::int_le(gi.solver_data(), gi.asIntVar(lhs), gi.asIntVar(rhs), -1, gi.asBoolVar(r));
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
        geas::int_eq(gi.solver_data(), gi.asIntVar(lhs), gi.asIntVar(rhs), gi.asBoolVar(r));
        geas::int_ne(gi.solver_data(), gi.asIntVar(lhs), gi.asIntVar(rhs), ~gi.asBoolVar(r));
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
        geas::int_ne(gi.solver_data(), gi.asIntVar(lhs), gi.asIntVar(rhs), gi.asBoolVar(r));
        geas::int_eq(gi.solver_data(), gi.asIntVar(lhs), gi.asIntVar(rhs), ~gi.asBoolVar(r));
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
        geas::int_le(gi.solver_data(), gi.asIntVar(lhs), gi.asIntVar(rhs), 0, gi.asBoolVar(r));
        geas::int_le(gi.solver_data(), gi.asIntVar(rhs), gi.asIntVar(lhs), -1, ~gi.asBoolVar(r));
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
        geas::int_le(gi.solver_data(), gi.asIntVar(lhs), gi.asIntVar(rhs), -1, gi.asBoolVar(r));
        geas::int_le(gi.solver_data(), gi.asIntVar(rhs), gi.asIntVar(lhs), 0, ~gi.asBoolVar(r));
      }
    }

    void p_int_abs(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* org = call->arg(0);
      Expression* res = call->arg(1);
      geas::int_abs(gi.solver_data(), gi.asIntVar(res), gi.asIntVar(org));
    }

    void p_int_times(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      Expression* res = call->arg(2);
      geas::int_mul(gi.solver_data(), gi.asIntVar(lhs), gi.asIntVar(rhs), gi.asIntVar(res));
    }

    void p_int_div(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      Expression* res = call->arg(2);
      geas::int_div(gi.solver_data(), gi.asIntVar(res), gi.asIntVar(lhs), gi.asIntVar(rhs));
    }

    void p_int_max(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      vec<geas::intvar> vars = {gi.asIntVar(lhs), gi.asIntVar(rhs)};
      Expression* res = call->arg(2);
      geas::int_max(gi.solver_data(), gi.asIntVar(res), vars);
    }

    void p_int_min(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      vec<geas::intvar> vars = {-gi.asIntVar(lhs), -gi.asIntVar(rhs)};
      Expression* res = call->arg(2);
      geas::int_max(gi.solver_data(), -gi.asIntVar(res), vars);
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
        vars[i] = gi.asIntVar((*bs)[i]);
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
        vars[i] = gi.asIntVar((*bs)[i]);
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
        vars[i] = gi.asIntVar((*bs)[i]);
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
        vars[i] = gi.asIntVar((*bs)[i]);
      }
      // TODO: Rewrite using MiniZinc Library??
      geas::linear_le(gi.solver_data(), pos, vars, static_cast<int>(c.toInt()), gi.asBoolVar(r));
      geas::linear_le(gi.solver_data(), neg, vars, -static_cast<int>(c.toInt()), gi.asBoolVar(r));
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
        vars[i] = gi.asIntVar((*bs)[i]);
      }
      geas::linear_ne(gi.solver_data(), cons, vars, static_cast<int>(c.toInt()), gi.asBoolVar(r));
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
        vars[i] = gi.asIntVar((*bs)[i]);
      }
      geas::linear_le(gi.solver_data(), cons, vars, static_cast<int>(c.toInt()), gi.asBoolVar(r));
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
        vars[i] = gi.asIntVar((*bs)[i]);
      }
      // TODO: Rewrite using MiniZinc Library??
      geas::linear_le(gi.solver_data(), pos, vars, static_cast<int>(c.toInt()), gi.asBoolVar(r));
      geas::linear_le(gi.solver_data(), neg, vars, -static_cast<int>(c.toInt()), gi.asBoolVar(r));
      geas::linear_ne(gi.solver_data(), pos, vars, static_cast<int>(c.toInt()), ~gi.asBoolVar(r));
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
        vars[i] = gi.asIntVar((*bs)[i]);
      }
      geas::linear_ne(gi.solver_data(), pos, vars, static_cast<int>(c.toInt()), gi.asBoolVar(r));
      geas::linear_le(gi.solver_data(), pos, vars, static_cast<int>(c.toInt()), ~gi.asBoolVar(r));
      geas::linear_le(gi.solver_data(), neg, vars, -static_cast<int>(c.toInt()), ~gi.asBoolVar(r));
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
        vars[i] = gi.asIntVar((*bs)[i]);
      }
      geas::linear_le(gi.solver_data(), pos, vars, static_cast<int>(c.toInt()), gi.asBoolVar(r));
      geas::linear_le(gi.solver_data(), neg, vars, -static_cast<int>(c.toInt())-1, ~gi.asBoolVar(r));
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
        gi.solver().post(b ? gi.asBoolVar(lhs) : ~gi.asBoolVar(lhs));
      } else {
        geas::add_clause(gi.solver_data(), gi.asBoolVar(lhs), ~gi.asBoolVar(rhs));
        geas::add_clause(gi.solver_data(), ~gi.asBoolVar(lhs), gi.asBoolVar(rhs));
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
        gi.solver().post(b ? ~gi.asBoolVar(lhs) : gi.asBoolVar(lhs));
      } else {
        geas::add_clause(gi.solver_data(), gi.asBoolVar(lhs), gi.asBoolVar(rhs));
        geas::add_clause(gi.solver_data(), ~gi.asBoolVar(lhs), ~gi.asBoolVar(rhs));
      }
    }

    void p_bool_le(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      if(!lhs->type().isvarbool()) {
        if (eval_bool(gi.env().envi(), lhs)) {
          gi.solver().post(gi.asBoolVar(rhs));
        }
      } else if (!rhs->type().isvarbool()) {
        if (!eval_bool(gi.env().envi(), rhs)) {
          gi.solver().post(~gi.asBoolVar(lhs));
        }
      } else {
        geas::add_clause(gi.solver_data(), ~gi.asBoolVar(lhs), gi.asBoolVar(rhs));
      }
    }

    void p_bool_lt(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      gi.solver().post(~gi.asBoolVar(lhs));
      gi.solver().post(gi.asBoolVar(rhs));
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
        geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), ~gi.asBoolVar(lhs), gi.asBoolVar(rhs));
        geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), gi.asBoolVar(lhs), ~gi.asBoolVar(rhs));
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
        geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), gi.asBoolVar(lhs), gi.asBoolVar(rhs));
        geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), ~gi.asBoolVar(lhs), ~gi.asBoolVar(rhs));
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
        geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), ~gi.asBoolVar(lhs), gi.asBoolVar(rhs));
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
        geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), ~gi.asBoolVar(lhs));
        geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), gi.asBoolVar(rhs));
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
        geas::add_clause(gi.solver_data(), gi.asBoolVar(r), gi.asBoolVar(lhs), gi.asBoolVar(rhs));
        geas::add_clause(gi.solver_data(), gi.asBoolVar(r), ~gi.asBoolVar(lhs), ~gi.asBoolVar(rhs));
        geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), ~gi.asBoolVar(lhs), gi.asBoolVar(rhs));
        geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), gi.asBoolVar(lhs), ~gi.asBoolVar(rhs));
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
        geas::add_clause(gi.solver_data(), gi.asBoolVar(r), ~gi.asBoolVar(lhs), gi.asBoolVar(rhs));
        geas::add_clause(gi.solver_data(), gi.asBoolVar(r), gi.asBoolVar(lhs), ~gi.asBoolVar(rhs));
        geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), gi.asBoolVar(lhs), gi.asBoolVar(rhs));
        geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), ~gi.asBoolVar(lhs), ~gi.asBoolVar(rhs));
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
        geas::add_clause(gi.solver_data(), gi.asBoolVar(r), ~gi.asBoolVar(rhs));
        geas::add_clause(gi.solver_data(), gi.asBoolVar(r), gi.asBoolVar(lhs));
        geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), ~gi.asBoolVar(lhs), gi.asBoolVar(rhs));
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
        geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), ~gi.asBoolVar(lhs));
        geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), gi.asBoolVar(rhs));
        geas::add_clause(gi.solver_data(), gi.asBoolVar(r), gi.asBoolVar(lhs), ~gi.asBoolVar(rhs));
      }
    }

    void p_bool_or(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      Expression* r = call->arg(2);
      geas::add_clause(gi.solver_data(), gi.asBoolVar(r), ~gi.asBoolVar(lhs));
      geas::add_clause(gi.solver_data(), gi.asBoolVar(r), ~gi.asBoolVar(rhs));
      geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), gi.asBoolVar(lhs), gi.asBoolVar(rhs));
    }

    void p_bool_and(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      Expression* r = call->arg(2);
      geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), gi.asBoolVar(lhs));
      geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), gi.asBoolVar(rhs));
      geas::add_clause(gi.solver_data(), gi.asBoolVar(r), ~gi.asBoolVar(lhs), ~gi.asBoolVar(rhs));
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
      geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), gi.asBoolVar(lhs), gi.asBoolVar(rhs));
    }

    void p_bool_and_imp(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* lhs = call->arg(0);
      Expression* rhs = call->arg(1);
      Expression* r = call->arg(2);
      geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), gi.asBoolVar(lhs));
      geas::add_clause(gi.solver_data(), ~gi.asBoolVar(r), gi.asBoolVar(rhs));
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
        clause.push(gi.asBoolVar((*pos)[i]));
      }
      for (int j = 0; j < neg->size(); ++j) {
        clause.push(~gi.asBoolVar((*neg)[j]));
      }
      geas::add_clause(*gi.solver_data(), clause);
    }

    void p_array_bool_or(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      auto arr = eval_array_lit(gi.env().envi(), call->arg(0));
      geas::patom_t r = gi.asBoolVar(call->arg(1));
      vec<geas::clause_elt> clause;
      clause.push(~r);
      for (int i = 0; i < arr->size(); ++i) {
        geas::patom_t elem = gi.asBoolVar((*arr)[i]);
        geas::add_clause(gi.solver_data(), r, ~elem);
        clause.push(elem);
      }
      geas::add_clause(*gi.solver_data(), clause);
    }

    void p_array_bool_and(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      auto arr = eval_array_lit(gi.env().envi(), call->arg(0));
      geas::patom_t r = gi.asBoolVar(call->arg(1));
      vec<geas::clause_elt> clause;
      clause.push(r);
      for (int i = 0; i < arr->size(); ++i) {
        geas::patom_t elem = gi.asBoolVar((*arr)[i]);
        geas::add_clause(gi.solver_data(), ~r, elem);
        clause.push(~elem);
      }
      geas::add_clause(*gi.solver_data(), clause);
    }

    void p_bool_clause_imp(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      auto pos = eval_array_lit(gi.env().envi(), call->arg(0));
      auto neg = eval_array_lit(gi.env().envi(), call->arg(1));
      geas::patom_t r = gi.asBoolVar(call->arg(2));
      vec<geas::clause_elt> clause;
      clause.push(~r);
      for (int i = 0; i < pos->size(); ++i) {
        clause.push(gi.asBoolVar((*pos)[i]));
      }
      for (int j = 0; j < neg->size(); ++j) {
        clause.push(~gi.asBoolVar((*neg)[j]));
      }
      geas::add_clause(*gi.solver_data(), clause);
    }

    void p_array_bool_or_imp(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      auto arr = eval_array_lit(gi.env().envi(), call->arg(0));
      geas::patom_t r = gi.asBoolVar(call->arg(1));
      vec<geas::clause_elt> clause;
      clause.push(~r);
      for (int i = 0; i < arr->size(); ++i) {
        geas::patom_t elem = gi.asBoolVar((*arr)[i]);
        clause.push(elem);
      }
      geas::add_clause(*gi.solver_data(), clause);
    }

    void p_array_bool_and_imp(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      auto arr = eval_array_lit(gi.env().envi(), call->arg(0));
      geas::patom_t r = gi.asBoolVar(call->arg(1));
      for (int i = 0; i < arr->size(); ++i) {
        geas::add_clause(gi.solver_data(), ~r, gi.asBoolVar((*arr)[i]));
      }
    }

    void p_bool_clause_reif(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      auto pos = eval_array_lit(gi.env().envi(), call->arg(0));
      auto neg = eval_array_lit(gi.env().envi(), call->arg(1));
      geas::patom_t r = gi.asBoolVar(call->arg(2));
      vec<geas::clause_elt> clause;
      clause.push(~r);
      for (int i = 0; i < pos->size(); ++i) {
        geas::patom_t elem = gi.asBoolVar((*pos)[i]);
        geas::add_clause(gi.solver_data(), r, ~elem);
        clause.push(elem);
      }
      for (int j = 0; j < neg->size(); ++j) {
        geas::patom_t elem = gi.asBoolVar((*neg)[j]);
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
        vars[i] = gi.asBoolVar((*bs)[i]);
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
        vars[i] = gi.asBoolVar((*bs)[i]);
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
        vars[i] = gi.asBoolVar((*bs)[i]);
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
        vars[i] = gi.asBoolVar((*bs)[i]);
      }
      // TODO: Rewrite using MiniZinc Library??
      geas::bool_linear_le(gi.solver_data(), gi.asBoolVar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt()));
      geas::bool_linear_ge(gi.solver_data(), gi.asBoolVar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt()));
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
        vars[i] = gi.asBoolVar((*bs)[i]);
      }
      geas::bool_linear_ne(gi.solver_data(), cons, vars, static_cast<int>(c.toInt()), gi.asBoolVar(r));
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
        vars[i] = gi.asBoolVar((*bs)[i]);
      }
      geas::bool_linear_le(gi.solver_data(), gi.asBoolVar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt()));
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
        vars[i] = gi.asBoolVar((*bs)[i]);
      }
      // TODO: Rewrite using MiniZinc Library??
      geas::bool_linear_le(gi.solver_data(), gi.asBoolVar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt()));
      geas::bool_linear_ge(gi.solver_data(), gi.asBoolVar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt()));
      geas::bool_linear_ne(gi.solver_data(), cons, vars, static_cast<int>(c.toInt()), ~gi.asBoolVar(r));
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
        vars[i] = gi.asBoolVar((*bs)[i]);
      }
      // TODO: Rewrite using MiniZinc Library??
      geas::bool_linear_ne(gi.solver_data(), cons, vars, static_cast<int>(c.toInt()), gi.asBoolVar(r));
      geas::bool_linear_le(gi.solver_data(), ~gi.asBoolVar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt()));
      geas::bool_linear_ge(gi.solver_data(), ~gi.asBoolVar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt()));
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
        vars[i] = gi.asBoolVar((*bs)[i]);
      }
      // TODO: Rewrite using MiniZinc Library??
      geas::bool_linear_le(gi.solver_data(), gi.asBoolVar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt()));
      geas::bool_linear_ge(gi.solver_data(), ~gi.asBoolVar(r), gi.zero, cons, vars, -static_cast<int>(c.toInt())-1);
    }

    void p_bool2int(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      Expression* b = call->arg(0);
      Expression* i = call->arg(1);
      geas::add_clause(gi.solver_data(), gi.asBoolVar(b), gi.asIntVar(i) <= 0);
      geas::add_clause(gi.solver_data(), ~gi.asBoolVar(b), gi.asIntVar(i) >= 1);
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
        gi.solver().post(gi.asIntVar(res)==elem);
      } else if (!res->type().isvar()) {
        IntVal resval = eval_int(gi.env().envi(), res);
        geas::intvar ivar = gi.asIntVar(i);
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
        geas::int_element(gi.solver_data(), gi.asIntVar(res), gi.asIntVar(i), vals);
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
        gi.solver().post(elem ? gi.asBoolVar(res) : ~gi.asBoolVar(res));
      } else if (!res->type().isvar()) {
        bool resval = eval_bool(gi.env().envi(), res);
        geas::intvar ivar = gi.asIntVar(i);
        for (int j = 0; j < array->size(); ++j) {
          if (eval_bool(gi.env().envi(), (*array)[j]) != resval) {
            gi.solver().post(ivar != j+1);
          }
        }
      } else {
        geas::intvar ivar = gi.asIntVar(i);
        geas::patom_t resvar = gi.asBoolVar(res);
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
        gi.solver().post(gi.asIntVar((*array)[ival - 1]) == resval);
      } else if (!i->type().isvar()) {
        int ival = static_cast<int>(eval_int(gi.env().envi(), i).toInt());
        Expression* elem = (*array)[ival-1];
        if (!elem->type().isvar()) {
          return p_array_int_element(s, call);
        } else {
          geas::int_eq(gi.solver_data(), gi.asIntVar(elem), gi.asIntVar(res));
        }
      } else if (!res->type().isvar()) {
        IntVal resval = eval_int(gi.env().envi(), res);
        geas::intvar ivar = gi.asIntVar(i);
        for (int j = 0; j < array->size(); ++j) {
          Expression* elem = (*array)[j];
          if (elem->type().isvar()) {
            geas::add_clause(gi.solver_data(), ivar != j+1, gi.asIntVar(elem) == resval.toInt());
          } else {
            if (eval_int(gi.env().envi(), elem) != resval) {
              gi.solver().post(ivar != j+1);
            }
          }
        }
      } else {
        vec<geas::intvar> vals;
        for (int j = 0; j < array->size(); ++j) {
          geas::intvar val = gi.asIntVar((*array)[j]);
          vals.push(val);
        }
        geas::var_int_element(gi.solver_data(), gi.asIntVar(res), gi.asIntVar(i), vals);
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
        gi.solver().post(resval ? gi.asBoolVar((*array)[ival - 1]) : ~gi.asBoolVar((*array)[ival - 1]));
      } else if (!i->type().isvar()) {
        int ival = static_cast<int>(eval_int(gi.env().envi(), i).toInt());
        Expression* elem = (*array)[ival-1];
        if (!elem->type().isvar()) {
          return p_array_bool_element(s, call);
        } else {
          geas::add_clause(gi.solver_data(), gi.asBoolVar(res), ~gi.asBoolVar(elem));
          geas::add_clause(gi.solver_data(), ~gi.asBoolVar(res), gi.asBoolVar(elem));
        }
      } else if (!res->type().isvar()) {
        bool resval = eval_bool(gi.env().envi(), res);
        geas::intvar ivar = gi.asIntVar(i);
        for (int j = 0; j < array->size(); ++j) {
          Expression* elem = (*array)[j];
          if (elem->type().isvar()) {
            geas::add_clause(gi.solver_data(), ivar != j+1, resval ? gi.asBoolVar(elem) : ~gi.asBoolVar(elem));
          } else {
            if (eval_bool(gi.env().envi(), elem) != resval) {
              gi.solver().post(ivar != j+1);
            }
          }
        }
      } else {
        geas::intvar ivar = gi.asIntVar(i);
        geas::patom_t resvar = gi.asBoolVar(res);
        for (int j = 0; j < array->size(); ++j) {
          geas::patom_t bvar = gi.asBoolVar((*array)[j]);
          geas::add_clause(gi.solver_data(), ivar != j+1, ~bvar, resvar);
          geas::add_clause(gi.solver_data(), ivar != j+1, bvar, ~resvar);
        }
      }
    }

    void p_all_different(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      ArrayLit* array = eval_array_lit(gi.env().envi(), call->arg(0));
      vec<geas::intvar> vals;
      for (int i = 0; i < array->size(); ++i) {
        geas::intvar val = gi.asIntVar((*array)[i]);
        vals.push(val);
      }
      geas::all_different_int(gi.solver_data(), vals);
    }

    void p_all_different_except_0(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      ArrayLit* array = eval_array_lit(gi.env().envi(), call->arg(0));
      vec<geas::intvar> vars;
      for (int i = 0; i < array->size(); ++i) {
        geas::intvar val = gi.asIntVar((*array)[i]);
        vars.push(val);
      }
      geas::all_different_except_0(gi.solver_data(), vars);
    }

    void p_at_most(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      int n = static_cast<int>(eval_int(gi.env().envi(), call->arg(0)).toInt());
      ArrayLit* array = eval_array_lit(gi.env().envi(), call->arg(1));
      int v = static_cast<int>(eval_int(gi.env().envi(), call->arg(2)).toInt());
      vec<geas::patom_t> vars;
      for (int i = 0; i < array->size(); ++i) {
        geas::intvar var = gi.asIntVar((*array)[i]);
        vars.push(var == v);
      }
      if (n == 1) {
        geas::atmost_1(gi.solver_data(), vars);
      } else {
        geas::atmost_k(gi.solver_data(), vars, n);
      }
    }

    void p_at_most1(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      ArrayLit* array = eval_array_lit(gi.env().envi(), call->arg(1));
      int v = static_cast<int>(eval_int(gi.env().envi(), call->arg(2)).toInt());
      vec<geas::patom_t> vars;
      for (int i = 0; i < array->size(); ++i) {
        geas::intvar var = gi.asIntVar((*array)[i]);
        vars.push(var == v);
      }
      geas::atmost_1(gi.solver_data(), vars);
    }

    void p_cumulative(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      vec<geas::intvar> st = gi.asIntVar(eval_array_lit(gi.env().envi(), call->arg(0)));
      if (PAR(1) && PAR(2) && PAR(3)) {
        vec<int> d = gi.asInt(eval_array_lit(gi.env().envi(), call->arg(1)));
        vec<int> r = gi.asInt(eval_array_lit(gi.env().envi(), call->arg(2)));
        int b = gi.asInt(call->arg(3));
        geas::cumulative(gi.solver_data(), st, d, r, b);
      } else {
        vec<geas::intvar> d = gi.asIntVar(eval_array_lit(gi.env().envi(), call->arg(1)));
        vec<geas::intvar> r = gi.asIntVar(eval_array_lit(gi.env().envi(), call->arg(2)));
        geas::intvar b = gi.asIntVar(call->arg(3));
        geas::cumulative_var(gi.solver_data(), st, d, r, b);
      }
    }

    void p_disjunctive(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      vec<geas::intvar> st = gi.asIntVar(ARRAY(0));
      if (PAR(1)) {
        vec<int> d = gi.asInt(ARRAY(1));
        geas::disjunctive_int(gi.solver_data(), st, d);
      } else {
        vec<geas::intvar> d = gi.asIntVar(ARRAY(1));
        geas::disjunctive_var(gi.solver_data(), st, d);
      }
    }

    void p_global_cardinality(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      vec<geas::intvar> x = gi.asIntVar(ARRAY(0));
      vec<int> cover = gi.asInt(ARRAY(1));
      vec<int> count = gi.asInt(ARRAY(2));

      vec<int> srcs(x.size(), 1);
      vec<geas::bflow> flows;
      for (int i = 0; i < x.size(); ++i) {
        for (int j = 0; j < cover.size(); ++j) {
          if (x[i].lb(gi.solver_data()) <= cover[j] && cover[j] <= x[i].ub(gi.solver_data())) {
            flows.push({i, j, x[i] == cover[j]});
          }
        }
      }
      geas::bipartite_flow(gi.solver_data(), srcs, count, flows);
    }

    void p_table_int(SolverInstanceBase& s, const Call* call) {
      auto& gi = static_cast<GeasSolverInstance&>(s);
      vec<geas::intvar> vars = gi.asIntVar(ARRAY(0));
      vec<int> tmp = gi.asInt(ARRAY(1));
      assert(tmp.size() % vars.size() == 0);
      vec<vec<int>> table(tmp.size()/vars.size());
      for (int i = 0; i < table.size(); ++i) {
        vec<int> row(vars.size());
        for (int j = 0; j < vars.size(); ++j) {
          row[j] = tmp[i*vars.size() + j];
        }
        table.push(row);
      }
      geas::table_id id = geas::table::build(gi.solver_data(), table);
      // TODO: Annotations for table versions
      geas::table::post(gi.solver_data(), id, vars);
    }

  }
}
