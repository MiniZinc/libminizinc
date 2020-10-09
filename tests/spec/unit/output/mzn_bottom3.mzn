/***
!Test
expected:
- !Result
  solution: !Solution
    _output_item: '1..2'
***/

output [show(index_set_1of2([|1, _ | 1, _|]))];
solve satisfy;
