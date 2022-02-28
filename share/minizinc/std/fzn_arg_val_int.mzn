include "fzn_arg_max_bool.mzn";

predicate fzn_arg_val_int(array[int] of var int: x, var int: v, var int: i) =
	fzn_maximum_arg_bool([j: x[j] == v | j in index_set(x)], i)
	/\ x[i] = v;
