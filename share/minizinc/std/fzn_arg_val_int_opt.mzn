include "fzn_arg_val_int.mzn";

predicate fzn_arg_val_int_opt(array [int] of var opt int: xs, var opt int: value, var int: idx) =
  let {
    any: values = xs ++ [value];
    int: def =
      if not had_zero(values) /\ lb_array(values) > 0 then 0 else lb_array(values) - 1 endif;
  } in fzn_arg_val_int([xi default def | xi in xs], value default def, idx);
