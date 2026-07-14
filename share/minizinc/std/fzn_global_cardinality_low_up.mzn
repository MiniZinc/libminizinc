include "count_fn.mzn";

predicate fzn_global_cardinality_low_up(
  array [int] of var int: xs,
  array [int] of int: cover,
  array [int] of int: lbound,
  array [int] of int: ubound,
) =
  forall (i in index_set(cover)) (
    if ubound[i] >= length(xs) then
      count (xi in xs) (xi = cover[i]) >= lbound[i]
    elseif lbound[i] <= 0 then
      count (xi in xs) (xi = cover[i]) <= ubound[i]
    else
      count (xi in xs) (xi = cover[i]) in lbound[i]..ubound[i]
    endif
  );
