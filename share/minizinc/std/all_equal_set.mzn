include "fzn_all_equal_set.mzn";   
include "fzn_all_equal_set_reif.mzn";   

%-----------------------------------------------------------------------------%
% Constrains the array of objects 'x' to be all equal.
%-----------------------------------------------------------------------------%

predicate all_equal_set(array[int] of var set of int: x) =
	fzn_all_equal_set(x);

predicate all_equal_set_reif(array[int] of var set of int: x, var bool: b) =
	fzn_all_equal_set_reif(x,b);
