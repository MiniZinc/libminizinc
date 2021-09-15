/***
!Test
expected:
- !Result
  solution: !Solution
    _output_item: "[| | | |]"
***/

set of int: Index = {};
array[Index, Index, Index] of int: matrix = [| | | |];
output [show3d(matrix)];
