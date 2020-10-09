/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: All is well
***/

array [1..2] of string: s = ["All", " is"];
solve satisfy;
output s ++ [" well"];
