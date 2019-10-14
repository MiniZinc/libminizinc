include "fzn_if_then_else_float.mzn";
include "fzn_if_then_else_var_bool.mzn";

predicate fzn_if_then_else_opt_float(array[int] of var bool: c, array[int] of opt float: x, var opt float: y) =
  fzn_if_then_else_float(array1d(c),[deopt(x[i]) | i in index_set(x)],deopt(y)) /\
  fzn_if_then_else_var_bool(array1d(c),[occurs(x[i]) | i in index_set(x)],occurs(y));
