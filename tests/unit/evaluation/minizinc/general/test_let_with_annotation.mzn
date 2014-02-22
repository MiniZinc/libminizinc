% RUNS ON mzn20_fd

var 1..10: x;

constraint x =
	let
	{ var int: a::domain }
	in
	a;

solve satisfy;

output ["x = ", show(x), ";\n"];
