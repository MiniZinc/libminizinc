/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    y: 1
***/

var 0..1: y; 

constraint not(y < 1 \/ 1 div y != 1); 

solve satisfy;
