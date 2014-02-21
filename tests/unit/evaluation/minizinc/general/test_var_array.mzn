% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lazyfd
% RUNS ON mzn_cd_fd
array [1..3] of var bool: b;
constraint b[1] = true;
solve satisfy;
output [ "b = ", show(b), "\n" ];
