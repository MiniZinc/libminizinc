include "arg_min.mzn";

predicate fzn_minimum_arg_int_opt(array [int] of var opt int: xs, var int: idx) =
  let {
    int: def = if not had_zero(xs) /\ ub_array(xs) < 0 then 0 else ub_array(xs) + 1 endif;
  } in minimum_arg(array1d(index_set(xs), [xi default def | xi in xs]), idx);
