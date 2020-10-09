/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: 1
***/

set of int: X = -1..1;
set of int: Y = 0..3;

var Y: y;

function var X: f( var Y: y ) = y+1;

var int: x :: add_to_output = f(y);

solve satisfy;
