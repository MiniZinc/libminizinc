include "fzn_arg_min_float.mzn";

predicate minimum_arg_float(array[$$E] of var float: x, var $$E: i) =
	fzn_minimum_arg_float(x, i);
