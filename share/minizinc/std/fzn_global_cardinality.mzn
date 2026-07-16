include "count.mzn";

predicate fzn_global_cardinality(
  array [int] of var int: xs,
  array [int] of int: cover,
  array [int] of var int: count,
) =
  forall (i in index_set(cover)) (count(xs, cover[i], count[i])) /\
    % Implied constraint
    length(xs) >= sum(count);
