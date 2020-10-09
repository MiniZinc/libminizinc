/***
!Test
expected:
- !Result
  solution: !Solution
    q: [1, 2]
***/

% Regression test for bug #287: the first solution violated the alldifferent
% constraint.

include "alldifferent.mzn";

int: n = 2;
array [1..n] of var 1..n: q;

constraint alldifferent(q) :: domain; 
constraint q[1] != q[2] + 1;

solve satisfy;

output [show(q)];

