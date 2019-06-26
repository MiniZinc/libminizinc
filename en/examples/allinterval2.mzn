include "alldifferent.mzn";

int: n;

array[1..n] of var 1..n: y;  % position of each number
array[1..n-1] of var 1..n-1: v; % position of difference i

constraint alldifferent(y);
constraint alldifferent(v);
constraint forall(i,j in 1..n where i < j)(
	   	 (y[i] - y[j] = 1 -> v[j-i] = y[j]) /\
                 (y[j] - y[i] = 1 -> v[j-i] = y[i])
	   );

constraint abs(y[1] - y[n]) = 1 /\ v[n-1] = min(y[1], y[n]);

solve :: int_search(y, first_fail, indomain_min)
      satisfy;

array[1..n] of 1..n: x :: output_only
  = [ sum(i in 1..n)(i*(fix(y[j]) = i) | j in 1..n ];
output [ "x = \(x);\n" ]
