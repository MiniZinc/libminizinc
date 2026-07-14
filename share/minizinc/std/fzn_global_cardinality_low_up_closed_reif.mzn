predicate fzn_global_cardinality_low_up_closed_reif(
  array [int] of var int: xs,
  array [int] of int: cover,
  array [int] of int: lbound,
  array [int] of int: ubound,
  var bool: b,
) =
  b <->
    (
      forall (i in index_set(xs)) (xs[i] in {d | d in array2set(cover)}) /\
        global_cardinality_low_up(xs, cover, lbound, ubound) /\
        % Implied condition
        length(xs) in sum(lbound)..sum(ubound)
    );

include "global_cardinality_low_up.mzn";
