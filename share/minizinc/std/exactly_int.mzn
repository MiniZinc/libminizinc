include "fzn_exactly_int.mzn";
include "fzn_exactly_int_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires exactly 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate exactly_int(int: n, array[$X] of var $$E: x, $$E: v) =
	fzn_exactly_int(n, array1d(x), v);
