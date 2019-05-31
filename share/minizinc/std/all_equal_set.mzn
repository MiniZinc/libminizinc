include "fzn_all_equal_set.mzn";   

%-----------------------------------------------------------------------------%
% Constrains the array of objects 'x' to be all equal.
%-----------------------------------------------------------------------------%

predicate all_equal_set(array[int] of var set of int: x) =
	fzn_all_equal_set(x);
