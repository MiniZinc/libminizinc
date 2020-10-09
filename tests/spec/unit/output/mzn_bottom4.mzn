/***
!Test
expected:
- !Result
  solution: !Solution
    _output_item: '0'
***/

int: x = length([]) + length([]);
solve satisfy;
output([show(x)]);
