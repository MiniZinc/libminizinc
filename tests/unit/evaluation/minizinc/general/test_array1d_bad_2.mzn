% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip
% RUNS without failure ON mzn_cd_fd
array [0..3] of int: a :: is_output = [1, 2, 3, 4];

solve satisfy;
