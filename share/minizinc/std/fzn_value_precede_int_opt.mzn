include "value_precede.mzn";

predicate fzn_value_precede_int_opt(int: before, int: after, array [int] of var opt int: xs) =
  let {
    int: def = if had_zero(xs) then max(before, after) + 1 else 0 endif;
  } in value_precede(before, after, [y default def | y in xs]);
