include "fzn_arg_max_int.mzn";

predicate maximum_arg_int(array[$$E] of var int: x, var $$E: i) =
	fzn_maximum_arg_int(x, i);
