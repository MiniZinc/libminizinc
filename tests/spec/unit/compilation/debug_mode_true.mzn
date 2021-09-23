/***
!Test
solvers: [gecode]
options:
  debug: true
expected: !Result
  solution: !Solution
    b: true
***/

var bool: b;

constraint b = debug_mode;
