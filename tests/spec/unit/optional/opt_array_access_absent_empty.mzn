/***
!Test
expected: !Result
  solution: !Solution
    x: []
    i: null
    y: null
***/
array [1..0] of var 1..1: x;
var opt 1..1: i;
var opt 1..1: y :: output = x[i];
