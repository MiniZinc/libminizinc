include "arg_max.mzn";

predicate fzn_maximum_arg_int_opt(array [int] of var opt int: xs, var int: idx) =
  let {
    int: def = if not had_zero(xs) /\ lb_array(xs) > 0 then 0 else lb_array(xs) - 1 endif;
  } in maximum_arg(array1d(index_set(xs), [xi default def | xi in xs]), idx);
