/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: a, b, c, d
***/

string: s = join(", ", ["a", "b", "c", "d"]);
solve satisfy;
output [s];
