include "fzn_arg_max_float.mzn";

predicate maximum_arg_float(array[int] of var float: x, var int: i) =
     fzn_maximum_arg_float(x, i);

predicate maximum_arg_float_reif(array[int] of var float: x, var int: i, var bool: b) =
     b <-> i=arg_max(x);
