% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp
% RUNS ON mzn_lazyfd
% RUNS ON mzn_mer_lazyfd
array [ 1.. 4] of var 1..10: a = [1, 2, 3, 4];
array [11..14] of var 1..10: b = array1d(11..14, a);
array [1..2, 3..4] of var 1..10: c = array2d(1..2, 3..4, b);

solve satisfy;

output ["a = ", show(a), ";\n"];
