/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: [1, 3, 5, 7, 9]
    y: [1, 3, 5, 8, 9]
***/

array[1..5] of var int: x :: add_to_output = [1,3,5,7,9];
array[4..8] of var int: y :: add_to_output = array1d(4..8, [1,3,5,8,9]);

constraint
    let { int: lx = min(index_set(x)),
          int: ux = max(index_set(x)),
          int: ly = min(index_set(y)),
          int: uy = max(index_set(y)),
          int: size = max(ux - lx, uy - ly),
          array[0..size+1] of var bool: b }
    in
        b[size+1] = (ux - lx < uy - ly) /\
        forall (i in 0..size) (
            b[i] = ((x[lx+i] < y[ly+i])
                     \/ ((x[lx+i] == y[ly+i]) /\ b[i+1]))
        ) /\ b[0];
solve satisfy;
output [
    "x = ", show(x), ";\n",
    "y = ", show(y), ";\n"
];
