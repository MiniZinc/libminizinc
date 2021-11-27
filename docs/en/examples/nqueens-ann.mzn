annotation bitdomain(int:nwords);

include "alldifferent.mzn";

int: n;
array [1..n] of var 1..n: q :: bitdomain(n div 32 + 1);

constraint alldifferent(q) :: domain_propagation;                      
constraint alldifferent([ q[i] + i | i in 1..n]) :: domain_propagation;
constraint alldifferent([ q[i] - i | i in 1..n]) :: domain_propagation;

ann: search_ann;

solve :: search_ann satisfy;

output [ if fix(q[j]) == i then "Q" else "." endif ++
         if j == n then "\n" else "" endif | i,j in 1..n]
