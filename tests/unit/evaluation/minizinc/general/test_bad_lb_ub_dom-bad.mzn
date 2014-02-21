% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp
% RUNS ON mzn_cd_fd
var int: x :: is_output;

var lb(x)..ub(x): y :: is_output;

solve satisfy;
