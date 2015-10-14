% RUNS ON mzn20_fd
% Test var_seq_sym_ord for var int.
include "var_seq_sym.mzn";
include "all_different.mzn";

int: n = 4;

array[1..n,1..n] of var 1..n: x;

constraint forall (i in 1..n) (all_different([x[i,j] | j in 1..n]));
constraint forall (i in 1..n) (all_different([x[j,i] | j in 1..n]));

constraint var_seq_sym(array2d(1..n, 1..n, [x[i,j] | i, j in 1..n]));
constraint var_seq_sym(array2d(1..n, 1..n, [x[j,i] | i, j in 1..n]));

solve satisfy;
output [ show(x) ];
