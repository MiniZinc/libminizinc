include "count.mzn";

predicate fzn_global_cardinality_reif(array[int] of var int: x,
                             array[int] of int: cover,
                             array[int] of var int: counts,
                             var bool: b) =
    b <-> (
      forall(i in index_set(cover))( count(x, cover[i], counts[i]) )
      /\
      % Implied constraint
      length(x) >= sum(counts)
    );
