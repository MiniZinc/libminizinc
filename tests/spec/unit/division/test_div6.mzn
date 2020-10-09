/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    y: 0
***/

var 0..1: y; 

constraint y < 1 \/ not (1 div y = 1); 

solve satisfy;
