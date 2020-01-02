/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    y: 1
***/

var 0..2: y; 

constraint 2 div y + 2 div (y - 2) == 0 ; 

solve satisfy;