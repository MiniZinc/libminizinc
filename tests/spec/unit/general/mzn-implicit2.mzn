/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    ia: !Range 1..3
    ib: !Range 2..5
***/

array[int] of int: a = [1, 2, 3];
set of int: ia :: add_to_output = index_set(a);

array[int] of int: b = array1d(2..5, [1, 2, 3, 4]);
set of int: ib :: add_to_output = index_set(b);

solve satisfy;
