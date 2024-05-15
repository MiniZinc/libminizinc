/***
!Test
solver: [gecode]
expected: !Result
  solution: !Solution
    x: null
    y: 0
***/

var opt 0..1: x;
var 0..1: y;

constraint (y > 0) <-> occurs(x); 
constraint absent(x);
