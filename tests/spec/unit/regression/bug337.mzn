/***
!Test
expected:
- !Result
  solution: !Solution
    x: [false, false]
    y: 0
***/

% Regression test for bug #337: mzn2fzn was incorrectly setting the initial
% domains of variables introduced in the reification of the division operation
% to be empty.  This was causing the model to incorrectly reported as
% unsatisfiable.

array[1..2] of var bool: x;
var 0..0: y;
constraint not(x[1 div y]);
solve satisfy;
output [
    "x = array1d(1..2, ", show(x), ";\n",
    "y = ", show(y), ";\n"
];
