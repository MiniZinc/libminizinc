% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx
% RUNS ON minizinc_mip

% Test xorall with multi-dimensional arrays.

bool: b = xorall([|true, false | true \/ false, false|]);

array[1..2,1..2,1..2] of var bool: bs;
constraint xorall(bs);

solve satisfy;

output [
   "% b = ", show(b), ";\n",
   "bs = ", show(bs), ";\n"
];
