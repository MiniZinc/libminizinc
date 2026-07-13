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

% `list' fields inside tuples/records (and nested structs) are 1-based, just like a top-level
% `list' parameter. A struct built from 1-based arrays is accepted as-is.
function int: first_in_tuple(tuple(int, list of int): x) = x.2[1];
function int: first_in_record(record(int: k, list of int: v): x) = x.v[1];
function int: first_nested(tuple(int, tuple(int, list of int)): x) = x.2.2[1];

var 0..100: t;
var 0..100: r;
var 0..100: n;
constraint t = first_in_tuple((9, [1, 2, 3]));
constraint r = first_in_record((k: 9, v: [4, 5, 6]));
constraint n = first_nested((1, (2, [7, 8, 9])));
solve satisfy;
