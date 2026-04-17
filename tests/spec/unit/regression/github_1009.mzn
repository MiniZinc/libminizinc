/***
!Test
solvers: [gecode]
expected:
- !Error
  type: TypeError
type: compile
***/

type t1 = tuple(0..10, bool);
type t2 = tuple(11..20, 0.0..1.0);
type t3 = set of t1 ++ t2;
var t3: x;
