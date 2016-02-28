% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip

var 1..10: x;

constraint x =
	let
	{ var int: a::domain }
	in
	a;

solve satisfy;

output ["x = ", show(x), ";\n"];
