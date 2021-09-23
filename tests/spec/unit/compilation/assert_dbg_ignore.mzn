/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: 1 
***/
var 1..10: x;
constraint assert_dbg(false, "This should be ignored") /\ x = 1;
