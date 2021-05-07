include "global_cardinality.mzn";

predicate fzn_global_cardinality_low_up_opt(array[int] of var opt int: x,
                                            array[int] of int: cover,
                                            array[int] of int: lbound,
                                            array[int] of int: ubound) =
  let {
    % Set <> to something outside the cover
    int: def = if 0 in cover then min(cover) - 1 else 0 endif;
  } in global_cardinality([x_i default def | x_i in x], cover, lbound, ubound);
