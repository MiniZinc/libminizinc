% RUNS ON mzn_mer_fd

% Regression test for a bug in mzn2fzn version 1.2 (and up to
% r13389 on the trunk).  The following model used to result in
% variable declaration with a self-assignment being produced
% by mzn2fzn.

array[1..3] of var set of {1,2,3}: partitions;

constraint array_union(partitions) == array_union(partitions);

solve satisfy;

output ["partitions = ", show(partitions), ";\n"];
