% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

% Test that test items work in output items.

test bar(string: s) = s = "foo" /\ fix(b);

var bool: b;
var 1..2: x;
var 1..2: y;
constraint x < y;

solve satisfy;

output [
	"x = ", show(x), "\n",
	"y = ", show(y), "\n",
	if bar("foo") then "YES" else "NO" endif, "\n"
];
