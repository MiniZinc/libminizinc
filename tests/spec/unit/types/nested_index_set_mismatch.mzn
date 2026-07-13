/***
!Test
solvers: [gecode]
expected: !Error
  type: EvaluationError
***/

% An array with a declared index set nested inside the *elements* of an array is checked against
% the actual value, even when the outer array's index set is undeclared (`array[int] of ...').
% Such a nested index set used to go unchecked, silently accepting a mismatched array.
function int: g(array[int] of tuple(record(array[3..6] of int: x), int): a) = a[1].1.x[3];

int: r = g([((x: [10, 20, 30, 40]), 9)]);  % inner array is 1..4, declared 3..6

output ["\(r)"];
