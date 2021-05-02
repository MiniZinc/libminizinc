/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc implied_hr.fzn
***/

var int: x;
var int: y;

constraint implied_constraint(x >= 5 \/ y > 4);
