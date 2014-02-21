% RUNS ON minizinc
% Test string comparison.
%
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_cd_fd

array [1..6] of var 1..3: xs;

array [1..6] of string: ss = ["a", "b", "c", "a", "b", "c"];

constraint
	forall (i, j in 1..6 where i < j /\ ss[i] < ss[j]) (xs[i] < xs[j]);

solve satisfy;

output ["xs = ", show(xs), ";\n"];
