/***
!Test
expected:
- !Result
  status: SATISFIED
  solution: !Solution
    x: 1
- !Result
  status: SATISFIED
  solution: !Solution
    x: 10
***/

var 1..10: x :: add_to_output;
constraint not( let { var 1..10: y = x + 1 } in x = y );
solve satisfy;

