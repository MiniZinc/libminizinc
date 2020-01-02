/***
!Test
expected:
- !Result
  solution: !Solution
    inc_avb: [false, true, true, true, true]
    inc_avi: [1, 1, 3, 3, 5]
- !Result
  solution: !Solution
    inc_avb: [false, true, true, true, true]
    inc_avi: [1, 1, 3, 5, 5]
- !Result
  solution: !Solution
    inc_avb: [false, true, true, true, true]
    inc_avi: [1, 3, 3, 3, 5]
- !Result
  solution: !Solution
    inc_avb: [false, true, true, true, true]
    inc_avi: [1, 3, 3, 5, 5]
***/

include "increasing.mzn";

%-----------------------------------------------------------------------------%
% increasing
%-----------------------------------------------------------------------------%
array[1..5] of var -100..100:   inc_avi ::add_to_output = [1, _, 3, _, 5];
array[1..5] of var bool:        inc_avb ::add_to_output = [false, true, _, _, _];
%array[1..8] of var set of 1..3: inc_avsi; % XXX :: is_output

constraint increasing(inc_avi);
constraint increasing(inc_avb);
% XXX: comparisons not implemented for sets
%constraint increasing(inc_avsi);

solve satisfy;

output [
   "inc_avb = array1d(1..5, ", show(inc_avb), ");\n",
   "inc_avi = array1d(1..5, ", show(inc_avi), ");\n"
];
