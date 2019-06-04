include "fzn_arg_max_float.mzn";

predicate maximum_arg_float(array[int] of var float: x, var int: i) =
     fzn_maximum_arg_float(x, i);