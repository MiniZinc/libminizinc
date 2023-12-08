/***
--- !Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: [true]
--- !Test
solvers: [gecode]
options:
  -O0: true
expected: !Result
  solution: !Solution
    x: [true]
***/
array [1..1] of var bool: x;
constraint x = [true];
constraint x[1];
