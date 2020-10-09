include "count_fn.mzn";

predicate fzn_global_cardinality_low_up_reif(array[int] of var int: x,
                                             array[int] of int: cover,
                                             array[int] of int: lbound,
                                             array[int] of int: ubound,
                                             var bool: b) =
  b <-> forall(i in index_set(cover))(
    count(x, cover[i]) in lbound[i]..ubound[i]
  );
