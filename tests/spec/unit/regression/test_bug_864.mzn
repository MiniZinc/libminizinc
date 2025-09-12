/***
!Test
expected: !Result
  status: SATISFIED
***/
array[1..2] of var opt 0..10: y;
constraint forall (j in 1..2) (y[j] == if j == 1 then <> else let { var 0..10: x} in x endif);
