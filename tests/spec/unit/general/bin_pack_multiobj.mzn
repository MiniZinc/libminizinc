/***
!Test
solvers: [gurobi]
expected: !Result
  solution: !Solution
    load: [3, 8, 10]
***/

include "bin_packing_load_fn.mzn";
include "experimental.mzn";

array [1..6] of var 1..3: bin;
array [1..3] of var 0..10: load;

array [1..6] of int: weight = [i | i in 1..6];


constraint load = bin_packing_load(bin, weight);

constraint load[1] >= 3 /\ load[3] <= 10;

solve
  :: goal_hierarchy([int_min_goal(load[1]), int_min_goal(load[2]), int_min_goal(load[3])])
  satisfy;

output ["bin = ", show(bin), "\n", "load = ", show(load)];
