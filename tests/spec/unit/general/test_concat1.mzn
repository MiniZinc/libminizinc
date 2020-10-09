/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: '"abcdefghi"'
***/

string: s = concat(["abc", "def", "ghi"]);
solve satisfy;
output [show(s)];
