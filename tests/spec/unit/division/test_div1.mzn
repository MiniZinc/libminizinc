/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: 0
***/

var 0..1: x; 

constraint not (1 div x = 1); 

solve satisfy;