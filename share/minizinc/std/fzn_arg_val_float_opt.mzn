include "fzn_arg_val_float.mzn";

predicate fzn_arg_val_float_opt(
  array [int] of var opt float: xs,
  var opt float: value,
  var int: idx,
) =
  let {
    any: values = xs ++ [value];
    float: def = lb_array(values) - 1;
  } in fzn_arg_val_float([xi default def | xi in xs], value default def, idx);
