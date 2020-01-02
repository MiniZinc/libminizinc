/***
!Test
expected:
- !Result
  solution: !Solution
    among_avi1: [4, 5, 5, 6, 6, 6, 7, 7, 7, 7]
    among_avi2: [4, 5, 5, 6, 6, 6, 7, 7, 8, 8]
    among_vi: 7
***/

include "among.mzn";

%-----------------------------------------------------------------------------%
% among
%-----------------------------------------------------------------------------%
var -100..100: among_vi ::add_to_output;
array[1..10] of var -100..100: among_avi1 ::add_to_output =
	[4, 5, 5, 6, 6, 6, 7, 7, 7, 7];
array[1..10] of var -100..100: among_avi2 ::add_to_output =
	[4, 5, 5, 6, 6, 6, _, _, 8, 8];

% Testing it three ways, with different args unfixed each time.
constraint among(among_vi, among_avi1, {6,7});   % 1st arg unfixed -> 7
constraint among(5,        among_avi2, {6,7});   % 2nd arg unfixed -> [...]
constraint forall(i in 7..10) ( among_avi2[i] != 6 );

solve satisfy;

output [
   "among_avi1 = array1d(1..10, ", show(among_avi1), ");\n",
   "among_avi2 = array1d(1..10, ", show(among_avi2), ");\n",
   "among_vi = ", show(among_vi), ";\n"
];
