% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_cd_fd
% A regression test for G12 bug #69.
%
var set of 1..10: y;
predicate p(var set of 1..3: x) = (2 in x);
constraint p(y);
solve satisfy;
output ["y = ", show(y), ";\n"];
