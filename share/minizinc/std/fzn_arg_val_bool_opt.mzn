include "fzn_arg_val_int.mzn";

predicate fzn_arg_val_bool_opt(array [int] of var opt bool: xs, var opt bool: value, var int: idx) =
  fzn_arg_val_int([xi default 2 | xi in xs], value default 2, idx);
