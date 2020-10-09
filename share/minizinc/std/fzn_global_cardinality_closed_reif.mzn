include "global_cardinality.mzn";

predicate fzn_global_cardinality_closed_reif(array[int] of var int: x,
                                    array[int] of int: cover,
                                    array[int] of var int: counts,
                                    var bool: b) =
    b <-> (
      forall(i in index_set(x))( x[i] in { d | d in cover } )
      /\
      global_cardinality(x, cover, counts)
    );
