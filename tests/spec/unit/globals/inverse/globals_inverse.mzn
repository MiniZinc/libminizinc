/***
!Test
expected:
- !Result
  solution: !Solution
    inv_f1: [5, 7, 6, 8]
    inv_f2: [1, 3, 2, 4]
- !Result
  solution: !Solution
    inv_f1: [5, 7, 8, 6]
    inv_f2: [1, 4, 2, 3]
***/

include "inverse.mzn";

%-----------------------------------------------------------------------------%
% inverse
%-----------------------------------------------------------------------------%
array[1..4] of var -100..100: inv_f1 :: add_to_output = [5, 7, _, _];
array[5..8] of var -100..100: inv_f2 :: add_to_output = array1d(5..8, [1, _, 2, _]);

constraint inverse(inv_f1, inv_f2);

solve satisfy;

output [
    "inv_f1 = array1d(1..4, ", show(inv_f1), ");\n",
    "inv_f2 = array1d(5..8, ", show(inv_f2), ");\n"
];
