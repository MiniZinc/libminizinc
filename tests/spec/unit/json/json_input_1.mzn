/***
!Test
extra_files:
- json_input_1.json
expected: !Result
  solution: !Solution
    int_set: !!set {1, 2, 4, 5, 7, 9}
    float_set: !!set {1.0, 2.0}
    array_1d_float: [1.0, 2.0, 3.0]
    array_2d_bool: [[true]]
    array_3d_int: [[[1, 2], [3, 4]], [[5, 6], [7, 8]]]
    array_opt_float: [1.5, null]
    x: 1
    y: 2.0
    z: true
    o: null
***/

set of int: int_set :: add_to_output;
set of float: float_set :: add_to_output;

array [1..3] of float: array_1d_float :: add_to_output;
array [1..1, 1..1] of bool: array_2d_bool :: add_to_output;
array [1..2, 1..2, 1..2] of int: array_3d_int :: add_to_output;

array [1..2] of opt float: array_opt_float :: add_to_output;

int: x :: add_to_output;
float: y :: add_to_output;
bool: z :: add_to_output;

opt int: o :: add_to_output;