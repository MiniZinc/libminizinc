/***
!Test
solvers: [gecode]
options:
  -Werror: true
expected: !Error
  type: MiniZincError
***/

int: x == 1;
solve satisfy;
