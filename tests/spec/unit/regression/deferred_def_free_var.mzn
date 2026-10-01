/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

% y is only used in the definition of c, which is flattened after its uses. y must not be removed
% as unused before that, or the FlatZinc refers to an undefined variable.
var bool: q;
function var bool: f(int: k) = let { var 0..9: y; var bool: c = y > k } in c;
constraint f(5) \/ q;
