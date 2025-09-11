/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/

function set of $T: 'union'(set of $T:x, set of $T: y);
any: x = {1,2,3} union {};
