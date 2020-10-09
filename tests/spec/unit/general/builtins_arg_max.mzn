/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    bool_output: 3
    int_output: 1
    float_output: 2
    indexed_output: 4
    enumed_output: "TWO"
***/

% Base cases
array[int] of int: bool_array = [false, false, true, true];
var int: bool_output ::add_to_output = arg_max(bool_array);

array[int] of int: int_array = [-4,-5,-4];
var int: int_output ::add_to_output = arg_max(int_array);

array[int] of float: float_array = [1.4, 4.3, 2.3, 4.2];
var int: float_output ::add_to_output = arg_max(float_array);

% Test using arrays not starting at 1
array[3..5] of int: indexed_array = array1d(3..5, [4,5,3]);
var int: indexed_output ::add_to_output = arg_max(indexed_array);

% Test using an enum as index
enum TENUM = {ONE, TWO, THREE};
array[TENUM] of bool: enumed_array = [false, true, false];
var TENUM: enumed_output ::add_to_output = arg_max(enumed_array);
