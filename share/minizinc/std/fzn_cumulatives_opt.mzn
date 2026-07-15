include "cumulatives.mzn";

predicate fzn_cumulatives_opt(
  array [int] of var opt int: starts,
  array [int] of var int: durations,
  array [int] of var int: resources,
  array [int] of var int: machines,
  array [int] of var int: bounds,
  bool: is_upper_bound,
  int: min_machine,
) = cumulatives_helper(
    [si default 0 | si in starts],
    [if occurs(starts[i]) then durations[i] else 0 endif | i in index_set(starts)],
    resources, machines, bounds, is_upper_bound);
