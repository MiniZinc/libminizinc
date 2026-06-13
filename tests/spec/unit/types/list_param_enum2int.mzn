/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    lo: 1
    hi: 3
***/

% enum2int preserves index sets and an enum array literal is 1-based, so passing
% enum2int([A,B,C]) to a `list' parameter succeeds with index set 1..3.
enum E = {A, B, C};
function int: get_lo(list of int: x) = min(index_set(x));
function int: get_hi(list of int: x) = max(index_set(x));

var 0..10: lo;
var 0..10: hi;

constraint lo = get_lo(enum2int([A, B, C]));
constraint hi = get_hi(enum2int([A, B, C]));

solve satisfy;
