% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp
% RUNS ON mzn_cd_fd
% RUNS ON mzn_lazyfd
% RUNS ON mzn_mer_lazyfd
% RUNS ON mzn_cd_lazyfd
% A regression test for G12 bug #85.
%
include "all_different.mzn";

int: m = 3;
int: n = m*m;

array [1..m] of var 0..n: mark = [0, 1, 3];

array[1..(m*(m-1)) div 2] of var 0..n: differences =
    [ mark[j] - mark[i] | i in 1..m, j in i+1..m];

constraint mark[1] = 0;

constraint forall ( i in 1..m-2 ) ( mark[i] < mark[i+1] );

constraint all_different(differences);

constraint mark[2] - mark[1]  <  mark[m] - mark[m-1];

solve minimize mark[m];

output ["% golomb ", show(mark), "\n"];
