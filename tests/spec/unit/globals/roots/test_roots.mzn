/***
!Test
expected:
- !Result
  solution: !Solution
    x: 1
***/

var 1..1: x ::add_to_output;	% Just so we have something to print out.

include "roots.mzn";

constraint roots([], {}, {});
constraint roots([1, 2, 3], {}, {});
constraint roots([1, 3, 1, 2, 3], {2, 4, 5}, {2, 3, 8});
constraint roots([1, 1, 1], {1, 2, 3}, {1, 2});

solve satisfy;

output [
   "x = ", show(x), ";\n"
];
