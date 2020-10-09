/***
!Test
expected:
- !Result
  status: SATISFIED
  solution: !Solution
    i: 2
    y: true
- !Result
  status: SATISFIED
  solution: !Solution
    i: 2
    y: false
***/

var 1..3: i;
var bool: y;
constraint y = [not(y), y, not(y)][i];

solve satisfy;
