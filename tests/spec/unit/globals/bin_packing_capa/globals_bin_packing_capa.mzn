/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    bins: [4, 2, 3, 1]
  - !Solution
    bins: [4, 2, 1, 3]
options:
  all_solutions: true
***/

include "bin_packing_capa.mzn";

% The example from the GCC.
%
constraint bin_packing_capa([4, 3, 5, 3, 3], [3, 1, 3], [4, 3, 1]);

array[1..4] of var 1..4: bins;

constraint bin_packing_capa([1, 2, 1, 3], bins, [3, 2, 1, 1]);

solve satisfy;

output ["bins = array1d(1..4, ", show(bins), ");\n"];
