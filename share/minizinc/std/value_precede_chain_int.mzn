include "fzn_value_precede_chain_int.mzn";

predicate value_precede_chain_int(array[int] of int: c, array[int] of var int: x) =
    fzn_value_precede_chain_int(c, x);
