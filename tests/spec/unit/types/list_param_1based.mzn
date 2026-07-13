/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    first: 100
    len: 3
***/

% A `list of T' parameter is 1-based: a non-1-based array has to be rebased explicitly with
% array1d by the caller. Inside the body, index_set reports the actual (1-based) index set
% rather than the declared `1..infinity'.
function int: get_first(list of int: x) = x[1];
function int: get_len(list of int: x) = max(index_set(x));

array[3..5] of int: a = array1d(3..5, [100, 200, 300]);

var 0..1000: first;
var 0..1000: len;

constraint first = get_first(array1d(a));
constraint len = get_len(array1d(a));

solve satisfy;
