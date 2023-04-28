/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc int_inf_dom.fzn
***/

% Test whether the usage of a int domain with single range using infinity is
% translated into a constraint
var int: x;
constraint x >= 0;
