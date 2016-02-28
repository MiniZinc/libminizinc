% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip

% Check that the MiniZinc round builtin operation.
% (mzn2fzn 1.1.3 didn't implement it.)
%
par float: x = 3.6;
var int: y = round(x);
solve satisfy;
output ["y = ", show(y), ";\n"];
