/***
!Test
expected:
- !Result
  solution: !Solution
    max_vi1: 9
    max_vi2: 9
***/

include "maximum.mzn";

%-----------------------------------------------------------------------------%
% maximum
%-----------------------------------------------------------------------------%

var 0..20: max_vi1 ::add_to_output;
var 0..20: max_vi2 ::add_to_output;

constraint maximum(max_vi1, [5, 3, 8, 9,       4]);
constraint maximum(9,       [5, 3, 8, max_vi2, 4]);

solve satisfy;

output [
    "max_vi1 = ", show(max_vi1), ";\n",
    "max_vi2 = ", show(max_vi2), ";\n"
];
