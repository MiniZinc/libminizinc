% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear

include "all_disjoint.mzn";

%-----------------------------------------------------------------------------%
% all_disjoint
%-----------------------------------------------------------------------------%
array[0..3] of var set of 1..8: alldisj_avsi;

constraint forall(i in 0..3) ( card(alldisj_avsi[i]) > 1 );
constraint all_disjoint(alldisj_avsi);
constraint 8 in alldisj_avsi[2];

solve satisfy;

output ["alldisj_avsi = array1d(0..3, ", show(alldisj_avsi), ");\n"];
