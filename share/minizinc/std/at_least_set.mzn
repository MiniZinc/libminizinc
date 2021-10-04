include "fzn_at_least_set.mzn";
include "fzn_at_least_set_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires at least 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate at_least_set(int: n, array[$X] of var set of $$E: x, set of $$E: v) =
	fzn_at_least_set(n, array1d(x), v);
