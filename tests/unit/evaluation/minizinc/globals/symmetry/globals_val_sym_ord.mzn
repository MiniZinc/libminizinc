% RUNS ON mzn20_fd
% Test val_sym_ord for var int.
include "val_sym.mzn";
include "count.mzn";

int: n = 10;
set of int: INDEX = 1..n;
array[INDEX] of var INDEX: x;
% Reorderings of x
array[INDEX] of var INDEX: x2
  = [ if i >= n div 2 then x[n + (n div 2) - i] else x[i] endif | i in 1..n ];
array[INDEX] of var INDEX: x3
  = [ if i mod 2 == 0 then x[i div 2] else x[i div 2 + n div 2 + 1] endif
      | i in 1..n ];
array[1..2*n] of var INDEX: x4
  = [ x[n - i mod n] | i in 1..2*n ];

constraint val_sym(x, INDEX);
constraint val_sym(x2, INDEX);
constraint val_sym(x3, INDEX);
constraint val_sym(x4, INDEX);
constraint count(x, 1, 2);
constraint count(x, 2, 2);
constraint count(x, 3, 2);
constraint count(x, 4, 2);
constraint count(x, 5, 2);

solve satisfy;
output [];
