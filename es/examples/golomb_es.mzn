include "alldifferent.mzn";

% Número de marcas en la regla.
int: n;

% Longitud máxima de la regla.
int: m;

array[1..n] of var 0..m: mark;
array[1..n,1..n] of var 0..m: diffs;

constraint mark[1] = 0;
constraint forall ( i in 1..n-1 ) ( mark[i] < mark[i+1] );
constraint forall (i,j in 1..n where i > j)          % (diff)
                  (diffs[i,j] = mark[i] - mark[j]);  % (diff)
constraint alldifferent([ diffs[i,j] | i,j in 1..n where i > j]);

% Ruptura de la simetría.
constraint diffs[2,1] < diffs[n,n-1];

solve satisfy;

output ["Marcas = ",show(mark),"\n"];
