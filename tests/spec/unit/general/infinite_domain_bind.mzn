/***
!Test
solvers: [gecode]
expected: !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    x: 10
***/

var 10..infinity: x :: add_to_output =
  let {
    var 0..infinity: y
  } in y;
solve minimize x;
