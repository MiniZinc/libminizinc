/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: 'named arguments are not yet supported'
type: compile
***/

% Named-argument call syntax must parse (step 5 of the named-args feature) but
% is rejected by a typechecker guard until step 6 implements proper unwrapping.
% Before the parser change, the colon would be a syntax error; afterwards, the
% guard catches the VarDecl sentinel and reports a clear message. Step 6 will
% replace the guard with real name resolution and tighten this test.

predicate foo(int: x, int: y);
constraint foo(x: 1, y: 2);

solve satisfy;
