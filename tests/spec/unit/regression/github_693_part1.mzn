/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    c: true
***/
var bool: c;
var set of int: x = if c then 1..3 else 1..5 endif;
var set of int: y = if not c then 1..3 else 1..5 endif;

constraint c = true;

constraint x subset y;
