include "arg_min.mzn";

predicate fzn_minimum_arg_float_opt(array[int] of var opt float: x, var int: z) =
  let {
    float: def = ub_array(x) + 1;
  } in minimum_arg(array1d(index_set(x), [xi default def | xi in x]), z);
