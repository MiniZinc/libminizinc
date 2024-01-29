/***
!Test
expected: !FlatZinc 'github_773.fzn'
solvers: [gecode]
type: compile
***/

% Regression test bug where substituting fixed variables removed the `objective`
% declaration but left it in the solve item.
var 1..3: objective;
solve :: int_search([objective], input_order, indomain_min, complete) minimize objective;
constraint objective = 2;
