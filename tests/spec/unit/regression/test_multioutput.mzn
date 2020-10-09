/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: |
      one
      two
***/

output ["one\n"];
output ["two"];
solve satisfy;
