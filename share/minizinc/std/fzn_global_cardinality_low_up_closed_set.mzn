include "global_cardinality.mzn";

predicate fzn_global_cardinality_low_up_closed_set(array[int] of var set of int: x,
                                                   array[int] of int: cover,
                                                   array[int] of int: lbound,
                                                   array[int] of int: ubound) =
    global_cardinality(x, cover, lbound, ubound) /\
    forall (v in x) (v subset { d | d in cover });
