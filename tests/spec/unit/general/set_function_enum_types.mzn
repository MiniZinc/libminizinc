/***
!Test
solvers: [gecode]
options:
  -Werror: true
expected: !Result
  status: SATISFIED
***/

enum A = {a,b,c};
var set of A: x;
var set of A: y;

any: p = [x] < [y];
any: q = card(x);
