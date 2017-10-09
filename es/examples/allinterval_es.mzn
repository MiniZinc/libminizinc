include "alldifferent.mzn";

int: n;

% Secuencia de números.
array[1..n] of var 1..n: x;

% Secuencia de diferencias.
array[1..n-1] of var 1..n-1: u;

constraint alldifferent(x);
constraint alldifferent(u);
constraint forall(i in 1..n-1)(u[i] = abs(x[i+1] - x[i]));

solve :: int_search(x, first_fail, indomain_min, complete)
      satisfy;
output ["x = ",show(x),"\n"];
