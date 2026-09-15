/***
--- !Test
type: compile
solvers: [gecode]
expected: !FlatZinc aggregation_shared_core.fzn
***/

% Expect one int_lin_eq defining the core sum because reordered, shifted, and
% scaled forms normalise to the same expression.

array [1..4] of var 0..10: a;
var bool: p;
var bool: q;

constraint (a[1] + a[2] + a[3] + a[4] <= 5) \/ p;
constraint (2 * a[4] + 2 * a[3] + 2 * a[2] + 2 * a[1] >= 12) \/ q;
constraint a[4] + a[3] + a[2] + a[1] + 3 != 9;
