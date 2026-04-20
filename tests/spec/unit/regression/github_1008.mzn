/***
!Test
solvers: [gecode]
options:
  -Werror: true
expected: !Result
  status: SATISFIED
***/

enum E = { E1 };
tuple(E, int, int, int): t1 = (E1, 0, 0, 0);
array [int] of int: a = [t.3 | t=t1];
