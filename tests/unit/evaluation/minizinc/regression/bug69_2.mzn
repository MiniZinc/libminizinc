% RUNS ON mzn20_fd
% A regression test for G12 bug #69.
%
var int: y;
predicate p(var 1..10: x) = (x mod 2 = 0);
constraint p(y);
solve satisfy;
output ["y = ", show(y), ";\n"];
