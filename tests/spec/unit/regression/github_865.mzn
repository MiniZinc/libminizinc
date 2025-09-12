/***
!Test
solvers: [gecode]
options:
  -G: std
expected: !Result
  solution: !Solution
***/

include "bin_packing.mzn";
include "bin_packing_capa.mzn";
include "bin_packing_load.mzn";
include "bin_packing_load_fn.mzn";

constraint bin_packing(0, [], []);
constraint bin_packing_capa([], [], []);
constraint bin_packing_load([], [], []);
constraint bin_packing_load([0], [], []);
constraint bin_packing_load([], []) == [];
