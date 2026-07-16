include "count_fn.mzn";

predicate fzn_global_cardinality_low_up(
  array [int] of var int: xs,
  array [int] of int: cover,
  array [int] of int: lower_bound,
  array [int] of int: upper_bound,
) =
  forall (i in index_set(cover)) (
    if upper_bound[i] >= length(xs) then
      count (xi in xs) (xi = cover[i]) >= lower_bound[i]
    elseif lower_bound[i] <= 0 then
      count (xi in xs) (xi = cover[i]) <= upper_bound[i]
    else
      count (xi in xs) (xi = cover[i]) in lower_bound[i]..upper_bound[i]
    endif
  );
