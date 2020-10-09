include "fzn_arg_max_int.mzn";

predicate maximum_arg_int(array[int] of var int: x, var int: i) =
     fzn_maximum_arg_int(x, i);

predicate maximum_arg_int_reif(array[int] of var int: x, var int: i, var bool: b) =
     b <-> i=arg_max(x);
