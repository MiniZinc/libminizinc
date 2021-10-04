include "fzn_decreasing_set.mzn";
include "fzn_decreasing_set_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in decreasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate decreasing_set(array[$X] of var set of int: x) =
	fzn_decreasing_set(array1d(x));

