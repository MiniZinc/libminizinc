include "cumulatives.mzn";

predicate fzn_cumulatives_opt(
  array [int] of var opt int: start,
  array [int] of var int: duration,
  array [int] of var int: resource,
  array [int] of var int: machine,
  array [int] of var int: bound,
  bool: is_upper_bound,
  int: min_machine,
) = cumulatives_helper(
    [si default 0 | si in start],
    [if occurs(start[i]) then duration[i] else 0 endif | i in index_set(start)],
    resource, machine, bound, is_upper_bound);
