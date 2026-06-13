/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    y: [1, 1, -2]
***/

% Variable `list of var T' parameter: passing an array with a non-1-based index set
% (here 3..5) used to read out of bounds at x[1]. It is now coerced to 1-based, so the
% sum ranges over all three elements.
predicate sum_zero(list of var int: x) = sum (i in 1..length(x)) (x[i]) = 0;

array[3..5] of var -2..2: y;
constraint sum_zero(y);
constraint y[3] = 1 /\ y[4] = 1;

solve satisfy;
