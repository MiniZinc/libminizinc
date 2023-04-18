/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc float_inf_range_dom.fzn
***/

% Test whether the usage of a float domain with multiple ranges an infinity is
% translated into constraints
var 1.0..2.0 union 3.0..infinity: x;