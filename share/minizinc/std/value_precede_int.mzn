include "fzn_value_precede_int.mzn";

predicate value_precede_int(int: s, int: t, array[int] of var int: x) =
    fzn_value_precede_int(s, t, x);