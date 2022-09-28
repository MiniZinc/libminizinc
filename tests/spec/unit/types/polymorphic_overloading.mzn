/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: Ok
***/

predicate foo(any $T: x, any $T: y) = true;
test foo($T: x, $T: y) = false;

output [if foo(0, 0) then "Wrong" else "Ok" endif];
