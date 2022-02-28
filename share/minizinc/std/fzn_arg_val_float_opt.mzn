include "fzn_arg_val_float.mzn";

predicate fzn_arg_val_float_opt(array[int] of var opt float: x, var opt float: v, var int: i) =
	let {
		any: values = x ++ [v];
    float: def = lb_array(values) - 1;
  } in fzn_arg_val_float([xi default def | xi in x], v default def, i);
