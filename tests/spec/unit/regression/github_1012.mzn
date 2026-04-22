/***
!Test
solvers: [gecode]
options: 
  -O2: true
expected: !Result
  solution: !Solution
    x: [null, null]
***/

tuple(var opt 1..1, var opt 1..1): x;
constraint x = (<>, <>);
