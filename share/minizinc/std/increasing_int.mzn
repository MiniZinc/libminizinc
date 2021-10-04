include "fzn_increasing_int.mzn";
include "fzn_increasing_int_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in increasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate increasing_int(array[$X] of var int: x) =
	fzn_increasing_int(array1d(x));
