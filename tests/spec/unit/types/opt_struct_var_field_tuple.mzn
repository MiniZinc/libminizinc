/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
type: compile
***/

% A tuple with any var field is itself var, so an `opt' tuple may not have var fields either.
opt tuple(var int, int): a;
solve satisfy;
