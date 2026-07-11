/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: "ambiguous overloading"
type: compile
***/

% Every parameter of both declarations has a default, so neither has a parameter that must
% be supplied: the call `foo()` names nothing and matches both. The declarations are
% rejected even though their parameters agree on no name at all.

predicate foo(int: x = 1) = x > 0;
predicate foo(bool: y = true) = y;

constraint foo();

solve satisfy;
