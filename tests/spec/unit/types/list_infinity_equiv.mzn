/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    first: 100
***/

% `list of T' is sugar for `array[1..infinity] of T'; the hand-written form requires a
% 1-based argument in exactly the same way.
function int: get_first(array[1..infinity] of int: x) = x[1];

array[3..5] of int: a = array1d(3..5, [100, 200, 300]);

var 0..1000: first;
constraint first = get_first(array1d(a));

solve satisfy;
