include "fzn_arg_min_bool.mzn";

predicate minimum_arg_bool(array[int] of var bool: x, var int: i) =
     fzn_minimum_arg_bool(x, i);

predicate minimum_arg_bool_reif(array[int] of var bool: x, var int: i, var bool: b) =
     b <-> i=arg_min(x);
