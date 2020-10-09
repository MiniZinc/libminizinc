/***
!Test
expected: !Result
  solution: !Solution
    x: [[1, 1], [1, 1]]
    ix: !Range 3..4
***/

array[int, int] of var 1..1: x :: add_to_output = array2d(1..2, 3..4, [1, _, _, _]);
set of int: ix :: add_to_output = index_set_2of2(x);
solve satisfy;