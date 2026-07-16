include "count.mzn";

predicate fzn_global_cardinality_reif(
  array [int] of var int: xs,
  array [int] of int: cover,
  array [int] of var int: count,
  var bool: b,
) =
  b <->
    (
      forall (i in index_set(cover)) (count(xs, cover[i], count[i])) /\
        % Implied constraint
        length(xs) >= sum(count)
    );
