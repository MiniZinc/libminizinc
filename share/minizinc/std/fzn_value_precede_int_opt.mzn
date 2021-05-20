include "value_precede.mzn";

predicate fzn_value_precede_int_opt(int: s, int: t, array[int] of var opt int: x) =
  let {
    int: def = if had_zero(x) then max(s, t) + 1 else 0 endif;
  } in value_precede(s, t, [y default def | y in x]);
