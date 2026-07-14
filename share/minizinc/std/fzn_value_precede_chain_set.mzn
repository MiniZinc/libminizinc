include "value_precede.mzn";

predicate fzn_value_precede_chain_set(array [int] of int: order, array [int] of var set of int: xs) =
  forall (i in min(index_set(order)) + 1..max(index_set(order))) (value_precede(order[i - 1], order[i], xs));
