% RUNS ON minizinc
% RUNS ON mzn_fdlp
% RUNS ON mzn_mer_fdlp
% RUNS ON mzn_cd_fdlp

var 1..10: x;
var 1.0..10.0: y :: lp;

constraint channel_fd_int_to_lp_float(x, y);
constraint channel_lp_float_to_fd_int(y, x);

solve maximize x;

output [
    "x = ", show(x), ";\n",
    "y = ", show(y), ";\n"
];
