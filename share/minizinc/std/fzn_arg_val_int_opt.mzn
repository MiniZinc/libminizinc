include "fzn_arg_val_int.mzn";

predicate fzn_arg_val_int_opt(array[int] of var opt int: x, var opt int: v, var int: i) =
	let {
		any: values = x ++ [v];
    int: def = if not had_zero(values) /\ lb_array(values) > 0 then 0 else lb_array(values) - 1 endif;
  } in fzn_arg_val_int([xi default def | xi in x], v default def, i);
