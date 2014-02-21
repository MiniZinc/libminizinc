% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
int: x =
	let {
		int: a = 42
	}
	in a;
solve satisfy;
output ["x = ", show(x), ";\n"];
