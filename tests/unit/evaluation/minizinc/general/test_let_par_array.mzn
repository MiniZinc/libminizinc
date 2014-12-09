% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear

int: x =
	let {
		array [1..3] of int: a = [1, 2, 3]
	}
	in a[1];
solve satisfy;
output [ "okay\n" ];
