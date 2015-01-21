% testing generating a random list of integers
int: n = 10;
array[1..n] of int: coeff = [ uniform(1,10) | i in 1..n];
array[1..n] of var 0..n: x;

constraint sum (i in 1..n) (coeff[i]*x[i]) = n*10;

solve satisfy;
