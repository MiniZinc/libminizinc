include "fzn_arg_min_int.mzn";

predicate minimum_arg_int(array[int] of var int: x, var int: i) =
    fzn_minimum_arg_int(x, i);