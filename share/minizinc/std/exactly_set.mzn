include "fzn_exactly_set.mzn";
include "fzn_exactly_set_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires exactly 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate exactly_set(int: n, array[$X] of var set of $$E: x, set of $$E: v) =
	fzn_exactly_set(n, array1d(x), v);
