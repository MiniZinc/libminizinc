/***
!Test
solvers: [gecode]
expected: !Error
  type: EvaluationError
***/

% A non-1-based argument to a `list of T' parameter is *not* coerced automatically: it is an
% index set mismatch, and the caller has to rebase the array with array1d.
function int: get_first(list of int: x) = x[1];

array[3..5] of int: a = array1d(3..5, [100, 200, 300]);

var 0..1000: first;
constraint first = get_first(a);

solve satisfy;
