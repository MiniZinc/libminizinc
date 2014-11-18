% Test string comparison.
%
% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear

array [1..6] of var 1..3: xs;

array [1..6] of string: ss = ["a", "b", "c", "a", "b", "c"];

constraint
	forall (i, j in 1..6 where i < j /\ ss[i] < ss[j]) (xs[i] < xs[j]);

solve satisfy;

output ["xs = ", show(xs), ";\n"];
