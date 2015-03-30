% testing generating a random list of integers
int: n = 10;
array[1..n] of int: coeff1 = [ uniform(1,10) | i in 1..n];
array[1..n] of int: coeff2 = [ uniform(1,10) | i in 1..n];
array[1..n] of int: coeff3 = [ uniform(1,10) | i in 1..n];
array[1..n] of var 0..n: x;

constraint 
trace("coeff1 = "++show(coeff1)++"\n") /\
trace("coeff2 = "++show(coeff2)++"\n") /\
trace("coeff3 = "++show(coeff3)++"\n");

constraint sum (i in 1..n) (coeff1[i]*x[i]) <= n*10;
constraint sum (i in 1..n) (coeff2[i]*x[i]) <= n*4;
constraint sum (i in 1..n) (coeff3[i]*x[i]) >= n*5;

solve satisfy;
