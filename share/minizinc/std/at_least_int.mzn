include "fzn_at_least_int.mzn";
include "fzn_at_least_int_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires at least 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate at_least_int(int: n, array[$X] of var $$E: x, $$E: v) =
	fzn_at_least_int(n, array1d(x), v);
