/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: 1
***/

var 0..1: x; 

constraint not (not (1 div x = 1)); 

solve satisfy;
