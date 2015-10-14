% RUNS ON mzn20_fd
% Test val_seq_sym_ord for var int.
include "val_seq_sym.mzn";
include "all_different.mzn";

int: n = 4;
set of int: INDEX = 1..n;
array[INDEX] of var 1..n*2: x;
% Reordering of x
array[INDEX] of var 1..n*2:
 y = [ x[2], x[1], x[3], x[4]];

array[1..2,1..4] of int:
  s1 = [| 1, 3, 7, 4
        | 6, 5, 8, 2 |];

array[1..2,1..4] of int:
  s2 = [| 3, 2, 7, 8
        | 1, 4, 6, 5 |];

constraint val_seq_sym(x, s1);
constraint val_seq_sym([x[n + 1 - i] | i in 1..n], s2);
constraint all_different(x);

solve satisfy;
output [show(x)];
