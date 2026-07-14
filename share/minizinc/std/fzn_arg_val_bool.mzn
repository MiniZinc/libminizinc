include "fzn_arg_max_bool.mzn";
include "fzn_arg_min_bool.mzn";

predicate fzn_arg_val_bool(array [int] of var bool: xs, var bool: value, var int: idx) =
  if value then fzn_maximum_arg_bool(xs, idx) else fzn_minimum_arg_bool(xs, idx) endif /\ xs[idx] = value;
