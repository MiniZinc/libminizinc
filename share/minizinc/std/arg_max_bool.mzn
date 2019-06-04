include "fzn_arg_max_bool.mzn";

predicate maximum_arg_bool(array[int] of var bool: x, var int: i) =
    fzn_maximum_arg_bool(x, i);