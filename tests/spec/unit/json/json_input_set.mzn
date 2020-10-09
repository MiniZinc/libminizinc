/***
!Test
extra_files:
- json_input_set.json
expected: !Result
  solution: !Solution
    _output_item: 'x = {1,2,3,4,6,9,10,11}, y = 1.0..1.0 union 2.0..4.0 union 6.0..6.0 union 9.0..11.0'
***/

set of int: x;
set of float: y;

output ["x = \(x), y = \(y)"];