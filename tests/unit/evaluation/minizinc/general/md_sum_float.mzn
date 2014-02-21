% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

% Test sum/1 for floats with multi-dimensional arrays.

float : a = sum([|1.0, 2.0, 3.0, 4.0, 5.0 | 6.0, 7.0, 8.0, 9.0, 10.0|]);

solve satisfy;

output [
   "% a = ", show(a), ";\n"
];
