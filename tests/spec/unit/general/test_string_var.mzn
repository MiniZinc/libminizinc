/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: All is well
***/

string: s = "All is well";
solve satisfy;
output [s];
