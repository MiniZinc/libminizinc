/***
!Test
expected: !Result
  status: SATISFIED
***/
array[int, int] of int: X = [| 1,2 |];
output show(dom_array(X[.., 1]));
