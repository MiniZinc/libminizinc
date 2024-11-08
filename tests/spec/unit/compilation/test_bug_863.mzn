/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc test_bug_863.fzn
***/

array[1..2] of var 1..2: x;
var 1..2: y;
constraint x[y] = x[2];
constraint y == x[2];
