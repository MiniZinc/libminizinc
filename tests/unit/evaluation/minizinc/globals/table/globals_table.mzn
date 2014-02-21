% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp
% RUNS ON minizinc_cpx

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

var 1..10: x;
var 1..10: y;
var 1..10: z; 

constraint table([x,y,z], t);

constraint x != y /\ z < 3;

solve satisfy;

output [
    "x = ", show(x), ";\n",
    "y = ", show(y), ";\n",
    "z = ", show(z), ";\n"
];
