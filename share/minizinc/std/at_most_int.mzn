include "fzn_at_most_int.mzn";
include "fzn_at_most_int_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires at most 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate at_most_int(int: n, array[$X] of var $$E: x, $$E: v) =
	fzn_at_most_int(n, array1d(x), v);
