/***
!Test
expected:
- !Result
  solution: !Solution
    y: !Range 1..3
- !Result
  solution: !Solution
    y: !!set {2}
***/

% A regression test for G12 bug #69.
%
var set of 1..10: y;
predicate p(var set of 1..3: x) = (2 in x);
constraint p(y);
solve satisfy;
output ["y = ", show(y), ";\n"];
