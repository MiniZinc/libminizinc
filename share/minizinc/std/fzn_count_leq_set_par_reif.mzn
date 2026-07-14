include "fzn_count_leq_set_reif.mzn";

predicate fzn_count_leq_set_par_reif(
  array [int] of var set of int: xs,
  set of int: value,
  int: n,
  var bool: b,
) = fzn_count_leq_set_reif(xs, value, n, b);

%-----------------------------------------------------------------------------%
