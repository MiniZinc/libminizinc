include "fzn_arg_max_bool.mzn";

predicate fzn_arg_val_int(array [int] of var int: xs, var int: value, var int: idx) =
  fzn_maximum_arg_bool([j: xs[j] == value | j in index_set(xs)], idx) /\ xs[idx] = value;
