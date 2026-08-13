/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

% Used to get past typechecker due to incorrect return type
array [int] of int: x = concat_index_1([1], [<>]);
