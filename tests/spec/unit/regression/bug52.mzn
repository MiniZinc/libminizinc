/***
!Test
expected:
- !Result
  solution: !Solution
    x1d1: 3
    x1d9: 10
***/

% A regression test for G12 bug #52.
%
array[1..3,1..3] of var int: x;
array[1..9] of var int: x1d = [x[w,h] | w in 1..3, h in 1..3];
var int: x1d1 :: add_to_output = x1d[1];
var int: x1d9 :: add_to_output = x1d[9];

constraint x[1, 1] = 3;

constraint x[3, 3] = 10;

solve satisfy;

output [
   "x1d1 = ", show(x1d1), ";\n",
   "x1d9 = ", show(x1d9), ";\n"
];
