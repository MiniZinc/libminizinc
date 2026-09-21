/***
--- !Test
type: compile
solvers: [gecode]
expected: !FlatZinc aggregation_single_use_fold.fzn
***/

% Expect one direct int_lin_le with no sum variable because the named sum has
% only one reader and is inlined.

array [1..4] of var 0..10: a;
var 0..10: w;

constraint let { var int: t = sum(a) } in t + w <= 5;
