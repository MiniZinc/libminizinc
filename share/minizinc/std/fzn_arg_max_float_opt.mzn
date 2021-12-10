include "arg_max.mzn";

predicate fzn_maximum_arg_float_opt(array[int] of var opt float: x, var int: z) =
  let {
    float: def = lb_array(x) - 1;
  } in maximum_arg(array1d(index_set(x), [xi default def | xi in x]), z);
