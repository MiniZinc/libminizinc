% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip
% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
array [1..3] of int: a :: is_output = [1, 2, 3, 4];

solve satisfy;
