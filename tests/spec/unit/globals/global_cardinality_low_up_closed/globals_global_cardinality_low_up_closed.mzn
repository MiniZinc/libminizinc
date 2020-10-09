/***
!Test
expected:
- !Result
  solution: !Solution
    x: [6, 5, 3, 3]
- !Result
  solution: !Solution
    x: [5, 3, 3, 6]
- !Result
  solution: !Solution
    x: [3, 6, 5, 3]
- !Result
  solution: !Solution
    x: [3, 3, 5, 6]
- !Result
  solution: !Solution
    x: [6, 3, 3, 5]
- !Result
  solution: !Solution
    x: [3, 3, 6, 5]
- !Result
  solution: !Solution
    x: [5, 6, 3, 3]
***/

include "global_cardinality_low_up_closed.mzn";

array[1..4] of var 0..8: x;

constraint
     global_cardinality_low_up_closed(
         %[3, 3, 5, 6],
         x,
         [3, 5, 6],
         [2, 1, 1],
         [3, 2, 1]);

solve satisfy;

output ["x = array1d(1..4, ", show(x), ");\n"];
