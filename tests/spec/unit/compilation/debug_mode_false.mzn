/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    b: false
***/

var bool: b;

constraint b = debug_mode;
