/***
!Test
solvers: [gecode]
expected: !Error
  type: EvaluationError
***/

% A `list' nested in the *elements* of an array is checked too, even when the outer array has
% an undeclared index set (`array[int] of ...'). Such a nested list used to be neither coerced
% nor rejected, silently leaving a non-1-based array in a `list' position.
function int: inner_first(array[int] of list of int: x) = x[1][1];

var 0..100: a;
constraint a = inner_first([array1d(3..5, [10, 20, 30])]);
solve satisfy;
