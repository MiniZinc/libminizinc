/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc struct_table_filter.fzn
***/

enum E = {A, B};
type PT = tuple(string, int, tuple(set of float, bool, E));
type VT = tuple(string, var 1..2, tuple(set of float, var bool, var E));

VT: x ::output = let {
	var 1..2: a;
	var bool: b;
	var E: c;
} in ("x", a, (2.0..3.0, b, c));
array[_] of PT: xs = [
	("x", 1, (1.0..2.0, true, A)),
	("y", 1, (1.0..2.0, true, A)),
	("x", 2, (1.0..2.0, false, B)),
	("y", 2, (1.0..2.0, false, B)),
	("x", 2, (2.0..3.0, true, A)),
	("y", 2, (2.0..3.0, true, A)),
	("x", 1, (2.0..3.0, false, B)),
	("y", 1, (2.0..3.0, false, B)),
];

constraint x in xs;