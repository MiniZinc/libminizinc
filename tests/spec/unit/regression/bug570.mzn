/***
!Test
type: output-model
solvers: [gecode]
expected: !OutputModel bug570.ozn
***/

% 2.6.0 had a regression where the item for S was not added to the output model
set of int: S = 1..1;
array [S] of var 1..1: x;
