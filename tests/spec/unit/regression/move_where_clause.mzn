/***
!Test
expected: !Result
  status: SATISFIED
***/

array[1..4] of bool: t = [true, false, true, false];
array[int] of int: x = [ i+j | i in 1..4 where t[i], j in 1..4];
constraint sum(x) = 36;
