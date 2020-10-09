/***
!Test
expected:
- !Result
  status: UNSATISFIABLE
- !Error
***/

var int: x;

constraint x = min (i in 3..2) ([1, 2, 3][i]);

solve satisfy;
