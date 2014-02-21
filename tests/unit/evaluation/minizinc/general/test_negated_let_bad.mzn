% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_cd_fd
var 1..10: x;
constraint not( let { var 1..10: y } in x = y );
solve satisfy;
