/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc struct_table.fzn
***/

enum E = {A,B,C};
type T = tuple(int, E, record(int: z, bool: a));

var T: x;
array[_] of T: xs = [
		(1, A, (z: 3, a: true)),
		(2, B, (z: 6, a: false)),
		(3, C, (z: 9, a: false)),
];

constraint x in xs;
