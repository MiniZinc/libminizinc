/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: true
***/

% Used to give UNSAT with -O1 due to incorrect simplification of bool_eq

var bool: x;
var bool: y = not x;
var bool: z = x = y;

constraint (y \/ z) = (x /\ false);
