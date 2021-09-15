/***
!Test
expected:
- !Result
  solution: !Solution
    _output_item: "[| |]"
***/

set of int: Index = {};
array[Index, Index] of int: matrix = [| |];
output [show2d(matrix)];
