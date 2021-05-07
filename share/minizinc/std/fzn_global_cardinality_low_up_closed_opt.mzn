include "global_cardinality.mzn";

predicate fzn_global_cardinality_low_up_closed_opt(array[int] of var opt int: x,
                                                   array[int] of int: cover,
                                                   array[int] of int: lbound,
                                                   array[int] of int: ubound) =
  forall (xi in x) (absent(xi) \/ deopt(xi) in { d | d in cover })
  /\
  global_cardinality(x, cover, lbound, ubound);
