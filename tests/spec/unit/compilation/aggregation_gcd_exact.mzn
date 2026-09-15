/***
--- !Test
type: compile
solvers: [gecode]
expected: !FlatZinc aggregation_gcd_exact.fzn
***/

% Expect mathematically exact rounded bounds because GCD normalisation uses
% integer arithmetic beyond the precision of double.

var -9007199254740995..9007199254740995: x;
var 0..1: y;

constraint 2 * x + 4 * y <= 9007199254740995;
constraint 2 * x + 4 * y >= -9007199254740993;
