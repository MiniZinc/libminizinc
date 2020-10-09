include "fzn_value_precede_chain_set.mzn";
include "fzn_value_precede_chain_set_reif.mzn";

predicate value_precede_chain_set(array[int] of int: c, array[int] of var set of int: x) =
    fzn_value_precede_chain_set(c, x);
