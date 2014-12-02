% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
array [1..3] of var bool: b;
constraint b[1] = true;
solve satisfy;
output [ "b = ", show(b), "\n" ];
