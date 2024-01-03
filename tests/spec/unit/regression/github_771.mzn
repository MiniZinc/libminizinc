/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: "0"
solvers: [gecode]
***/
output let { var 0..0: Y } in [ show(Y) ];
