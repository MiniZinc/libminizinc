/***
!Test
solvers: [gecode]
options: 
  -O2: true
expected: !Result
  solution: !Solution
    x: [1]
***/

% Test for issue #536
% Previously with -O2, MiniZinc copied the [a[1]] ArrayLit and incorrectly set it as flat

array [1..1] of var 1..1: x;
constraint [x[1]][1] = 1;
