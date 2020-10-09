/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    i: 4
***/

array [1..3] of int: x = [1, 4, 9]; 

var 1..4: i; 

constraint x[i] < 11 -> i > 3;

solve satisfy;
