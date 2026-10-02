/***
--- !Test
type: compile
solvers: [gecode]
expected: !FlatZinc aggregation_solve_ann.fzn
***/

% Expect the local sum to keep its variable because a solve annotation cannot
% absorb its terms.

annotation foo(var int: a);

var 0..10: x;
var 0..10: y;

solve :: (let { var 0..20: s = x + y } in foo(s)) satisfy;
