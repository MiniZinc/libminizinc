include "fzn_value_precede_set.mzn";
include "fzn_value_precede_set_reif.mzn";

predicate value_precede_set($$E: s, $$E: t, array[int] of var set of $$E: x) =
    fzn_value_precede_set(s, t, x);
