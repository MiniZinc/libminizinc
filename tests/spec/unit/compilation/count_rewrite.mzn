/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc count_rewrite.fzn
***/
include "count.mzn";

array [1..3] of var 1..10: x;
var 1..10: y;
var 1..3: c;

constraint count(x, y) >= c;