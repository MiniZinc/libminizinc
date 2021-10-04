include "fzn_decreasing_int.mzn";
include "fzn_decreasing_int_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in decreasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate decreasing_int(array[$X] of var int: x) =
	fzn_decreasing_int(array1d(x)); 

