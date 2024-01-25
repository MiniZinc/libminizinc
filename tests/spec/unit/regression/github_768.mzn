/***
!Test
expected: !FlatZinc 'github_768.fzn'
solvers: [gecode]
type: compile
***/

% Regression test for bug where the search annotation ended up with an array1d call

array [0..0] of var 1..2: x;
ann: s = int_search(x, input_order, indomain_min, complete);
ann: t = warm_start(x, [0: 1]);
ann: u = seq_search([3: s, t]);
solve :: u satisfy;
