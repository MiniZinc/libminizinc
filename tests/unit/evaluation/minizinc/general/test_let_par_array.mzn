% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_cd_fd

int: x =
	let {
		array [1..3] of int: a = [1, 2, 3]
	}
	in a[1];
solve satisfy;
output [ "okay\n" ];
