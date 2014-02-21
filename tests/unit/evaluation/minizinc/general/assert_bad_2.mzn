% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp
% RUNS ON mzn_cd_fd
% Test assertions that should succeed.

array [1..10] of int: a =
	[i | i in 1..10];

% Function style assertion.
%
test odd(int: x) = (x mod 2 = 1);

array [1..10] of int: b =
	[assert(odd(2 * a[i]), "that's odd...", 2 * a[i]) | i in 1..10];

var 1..10: x;

constraint b[x] = max(b);

solve satisfy;
