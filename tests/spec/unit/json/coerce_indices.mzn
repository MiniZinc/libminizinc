/***
!Test
extra_files:
- coerce_indices.json
expected: !Result
  solution: !Solution
  offset1d: [-10,-9,-8]
  offset2d: [[1, 2], [3, 4]]
  coerce1d: [[[1, 2, 3], [4, 5, 6]]]
  singleUndef: [1, 2, 3]
  coerceCalc: [[1, 2], [3, 4]]
  ndimEmpty: []
***/


array[-10..-8] of int: offset1d ::add_to_output;
array[4..5, 0..1] of int: offset2d ::add_to_output;

array[1..1, 1..2, 1..3] of int: coerce1d ::add_to_output;
array[2..3, int] of int: coerceCalc ::add_to_output;

array[int] of int: singleUndef ::add_to_output;
array[int, 1..2] of int: ndimEmpty ::add_to_output;

