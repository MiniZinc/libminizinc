/***
!Test
expected: !FlatZinc 'github_768b.fzn'
solvers: [gecode]
type: compile
***/

% Regression test for bug where the search annotation ended up with an int_eq call

var 1..3: x;
ann: s = bool_search([x = 1], input_order, indomain_max);
solve :: s minimize x;
