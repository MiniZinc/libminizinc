include "fzn_arg_sort_int.mzn";
include "fzn_arg_sort_int_reif.mzn";

predicate arg_sort_int(
	array[$$E] of var int: x,
	array[int] of var $$E: p,
) =
	assert(
		index_set(p) = 1..length(x),
		"arg_sort_int: second argument must have index 1..length(first argument)",
		fzn_arg_sort_int(x, p)
	);

predicate arg_sort_int(
	array[$$E] of var int: x,
	array[int] of var $$E: p,
	var bool:b
) = b <-> fzn_arg_sort_int(x, p);
