include "global_cardinality.mzn";

predicate fzn_inverse_in_range(array [int] of var int: xs, array [int] of var int: ys) =
  forall (i in index_set(xs)) (xs[i] in index_set(ys) -> (ys[xs[i]] == i)) /\
    forall (j in index_set(ys)) (ys[j] in index_set(xs) -> (xs[ys[j]] == j)) /\
    redundant_constraint(strengthen_injection_for_inverse_in_range(xs, ys)) /\
    redundant_constraint(strengthen_injection_for_inverse_in_range(ys, xs));

predicate strengthen_injection_for_inverse_in_range(
  array [int] of var int: f,
  array [int] of var int: invf,
) =
  let {
    set of int: sinvf = {i | i in index_set(invf) where dom(invf[i]) subset index_set(f)};
  } in global_cardinality(f, [i | i in sinvf], [1 | i in sinvf]);
