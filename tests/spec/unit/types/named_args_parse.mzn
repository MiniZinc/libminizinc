/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution {}
***/

% Named-argument call syntax must parse and resolve to the matching decl
% (step 6 of the named-args feature). Before step 6 the typechecker raised
% "named arguments are not yet supported"; this test pins that the call
% now reaches the solver and reports SATISFIED.

predicate foo(int: x, int: y) = true;
constraint foo(x: 1, y: 2);

solve satisfy;
