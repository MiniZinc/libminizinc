include "fzn_arg_max_bool.mzn";

predicate fzn_arg_val_float(array[int] of var float: x, var float: v, var int: i) =
	fzn_maximum_arg_bool([j: x[j] == v | j in index_set(x)], i)
	/\ x[i] = v;
