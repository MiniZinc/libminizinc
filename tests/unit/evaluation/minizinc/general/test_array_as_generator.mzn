% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip
array [1..4] of int : xs = [1,2,3,4];
constraint 0 <= sum(x in xs)(x-1);
solve satisfy;
