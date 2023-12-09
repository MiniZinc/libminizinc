/***
!Test
expected: 
- !Result
  solution: !Solution
    x: [true]
- !Result
  solution: !Solution
    x: [false]
solvers: [chuffed]
***/
include "increasing.mzn";
array[1..1] of var bool: x;
constraint increasing(x[1..0]);
