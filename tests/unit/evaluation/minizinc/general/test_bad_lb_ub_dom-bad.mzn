% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip
var int: x :: is_output;

var lb(x)..ub(x): y :: is_output;

solve satisfy;
