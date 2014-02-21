% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp
% RUNS ON mzn_cd_fd
array [1..4] of int : xs = [1,2,3,4];
constraint 0 <= sum(x in xs)(x-1);
solve satisfy;
