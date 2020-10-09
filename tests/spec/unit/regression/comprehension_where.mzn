/***
!Test
expected: !Result
  status: SATISFIED
***/

% Used to give UNSATISFIABLE due to introducing a call for the array comprehension.

array [1..1] of var 0..1: x;
array [1..1] of var 1..1: y;

constraint absent(z[1]);

array [1..1] of var opt 1..1: z = [y[x[i]] | i in 1..1 where x[i] != 0];
