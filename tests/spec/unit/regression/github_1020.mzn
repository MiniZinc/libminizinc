/***
!Test
solvers: [highs]
options:
  -O1: true
expected: !Result
  status: SATISFIED
***/

var 0.0..1000.0: x;
var 0.0..1000.0: y;
constraint x = 100.0;
constraint let {
  array[0..1] of var bool: a;
  constraint a[0] -> y <= x;
  constraint a[1] -> y <= (0.79 * x);
} in sum(i in 0..1)(a[i]) = 1;
solve satisfy;
