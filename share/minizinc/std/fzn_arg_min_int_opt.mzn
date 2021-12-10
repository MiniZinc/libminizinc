include "arg_min.mzn";

predicate fzn_minimum_arg_int_opt(array[int] of var opt int: x, var int: z) =
  let {
    int: def = if not had_zero(x) /\ ub_array(x) < 0 then 0 else ub_array(x) + 1 endif;
  } in minimum_arg(array1d(index_set(x), [xi default def | xi in x]), z);
