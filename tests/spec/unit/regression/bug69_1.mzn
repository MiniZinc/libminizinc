/***
!Test
expected:
- !Result
  status: UNSATISFIABLE
***/

% A regression test for G12 bug #69.
%
var int: y;
predicate p(var 1..10: x) = (x = y);
constraint p(2063);
solve satisfy;
output ["y = ", show(y), "\n"];
