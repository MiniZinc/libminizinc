/***
!Test
expected:
- !Result
  solution: !Solution
    gcc_c: [5, 1, 2, 1]
    gcc_x: [6, 7, 6, 8, 6, 9, 6, 8, 6]
***/

include "global_cardinality.mzn";

% The example from the GCC.
%
constraint global_cardinality([3, 3, 8, 6], [3, 5, 6], [2, 0, 1]);

array[1..9] of var -100..100: gcc_x ::add_to_output = [6, 7, _, 8, _, 9, _, 8, 6];
array[1..4] of var -100..100: gcc_c ::add_to_output = [5, 1, 2, _];

constraint global_cardinality(gcc_x, [6, 7, 8, 9], gcc_c);

solve satisfy;

output [
   "gcc_c = array1d(1..4, ", show(gcc_c), ");\n",
   "gcc_x = array1d(1..9, ", show(gcc_x), ");\n"
];
