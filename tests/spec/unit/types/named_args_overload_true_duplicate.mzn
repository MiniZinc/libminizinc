/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: 'function with the same type already defined'
type: compile
***/

% Two predicates with identical parameter types AND identical parameter names
% must still error as a true duplicate (named-arguments step 3 only relaxes
% the check for name-differing decls).

predicate dupfn(int: x, int: y) = true;
predicate dupfn(int: x, int: y) = false;

solve satisfy;
