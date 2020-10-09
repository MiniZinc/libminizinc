/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: a
***/

% Used to fail on Windows when using MiniZinc directly

string: s = "a";
solve satisfy;
output [s];