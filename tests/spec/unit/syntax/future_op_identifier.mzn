/***
!Test
solvers: [gecode]
options:
  -Werror: true
expected: !Error
  type: MiniZincError
***/

enum op = {A};
op: x = A;
solve satisfy;
