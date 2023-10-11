/***
!Test
expected: !Result
  solution: !Solution
    x: [null]
***/

any: x :: output = [if false then "a" else <> endif | i in 1..1];
