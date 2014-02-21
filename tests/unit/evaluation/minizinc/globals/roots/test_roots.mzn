% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_std_fd
% RUNS ON minizinc_cpx

var 1..1: x;	% Just so we have something to print out.

include "roots.mzn";

constraint roots([], {}, {});
constraint roots([1, 2, 3], {}, {});
constraint roots([1, 3, 1, 2, 3], {2, 4, 5}, {2, 3, 8});
constraint roots([1, 1, 1], {1, 2, 3}, {1, 2});

solve satisfy;

output [
   "x = ", show(x), ";\n"
];
