int: n;
% La reina de la i columna está en la fila q[i]
array [1..n] of var 1..n: q;

include "alldifferent.mzn";

% Distintas filas.
constraint alldifferent(q);

% Distintas diagonales.
constraint alldifferent([ q[i] + i | i in 1..n]);

% Hacia arriba + hacia abajo.
constraint alldifferent([ q[i] - i | i in 1..n]);

% Búsqueda.
solve :: int_search(q, first_fail, indomain_min, complete)
      satisfy;
output [ if fix(q[j]) == i then "Q" else "." endif ++
         if j == n then "\n" else "" endif | i,j in 1..n]
