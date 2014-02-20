% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lazyfd
% RUNS ON zinc_mip_hl
% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx
% latin_squares_fd.mzn
% vim: ft=zinc ts=4 sw=4 et tw=0
% Author: Peter Stuckey
% Edited: Ralph Becket
% 
% Latin squares FD only.

int: size = 9;

set of int: range = 1..size;

array[range, range, range] of var 0..1: x;

constraint forall (i, j in range) ((sum (k in range) (x[i,j,k]) = 1));
constraint forall (i, k in range) ((sum (j in range) (x[i,j,k]) = 1));
constraint forall (j, k in range) ((sum (i in range) (x[i,j,k]) = 1));

solve satisfy;

output  [ if j = 1 /\ k = 1    then "\n"    else "" endif ++
          if fix(x[i,j,k]) = 1 then show(k) else "" endif
        | i, j, k in range ] ++ ["\n"];
