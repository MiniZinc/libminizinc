/***
!Test
expected:
- !Result
  solution: !Solution
    queens: [4, 6, 1, 5, 2, 8, 3, 7]
- !Result
  solution: !Solution
    queens: [4, 6, 8, 3, 1, 7, 5, 2]
- !Result
  solution: !Solution
    queens: [8, 4, 1, 3, 6, 2, 7, 5]
- !Result
  solution: !Solution
    queens: [2, 5, 7, 4, 1, 8, 6, 3]
***/

% Regression test for bug #269: the assignment to the annotation
% variable "var_selection" (marked with ** below) was being emitted
% in the .ozn file leading to an abort in solns2out.

include "globals.mzn";
int: n;
array[1..n] of var 1..n: queens;

ann: var_selection;
solve :: int_search(
   queens,
   var_selection,
   indomain_median,
   complete)
  satisfy;

constraint all_different(queens);
constraint all_different([queens[i]+i | i in 1..n]);
constraint all_different([queens[i]-i | i in 1..n]);
output ["Ok\n"];

n = 8;
var_selection = first_fail;	% (**)
