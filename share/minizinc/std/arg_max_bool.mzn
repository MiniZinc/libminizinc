include "fzn_arg_max_bool.mzn";

predicate maximum_arg_bool(array[int] of var bool: x, var int: i) =
    fzn_maximum_arg_bool(x, i);

predicate maximum_arg_bool_reif(array[int] of var bool: x, var int: i, var bool: b) =
    b <-> i=arg_max(x);
