% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp
% DISABLED ON mzn_cd_fd
% A regression test for G12 bug #69.
%
array [1..3] of var int: y;
predicate p(array [int] of var 1..10: x) = (x[1] mod 2 = 0);
constraint p(y);
solve satisfy;
output ["y = ", show(y), ";\n"];
