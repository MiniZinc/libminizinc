include "fzn_value_precede_chain_int.mzn";
include "fzn_value_precede_chain_int_reif.mzn";

predicate value_precede_chain_int(array[int] of int: c, array[int] of var int: x) =
    fzn_value_precede_chain_int(c, x);
