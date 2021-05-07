/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    gcc_x: [null, 6, 7, 8, 8, 6, 9, null, null, 6]
    gcc_c: [3, 1, 2, 1]
  - !Solution
    gcc_x: [null, 6, 7, 6, 8, 8, 9, null, null, 6]
    gcc_c: [3, 1, 2, 1]
  - !Solution
    gcc_x: [null, 6, 7, 8, 8, null, 9, 6, null, 6]
    gcc_c: [3, 1, 2, 1]
  - !Solution
    gcc_x: [null, 6, 7, 6, 8, null, 9, 8, null, 6]
    gcc_c: [3, 1, 2, 1]
  - !Solution
    gcc_x: [null, 6, 7, null, 8, 8, 9, 6, null, 6]
    gcc_c: [3, 1, 2, 1]
  - !Solution
    gcc_x: [null, 6, 7, null, 8, 6, 9, 8, null, 6]
    gcc_c: [3, 1, 2, 1]
  - !Solution
    gcc_x: [null, 6, 7, 8, 8, null, 9, null, 6, 6]
    gcc_c: [3, 1, 2, 1]
  - !Solution
    gcc_x: [null, 6, 7, 6, 8, null, 9, null, 8, 6]
    gcc_c: [3, 1, 2, 1]
  - !Solution
    gcc_x: [null, 6, 7, null, 8, 8, 9, null, 6, 6]
    gcc_c: [3, 1, 2, 1]
  - !Solution
    gcc_x: [null, 6, 7, null, 8, 6, 9, null, 8, 6]
    gcc_c: [3, 1, 2, 1]
  - !Solution
    gcc_x: [null, 6, 7, null, 8, null, 9, 8, 6, 6]
    gcc_c: [3, 1, 2, 1]
  - !Solution
    gcc_x: [null, 6, 7, null, 8, null, 9, 6, 8, 6]
    gcc_c: [3, 1, 2, 1]
options:
  all_solutions: true
***/

include "globals.mzn";

constraint global_cardinality([3, 3, <>, 8, 6, <>], [3, 5, 6], [2, 0, 1]);

array[1..10] of var opt 6..10: gcc_x :: output = [<>, 6, 7, _, 8, _, 9, _, _, 6];
array[1..4] of var -100..100: gcc_c :: output = [3, 1, 2, _];

constraint global_cardinality(gcc_x, [6, 7, 8, 9], gcc_c);
constraint count (xi in gcc_x) (absent(xi)) > 2;

solve satisfy;
