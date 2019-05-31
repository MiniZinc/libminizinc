include "fzn_arg_max_int.mzn";

predicate maximum_arg_int(array[int] of var int: x, var int: i) =
     fzn_maximum_arg_int(x, i);