/***
!Test
expected:
- !Result
  solution: !Solution
    x: [null, 6, 8, 3]
- !Result
  solution: !Solution
    x: [null, 3, 8, 6]
***/

include "global_cardinality.mzn";

array[1..4] of var opt 0..8: x;

% Make one of the array elements a value that is not in the cover.
% This is to check that we get the behaviour specified in the global
% constraint catalogue.
constraint x[3] = 8;
constraint absent(x[1]);

constraint
     global_cardinality(
         %[3, 3, 8, 6],
         x,
         [3, 5, 6],
         [1, 0, 1],
         [2, 2, 1]);

solve satisfy;

output ["x = array1d(1..4, ", show(x), ");\n"];
