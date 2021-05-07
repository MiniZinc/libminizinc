/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    gcc_x: [6, 7, 6, 8, 6, 9, null, 8, 6, null]
    gcc_c: [4, 1, 2, 1]
  - !Solution
    gcc_x: [6, 7, 6, 8, null, 9, 6, 8, 6, null]
    gcc_c: [4, 1, 2, 1]
  - !Solution
    gcc_x: [6, 7, null, 8, 6, 9, 6, 8, 6, null]
    gcc_c: [4, 1, 2, 1]
  - !Solution
    gcc_x: [6, 7, 9, 8, 6, 9, 6, 8, 6, null]
    gcc_c: [4, 1, 2, 2]
  - !Solution
    gcc_x: [6, 7, 6, 8, 9, 9, 6, 8, 6, null]
    gcc_c: [4, 1, 2, 2]
  - !Solution
    gcc_x: [6, 7, 6, 8, 6, 9, 9, 8, 6, null]
    gcc_c: [4, 1, 2, 2]
***/

include "global_cardinality_closed.mzn";

constraint global_cardinality_closed([3, <>, 3, 5, <>, 6], [3, 5, 6], [2, 1, 1]);

array [int] of var opt -100..100: gcc_x ::add_to_output = [6, 7, _, 8, _, 9, _, 8, 6, <>];
array [int] of var -100..100: gcc_c ::add_to_output = [4, 1, 2, _];

constraint global_cardinality_closed(gcc_x, [6, 7, 8, 9], gcc_c);

solve satisfy;
