% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp
% RUNS ON mzn_cd_fd
array [1..3] of int: a :: is_output = array1d(1..3, [1, 2, 3, 4]);

solve satisfy;
