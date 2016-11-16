% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip

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
