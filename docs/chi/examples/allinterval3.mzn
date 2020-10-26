include "inverse.mzn";

int: n;

array[1..n] of var 1..n: x;  % 数值的序列
array[1..n-1] of var 1..n-1: u;  % 差值的序列

constraint forall(i in 1..n-1)(u[i] = abs(x[i+1] - x[i])); 

array[1..n] of var 1..n: y;  % 每个数值的位置
array[1..n-1] of var 1..n-1: v; % 差值的位置 

constraint inverse(x,y);
constraint inverse(u,v);

constraint abs(y[1] - y[n]) = 1 /\ v[n-1] = min(y[1], y[n]);

solve :: int_search(y, first_fail, indomain_min, complete)
      satisfy;

output ["x = ",show(x),"\n"];
