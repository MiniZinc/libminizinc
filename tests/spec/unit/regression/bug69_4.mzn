/***
!Test
expected:
- !Result
  solution: !Solution
    y: [2, 1, 1]
- !Result
  solution: !Solution
    y: [10, 10, 10]
***/

% DISABLED ON mzn_cd_fd
% A regression test for G12 bug #69.
%
array [1..3] of var int: y;
predicate p(array [int] of var 1..10: x) = (x[1] mod 2 = 0);
constraint p(y);
solve satisfy;
output ["y = ", show(y), ";\n"];
