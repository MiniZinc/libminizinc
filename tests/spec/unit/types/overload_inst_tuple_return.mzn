/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

function tuple(int): foo(int: x) = (x,);
function tuple(var int): foo(var int: x) = (x,);
