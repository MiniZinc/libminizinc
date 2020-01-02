/***
!Test
expected:
- !Result
  solution: !Solution
    x: [1, 1, 1]
- !Result
  solution: !Solution
    x: [1, 1, 4]
- !Result
  solution: !Solution
    x: [1, 4, 1]
- !Result
  solution: !Solution
    x: [1, 4, 4]
- !Result
  solution: !Solution
    x: [4, 1, 1]
- !Result
  solution: !Solution
    x: [4, 1, 4]
- !Result
  solution: !Solution
    x: [4, 4, 1]
- !Result
  solution: !Solution
    x: [4, 4, 4]
***/

% At for value_precede_ch variables.

include "value_precede_chain.mzn";

array[1..3] of var 1..4: x;

constraint value_precede_chain([3, 2, 1], [3, 2, 1]);
constraint value_precede_chain([5, 3, 2], x);
solve satisfy;
output ["x = array1d(1..3, ", show(x), ");\n"];
