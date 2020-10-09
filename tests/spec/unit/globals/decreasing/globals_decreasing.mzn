/***
!Test
expected:
- !Result
  solution: !Solution
    inc_avb: [true, false, false, false, false]
    inc_avi: [5, 3, 3, 1, 1]
- !Result
  solution: !Solution
    inc_avb: [true, false, false, false, false]
    inc_avi: [5, 5, 3, 1, 1]
- !Result
  solution: !Solution
    inc_avb: [true, false, false, false, false]
    inc_avi: [5, 5, 3, 3, 1]
***/

include "decreasing.mzn";

%-----------------------------------------------------------------------------%
% decreasing
%-----------------------------------------------------------------------------%

array[1..5] of var -100..100:         inc_avi ::add_to_output = [5, _, 3, _, 1];
array[1..5] of var bool:        inc_avb ::add_to_output = [true, false, _, _, _];
%array[1..8] of var set of 1..3: inc_avsi; % XXX :: is_output

constraint decreasing(inc_avi);
constraint decreasing(inc_avb);
% XXX: comparisons not implemented for sets
%constraint increasing(inc_avsi);

solve satisfy;

output [
    "inc_avb = array1d(1..5, ", show(inc_avb), ");\n",
    "inc_avi = array1d(1..5, ", show(inc_avi), ");\n"
];
