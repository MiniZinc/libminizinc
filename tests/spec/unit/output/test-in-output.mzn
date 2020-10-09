/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    _output_item: |
      x = 1
      y = 2
      NO
  - !Solution
    _output_item: |
      x = 1
      y = 2
      YES

options:
  all_solutions: true
***/

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
