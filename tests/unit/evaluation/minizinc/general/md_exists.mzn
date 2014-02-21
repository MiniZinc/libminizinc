% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

% Test exists/1 with multi-dimensional arrays.

bool: b = exists([|false, true | true /\false, not true|]);

array[1..2,1..2,1..2] of var bool: bs;
constraint exists(bs);

constraint bs[1, 1, 1] = true;
constraint bs[1, 1, 2] = true;
constraint bs[2, 2, 1] = false;
constraint bs[2, 2, 2] = false;

solve satisfy;
output [
   "% b = ", show(b), ";\n",
   "bs = ", show(bs), ";\n"
];
