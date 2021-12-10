include "arg_max.mzn";

predicate fzn_maximum_arg_bool_opt(array[int] of var opt bool: x, var int: z) =
  let {
    array [index_set(x)] of var 0..2: dx = array1d(index_set(x), [(xi + 1) default 0 | xi in x]);
  } in maximum_arg(dx, z);
