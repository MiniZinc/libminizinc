include "fzn_decreasing_float.mzn";
include "fzn_decreasing_float_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in decreasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate decreasing_float(array[$X] of var float: x) =
	fzn_decreasing_float(array1d(x));
