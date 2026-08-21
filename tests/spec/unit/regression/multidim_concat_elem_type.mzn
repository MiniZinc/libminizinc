/***
!Test
solvers: [gecode]
expected: !Error
  regex: .*array access out of bounds, dimension 2 of array `b' has index set 1\.\.2, but given index is 0.*
***/

% The body of the multi-dimensional '++' overload is not type specialised, so the
% concatenation used to evaluate to an array typed `var opt top', which then made
% reporting an out of bounds access on `b' fail with an internal error.
array [1..2, 1..2] of int: a = array2d(1..2, 1..2, [1, 2, 3, 4]);
array [1..2, 1..2, 1..2] of int: b = array3d(1..2, 1..2, 1..2, a ++ a);
array [int] of int: y = [b[i, j, k] | i in 1..2, j in 0..2, k in 1..2];
var int: z;
constraint z = y[1];
solve satisfy;
