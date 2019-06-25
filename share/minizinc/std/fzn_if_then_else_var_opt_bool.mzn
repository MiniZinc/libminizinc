include "fzn_if_then_else_var_bool.mzn";

predicate fzn_if_then_else_var_opt_bool(array[int] of var bool: c, array[int] of var opt bool: x, var opt bool: y) =
  fzn_if_then_else_var_bool(array1d(c),[deopt(x[i]) | i in index_set(x)],deopt(y)) /\
  fzn_if_then_else_var_bool(array1d(c),[occurs(x[i]) | i in index_set(x)],occurs(y));
