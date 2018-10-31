include "alldifferent.mzn";

int: n;

array[1..n] of var 1..n: x;      % 数字的序列
array[1..n-1] of var 1..n-1: u;  % 差的序列

constraint alldifferent(x);
constraint alldifferent(u);
constraint forall(i in 1..n-1)(u[i] = abs(x[i+1] - x[i]));

solve :: int_search(x, first_fail, indomain_min, complete)
      satisfy;
output ["x = ",show(x),"\n"];
