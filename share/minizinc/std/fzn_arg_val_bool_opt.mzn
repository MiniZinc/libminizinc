include "fzn_arg_val_int.mzn";

predicate fzn_arg_val_bool_opt(array[int] of var opt bool: x, var opt bool: v, var int: i) =
	fzn_arg_val_int([xi default 2 | xi in x], v default 2, i);
