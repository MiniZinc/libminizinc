include "value_precede_chain.mzn";

predicate fzn_value_precede_chain_int_opt(array [int] of int: order, array [int] of var opt int: xs) =
  let {
    int: def = if had_zero(xs) then max(order) + 1 else 0 endif;
    array [int] of var int: x = array1d(index_set(xs), [xi default def | xi in xs]);
  } in value_precede_chain(order, x);
