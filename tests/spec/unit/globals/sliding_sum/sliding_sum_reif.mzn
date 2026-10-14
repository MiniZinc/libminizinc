/***
!Test
expected:
- !Result
  solution: !Solution
    b1: true
    b2: false
***/

include "sliding_sum.mzn";

array [1..4] of var 0..1: x;
array [1..4] of var 0..1: y;
var bool: b1 ::output = sliding_sum(0, 1, 2, x);
var bool: b2 ::output = sliding_sum(1, 1, 2, y);
constraint x = [1, 0, 1, 0];
constraint y = [1, 1, 0, 0];
