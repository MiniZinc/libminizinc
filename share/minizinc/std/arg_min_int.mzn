include "fzn_arg_min_int.mzn";

predicate minimum_arg_int(array[$$E] of var int: x, var $$E: i) =
	fzn_minimum_arg_int(x, i);
