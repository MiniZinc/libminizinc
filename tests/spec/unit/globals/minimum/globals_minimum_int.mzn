/***
!Test
expected:
- !Result
  solution: !Solution
    min_vi1: 0
    min_vi2: 0
***/

include "minimum.mzn";
include "maximum.mzn";

%-----------------------------------------------------------------------------%
% minimum
%-----------------------------------------------------------------------------%
var 0..20: min_vi1 ::add_to_output;
var 0..20: min_vi2 ::add_to_output;

constraint minimum(min_vi1, [5, 3, 8, 0,       4]);
constraint minimum(0,       [5, 3, 8, min_vi2, 4]);

solve satisfy;

output [
    "min_vi1 = ", show(min_vi1), ";\n",
    "min_vi2 = ", show(min_vi2), ";\n"
];
