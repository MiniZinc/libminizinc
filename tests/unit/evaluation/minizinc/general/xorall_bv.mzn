% RUNS ON minizinc_cpx
% RUNS ON minizinc_fd
% RUNS ON minizinc_mip

constraint xorall([]) = false;
constraint xorall([false]) = false;
constraint xorall([true]) = true;
constraint xorall([false, true]) = true;
constraint xorall([true, false]) = true;
constraint xorall([true, true]) = false;
constraint xorall([false, false]) = false;

solve satisfy;
output ["Ok\n"];
