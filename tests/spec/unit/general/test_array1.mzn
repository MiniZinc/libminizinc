/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    i: 4
***/

array [1..3] of int: x = [1, 4, 9]; 

var 1..4: i; 

constraint i <= 3 -> x[i] > 10;

solve satisfy;

