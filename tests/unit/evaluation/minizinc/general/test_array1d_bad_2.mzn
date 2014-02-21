% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp
% RUNS without failure ON mzn_cd_fd
array [0..3] of int: a :: is_output = [1, 2, 3, 4];

solve satisfy;
