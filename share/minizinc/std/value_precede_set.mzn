include "fzn_value_precede_set.mzn";

predicate value_precede_set(int: s, int: t, array[int] of var set of int: x) =
    fzn_value_precede_set(s, t, x);
