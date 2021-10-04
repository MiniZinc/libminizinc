include "fzn_arg_max_float.mzn";

predicate maximum_arg_float(array[$$E] of var float: x, var $$E: i) =
	fzn_maximum_arg_float(x, i);
