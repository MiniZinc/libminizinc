include "fzn_at_most_set.mzn";
include "fzn_at_most_set_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires at most 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate at_most_set(int: n, array[$X] of var set of $$E: x, set of $$E: v) =
	fzn_at_most_set(n, array1d(x), v);
