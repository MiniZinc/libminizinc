/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a: 10
    b: 40
    c: 30
    d: 11
***/

% `list of list of T` parses and both index sets are coerced 1-based: a non-1-based outer
% array and non-1-based inner arrays all become 1-based inside the function.
function int: ll(list of list of int: x) = x[1][1];
function int: outer2(list of list of int: x) = x[2][1];
function int: inner3(list of list of int: x) = x[1][3];
% triple nesting also works:
function int: lll(list of list of list of int: x) = x[1][1][2];

var 0..100: a;
var 0..100: b;
var 0..100: c;
var 0..100: d;
constraint a = ll(array1d(3..4, [array1d(3..5, [10, 20, 30]), array1d(7..9, [40, 50, 60])]));
constraint b = outer2(array1d(3..4, [array1d(3..5, [10, 20, 30]), array1d(7..9, [40, 50, 60])]));
constraint c = inner3(array1d(3..4, [array1d(3..5, [10, 20, 30]), array1d(7..9, [40, 50, 60])]));
constraint d = lll(array1d(2..2, [array1d(3..3, [array1d(5..7, [10, 11, 12])])]));
solve satisfy;
