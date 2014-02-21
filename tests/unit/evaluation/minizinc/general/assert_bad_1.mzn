% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp
% RUNS ON mzn_cd_fd
% Test assertions that should succeed.

array [1..10] of int: a =
	[i | i in 1..10];

% Predicate style assertion.
%
constraint assert(forall (i in 1..9) (a[i] > a[i + 1]), "a not decreasing");

var 1..10: x;

constraint a[x] = max(a);

solve satisfy;

output ["x = ", show(x), ";\n"];
