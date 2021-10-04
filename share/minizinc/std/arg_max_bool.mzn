include "fzn_arg_max_bool.mzn";

predicate maximum_arg_bool(array[$$E] of var bool: x, var $$E: i) =
	fzn_maximum_arg_bool(x, i);
