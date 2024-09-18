/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: 1
***/

% Previously the var int overload was incorrectly used for max due to missing the par opt version

array[int] of opt int: X = [<>, 1];
opt int: x :: output = max(X);
