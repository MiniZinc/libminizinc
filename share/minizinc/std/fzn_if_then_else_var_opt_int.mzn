include "fzn_if_then_else_var_bool.mzn";
include "fzn_if_then_else_var_int.mzn";

predicate fzn_if_then_else_var_opt_int(array[int] of var bool: c, array[int] of var opt int: x, var opt int: y) =
  fzn_if_then_else_var_int(array1d(c),[deopt(x[i]) | i in index_set(x)],deopt(y)) /\
  fzn_if_then_else_var_bool(array1d(c),[occurs(x[i]) | i in index_set(x)],occurs(y));
