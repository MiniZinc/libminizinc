predicate fzn_global_cardinality_low_up_closed_reif(
  array [int] of var int: xs,
  array [int] of int: cover,
  array [int] of int: lower_bound,
  array [int] of int: upper_bound,
  var bool: b,
) =
  b <->
    (
      forall (i in index_set(xs)) (xs[i] in {d | d in array2set(cover)}) /\
        global_cardinality_low_up(xs, cover, lower_bound, upper_bound) /\
        % Implied condition
        length(xs) in sum(lower_bound)..sum(upper_bound)
    );

include "global_cardinality_low_up.mzn";
