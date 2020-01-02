/***
!Test
expected:
- !Result
  solution: !Solution
    dist_base: [6, 7, 6, 8, 6, 9, 6]
    dist_card: [4, 1, 1, 1]
    dist_value: [6, 7, 8, 9]
***/

include "distribute.mzn";

%-----------------------------------------------------------------------------%
% distribute
%-----------------------------------------------------------------------------%
array[1..4] of var -100..100: dist_card ::add_to_output  = [4, _, 1, _];
array[1..4] of var -100..100: dist_value ::add_to_output = [_, 7, 8, 9];
array[1..7] of var -100..100: dist_base ::add_to_output = [_, 7, 6, 8, 6, 9, _];

constraint distribute(dist_card, dist_value, dist_base);

solve satisfy;

output [
   "dist_base = array1d(1..7, ", show(dist_base), ");\n",
   "dist_card = array1d(1..4, ", show(dist_card), ");\n",
   "dist_value = array1d(1..4, ", show(dist_value), ");\n"
];
