/***
--- !Test
type: compile
solvers: [gecode]
options:
  -G: std
expected: !FlatZinc nvalue_linexp.fzn
***/

% Expect one int_lin_eq per element followed by scalar int_eq_reif calls because
% each repeatedly read element expression keeps its name.

include "nvalue.mzn";

array [1..2] of var 1..2: r;
array [1..2] of var 1..2: d;

constraint nvalue(2, [(r[p] - 1) * 2 + d[p] | p in 1..2]);
