include "arg_min.mzn";

predicate fzn_minimum_arg_bool_opt(array [int] of var opt bool: xs, var int: idx) =
  let {
    array [index_set(xs)] of var 0..2: dx = array1d(index_set(xs), [xi default 2 | xi in xs]);
  } in minimum_arg(dx, idx);
