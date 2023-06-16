/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

function int: foo(record(record(int: a): b): c) = 1;
function int: bar(record(tuple(int): a): c) = 2;
