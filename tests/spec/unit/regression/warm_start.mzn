/***
!Test
solves: [gecode]
expected: !Result
  status: SATISFIED
***/

var 1..10: x;
var 1..10: y;

constraint x + y <= 5;

% warm_start declaration for optional values accidentally removed
solve :: warm_start([x, y], [3, <>]) satisfy;
