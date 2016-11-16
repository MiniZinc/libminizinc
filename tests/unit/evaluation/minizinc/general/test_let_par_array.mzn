% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip

int: x =
	let {
		array [1..3] of int: a = [1, 2, 3]
	}
	in a[1];
solve satisfy;
output [ "okay\n" ];
