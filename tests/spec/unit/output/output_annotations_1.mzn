/***
--- !Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    b: 4
    y: 1
    z: 1
***/

int: a = 3;
int: b :: output = 4;

var 1..10: x :: no_output;
var 1..10: y :: output;
var 1..10: z;

solve minimize x + y + z;
