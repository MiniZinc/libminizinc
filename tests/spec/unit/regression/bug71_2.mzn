/***
!Test
expected:
- !Result
  solution: !Solution
    x: 1
***/

var int: x;

constraint (x = min (i in 3..2) ([1, 2, 3][i]) \/ x = 1);

solve satisfy;
