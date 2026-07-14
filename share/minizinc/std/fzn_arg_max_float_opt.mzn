include "arg_max.mzn";

predicate fzn_maximum_arg_float_opt(array [int] of var opt float: xs, var int: idx) =
  let {
    float: def = lb_array(xs) - 1;
  } in maximum_arg(array1d(index_set(xs), [xi default def | xi in xs]), idx);
