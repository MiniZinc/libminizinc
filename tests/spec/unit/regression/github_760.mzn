/***
!Test
expected: 
- !Result
  solution: !Solution
    x: 1
- !Result
  solution: !Solution
    x: 2
solvers: [gecode]
***/
var 1..2: x;
constraint ( ( (x - 1 mod 0) = 1) <-> false);
