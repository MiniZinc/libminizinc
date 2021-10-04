include "fzn_value_precede_int.mzn";
include "fzn_value_precede_int_reif.mzn";

predicate value_precede_int($$E: s, $$E: t, array[int] of var $$E: x) =
    fzn_value_precede_int(s, t, x);
