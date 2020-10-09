/***
!Test
expected:
- !Result
  solution: !Solution
    x: [5, 5, 5]
***/

include "roots.mzn";

array[0..2] of var 0..1000: x ::add_to_output;

constraint roots(x, {0, 1, 2}, {5});

solve satisfy;

output [
   "x = array1d(0..2, ", show(x), ");\n"
];
