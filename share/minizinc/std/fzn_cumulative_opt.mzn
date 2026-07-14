include "fzn_cumulative_opt_decomp.mzn";

predicate fzn_cumulative_opt(
  array [int] of var opt int: starts,
  array [int] of var int: durations,
  array [int] of var int: resources,
  var int: capacity,
) = fzn_cumulative_opt_decomp(starts, durations, resources, capacity);
