/***
!Test
solvers: [gecode]
expected: !Error
  type: EvaluationError
***/

% A `list' field nested inside a tuple is checked just like a top-level `list': a non-1-based
% array in that field is an index set mismatch, not something that gets coerced silently.
function int: first_in_tuple(tuple(int, list of int): x) = x.2[1];

array[3..5] of int: a = array1d(3..5, [1, 2, 3]);

var 0..100: t;
constraint t = first_in_tuple((9, a));
solve satisfy;
