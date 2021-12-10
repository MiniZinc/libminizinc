include "arg_min.mzn";

predicate fzn_minimum_arg_bool_opt(array[int] of var opt bool: x, var int: z) =
  let {
    array [index_set(x)] of var 0..2: dx = array1d(index_set(x), [xi default 2 | xi in x]);
  } in minimum_arg(dx, z);
