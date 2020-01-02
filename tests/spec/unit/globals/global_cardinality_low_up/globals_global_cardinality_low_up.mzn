/***
!Test
expected:
- !Result
  solution: !Solution
    x: [6, 3, 8, 3]
- !Result
  solution: !Solution
    x: [3, 6, 8, 3]
- !Result
  solution: !Solution
    x: [3, 3, 8, 6]
***/

include "global_cardinality_low_up.mzn";

array[1..4] of var 0..8: x;

% Make one of the array elements a value that is not in the cover.
% This is to check that we get the behaviour specified in the global
% constraint catalogue.
constraint x[3] = 8;

constraint
     global_cardinality_low_up(
         %[3, 3, 8, 6],
         x,
         [3, 5, 6],
         [2, 0, 1],
         [3, 2, 1]);

solve satisfy;

output ["x = array1d(1..4, ", show(x), ");\n"];
