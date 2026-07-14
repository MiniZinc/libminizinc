include "fzn_arg_max_bool.mzn";

predicate fzn_arg_val_float(array [int] of var float: xs, var float: value, var int: idx) =
  fzn_maximum_arg_bool([j: xs[j] == value | j in index_set(xs)], idx) /\ xs[idx] = value;
