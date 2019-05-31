include "fzn_minimum_arg_bool";

predicate minimum_arg_bool(array[int] of var bool: x, var int: i) =
     fzn_minimum_arg_bool(x, i);