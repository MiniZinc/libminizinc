% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_fdlp
% RUNS ON minizinc_cpx

include "partition_set.mzn";

%-----------------------------------------------------------------------------%
% partition_set
%-----------------------------------------------------------------------------%
array[1..4] of var set of 1..10: partset_avsi =
	[{1,3,5}, _, {}, {4,9}];

constraint partition_set(partset_avsi, 1..10);

solve satisfy;

output ["partset_avsi = array1d(1..4, ", show(partset_avsi), ");\n"];
