/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    b: false
    i: 0
  - !Solution
    b: true
    i: 1
  - !Solution
    b: false
    i: 2
  - !Solution
    b: false
    i: 3
options:
  all_solutions: true
***/

% Regression test for bug #244: when asked for all solutions (with G12/FD)
% this model did not produce the solution (i = 0, b = false).  The problem
% was that the flattening of the reified array lookup in the constraint was
% incorrect.  We were emitting (in pseudcode):
%
%	(i = i' <-> i <= size(arr)) /\ i >= 1
%
% rather than
%
%       (i = i' <-> i (i >= 1 /\ i <= size(arr)))
%
% i' is the fesh index variable.

var 0..3: i;
var bool: b;
array[1..2] of bool: arr = [true, false];
constraint b = arr[i];
solve satisfy;
output [
	"b = ", show(b), ";\n",
	"i = ", show(i), ";\n"
];
