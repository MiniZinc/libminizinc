/***
!Test
expected:
- !Result
  solution: !Solution
    x: 1
    y: 2
    z: 1
- !Result
  solution: !Solution
    x: 2
    y: 5
    z: 2
- !Result
  solution: !Solution
    x: 6
    y: 5
    z: 1
***/

include "table.mzn";

array[1..10,1..3] of int: t;

t = [| 1,1,1 
         | 1,2,3
         | 2,1,3
         | 4,1,3
         | 2,3,4
         | 6,5,1
         | 1,3,3
         | 2,5,2
         | 1,2,1
         | 3,4,4 |];

var 1..10: x ::add_to_output;
var 1..10: y ::add_to_output;
var 1..10: z ::add_to_output; 

constraint table([x,y,z], t);

constraint x != y /\ z < 3;

solve satisfy;

output [
    "x = ", show(x), ";\n",
    "y = ", show(y), ";\n",
    "z = ", show(z), ";\n"
];
