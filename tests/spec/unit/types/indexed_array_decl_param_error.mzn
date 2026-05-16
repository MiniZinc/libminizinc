/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: .*indexed array declarations.*are only allowed in top-level or let-bound variable declarations
***/

% Indexed array declarations (`array[c in S] of ...`) are not allowed in
% function parameter type-insts.
function var int: bad(array[c in 1..3] of var 1..c: a) = a[1];

var int: x = bad([1, 2, 3]);

solve satisfy;
