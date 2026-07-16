include "fzn_cumulative_opt_decomp.mzn";

predicate fzn_cumulative_opt(
  array [int] of var opt int: start,
  array [int] of var int: duration,
  array [int] of var int: resource,
  var int: capacity,
) = fzn_cumulative_opt_decomp(start, duration, resource, capacity);
