% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd

int: X = 10;
var int: x = X;
var int: y;
constraint x != y;
solve satisfy;
output ["\(x) \(y)\n"];
