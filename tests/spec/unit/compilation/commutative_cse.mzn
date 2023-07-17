/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc commutative_cse.fzn
***/
var 1..5: x;
var 1..5: y;
var 1..5: z;
constraint max(x,y) <= max(y,x);
constraint min([x,y,z]) <= min([z,y,x]);
