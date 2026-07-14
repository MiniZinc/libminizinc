include "arg_min.mzn";

predicate fzn_minimum_arg_float_opt(array [int] of var opt float: xs, var int: idx) =
  let {
    float: def = ub_array(xs) + 1;
  } in minimum_arg(array1d(index_set(xs), [xi default def | xi in xs]), idx);
