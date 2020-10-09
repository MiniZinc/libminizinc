/***
!Test
expected:
- !Result
  solution: !Solution
    y: 2
- !Result
  solution: !Solution
    y: 10
***/

% A regression test for G12 bug #69.
%
var int: y;
predicate p(var 1..10: x) = (x mod 2 = 0);
constraint p(y);
solve satisfy;
output ["y = ", show(y), ";\n"];
