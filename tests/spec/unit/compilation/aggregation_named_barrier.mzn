/***
--- !Test
type: compile
solvers: [gecode]
expected: !FlatZinc aggregation_named_barrier.fzn
***/

% Expect one int_lin_eq defining the sum and scalar readers because the sum is
% used by multiple constraints and therefore keeps its name.

array [1..4] of var 0..10: a;

constraint let { var int: x = sum(a) } in (x < 5 \/ x > 10) /\ x != 7;
