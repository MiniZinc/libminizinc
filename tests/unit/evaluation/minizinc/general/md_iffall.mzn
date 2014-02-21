% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx
% RUNS ON minizinc_mip

% Test iffall with multi-dimensional arrays.

bool: b = iffall([|true, false | true /\ false, false|]);

array[1..2,1..2,1..2] of var bool: bs;
constraint iffall(bs);

solve satisfy;

output [
   "% b = ", show(b), ";\n",
   "bs = ", show(bs), ";\n"
];
