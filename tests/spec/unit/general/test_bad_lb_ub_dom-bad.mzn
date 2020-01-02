/***
!Test
expected: !Result
  status: SATISFIED
***/

var int: x :: add_to_output;

var lb(x)..ub(x): y :: add_to_output;

solve satisfy;
