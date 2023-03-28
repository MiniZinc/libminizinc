/***
!Test
solvers:
- gecode
expected: !Error
  regex: .*cannot coerce set into 2-dimensional array.*
***/

% Previously sets were incorrectly allowed to coerce to multidimensional arrays.
output [show2d(1..2)];
