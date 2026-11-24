/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc github_1115.fzn
***/

% The objective must stay defined by the linear equality, also when the sum
% is first named by an introduced variable that is later inlined.

array [1..3] of var 0..5: x;
var int: objective;
constraint objective = sum(i in 1..3)(2 * x[i]);
solve minimize objective;
