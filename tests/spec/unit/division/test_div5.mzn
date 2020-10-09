/***
!Test
expected: !Result
  status: UNSATISFIABLE
***/

var 0..1: x; 

constraint 1 div x != 1; 

solve satisfy;

