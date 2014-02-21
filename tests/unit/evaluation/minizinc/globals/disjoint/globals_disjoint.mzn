% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_fdlp
% RUNS ON minizinc_cpx

include "disjoint.mzn";

%-----------------------------------------------------------------------------%
% disjoint
%-----------------------------------------------------------------------------%
par set of 1..5: disj_s1 = {1,2,3};
var set of 1..5: disj_s2;

constraint disjoint(disj_s1, disj_s2);
constraint card(disj_s2) = 2;

solve satisfy;

output ["disj_s2 = ", show(disj_s2), ";\n"];
