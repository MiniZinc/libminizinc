/***
!Test
expected: !Result
  solution: !Solution
    x: [null, null, null]
***/

var set of 1..3: s;
array [int] of var opt int: x :: output = [<> | i in s];
