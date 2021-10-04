include "fzn_increasing_float.mzn";
include "fzn_increasing_float_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in increasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate increasing_float(array[$X] of var float: x) =
	fzn_increasing_float(array1d(x));
