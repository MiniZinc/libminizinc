/***
!Test
solvers: [xpress]
options:
  -D: "fXBZCuts01=true; fXBZCutGen=true"
expected: !Result
  solution: !Solution
    objective: 140.0
***/

% User cut test. The -D flags turn element constraints into a MIP_cut generator that the
% Xpress wrapper drives from its cut-round callback. User cuts only tighten the relaxation,
% so the optimum is 140.0 with or without them.

include "all_different.mzn";

int: n = 10;
array [1..n] of var 0.0..100.0: x;
array [1..5] of var 1..n: idx;
array [1..5] of var float: z;

constraint forall (i in 1..n) (x[i] <= 7.0 * i);
constraint forall (k in 1..5) (z[k] = x[idx[k]]);
constraint all_different(idx);
constraint sum (i in 1..n) (x[i]) <= 200.0;

var float: total = sum (k in 1..5) (z[k]) - 0.3 * sum (i in 1..n) (x[i]);

solve maximize total;

output ["idx = ", show(idx), ", total = ", show(total), "\n"];