/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
***/

int: x = case 1 of 1 => 2, _ => 3 endcase;
solve satisfy;
