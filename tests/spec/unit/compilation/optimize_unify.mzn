/***
!Test
solvers: [gecode]
options: 
  -O2: true
expected: !Result
  solution: !Solution
    x: false
    z: 0
    obj: 0
***/

var bool: x;
var int: y = sum(s in 1..1 where x) (1);
var int: z;
z = y;
var int: obj :: output = if <> == true then 0 else z endif;
solve minimize obj;
