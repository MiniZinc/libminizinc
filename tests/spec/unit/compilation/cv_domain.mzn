/***
!Test
expected: !Result
  solution: !Solution
    xs: [3, 3, 3, 3, 3]
    y: 15
***/
include "increasing.mzn";

function int: partial_bound(array[int] of var int: xs) = let {
  constraint increasing(xs);
} in 5;

array[1..5] of var 3..7: xs;
var partial_bound(xs)..15: y;
constraint sum(xs) = y;
