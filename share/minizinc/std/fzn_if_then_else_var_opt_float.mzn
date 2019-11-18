include "fzn_if_then_else_var_bool.mzn";
include "fzn_if_then_else_var_float.mzn";

predicate fzn_if_then_else_var_opt_float(array[int] of var bool: c, array[int] of var opt float: x, var opt float: y) =
  fzn_if_then_else_var_float(array1d(c),[deopt_float(x[i]) | i in index_set(x)],deopt_float(y)) /\
  fzn_if_then_else_var_bool(array1d(c),[occurs_float(x[i]) | i in index_set(x)],occurs_float(y));
