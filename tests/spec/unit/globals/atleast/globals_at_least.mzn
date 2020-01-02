/***
!Test
expected:
- !Result
  solution: !Solution
    atlm_avi: [4, 5, 5, 6, 6, 6, 6, 6, 8, 8]
    exact_avi: [4, 5, 5, 6, 6, 6, 6, 6, 8, 8]
***/

include "at_least.mzn";
include "at_most.mzn";
include "exactly.mzn";

%-----------------------------------------------------------------------------%
% at_least, at_most, exactly
%-----------------------------------------------------------------------------%
array[1..10] of var -100..100: atlm_avi ::add_to_output=
	[4, 5, 5, 6, 6, 6, _, _, 8, 8];
array[1..10] of var -100..100: exact_avi ::add_to_output =
	[4, 5, 5, 6, 6, 6, _, _, 8, 8];

constraint at_least(5, atlm_avi, 6);
constraint at_most (5, atlm_avi, 6);

constraint exactly(5, exact_avi, 6);

solve satisfy;

output [
   "atlm_avi = array1d(1..10, ", show(atlm_avi), ");\n", 
   "exact_avi = array1d(1..10, ", show(exact_avi), ");\n"
];
