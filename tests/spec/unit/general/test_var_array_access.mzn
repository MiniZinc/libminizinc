/***
!Test
expected: !Result
  solution: !Solution
    i: 1
    x: 1
    y: !!set {1}
***/

var 1..3: i;
var int: x;
var set of 1..3: y;
constraint x = [1, 2, 3][i];
constraint y = [{1}, {2}, {3}][i];

solve minimize(i);
