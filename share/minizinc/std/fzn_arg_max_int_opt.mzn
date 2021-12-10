include "arg_max.mzn";

predicate fzn_maximum_arg_int_opt(array[int] of var opt int: x, var int: z) =
  let {
    int: def = if not had_zero(x) /\ lb_array(x) > 0 then 0 else lb_array(x) - 1 endif;
  } in maximum_arg(array1d(index_set(x), [xi default def | xi in x]), z);
