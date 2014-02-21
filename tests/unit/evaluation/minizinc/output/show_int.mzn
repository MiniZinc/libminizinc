% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

var int: x;
constraint x > 560;
constraint x < 562;

solve satisfy;
output [
	show_int(0, x), "@\n",
	show_int(4, x), "@\n",
	show_int(-4, x), "@\n",
	show_int(4, 1234567), "@\n"
];
