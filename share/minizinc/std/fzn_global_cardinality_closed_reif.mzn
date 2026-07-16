include "global_cardinality.mzn";

predicate fzn_global_cardinality_closed_reif(
  array [int] of var int: xs,
  array [int] of int: cover,
  array [int] of var int: count,
  var bool: b,
) =
  b <->
    (forall (i in index_set(xs)) (xs[i] in {d | d in cover}) /\
      global_cardinality(xs, cover, count));
