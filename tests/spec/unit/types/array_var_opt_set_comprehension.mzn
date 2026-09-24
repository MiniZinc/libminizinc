/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !Solution
    xy_shapes: [!!set {2}, !!set {1, 2}]
    x_roff: null
***/

array[1..2] of var set of 1..2: xy_shapes;
constraint xy_shapes = [{2}, {1, 2}];

any: x_roff :: output = [{1} | l in 1..2, sh in xy_shapes[l]][1];
