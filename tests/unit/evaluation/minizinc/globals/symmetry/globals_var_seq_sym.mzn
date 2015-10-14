% RUNS ON mzn20_fd
% Test var_seq_sym for var int.
include "var_seq_sym.mzn";
include "count.mzn";

set of int: INDEX = 1..10;
array[INDEX] of var INDEX: x;
array[1..3,1..2] of var int:
  y = [| x[1],  x[4]
       | x[2],  x[5]
       | x[10], x[9] |];

constraint var_seq_sym(y);
constraint forall(i in 1..5)(count(x, i, 2));
constraint x[3] = x[8];
constraint x[7] = x[9];
constraint x[5] = x[6];

solve satisfy;

output ["x = ", show(x)];
