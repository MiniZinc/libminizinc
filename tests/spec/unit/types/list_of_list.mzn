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

% `list of list of T' parses, and every `list' position (outer and inner) must be 1-based.
function int: ll(list of list of int: x) = x[1][1];
function int: outer2(list of list of int: x) = x[2][1];
function int: inner3(list of list of int: x) = x[1][3];
% triple nesting also works:
function int: lll(list of list of list of int: x) = x[1][1][2];

var 0..100: a;
var 0..100: b;
var 0..100: c;
var 0..100: d;
constraint a = ll([[10, 20, 30], [40, 50, 60]]);
constraint b = outer2([[10, 20, 30], [40, 50, 60]]);
constraint c = inner3([[10, 20, 30], [40, 50, 60]]);
constraint d = lll([[[10, 11, 12]]]);
solve satisfy;
