% This file contains redefinitions of standard builtins that can be overridden
% by solvers.

predicate bool_clause_reif(array [int] of var bool: xs, array [int] of var bool: ys, var bool: b) =
  clause(xs, ys ++ [b]) /\
    forall (i in index_set(xs)) (xs[i] -> b) /\
    forall (i in index_set(ys)) (ys[i] \/ b);

predicate array_int_maximum(var int: m, array [int] of var int: xs) =
  let {
    int: l = min(index_set(xs));
    int: u = max(index_set(xs));
    int: ly = lb_array(xs);
    int: uy = ub_array(xs);
    array [l..u] of var ly..uy: y;
  } in y[l] = xs[l] /\ m = y[u] /\ forall (i in l + 1..u) (y[i] == max(xs[i], y[i - 1]));

predicate array_float_maximum(var float: m, array [int] of var float: xs) =
  let {
    int: l = min(index_set(xs));
    int: u = max(index_set(xs));
    float: ly = lb_array(xs);
    float: uy = ub_array(xs);
    array [l..u] of var ly..uy: y;
  } in y[l] = xs[l] /\ m = y[u] /\ forall (i in l + 1..u) (y[i] == max(xs[i], y[i - 1]));

predicate array_int_minimum(var int: m, array [int] of var int: xs) =
  let {
    int: l = min(index_set(xs));
    int: u = max(index_set(xs));
    int: ly = lb_array(xs);
    int: uy = ub_array(xs);
    array [l..u] of var ly..uy: y;
  } in y[l] = xs[l] /\ m = y[u] /\ forall (i in l + 1..u) (y[i] == min(xs[i], y[i - 1]));

predicate array_float_minimum(var float: m, array [int] of var float: xs) =
  let {
    int: l = min(index_set(xs));
    int: u = max(index_set(xs));
    float: ly = lb_array(xs);
    float: uy = ub_array(xs);
    array [l..u] of var ly..uy: y;
  } in y[l] = xs[l] /\ m = y[u] /\ forall (i in l + 1..u) (y[i] == min(xs[i], y[i - 1]));
