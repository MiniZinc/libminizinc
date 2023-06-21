/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !Solution
    x: !Range 2..3
    y: !Range 4..5
***/

function array[int] of var opt $$E: set2iter(var set of $$E: x) =
  [i + 1 | i in ub(x) where i in x];

var set of 1..4: x;
var set of int: y ::output = { i + 1 | i in x};

constraint x = 2..3;
