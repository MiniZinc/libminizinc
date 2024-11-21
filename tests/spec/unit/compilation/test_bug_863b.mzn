/***
!Test
type: compile
solvers: [chuffed]
expected: !FlatZinc test_bug_863b.fzn
***/

array[1..2] of var 1..2: arr;
var 1..2: index;
constraint arr[index] = arr[2];
constraint index == 2;
