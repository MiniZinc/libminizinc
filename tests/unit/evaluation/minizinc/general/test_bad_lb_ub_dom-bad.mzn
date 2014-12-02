% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
var int: x :: is_output;

var lb(x)..ub(x): y :: is_output;

solve satisfy;
