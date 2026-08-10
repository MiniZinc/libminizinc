/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
***/

output ["\(1, 2)"];
solve satisfy;
