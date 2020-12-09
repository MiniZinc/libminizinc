include "inverse.mzn";

int: n;

array[1..n] of var 1..n: x;  % sequence of numbers
array[1..n-1] of var 1..n-1: u;  % sequence of differences

constraint forall(i in 1..n-1)(u[i] = abs(x[i+1] - x[i])); 

array[1..n] of var 1..n: y;  % position of each number
array[1..n-1] of var 1..n-1: v; % position of difference i

constraint inverse(x,y);
constraint inverse(u,v);

constraint abs(y[1] - y[n]) = 1 /\ v[n-1] = min(y[1], y[n]);

solve :: int_search(y, first_fail, indomain_min)
      satisfy;

output ["x = ",show(x),"\n"];
