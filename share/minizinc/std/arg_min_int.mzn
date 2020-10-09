include "fzn_arg_min_int.mzn";

predicate minimum_arg_int(array[int] of var int: x, var int: i) =
    fzn_minimum_arg_int(x, i);

predicate minimum_arg_int_reif(array[int] of var int: x, var int: i, var bool: b) =
     b <-> i=arg_min(x);
