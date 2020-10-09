/***
!Test
expected:
- !Result
  solution: !Solution
    inv_f1: [5, 7, 6, 8]
    inv_f2: [1, 3, 2, 4]
- !Result
  solution: !Solution
    inv_f1: [5, 7, 6, 4]
    inv_f2: [1, 3, 2, 5]
- !Result
  solution: !Solution
    inv_f1: [5, 7, 4, 6]
    inv_f2: [1, 4, 2, 5]
- !Result
  solution: !Solution
    inv_f1: [5, 7, 8, 6]
    inv_f2: [1, 4, 2, 3]
- !Result
  solution: !Solution
    inv_f1: [5, 7, 4, 4]
    inv_f2: [1, 5, 2, 5]
- !Result
  solution: !Solution
    inv_f1: [5, 7, 4, 8]
    inv_f2: [1, 5, 2, 4]
- !Result
  solution: !Solution
    inv_f1: [5, 7, 8, 4]
    inv_f2: [1, 5, 2, 3]
***/

include "inverse_in_range.mzn";

%-----------------------------------------------------------------------------%
% inverse
%-----------------------------------------------------------------------------%
array[1..4] of var 4..8: inv_f1 :: add_to_output = [5, 7, _, _];
array[5..8] of var 1..5: inv_f2 :: add_to_output = array1d(5..8, [1, _, 2, _]);

constraint inverse_in_range(inv_f1, inv_f2);

solve satisfy;

output [
    "inv_f1 = array1d(1..4, ", show(inv_f1), ");\n",
    "inv_f2 = array1d(5..8, ", show(inv_f2), ");\n"
];
