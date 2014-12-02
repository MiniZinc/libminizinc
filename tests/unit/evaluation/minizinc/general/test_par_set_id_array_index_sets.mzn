% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
predicate p(array [int] of var int: xs) =
	let {
		set of int: ix = index_set(xs),
		array [ix] of var bool: ys
	} in
		ys[1];

solve satisfy;

output ["okay\n"];
