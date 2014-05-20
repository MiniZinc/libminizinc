% RUNS ON mzn20_fd
predicate p(array [int] of var int: xs) =
	let {
		set of int: ix = index_set(xs),
		array [ix] of var bool: ys
	} in
		ys[1];

solve satisfy;

output ["okay\n"];
