/***
!Test
expected:
- !Result
  solution: !Solution
    _output_item: '3'
***/

var -10..10: x = length([1, _, _]);
solve satisfy;
output([show(x)]);
