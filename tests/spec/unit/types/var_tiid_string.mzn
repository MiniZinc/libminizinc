/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: .*is not a valid type.*
***/

% A `var` type-inst variable cannot be instantiated with a type that has no var version
function var $T: g(var $T: x) = x;
any: z :: output = g("a");
