include "inverse.mzn";

int: n;

% Secuencia de números.
array[1..n] of var 1..n: x;

% Secuencia de diferencias.
array[1..n-1] of var 1..n-1: u;

constraint forall(i in 1..n-1)(u[i] = abs(x[i+1] - x[i]));

% Posición de cada número.
array[1..n] of var 1..n: y;

% Posición de la diferencia i.
array[1..n-1] of var 1..n-1: v;

constraint inverse(x,y);
constraint inverse(u,v);

constraint abs(y[1] - y[n]) = 1 /\ v[n-1] = min(y[1], y[n]);

solve :: int_search(y, first_fail, indomain_min, complete)
      satisfy;

output ["x = ",show(x),"\n"];
