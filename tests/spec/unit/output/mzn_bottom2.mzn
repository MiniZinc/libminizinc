/***
!Test
expected:
- !Result
  solution: !Solution
    _output_item: '1..3'
***/

output [show(index_set([1, _, _]))];
solve satisfy;
