/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    range_avi1:
    - 9
    - 8
    - 7
  - !Solution
    range_avi1:
    - 8
    - 9
    - 7
  - !Solution
    range_avi1:
    - 9
    - 7
    - 8
  - !Solution
    range_avi1:
    - 7
    - 9
    - 8
  - !Solution
    range_avi1:
    - 8
    - 7
    - 9
  - !Solution
    range_avi1:
    - 7
    - 8
    - 9
options:
  all_solutions: true
***/

include "range.mzn";

%-----------------------------------------------------------------------------%
% range
%-----------------------------------------------------------------------------%
array[1..3] of var int: range_avi1 ::add_to_output;

constraint range(range_avi1, {1, 2, 3}, {7, 8, 9});

solve satisfy;

output ["range_avi1 = array1d(1..3, ", show(range_avi1), ");\n"];
