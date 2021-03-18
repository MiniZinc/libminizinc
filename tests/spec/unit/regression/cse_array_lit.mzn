/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc cse_array_lit.fzn
***/

% Regression test for #458

% In 2.5.4, par arrays were not inserted into the CSE, 
% leading to duplicate variables and constraints in the FlatZinc.

var 1..10: x;
var 1..10: y;

constraint x < y;
constraint x < y;
