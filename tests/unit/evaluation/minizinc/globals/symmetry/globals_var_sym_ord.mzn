% RUNS ON mzn20_fd
% Test var_sym_ord for var int.
include "var_sym.mzn";
include "all_different.mzn";

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

% Forces different values for x[i], without breaking symmetry
constraint all_different(x);

% Declare multiple intersecting symmetries
% Without ordering there is unsatisfiability with constraints such as
% x[i] <= x[j], x[j] <= x[i] and x[j] != x[i] generated for some i != j
constraint var_sym(x);
constraint var_sym(x2);
constraint var_sym(x3);
constraint var_sym(x4);

solve satisfy;
output [];
