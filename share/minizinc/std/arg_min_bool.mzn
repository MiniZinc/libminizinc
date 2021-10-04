include "fzn_arg_min_bool.mzn";

predicate minimum_arg_bool(array[$$E] of var bool: x, var $$E: i) =
	fzn_minimum_arg_bool(x, i);
