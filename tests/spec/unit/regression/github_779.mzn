/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

array[1..2] of var set of 1..2: xy_shapes;

array[int] of var opt set of 1..2: x_roff =
    [ {1}
    | l in 1..2, sh in xy_shapes[l] ];
