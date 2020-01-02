/***
!Test
expected:
- !Result
  solution: !Solution
    x: [1, 1, 1]
    a: 1
    y: [1, 1, 1]
    b: 1
- !Result
  solution: !Solution
    x: [3, 3, 3]
    a: 9
    y: [3, 3, 3]
    b: 9
***/

include "test_bug218_inc.mzn";
var 1..ub(sum(x)): a;
array[1..3] of var 1..3: y;
solve satisfy;
