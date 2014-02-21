% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

float: f = 123.45679123;
solve satisfy;
output [
	show_float(8, 2, f), "@\n",
	show_float(-8, 2, f), "@\n",
	show_float(-8, 0, f), "@\n",
];
