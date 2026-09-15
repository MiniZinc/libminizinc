/***
--- !Test
type: compile
solvers: [gecode]
expected: !FlatZinc aggregation_array_reader.fzn
***/

% Expect a single int_lin_eq for the objective and no variables for the inner
% sums: their only reader is the array of the outer sum, which passes the terms
% on to that sum.

int: n = 12;
array [1..n] of var 1..4: x;
var int: objective;
constraint objective =
  2 * sum (i in 1..n) (sum (j in 1..n where j < i) (bool2int(x[i] = x[j]) * ((i * j) mod 7 - 3)));
solve maximize objective;
