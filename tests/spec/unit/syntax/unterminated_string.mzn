/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
***/

string: s = "no end;
solve satisfy;
