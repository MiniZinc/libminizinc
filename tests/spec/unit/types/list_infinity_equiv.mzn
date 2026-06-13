/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    first: 100
***/

% `list of T' is sugar for `array[1..infinity] of T'; the hand-written form gets the
% same 1-based coercion of its arguments.
function int: get_first(array[1..infinity] of int: x) = x[1];

var 0..1000: first;
constraint first = get_first(array1d(3..5, [100, 200, 300]));

solve satisfy;
