/***
!Test
solvers: [gecode]
expected: !Error
  regex: .*stack overflow.*
***/

test overflow(int: x) = overflow(x + 1);
int: cause_overflow = overflow(1);
