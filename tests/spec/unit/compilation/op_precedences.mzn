/***
!Test
check_against: []
expected:
- !Result
  status: SATISFIED
solvers:
- gecode
type: solve
***/
set of int: x = 1..3;
set of int: y = 2..4;
set of int: z = 3..5;

constraint (x union y intersect z) = (x union (y intersect z));
constraint -2^2 = -4;
