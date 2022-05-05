include "global_cardinality.mzn";

predicate fzn_global_cardinality_closed_set(array[int] of var set of int: x,
                                            array[int] of int: cover,
                                            array[int] of var int: counts) = 
    global_cardinality(x, cover, counts) /\
    forall (v in x) (v subset { d | d in cover });
