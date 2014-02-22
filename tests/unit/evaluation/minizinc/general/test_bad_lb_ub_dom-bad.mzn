% RUNS ON mzn20_fd
var int: x :: is_output;

var lb(x)..ub(x): y :: is_output;

solve satisfy;
