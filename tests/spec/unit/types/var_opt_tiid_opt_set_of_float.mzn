/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: .*is not a valid type.*
***/

% A `var` type-inst variable cannot be instantiated with a type that has no var version
predicate f(var opt $T: x, var bool: b) = b;
opt set of float: p = {1.0};
var bool: b;
constraint f(p, b);
