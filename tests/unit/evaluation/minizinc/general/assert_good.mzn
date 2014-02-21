% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp
% RUNS ON mzn_cd_fd
% Test assertions that should succeed.

array [1..10] of int: a =
	[i | i in 1..10];

% Predicate style assertion.
%
constraint assert(forall (i in 1..9) (a[i] < a[i + 1]), "a not increasing");

% Function style assertion.
%
test even(int: x) = (x mod 2 = 0);

array [1..10] of int: b =
	[assert(even(2 * a[i]), "that's odd...", 2 * a[i]) | i in 1..10];

var 1..10: x;

constraint b[x] = max(b);

solve satisfy;

output ["x = ", show(x), "\n"];
