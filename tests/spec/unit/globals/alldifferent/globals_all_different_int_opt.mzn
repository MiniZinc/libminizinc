/***
!Test
expected:
- !Result
  solution: !Solution
    alldiff_avi1: [1, 2, 3, 4, 5, null]
- !Result
  solution: !Solution
    alldiff_avi1: [1, null, 3, 4, 5, null]
- !Result
  solution: !Solution
    alldiff_avi1: [1, 2, 3, null, 5, null]
- !Result
  solution: !Solution
    alldiff_avi1: [1, null, 3, null, 5, null]
***/

% Test all_different for var int.

include "all_different.mzn";
array[5..10] of var opt 1..5:  alldiff_avi1 ::add_to_output =
	array1d(5..10, [1, _, 3, _, 5, <>]);

constraint all_different(alldiff_avi1);
constraint alldiff_avi1[6] < alldiff_avi1[7];
constraint alldiff_avi1[7] < alldiff_avi1[8];

solve satisfy;

output ["alldiff_avi1 = array1d(5..9, ", show(alldiff_avi1), ");\n"];

