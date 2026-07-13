/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    y: [1, 1, -2]
***/

% Variable `list of var T' parameter: the 1-based guarantee means the `1..length(x)' idiom in
% the body is sound. A non-1-based array (here 3..5) has to be rebased by the caller.
predicate sum_zero(list of var int: x) = sum (i in 1..length(x)) (x[i]) = 0;

array[3..5] of var -2..2: y;
constraint sum_zero(array1d(y));
constraint y[3] = 1 /\ y[4] = 1;

solve satisfy;
