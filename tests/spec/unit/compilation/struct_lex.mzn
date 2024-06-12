/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc struct_lex.fzn
***/

enum E = {A,B,C};
type T = tuple(1..2, E, record(1..2: z, bool: a));

var T: x;
var T: y;

constraint x < y;
constraint x <= y;