include "fzn_arg_max_bool.mzn";
include "fzn_arg_min_bool.mzn";

predicate fzn_arg_val_bool(array[int] of var bool: x, var bool: v, var int: i) =
	if v then
		fzn_maximum_arg_bool(x, i)
	else
		fzn_minimum_arg_bool(x, i)
	endif
	/\ x[i] = v;
