/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    t: 1
    r: 4
    n: 7
***/

% `list` fields inside tuples/records (and nested structs) are coerced 1-based when bound to a
% parameter, just like a top-level `list` parameter.
function int: first_in_tuple(tuple(int, list of int): x) = x.2[1];
function int: first_in_record(record(int: k, list of int: v): x) = x.v[1];
function int: first_nested(tuple(int, tuple(int, list of int)): x) = x.2.2[1];

var 0..100: t;
var 0..100: r;
var 0..100: n;
constraint t = first_in_tuple((9, array1d(3..5, [1, 2, 3])));
constraint r = first_in_record((k: 9, v: array1d(3..5, [4, 5, 6])));
constraint n = first_nested((1, (2, array1d(3..5, [7, 8, 9]))));
solve satisfy;
