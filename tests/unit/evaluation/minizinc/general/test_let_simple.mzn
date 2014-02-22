% RUNS ON mzn20_fd
int: x =
	let {
		int: a = 42
	}
	in a;
solve satisfy;
output ["x = ", show(x), ";\n"];
