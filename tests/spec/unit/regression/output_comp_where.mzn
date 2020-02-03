/***
!Test
options:
  all_solutions: true
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    p: 1
    x: [true]
  - !Solution
    p: 2
    x: [null]
***/

% Regression test for bug where outputting a comprehension with a generalized where clause would still remove absent values in output,
% leading to a mismatched index set in the displayed array.

var 1..2: p :: add_to_output;
array [1..1] of var opt bool: x :: add_to_output = [true | i in 1..1 where i = p];

solve satisfy;