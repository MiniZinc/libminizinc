/***
!Test
solvers: [gecode]
expected: !Error
  type: MiniZincError
  stack:
    - x = B
    - i = 2
***/

% The stack trace of an error shows the values of the comprehension variables at the point of the
% error, even though they are reset while the exception unwinds.

enum Foo = { A, B };

constraint forall (x in Foo, i in 1..3) (if x = B /\ i = 2 then abort("foo") else true endif);
