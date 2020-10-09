/***
!Test
expected:
- !Result
  solution: !Solution
    b: false
    x: 1
- !Result
  solution: !Solution
    b: false
    x: 10
***/

var 1..10: x;
var bool: b;
constraint b <-> ( let { var 1..10: y = x + 1 } in x = y );
solve satisfy;
