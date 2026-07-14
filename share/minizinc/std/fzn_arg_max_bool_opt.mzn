include "arg_max.mzn";

predicate fzn_maximum_arg_bool_opt(array [int] of var opt bool: xs, var int: idx) =
  let {
    array [index_set(xs)] of var 0..2: dx = array1d(index_set(xs), [(xi + 1) default 0 | xi in xs]);
  } in maximum_arg(dx, idx);
